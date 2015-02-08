#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import datetime
import time
  
class jsonrpc:
  
  def __init__(self, cli, stat = None, trace=False):
    self.cli = cli
    self.call_id = 0
    self.trace = trace
    self.stat = stat
    
    self.id_list = []
    self.req_by_id = {}
    self.res_by_id = {}
  
  def request(self, method, params):
    self.call_id += 1
    req_str = json.dumps({
        'jsonrpc':'2.0',
        'method':method,
        'params':params,
        'id': self.call_id
    })
    if self.trace :
      print(req_str)
    if self.stat:
      start = datetime.datetime.now()
    self.cli.send(req_str)
    res_str = self.cli.recv()
    if res_str == None:
      return None

    if self.stat:
      finish = datetime.datetime.now()
      delta = finish - start
      microseconds = delta.seconds*1000000 + delta.microseconds
      self.stat.add(method, microseconds)
      
    if self.trace :
      print(res_str)
    result = json.loads(res_str)
    if 'error' in result:
      raise Exception('jsonrpc', result['error'])
    return result['result']
  
  def notify(self, method, params):
    req_str = json.dumps({
        'jsonrpc':'2.0',
        'method':method,
        'params':params
    })
    if self.trace :
      print(req_str)
    self.cli.send(req_str)

  #
  # Асинхронный режим (блокируемый)
  # Отправляем несколько запросов, потом читаем ответы
  #   
  def async_request(self, method, params):
    self.call_id += 1
    req_str = json.dumps({
        'jsonrpc':'2.0',
        'method':method,
        'params':params,
        'id': self.call_id
    })
    if self.trace :
      print(req_str)
    if self.stat:
      start = datetime.datetime.now()
    else:
      start = None
    self.cli.send(req_str)
    self.id_list += [self.call_id]
    class Obj: 
      pass
    obj = Obj()
    obj.method = method
    obj.start = start
    self.req_by_id[self.call_id]=obj
    
  # seq = True ответы в том же порядке, что и отправленны запросы
  # seq Используеться для проверки запросов 
  def async_result(self, seq=True):
    res_str = self.cli.recv()
    if res_str == None:
      return None

    finish = datetime.datetime.now()
    
    if self.trace :
      print(res_str)

    result = json.loads(res_str)
    if 'error' in result:
      raise Exception('jsonrpc', result['error'])
    
    id = result['id']
    if not id in self.req_by_id:
      raise Exception('jsonrpc', "Invalid id="+str(id) )
    start = self.req_by_id[id].start
    method = self.req_by_id[id].method
    delta = finish - start
    microseconds = delta.seconds*1000000 + delta.microseconds
    if self.stat != None:
      self.stat.add(method, microseconds)
      
    result = json.loads(res_str)
    if 'error' in result:
      raise Exception('jsonrpc', result['error'])
    return result['result']
  
  

if __name__ == '__main__':
  pass  
