#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
sys.path.insert(0, '.')
import json

'''
  level1 уровень детальзаци
'''

jcommon = '''
[
  {
    "name":"enabled",
    "component_level":0,
    "field_level":0,
    "comment":"Включение/отключение компонента",
    "default":true,
    "path":[],
    "depth":2
  }
]
'''

def create_element(x, path):
  if type(x) is dict:
    return FormatObject( x, path)
  elif type(x) is list:
    return FormatList(x, path)
  else:
    return FormatValue(x, path)
    
def create_fields(fields, path):
  return [ FormatField(name, value, path) for name, value in fields.iteritems() ]

def create_items(items, path):
  return [ FormatItem(value, path) for value in items ]


# простой тип
class FormatValue:
  def __init__(self, value, path):
    self.path = path
    self.value = value
  def dump(self):
    print json.dumps(self.value)

# Элемент списка
class FormatItem:
  def __init__(self, value, path):
    self.value = create_element(value, path)
    self.path = path
  def dump(self):
    self.value.dump()
    
class FormatList:
  def __init__(self, items, path):
    self.path = path
    self.items = create_items(items, path)
  def dump(self):
    print "["
    for i in self.items:
      i.dump()
    print "]"

# поле объекта
class FormatField:
  def __init__(self, name, value, path):
    self.name = name
    self.value = create_element(value, path)
  def dump(self):
    print json.dumps(self.name), ":", 
    self.value.dump()

class FormatObject:
  def __init__(self, fields, path):
    self.path = path
    self.fields = create_fields(fields, path)
  def dump(self):
    print "{"
    for i in self.fields:
      i.dump()
    print "}"

class FormatRoot:
  def __init__(self, fields):
    self.root = create_element(fields, [])
  def dump(self):
    self.root.dump()
    

def run():
  # TODO: удалить коментарии для json
  config = json.load(sys.stdin)
  root = FormatRoot(config)
  print root
  root.dump()
  
if __name__ == '__main__':
  run()


