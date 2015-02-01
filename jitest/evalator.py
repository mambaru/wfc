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

sys.path.insert(0, '.')

class Evalator:

  ## Deserialize fd to a Python object
  #     @param fd a .read()-supporting file-like object containing a JSON document
  #     @param name имя последовательности или объекта запроса 
  #
  #  Так-же строит all последовательность, которя являеться объединением остальных
  def __init__(self, fd, name = 'all', max_replay = 0):
    
    # загрузка json файла
    fd.seek(0)
    orig = json.load( fd )
    
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
      elif isinstance(v1, list):
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

    # номер итерации
    self.flat['N']=0
    # позиция в списке
    self.flat['P']=0
    # счетчик прохода по списку (сколько раз прошли по списку)
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
      for k, v in self.flat.iteritems():
        if isinstance(v, list):
          if len(v) > 2 :
            # TODO: проверка валиидности списка
            names += [k]
            all+=v[:-1]
      all += [ u"union lists of [" + u",".join(names) + u"]" ]
      self.flat['all']=all
      
    if self.name in self.lists:
      self.cur_list = self.lists[self.name]
    elif self.name in self.flat:
      self.cur_list = [ self.flat[self.name], 1, "" ]
    else:
      raise "sequence '{0}' not found".format(self.name)

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

  def prepare_(self, obj, dct ):
    is_str =  isinstance(obj, str) or isinstance(obj, unicode)
    
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

  def prepare_string_(self, arg, dct ):
    if len(arg) < 3:
      return arg
    elif arg[0]=='{' and arg[1]=='%':
      return self.eval_(arg[2:-2], dct)
    elif arg[0]=='$' and arg[1]=='{':
      return self.sub_(arg[2:-1], dct)
    return self.replace_(arg, dct)
      
  def eval_(self, arg, dct ):
    arg = self.replace_(arg, dct)
    return eval(arg, self.modules)

  def replace_(self, arg, dct ):
    while True:
      beg = arg.find('$')
      if beg == -1:
        return arg
      end = beg+1
      while end<len(arg) and arg[end].isalnum():
        end +=1
      arg = arg[:beg] + str(self.sub_(arg[beg+1:end], dct)) + arg[end:]
    return arg
  
  def sub_(self, arg, dct):
    if arg in dct:
      return dct[arg]
    if arg in self.flat:
      val = copy.deepcopy(self.flat[arg])
      val = self.prepare_(val, dct)
      dct[arg] = val
      return val
    raise Exception("key '{0}' not found".format(arg))
  
  def create_(self):
    obj = None
    cur = self.cur_list
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
