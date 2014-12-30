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

class evalator:
  ## Deserialize fd to a Python object
  #     @param fd a .read()-supporting file-like object containing a JSON document
  #     @param name имя последовательности или объекта запроса 
  #
  #  Так-же строит all последовательность, которя являеться объединением остальных
  def __init__(self, fd, name = 'all'):
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
    # загрузка json файла
    orig = json.load( fd )
    # загрузка модyлей 
    self.modules = {}
    
    if "import" in orig:
      for im in orig["import"]:
        self.modules[im] = importlib.import_module(im)

    # в плоскую структуру
    self.flat = {}
    for k1, v1 in orig.iteritems():
      if not k1 in ['import']:
        for k2, v2 in v1.iteritems():
          self.flat[k2]=v2
          
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
    

    # номер итерации
    self.flat['N']=0
    # позиция в списке
    self.flat['P']=0
    # счетчик прохода по списку (сколько раз прошли по списку)
    self.flat['C']=0
    # счетчик списка
    self.flat['C1']=0
    # счетчик повторений 
    self.flat['C2']=0
    now = datetime.datetime.now()
    self.flat['start'] = time.mktime(now.timetuple())

  def next(self):
    # создаем объект и обновляем счетчики 
    obj = self.create_()
    obj = self.prepare_(obj, {})
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
        obj[i] = self.prepare_(obj[i])
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
    cur = self.flat[self.name]
    if isinstance(cur, list):
      # если нужно переходим на следующий элемент
      if cur[ self.flat['P'] +1 ] == self.flat['C2']:
        self.flat['P']+=2
        self.flat['C2']=0
      # если дошли до конца списка
      if self.flat['P'] == len(cur)-1:
        self.reset_(['P','C','C1','C2'])
        self.flat['C'] += 1
      obj = copy.deepcopy( cur[self.flat['P']] )
      self.inc_(['N','C','C1','C2'])
    else:
      obj = copy.deepcopy(cur)
    
    now = datetime.datetime.now()
    unix_now = time.mktime(now.timetuple())
    unix_start = self.flat['start']
    self.flat['N'] += 1
    self.flat['timestamp'] = int(unix_now)
    self.flat['timespan'] = int(unix_now - unix_start)
    return obj

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
  parser.add_argument("file", help="configuration file", type=argparse.FileType('r'))
  args = parser.parse_args()
  el = evalator(args.file, "all")
  for i in range(100):
    cur = el.next()
    print( json.dumps(cur, ensure_ascii=False, sort_keys=True) )
