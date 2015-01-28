#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import signal 

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


class client:

  def __init__(self, addr, port, pconn = False, udp=False):
    self.addr = addr
    self.port = port
    self.udp = False
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
    return result
    
  def recv(self):
    result = self.parse()
    while result==None:
      res = self.cli.recv(4096)
      if len(res) <= 0:
        break
      self.buff += res
      result = self.parse()
    if not self.pconn:
      self.close()
    return result
  

class bench:
  
  def __init__(self, id, threads = 1):
    self.id = id
    self.threads = threads
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
    
    if self.id==0:
      print("µ - microsecond")
      print("id: method(rate)\t0% µs(persec)\t50% µs(persec)\t80% µs(persec)\t95% µs(persec)\t99% µs(persec)\t100% µs(persec)")
  
  def is_finished(self):
    return self.count_max!=0 and self.count >= self.count_max
  
  def add(self, method, microseconds):
    # --------------------------------
    if not method in self.methods:
      class Object: pass
      stat = Object()
      #print (self.id, self.threads)
      stat.data = [0 for x in xrange( (self.arr_size/self.threads) * self.id ) ]
      stat.start = datetime.datetime.now()
      self.methods[method] = stat
    stat = self.methods[method]
    stat.data += [microseconds]
    
    # --------------------------------
    if len(stat.data) == self.arr_size:
      finish = datetime.datetime.now()
      delta = finish - stat.start
      microseconds = delta.seconds*1000000 + delta.microseconds
      rate = self.arr_size*1000000/microseconds
      stat.data.sort()
      arr = stat.data
      if arr[0]!=0:
        sec = 1000000
        perc50 = self.arr_size*50/100
        perc80 = self.arr_size*80/100
        perc90 = self.arr_size*90/100
        perc95 = self.arr_size*95/100
        perc99 = self.arr_size*99/100
        perc100 = self.arr_size-1
        print(str(self.id)+": {0}({1})\t{2}({3})\t{4}({5})\t{6}({7})\t{8}({9})\t{10}({11})\t{12}({13})".format(
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

    #if self.count_max!=0 and self.count >= self.count_max:
    #  print("Done")
    #  sys.exit()

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
    if res_str == None:
      return None
    
    finish = datetime.datetime.now()
    #self.microseconds = time.mktime((finish - start).timetuple()) 
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

is_working = 0


def do_thread(stat):
  global is_working
  is_working+=1
  try:
    cli = client( options['addr'], options['port'], options['pconn'])
    jrp = jsonrpc( cli, stat = stat, trace=options['trace'] )
    orig = options["config"]
    mods = options["modules"]
    start = datetime.datetime.now()
    itr = 0
    while is_working:
      conf = config.next_config(orig, mods, itr, start)
      sequence = conf["sequences"][options["sequence"]]
      itr += 1
      i = 0
      while is_working:
        query = config.next_request(sequence, i, mods)
        if query == None or not is_working:
          break
        i+=1
        # сделать notify и проверку результатов
        result = jrp.request(query['method'], query['params'])
        if stat.is_finished():
          if is_working!=0:
            is_working-=1
          return
  except IOError as e:
    print "I/O error({0}): {1}".format(e.errno, e.strerror)
  except ValueError:
    print "Could not convert data to an integer."
  except:
    print "Unexpected error:", sys.exc_info()[0]
    raise
  if is_working!=0:
    is_working-=1
    

def do_test():
  global is_working

  if options["threads"] == 0:
    stat = bench(0,1)
    do_thread(stat)
    return
  
  threads = []
  count = options["threads"]
  
  for t in xrange(count):
    stat = bench(t, count)
    thread = Thread(target = do_thread, args = (stat,))
    threads.append( thread )

  for t in xrange(count):
    threads[t].start()
  
  while is_working:
    time.sleep(1)

  for t in xrange(count):
    threads[t].join()
    


def do_probe():
  print("Текуще настройки:")
  for k, v in options.iteritems():
    if not k in ['modules', 'config', 'file']:
      print("\t{0:10}{1}".format(k,v) )
  print("\nОригинальный файл:")
  config.show_json(options["config"])
  print("\nДля первой итерации:")
  conf = config.next_config(options["config"], options["modules"], 0, datetime.datetime.now())
  config.show_json(conf)
  print("\nТестовый запрос для первой итерации:")
  
  cli = client( options['addr'], options['port'], pconn=False)
  jrp = jsonrpc( cli, stat = bench(), trace=True )
  query = config.next_request(conf["sequences"][options["sequence"]], 0, options["modules"])
  result = jrp.request(query['method'], query['params'])

def signal_handler(signal, frame):
  global is_working
  print("STOP")
  is_working = 0
  
def main():

  signal.signal(signal.SIGINT, signal_handler)
  
  conf = config.load_queries(options['file'])

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


if __name__ == '__main__':
  
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  
  parser.add_argument('-a', '--addr',  help="Имя или Интернет адрес сервера", default=options['addr'])
  parser.add_argument('-p', '--port',  help="Номер порта", type=int, default=options['port'])
  parser.add_argument('-u', '--udp',   help="Использовать udp-протокол", default=options['udp'], action='store_true') 
  parser.add_argument('-r', '--rate',  help="Количество запросов в секунду", type=int, default=options['rate'])
  parser.add_argument('-t', '--threads', type=int, help="число потоков", default=options['threads'])
  parser.add_argument('-c', '--count',   type=int,help="количество повторений на каждый поток", default=options['count'])
  parser.add_argument('-P', '--pconn', help="persistent connect", action='store_true')
  parser.add_argument('-C', '--check', help="check results for requests", action='store_true')
  parser.add_argument('-T', '--trace', help="trace", action='store_true')
  parser.add_argument('-f', '--file',  help="config file", type=argparse.FileType('r'), default=options['file'])
  parser.add_argument('-l', '--list',  help="отобразить список последовательностей и выйти", action='store_true')
  parser.add_argument('-s', '--sequence', help="бла-бла", default=options['sequence'])
  parser.add_argument("type", nargs='?',  help="бла-бла", choices=["probe", "test"], default=options['type'])

  args = parser.parse_args()
  
  options['addr']     = args.addr
  options['port']     = args.port
  options['udp']      = args.udp
  options['pconn']    = args.pconn
  options['trace']    = args.trace
  options['rate']     = args.rate
  options['file']     = args.file
  options['sequence'] = args.sequence
  options['type']     = args.type
  options['list']     = args.list
  options['check']    = args.check
  options['count']    = args.count
  options['threads']  = args.threads
  
  
  
  main()
  sys.exit(0)
  
  print(args.file)
  conf = config.load_queries(args.file)
  config.show_json(conf)
  
  print("---------------------")
  eval_modules = config.import_modules(conf)
  conf = config.next_config(conf, eval_modules, 33)
  config.show_json(conf)
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
