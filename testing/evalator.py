#!/usr/bin/python
# -*- coding: utf-8 -*-

## @package pyexample
#  Documentation for this module.
#
#  More details.
    
import json
import argparse
import copy
import datetime
import time
import importlib

## Deserialize f to a Python object
#  @param f a .read()-supporting file-like object containing a JSON document
#
#  Так-же проверяет и добавляет обязательные ключи и строит all последовательность,
#  которя являеться объединением остальных
 
class evalator:
  
  def __init__(self, name = None):
    # имя последовательности или запроса 
    self.name = name
    # текущая копия queries
    self.current = None
    # текущая копия values
    self.values  = None
    self.params  = None
    self.results  = None
    self.queries  = None
    self.target  = None
    self.modules = None
    self.start   = datetime.datetime.now()
    self.N = 0
    self.pos = 0
    self.count1 = 0
    self.count2 = 0
    
  def load(self, fd):
    self.orig = json.load( fd )
    
    for k in ['import', 'values', 'params', 'results', 'queries', 'sequences']:
      if not k in self.orig:
        self.orig[k]={}
        
    all=[]
    for k, v in self.orig['sequences'].iteritems():
      if len(v) > 2 :
        all+=v[:-1]
    all += [u"объединение всех последовательностей"]
    self.orig['sequences']['all'] = all
    
    self.modules = {}
    if "import" in self.orig:
      for im in self.orig["import"]:
        self.modules[im] = importlib.import_module(im)
  
  def show(self):
    if self.current != None:
      print( json.dumps(self.current, ensure_ascii=False, sort_keys=True) )
    else:
      print( json.dumps(self.orig, ensure_ascii=False, sort_keys=True) )

  def recreate(self):
    self.reinit()
    self.deep_eval(self.values)
    self.replace()
    self.deep_eval(self.target)
    
  def next(self):
    
    if isinstance(self.target, dict):
      self.recreate()
      self.deep_replace(self.target, {"C1":0, "C2":0})
      self.deep_eval(self.target)
      return self.target
    
    if self.target and self.pos == len(self.target)-1:
      self.pos = 0
      self.count1 = 0
      self.count2 = 0

    self.recreate()
    self.deep_replace(self.target[self.pos], {"C1": self.count1, "C2":self.count2} )
    self.deep_eval(self.target[self.pos])
    
    if self.count2 == self.target[self.pos + 1]:
      
      self.count2 = 0
      self.pos += 2
      self.count1 += 1

    self.count2 += 1
    return self.target[self.pos]

  def reinit(self):
    cpy = copy.deepcopy(self.orig)
    self.values  = cpy['values']
    self.params  = cpy['params']
    self.results  = cpy['results']
    self.queries = cpy['queries']
    
    if self.name in self.orig['queries']:
      self.target = self.orig['queries'][self.name]
      self.queries = None
    elif self.name in self.orig['sequences']:
      self.target = self.orig['sequences'][self.name]
    else:
      raise Exception('target not found')
    
    now = datetime.datetime.now()
    unix_now = time.mktime(now.timetuple())
    unix_start = time.mktime(self.start.timetuple())
    self.values['N'] = self.N
    self.values['timestamp'] = int(unix_now)
    self.values['timespan'] = int(unix_now - unix_start)
    self.N += 1
    
  def replace(self):
    self.deep_replace(self.params, self.values)
    self.deep_replace(self.results, self.values)

    if self.queries != None:
      self.deep_replace(self.queries, self.params)
      self.deep_replace(self.queries, self.results)
      self.deep_replace(self.queries, self.values)
      self.deep_replace(self.target, self.queries)
    
    self.deep_replace(self.target, self.results)
    self.deep_replace(self.target, self.values)
    self.deep_replace(self.target, self.params)

  def deep_eval(self, arg):
    if isinstance(arg, dict):
      self.eval_dict_(arg)
    elif isinstance(arg, list):
      self.eval_list_(arg)
    else:
      raise Exception("eval_foreach: type must be list or dict ")
    
  def deep_replace(self, arg, dct):
    if isinstance(arg, dict):
      self.replace_dict_(arg, dct)
    elif isinstance(arg, list):
      self.replace_list_(arg, dct)
    else:
      return self.replace_string_(arg, dct)
    return arg
    
# private

  def eval_value_(self, arg):
    if not isinstance(arg, str) and not isinstance(arg, unicode):
      return arg
    
    flag = True
    while flag:
      flag = False
      k1 = "{%"
      k2 = "%}"
      pos1 = arg.find(k1)
      flag = pos1 != -1
      if flag:
        pos2 = arg.find(k2, pos1)
        flag = pos2 != -1
      if flag:
        py = arg[ pos1+2 : pos2]
        res = eval(py, self.modules)
        arg = arg[:pos1] + str(res) + arg[pos2+2:]
    return arg

  def eval_dict_(self, arg):
    for k, v in arg.iteritems():
      if isinstance(v, dict):
        self.deep_eval(v)
      else:
        arg[k] = self.eval_value_(v)

  def eval_list_(self, arg):
    for i, v in enumerate(arg):
      if isinstance(v, dict):
        self.deep_eval(v)
      else:
        arg[i] = self.eval_value_(v)
        
  # replace 
  
  def replace_string_(self, arg, dct):
    if not isinstance(arg, str) and not isinstance(arg, unicode):
      return arg
    keys = dct.keys()
    keys.sort(reverse=True)
    flag = True
    while flag:
      flag = False
      for k in keys:
        k1 = "$"+k
        pos = arg.find(k1)
        flag = pos!=-1
        if flag:
          arg = arg.replace(k1, str(dct[k]))
        else:
          k1 = "${"+k
          pos1 = arg.find(k1)
          if pos1!=-1:
            return dct[k]
    return arg

  def replace_dict_(self, arg, dct):
    for k, v in arg.iteritems():
      newk = self.replace_string_(k, dct)
      arg[newk] = self.deep_replace(v, dct)
      if newk != k:
        del arg[k]

  def replace_list_(self, arg, dct):
    for i, v in enumerate(arg):
      arg[i] = self.deep_replace(v, dct)


if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("file", help="configuration file", type=argparse.FileType('r'))
  args = parser.parse_args()
  el = evalator("all")
  el.load(args.file)
  el.show()
  for i in range(100):
    trg = el.next()
    print( json.dumps(trg, ensure_ascii=False, sort_keys=True) )
    
  print("####################################")
  
  # el.show()
  
  '''
  args = parser.parse_args()
  config = load_queries(args.file)
  show_json(config)
  print("---------------------")
  eval_modules = import_modules(config)
  config = next_config(config, eval_modules, 33)
  show_json(config)
  '''
