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
def load_queries(f):
  queries = json.load( f )
  for k in ["import", "values", "params", "results", "queries", "sequences"]:
    if not k in queries:
      queries[k]={}
  all=[]
  for k, v in queries["sequences"].iteritems():
    all+=v[:-1]
  all += [u"объединение всех последовательностей"]
  queries["sequences"]["all"]=all
  return queries
    
def show_json(obj):
  print( json.dumps(obj, ensure_ascii=False, sort_keys=True) )
  
def import_modules(queries):
  eval_modules = {}
  if "import" in queries:
    for im in queries["import"]:
      eval_modules[im] = importlib.import_module(im)
  return eval_modules
 
 
 
class evalator1:
  
  def __init__(self, name):
    # имя последовательности или запроса 
    self.name = name
    self.orig = queries
    # текущая копия queries
    self.current = None
    # текущая копия values
    self.values  = None
    self.params  = None
    self.result  = None
    self.queries  = None
    self.target  = None
    self.modules = modules
    self.start   = datetime.datetime.now()
    self.N = 0
    self.pos = 0
    self.count1 = 0
    self.count2 = 0
    
  def load(self, fd):
    self.orig = json.load( fd )
    
    for k in ['import', 'values', 'params', 'results', 'queries', 'sequences']:
      if not k in queries:
        self.orig[k]={}
        
    all=[]
    for k, v in queries['sequences'].iteritems():
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
        
  '''
  def init(self):
    if self.name == 'all':
      all=[]
      for k, v in queries["sequences"].iteritems():
        all+=v[:-1]
      all += [u"объединение всех последовательностей"]
      self.target = all
    elif self.name in self.orig['queries']:
      self.target = self.orig['queries'][self.name]
    elif self.name in self.orig['sequences']:
      self.target = self.orig['sequences'][self.name]
  '''


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
    
    if self.pos == len(self.target)-1:
      self.pos = 0
      self.count1 = 0
      self.count2 = 0
    
    if self.count2 == self.target[self.pos + 1]:
      self.count2 = 0
      self.pos += 2
      self.count1 += 1

    self.count2 += 1
    
  def reinit(self):
    cpy = copy.deepcopy(self.orig)
    self.values  = cpy['values']
    self.params  = cpy['params']
    self.result  = cpy['result']
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
    self.deep_replace(self.results, config["values"])

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
    
  def deep_replace(self, arg):
    if isinstance(arg, dict):
      self.replace_dict_(arg)
    elif isinstance(arg, list):
      self.replace_list_(arg)
    else:
      return self.replace_string_(arg, vals)
    return arg
    
# private

  # eval 

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

  def eval_dict_(arg):
    for k, v in arg.iteritems():
      if isinstance(v, dict):
        self.deep_eval(v)
      else:
        arg[k] = self.eval_value_(v)

  def eval_list_(arg):
    for i, v in enumerate(arg):
      if isinstance(v, dict):
        self.deep_eval(v)
      else:
        arg[i] = self.eval_value_(v)
        
  # replace 
  
  def replace_value_(self, arg, dct):
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
    for k, v in orig.iteritems():
      newk = self.replace_value_(k, dct)
      arg[newk] = self.deep_replace(v, dct)
      if newk != k:
        del orig[k]

  def replace_list_(self, arg, dct):
    for i, v in enumerate(arg):
      arg[i] = self.deep_replace(v, dct)



    
    
 
# Дефолтные переменные в values
# @params N номер итерации
# @params start_ts время начала работы скрипта
def values_init(values, N = 0, start_dt = None ):
  if not start_dt:
    start_dt = datetime.datetime.now()
  now = datetime.datetime.now()
  unix_now = time.mktime(now.timetuple())
  unix_start = time.mktime(start_dt.timetuple())
  values["N"] = N
  values["timestamp"] = int(unix_now)
  values["timespan"] = int(unix_now - unix_start)

def eval_value(arg, eval_modules):
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
      res = eval(py, eval_modules)
      arg = arg[:pos1] + str(res) + arg[pos2+2:]
  return arg

def eval_dict(d, eval_modules):
  for k, v in d.iteritems():
    if isinstance(v, dict):
      eval_foreach(v, eval_modules)
    else:
      d[k] = eval_value(v, eval_modules)

def eval_list(d, eval_modules):
  for i, v in enumerate(d):
    if isinstance(v, dict):
      eval_foreach(v, eval_modules)
    else:
      d[i] = eval_value(v, eval_modules)

def eval_foreach(d, eval_modules):
  if isinstance(d, dict):
    eval_dict(d, eval_modules)
  elif isinstance(d, list):
    eval_list(d, eval_modules)
  else:
    raise Exception("eval_foreach: type must be list or dict ")

def replace_value(arg, vals):
  if not isinstance(arg, str) and not isinstance(arg, unicode):
    return arg
  keys = vals.keys()
  keys.sort(reverse=True)
  flag = True
  while flag:
    flag = False
    for k in keys:
      k1 = "$"+k
      pos = arg.find(k1)
      flag = pos!=-1
      if flag:
        arg = arg.replace(k1, str(vals[k]))
      else:
        k1 = "${"+k
        pos1 = arg.find(k1)
        if pos1!=-1:
          return vals[k]
  return arg

def replace_dict(orig, vals):
  for k, v in orig.iteritems():
    newk = replace_value(k, vals)
    orig[newk] = replace_foreach(v, vals)
    if newk != k:
      del orig[k]

def replace_list(orig, vals):
  for i, v in enumerate(orig):
    orig[i] = replace_foreach(v, vals)


def replace_foreach(orig, vals):
  if isinstance(orig, dict):
    replace_dict(orig, vals)
  elif isinstance(orig, list):
    replace_list(orig, vals)
  else:
    return replace_value(orig, vals)
  return orig
    

def replace_all(config):
  replace_foreach(config["params"], config["values"])
  replace_foreach(config["results"], config["values"])

  replace_foreach(config["queries"], config["params"])
  replace_foreach(config["queries"], config["results"])
  replace_foreach(config["queries"], config["values"])

  replace_foreach(config["sequences"], config["queries"])
  replace_foreach(config["sequences"], config["values"])


def next_config(orig_config, eval_modules, N = 0, start_dt = None):
  config = copy.deepcopy(orig_config)
  eval_foreach(config["values"], eval_modules)
  values_init(config["values"], N, start_dt)
  replace_all(config)
  eval_foreach(config, eval_modules)
  return config 

  
def next_request(sequence, count, eval_modules):
  '''@count'''
  count1 = 0
  for i in range( 0, len(sequence)-1, 2 ):
    count2 = 0
    for j in range(sequence[i+1]):
      if count1 == count:
        obj = copy.deepcopy(sequence[i])
        replace_foreach(obj, {"C1":count1, "C2":count2})
        eval_foreach(obj, eval_modules)
        return obj
      count1 +=1
      count2 +=1
  return None

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("file", help="configuration file", type=argparse.FileType('r'))
  args = parser.parse_args()
  config = load_queries(args.file)
  show_json(config)
  print("---------------------")
  eval_modules = import_modules(config)
  config = next_config(config, eval_modules, 33)
  show_json(config)
