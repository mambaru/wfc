#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import argparse
import copy
import datetime
from random import *
from math import *

config = {}
raw_config = {}


def load_config(filename):
  global raw_config
  raw_config = json.load( open(filename) )

# Дефолтные переменные в values
# @params N номер итерации
# @params start_ts время начала работы скрипта
def values_init(d, N = 0, start_ts = 0 ):
  now = datetime.now()
  d["N"] = N
  d["timestamp"] = now
  d["timespan"] = start_ts - now
  d["rand"] = 100
  d["rand10"] = 100
  d["rand100"] = 100
  d["rand1000"] = 100
  d["randstr10"] = "100"
  d["randstr100"] = "100"
  d["randstr1000"] = "100"

def eval_value(v):
  try:
    return eval(v)
  except:
    return v

def eval_dict(d):
  for k, v in d.iteritems():
    if isinstance(v, dict):
      config_foreach(v)
    else:
      d[k] = eval_value(v)

def eval_list(d):
  for i, v in enumerate(d):
    if isinstance(v, dict):
      config_foreach(v)
    else:
      d[i] = eval_value(v)

def config_foreach(d ):
  global config
  if isinstance(d, dict):
    eval_dict(d)
  elif isinstance(d, list):
    eval_list(d)
  else:
    raise

def next_config():
  global config
  
  pass

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument("file", help="configuration file")
  args = parser.parse_args()
  load_config(args.file)
  print(raw_config)
  print("---------------------")
  config = copy.deepcopy(raw_config)
  config_foreach(config)
  print(json.dumps(config))