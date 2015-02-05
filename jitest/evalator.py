#!/usr/bin/python
# -*- coding: utf-8 -*-

## @package evalator
#  Documentation for this module.
#
#  More details.
    
import json
import argparse
import copy
import datetime
import time
import importlib
import sys
import re

sys.path.insert(0, '.')

# from http://stackoverflow.com/a/241506
def comment_remover(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " " # note: a space and not an empty string
        else:
            return s
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)
  
def prepare_load(fd):
  fd.seek(0)
  text = fd.read()
  return comment_remover(text)
  
  

class Evalator:

  ## Deserialize fd to a Python object
  #     @param fd a .read()-supporting file-like object containing a JSON document
  #     @param name имя последовательности или объекта запроса 
  #
  #  Так-же строит all последовательность, которя являеться объединением остальных
  def __init__(self, fd, name = 'all', max_replay = 0):
    
    # загрузка json файла
    #fd.seek(0)
    txt = prepare_load(fd)
    try:
      orig = json.loads( txt )
      #orig = json.load( fd )
    except ValueError as e:
      print("Ошибка декодирования JSON конфигурации: {0}".format(e.message))
      raise e
    
    # загрузка модyлей 
    self.modules = {}
    
    if "import" in orig:
      for im in orig["import"]:
        self.modules[im] = importlib.import_module(im)

    # в плоскую структуру
    self.flat = {}
    # тестовые последовательности в формате [type, int, type, int, ..., "описание"]
    self.lists = {}
    for k1, v1 in orig.iteritems():
      #if not k1 in ['import'] :
      if isinstance(v1, dict):
        for k2, v2 in v1.iteritems():
          self.flat[k2]=v2
      elif isinstance(v1, list) and k1!="import":
        self.lists[k1]=v1
      else:
        self.lists[k1]=[v1,1,""]
    self.reset(name, max_replay)

  def reset(self, name = 'all', max_replay = 0):
    # Имя объекта или последовательности
    self.name = name
    # номер итерации
    self.N = 0
    # позиция в последовательности
    self.pos = 0
    # счетчик последовательноси
    self.count1 = 0
    # счетчик повторений 
    self.count2 = 0

    # глобальный счетчик
    self.flat['N']=0
    # позиция в списке
    self.flat['P']=0
    # номер итерации
    self.flat['I']=0
    # счетчик списка
    self.flat['C1']=0
    # счетчик повторений 
    self.flat['C2']=0
    self.max_replay = max_replay
    self.replay_count = 0

    now = datetime.datetime.now()
    self.flat['startstamp'] = int(time.mktime(now.timetuple()))
    self.next_time_()    

    if name=="all":
      # all объединение 
      all=[]
      names=[]
      for k, v in self.lists.iteritems():
        if isinstance(v, list):
          if len(v) > 2 :
            # TODO: проверка валиидности списка
            names += [k]
            all+=v[:-1]
      all += [ u"union lists of [" + u",".join(names) + u"]" ]
      if len(all) > 1: 
        self.lists['all']=all
#      else:
#        self.lists['all']=None
      
    if self.name in self.lists:
      self.cur_list = self.lists[self.name]
    elif self.name in self.flat:
      self.cur_list = [ self.flat[self.name], 1, "" ]
    else:
      raise Exception("sequence '{0}' not found".format(self.name))

  def next_time_(self):
    now = datetime.datetime.now()
    unix_now = time.mktime(now.timetuple())
    unix_start = self.flat['startstamp']
    self.flat['timestamp'] = int(unix_now)
    self.flat['timespan'] = int(unix_now - unix_start)

    
  def next(self):
    if self.max_replay!=0 and self.replay_count==self.max_replay:
      return None
    
    
    # создаем объект и обновляем счетчики 
    obj = self.create_()
    obj = self.prepare_(obj, {})
    self.next_counters_()
    return obj
  
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

  def is_str(self, obj):
    return isinstance(obj, str) or isinstance(obj, unicode)
  
  def prepare_(self, obj, dct ):
    is_str =  self.is_str(obj)
    if is_str:
      return self.prepare_string_(obj, dct)
    elif isinstance(obj, list):
      for i in range(len(obj)):
        obj[i] = self.prepare_(obj[i], dct)
    elif isinstance(obj, dict):
      keys = obj.keys()
      for k in keys:
        key = self.prepare_string_( k, dct)
        val = self.prepare_(obj[k], dct)
        if key != k:
          del obj[k]
        obj[key] = val
    return obj

  def prepare_string_(self, arg, dct, isPy=False ):
    if len(arg) < 3:
      return arg
    arr0 = [arg]
    flag=True
    while flag:
      flag = self.prepare_eval_(arr0, dct)
      if not flag:
        flag = self.prepare_sub_(arr0, dct, offset=0, isPy=isPy)
    return arr0[0]
  
  # Поиск ${name} в строке
  def prepare_sub_(self, arr0, dct, offset=0, isPy=False):
    arg = arr0[0]
    if not self.is_str(arg):
      return False
    if len(arg) - offset < 3:
      return False
    beg = arg.find('$', offset)
    if beg==-1:
      return False
    if arg[beg+1]!='{':
      # это просто $, продолжаем 
      return self.prepare_sub_(arr0, dct, offset=beg+1, isPy=isPy)
    self.do_sub_(arr0, dct, beg, isPy=isPy)
    return True
      
  # обработка ${name} в строке
  def do_sub_(self, arr0, dct, offset=0, isPy=False):
    arg = arr0[0]
    beg = offset+2
    end = arg.find('}', beg)
    if end==-1:
      raise Exception("evalator","expected of '}'")
    name=arg[beg:end]
    if name in dct:
      arr0[0]=self.replace_( arr0[0], dct[name], offset, end+1)
      return
    elif name in self.flat:
      obj=copy.deepcopy(self.flat[name])
      obj=self.prepare_(obj, dct)
      dct[name]=obj
      arr0[0]=self.replace_( arr0[0], obj, offset, end+1, isPy=isPy)
      return
    raise Exception("evalator","Object '{0}' not found".format(name))
    
  def replace_(self, arg, obj, beg, end, isPy=False):
    if beg==0 and end==len(arg):
      return obj
    if self.is_str(obj) or isPy:
      return arg[:beg] + str(obj) + arg[end:]
    return arg[:beg] + json.dumps(obj)+arg[end:]
      
  # Поиск {% code %} в строке
  # предполагаеться что внутри все ${name} уже обработаны 
  def prepare_eval_(self, arr0, dct):
    arg = arr0[0]
    if not self.is_str(arg):
      return False
    if len(arg) < 4:
      return False
    beg = arg.find('{%')
    if beg==-1:
      return False
    end = arg.find('%}',beg)
    if end==-1:
      raise Exception("evalator","expected of '%}'")
    self.do_eval_(arr0, dct,  beg, end+2)
    return True
    
  # обработка {% code %} в строке
  def do_eval_(self, arr0, dct, beg, end):
    arg = arr0[0]
    code = arg[beg+2:end-2]
    code =self.prepare_string_(code, dct, isPy=True)
    obj = eval(code, self.modules)
    obj = self.prepare_(obj, dct)
    arr0[0] = self.replace_(arg, obj, beg, end, isPy=True)

  def prepare_string_X(self, arg, dct ):
    if len(arg) < 3:
      return arg
    elif arg[0]=='{' and arg[1]=='%':
      return self.eval_(arg[2:-2], dct)
    elif arg[0]=='$' and arg[1]=='{':
      return self.sub_(arg[2:-1], dct)
    return self.replace_(arg, dct)
      
  def eval_X(self, arg, dct ):
    arg = self.replace_(arg, dct)
    return eval(arg, self.modules)

  def replace_X(self, arg, dct ):
    while True:
      beg = arg.find('$')
      if beg == -1:
        return arg
      end = beg+1
      while end<len(arg) and arg[end].isalnum():
        end +=1
      arg = arg[:beg] + str(self.sub_(arg[beg+1:end], dct)) + arg[end:]
    return arg
  
  def sub_X(self, arg, dct):
    if arg in dct:
      return dct[arg]
    if arg in self.flat:
      val = copy.deepcopy(self.flat[arg])
      val = self.prepare_(val, dct)
      dct[arg] = val
      return val
    raise Exception("key '{0}' not found".format(arg))

#-------------------------------------------------------------------------------

  def create_(self):
    obj = None
    cur = self.cur_list
    if not isinstance(cur[ self.flat['P']*2 +1 ], int):
      raise Exception("evalator", "invalid request sequence")
    if isinstance(cur, list):
      obj = copy.deepcopy( cur[self.flat['P']*2] )
    else:
      obj = copy.deepcopy(cur)
    self.next_time_()
    return obj
  
  def next_counters_(self):
    self.flat['N'] += 1
    cur = self.cur_list
    if isinstance(cur, list):
      self.inc_(['C1','C2'])
      # если нужно переходим на следующий элемент
      if cur[ self.flat['P']*2 +1 ] == self.flat['C2']:
        self.flat['P']+=1
        self.flat['C2']=0
      # если дошли до конца списка
      if self.flat['P']*2 == len(cur)-1:
        self.reset_(['P','C1','C2'])
        self.flat['I'] += 1
        self.replay_count += 1
    else:
      self.replay_count += 1
      self.flat['I'] += 1

  def reset_(self, keys):
    for k in keys:
      self.flat[k]=0

  def inc_(self, keys):
    for k in keys:
      self.flat[k] += 1
      
      
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------
#-------------------------------------------------------------------------------

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("file", help="Файл с шаблонами генерации в формате json", type=argparse.FileType('r'))
  parser.add_argument("seq", nargs='?', help="Имя последовательности или объекта", default="all")
  parser.add_argument("count", nargs='?', help="Кол-во генерируемых объектов для вывода", type=int, default=1)
  args = parser.parse_args()
  el = Evalator(args.file, args.seq,args.count)
  while True:
    cur = el.next()
    if cur == None:
      break
    print( json.dumps(cur, ensure_ascii=False, sort_keys=True) )
