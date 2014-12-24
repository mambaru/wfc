#!/usr/bin/python
# -*- coding: utf-8 -*-

import json
import argparse
import socket
import datetime
import time

import config


class client:
  
  # TODO: сделать UDP
  def __init__(self, addr, port, pconn = False, prot="tcp"):
    self.addr = addr
    self.port = port
    self.prot = prot
    self.buff = ""
    self.pconn = pconn
    if self.pconn:
      self.connect()
  
  def connect(self):
    self.cli = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.cli.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    self.cli.connect( (self.addr, self.port) )
    self.cli.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    self.buff = ""
    
  def close(self):
    self.cli.shutdown(socket.SHUT_RDWR)
    self.cli.close()
    del self.cli
    self.cli = None
    self.buff = ""
    
  def send(self, req):
    if not self.pconn:
      self.connect()
    self.cli.send(req+"\r\n")

  def parse(self):
    pos = self.buff.find("\r\n")
    if pos == -1:
      return None
    result = self.buff[:pos]
    self.buff = self.buff[pos+2:]
    #print(len(self.buff))
    return result
    
  def recv(self):
    result = self.parse()
    while result==None:
      self.buff += self.cli.recv(4096)
      result = self.parse()
    if not self.pconn:
      self.close()
    return result
  
class bench:
  def __init__(self):
    self.methods={}
    pass
  
  def add(self, method, microseconds):
    if not method in self.methods:
      self.methods[method]=[]
    self.methods[method] += [microseconds]
    if len(self.methods[method]) == 1000:
      self.methods[method].sort()
      arr = self.methods[method]
      rate = 1000000
      print("{0}\t{1}({2})\t{3}({4})\t{5}({6})\t{7}({8})\t{9}({10})\t{9}({10})\t{11}({12})".format(
        method, 
        arr[0], rate/arr[0], 
        arr[500], rate/arr[500], 
        arr[800], rate/arr[800], 
        arr[950], rate/arr[950], 
        arr[990], rate/arr[990], 
        arr[999], rate/arr[999]))
      self.methods[method] = []
      
    
  
class jsonrpc:
  
  def __init__(self, cli, stat = None, trace=False):
    self.cli = cli
    self.call_id = 0
    self.trace = trace
    self.stat = stat
  
  def request(self, method, params):
    self.call_id += 1
    req_str = json.dumps({
        'method':method,
        'id': self.call_id,
        'params':params
    })
    if self.trace :
      print(req_str)
    start = datetime.datetime.now()
    self.cli.send(req_str)
    res_str = self.cli.recv()
    finish = datetime.datetime.now()
    #self.microseconds = time.mktime((finish - start).timetuple()) 
    delta = finish - start
    self.microseconds = delta.seconds*1000000 + delta.microseconds
    if self.stat :
      self.stat.add(method, self.microseconds)
      
    if self.trace :
      print(res_str)
    result = json.loads(res_str)
    if 'error' in result:
      raise Exception('jsonrpc error', result['error'])
    return result['result']

    
    


if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('-a', '--addr', help="server address", default='0.0.0.0')
  parser.add_argument('-p', '--port', help="server port", type=int, default = 0)
  parser.add_argument('-f', '--file', help="config file", type=argparse.FileType('r'))
  parser.add_argument('-s', '--sequence', help="Последовательность")
  parser.add_argument("type", default = "probe", nargs='?', choices=["probe"])
  args = parser.parse_args()
  print(args.file)
  conf = config.load_config(args.file)
  config.show_config(conf)
  
  print("---------------------")
  eval_modules = config.import_modules(conf)
  conf = config.next_config(conf, eval_modules, 33)
  config.show_config(conf)
  print("=====================")
  
  
  #print(conf['sequences'][args.sequence]['sequence'][0])
  #tmp = conf['sequences'][args.sequence]['sequence'][0]
  #print(tmp)
  jrp = jsonrpc( client(args.addr, args.port, pconn=True), stat = bench(), trace=False )
  jrp.cli.connect()
  for i in range(1,1000000):
    result = jrp.request(conf['sequences']['seq1'][0][0], conf['sequences']['seq1'][0][1])
    ##if 0!=cmp(result, conf['sequences']['seq1'][0][2]):
    if not result['status']:
      raise Exception("Fuck")
    #print("{0} - {1}".format(jrp.microseconds, 1000000/jrp.microseconds))
    #print(jrp.microseconds)
    #print(1000000/jrp.microseconds)
