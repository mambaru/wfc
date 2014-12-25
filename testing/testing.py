#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys

sys.path.insert(0, '.')

import os
import json
import argparse
import socket
import datetime
import time
from threading import Thread, Lock
import importlib
from options import options


import config
'''
options = {
  'addr'     : '0.0.0.0',
  'port'     : '12345',
  'file'     : './testing.json',
  'prot'     : 'tcp',
  'sequence' : 'all',
  'threads'  : 0,
  'rate'     : 0,
  'count'    : 0,
  'check'    : False,
  'pconn'    : False,
  'trace'    : False,
  'list'     : False,
  'type'     : 'probe'
}
'''


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
    self.mutex = Lock()
    self.methods={}
    self.count_max = options["count"]
    self.rate_max = options["rate"]
    self.arr_size = 1000
    if self.rate_max != 0:
      if self.rate_max < 1000:
        self.arr_size = self.rate_max
      self.ts_min = 1000000/self.rate_max
      print( self.ts_min ) 
    self.count = 0
    self.start = datetime.datetime.now()
    pass
  
  def add(self, method, microseconds):
    #self.mutex.acquire()
    if not method in self.methods:
      class Object: pass
      stat = Object()
      stat.data = []
      stat.start = datetime.datetime.now()
      self.methods[method] = stat
    stat = self.methods[method]
    stat.data += [microseconds]
    if len(stat.data) == self.arr_size:
      finish = datetime.datetime.now()
      delta = finish - stat.start
      microseconds = delta.seconds*1000000 + delta.microseconds
      rate = self.arr_size*1000000/microseconds
      stat.data.sort()
      arr = stat.data
      sec = 1000000
      perc50 = self.arr_size*50/100
      perc80 = self.arr_size*80/100
      perc90 = self.arr_size*90/100
      perc95 = self.arr_size*95/100
      perc99 = self.arr_size*99/100
      perc100 = self.arr_size-1
      print("{0}({1})\t{2}({3})\t{4}({5})\t{6}({7})\t{8}({9})\t{10}({11})\t{12}({13})".format(
        method, rate,
        arr[0], sec/arr[0], 
        arr[perc50], sec/arr[perc50], 
        arr[perc80], sec/arr[perc80], 
        arr[perc95], sec/arr[perc95], 
        arr[perc99], sec/arr[perc99], 
        arr[perc100],sec/arr[perc100]))
      stat.data = []
      stat.start = finish
      
    self.count += 1

    if self.count_max!=0 and self.count >= self.count_max:
      print("Done")
      sys.exit()

    if self.rate_max != 0:
      while True:
        now = datetime.datetime.now()
        delta = now - self.start
        microseconds = delta.seconds*1000000 + delta.microseconds
        if microseconds < self.ts_min:
          time.sleep( (self.ts_min-microseconds)/(1000000.0*50.0))
        else:
          break
    self.start = datetime.datetime.now()
    #self.mutex.release()
      

    
  
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

'''
  'addr'     : '0.0.0.0',
  'port'     : '12345',
  'file'     : './testing.json',
  'prot'     : 'tcp',
  'sequence' : 'all',
  'pconn'    : True,
  'trace'    : False
  'type'     : 'probe'
'''

def do_thread(stat):
  cli = client( options['addr'], options['port'], options['pconn'])
  jrp = jsonrpc( cli, stat = stat, trace=options['trace'] )
  orig = options["config"]
  mods = options["modules"]
  start = datetime.datetime.now()
  itr = 0
  while True:
    conf = config.next_config(orig, mods, itr, start)
    sequence = conf["sequences"][options["sequence"]]
    itr += 1
    i = 0
    while True:
      query = config.next_request(sequence, i, mods)
      if query == None:
        break
      i+=1
      # сделать notify и проверку результатов
      result = jrp.request(query['method'], query['params'])
    

def do_test():  
  stat = bench()
  if options["threads"] == 0:
    do_thread(stat)
    return
  
  threads = []
  count = options["threads"]
  
  for t in xrange(count):
    thread = Thread(target = do_thread, args = (stat,))
    threads.append( thread )

  for t in xrange(count):
    #mutex.acquire()
    #print "start", t
    #mutex.release()
    threads[t].start()

  for t in xrange(count):
    threads[t].join()

  


def do_probe():
  print("Текуще настройки:")
  for k, v in options.iteritems():
    if not k in ['modules', 'config', 'file']:
      print("\t{0:10}{1}".format(k,v) )
  print("\nОригинальный файл:")
  config.show_config(options["config"])
  print("\nДля первой итерации:")
  conf = config.next_config(options["config"], options["modules"], 0, datetime.datetime.now())
  config.show_config(conf)
  print("\nТестовый запрос для первой итерации:")
  
  cli = client( options['addr'], options['port'], pconn=False)
  jrp = jsonrpc( cli, stat = bench(), trace=True )
  query = config.next_request(conf["sequences"][options["sequence"]], 0, options["modules"])
  result = jrp.request(query['method'], query['params'])

def main():
  conf = config.load_config(options['file'])

  if options['list']:
    print("Список доступных последовательностей:")
    for k, v in conf['sequences'].iteritems():
      print(u"\t{0} - {1}".format(k, v[-1]))
    sys.exit(0)
    
  eval_modules = config.import_modules(conf)
  options['modules'] = eval_modules
  options['config'] = conf
  test_type = options['type']
  
  if test_type == "probe":
    do_probe()
  elif test_type == "test":
    do_test()
  
  pass


if __name__ == '__main__':
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  parser.add_argument('-a', '--addr',  help="server address", default=options['addr'])
  parser.add_argument('-p', '--port',  help="server port", type=int, default=options['port'])
  parser.add_argument('-u', '--prot',  help="протокол", default=options['prot']) # TODO: сделать просто udp
  parser.add_argument('-r', '--rate',  help="скорость", type=int, default=options['rate'])
  parser.add_argument('-t', '--threads', type=int, help="число потоков", default=options['threads'])
  parser.add_argument('-c', '--count', type=int,help="количество повторений на каждый поток", default=options['count'])
  parser.add_argument('-P', '--pconn', help="persistent connect", action='store_true')
  parser.add_argument('-C', '--check', help="check results for requests", action='store_true')
  parser.add_argument('-T', '--trace', help="trace", action='store_true')
  parser.add_argument('-f', '--file',  help="config file", type=argparse.FileType('r'), default=options['file'])
  parser.add_argument('-l', '--list',  help="отобразить список последовательностей и выйти", action='store_true')
  parser.add_argument('-s', '--sequence', help="бла-бла", default=options['sequence'])
  parser.add_argument("type", nargs='?', help="бла-бла", choices=["probe", "test"], default=options['type'])

  args = parser.parse_args()
  
  options['addr']     = args.addr
  options['port']     = args.port
  options['prot']     = args.prot
  options['pconn']    = args.pconn
  options['trace']    = args.trace
  options['rate']    = args.rate
  options['file']     = args.file
  options['sequence'] = args.sequence
  options['type']     = args.type
  options['list']   = args.list
  options['check']     = args.check
  options['count']     = args.count
  options['threads']     = args.threads
  
  main()
  sys.exit(0)
  
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
  jrp = jsonrpc( client(args.addr, args.port, pconn=False), stat = bench(), trace=False )
  jrp.cli.connect()
  for i in range(1,1000000):
    result = jrp.request(conf['sequences']['seq1'][0][0], conf['sequences']['seq1'][0][1])
    if not result['status']:
      raise Exception("Fuck")
