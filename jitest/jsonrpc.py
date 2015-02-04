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
    start = datetime.datetime.now()
    self.cli.send(req_str)
    res_str = self.cli.recv()
    if res_str == None:
      return None
    
    finish = datetime.datetime.now()
    delta = finish - start
    self.microseconds = delta.seconds*1000000 + delta.microseconds
    if self.stat != None:
      self.stat.add(method, self.microseconds)
      
    if self.trace :
      print(res_str)
    result = json.loads(res_str)
    if 'error' in result:
      raise Exception('jsonrpc error', result['error'])
    return result['result']
  
  def notify(self, method, params):
    pass

if __name__ == '__main__':
  pass  
