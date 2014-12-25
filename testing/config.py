#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import argparse
import copy
import datetime
import time
import importlib

def load_config(f):
  conf = json.load( f )
  for k in ["import", "values", "params", "results", "queries", "sequences"]:
    if not k in conf:
      conf[k]={}
  all=[]
  for k, v in conf["sequences"].iteritems():
    all+=v[:-1]
  conf["sequences"]["all"]=all
  return conf
    
    

def show_config(config):
  print( json.dumps(config, ensure_ascii=False, sort_keys=True) )
  
def import_modules(config):
  eval_modules = {}
  if "import" in config:
    for im in config["import"]:
      eval_modules[im] = importlib.import_module(im)
  return eval_modules
  
 
# Дефолтные переменные в values
# @params N номер итерации
# @params start_ts время начала работы скрипта
def values_init(d, N = 0, start_dt = None ):
  if not start_dt:
    start_dt = datetime.datetime.now()
  now = datetime.datetime.now()
  unix_now = time.mktime(now.timetuple())
  unix_start = time.mktime(start_dt.timetuple())
  d["N"] = N
  d["timestamp"] = int(unix_now)
  d["timespan"] = int(unix_now - unix_start)

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

  '''
  if not isinstance(v, str) and not isinstance(v, unicode):
    return v
  try:
    return eval(v, eval_modules)
  except:
    return v
  '''

def eval_dict(d, eval_modules):
  for k, v in d.iteritems():
    if isinstance(v, dict):
      eval_foreach(v, eval_modules)
    else:
      #print("d[{0}]={1}".format(k, d[k]) )
      d[k] = eval_value(v, eval_modules)
      #print("d[{0}]={1}".format(k, d[k]) )

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
    #for k, v in vals.iteritems():
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
    

'''
def replace_foreach(orig, vals):
  for k,v in orig:
    if len(v) && v[0]='$':
      if v[1:], nv in  vals:
        orig[k] = nv
'''
def replace_all(config):
  replace_foreach(config["params"], config["values"])
  replace_foreach(config["results"], config["values"])

  replace_foreach(config["queries"], config["params"])
  replace_foreach(config["queries"], config["results"])
  replace_foreach(config["queries"], config["values"])

  replace_foreach(config["sequences"], config["queries"])
  replace_foreach(config["sequences"], config["values"])
  
  #for k, v in config["sequences"].iteritems():
  #  for i in range( 0, len(v)-1, 2 ):
  #    replace_foreach(v[i], {'C':})
    
  
def next_config(orig_config, eval_modules, N = 0, start_dt = None):
  config = copy.deepcopy(orig_config)
  eval_foreach(config["values"], eval_modules)
  values_init(config["values"], N, start_dt)
  replace_all(config)
  eval_foreach(config, eval_modules)
  return config 
  
def next_request(sequence, count, eval_modules):
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
      
        
  
  
  for i in range( 0, len(sequence)-1, 2 ):
    if sequence[i+1] + count1 < count:
      count2 = count1 - count
      print("{0}: {1} - {2}".format(count, count1, count2))
      return
    else:
      count1 += sequence[i+1]
      
    

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("file", help="configuration file", type=argparse.FileType('r'))
  args = parser.parse_args()
  config = load_config(args.file)
  show_config(config)
  print("---------------------")
  eval_modules = import_modules(config)
  config = next_config(config, eval_modules, 33)
  show_config(config)
