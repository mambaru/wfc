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
    #self.res_by_id = {}
  
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
    if 'result' in result:
      return result['result']
    return {}
  
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
    class Info: 
      pass
    info = Info()
    info.method = method
    info.start = start
    info.result = None
    self.req_by_id[self.call_id]=info
    
  # seq = True ответы в том же порядке, что и отправленны запросы
  # seq Используеться для проверки запросов 
  def async_result(self):
    result = None
    count_read = 0
    while result == None:
      self.async_read()
      has_result = len(self.id_list)!=0 and self.id_list[0] in self.req_by_id
      
        
      if not has_result:
        if count_read > 10:
          time.sleep( count_read/10.0)
        continue
      id = self.id_list[0]
      info = self.req_by_id[id]
      if info.result == None:
        continue
      
      del self.id_list[0]
      del self.req_by_id[id]
      return info.result
    return result
  
  def async_read(self):
    
    try:
      res_str=None
      
      res_str = self.cli.recv()
      
      if res_str == None:
        return
      
      if self.stat != None:
        finish = datetime.datetime.now()
      
      if self.trace :
        print(res_str)
      
      result = json.loads(res_str)
      
      if 'error' in result:
        raise Exception('jsonrpc', result['error'])
      
      id = result['id']
      
      if not id in self.req_by_id:
        raise Exception('jsonrpc', "Invalid id="+str(id) )
      info = self.req_by_id[id]
      
      if self.stat != None:
        start = info.start
        method = info.method
        delta = finish - start
        microseconds = delta.seconds*1000000 + delta.microseconds
        self.stat.add(method, microseconds)
        
      info.result = result['result']
    except Exception as e:
      if e.errno == 11:
        return
      print("Exception {0}, {1}".format(e, e.errno) )
      if res_str:
        print(res_str)
      raise e
      
    

  
  
  def async_result2(self, seq=True):
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
    
    info = self.req_by_id[id]
    
    start = info.start
    method = info.method
    delta = finish - start
    microseconds = delta.seconds*1000000 + delta.microseconds
    
    if self.stat != None:
      self.stat.add(method, microseconds)
    
    result = json.loads(res_str)
    
    if 'error' in result:
      raise Exception('jsonrpc', result['error'])
    
    if result['id'] != self.id_list[0]:
      self.res_by_id[id] = result
      return None
    
    del self.id_list[0]
    del self.req_by_id[id]
    del self.res_by_id[id]
    
    
    return result['result']
  
  
if __name__ == '__main__':
  pass  
