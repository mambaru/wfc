#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
sys.path.insert(0, '.')

import signal 
import json
import argparse
import datetime
import time
from threading import Thread, Lock

from options import options
from evalator import evalator
from client import Client
from jsonrpc import jsonrpc
from percentile import PercentileMethods

is_working = 0
request_counter = 0
result_code = 0

def to_json(obj):
  return json.dumps(obj, ensure_ascii=False, sort_keys=True)


def check_result(query, result):
  global result_code
  global is_working
  if query["result"]!=result:
    print("Неверный результат запроса '{0}'".format(query["method"]) )
    print("params: {0}".format( to_json(query["params"]) ))
    print("Полученный result: {0}".format( to_json(result)))
    print("Ожидаемый  result: {0}".format( to_json(query["result"]) ))
    result_code = 1
    is_working = 0
  elif "error" in result:
    print("Ошибка запроса '{0}'".format(query["method"]) )
    print("params: {0}".format( to_json(query["params"]) ))
    print("Полученный error: {0}".format( to_json(result)))
    result_code = 2
    is_working = 0
    
  

def do_thread(args, stat):
  global is_working
  global request_counter
  is_working+=1
  stat.show(timeout=1)
  try:
    rate_max = args.rate
    limit = args.limit
    call_count = rate_max-1
    start = datetime.datetime.now()
    
    cli = Client( args.addr, args.port, args.pconn , args.udp)
    jrp = jsonrpc( cli, stat = stat, trace=args.trace )
    args.file.seek(0)
    v = evalator( args.file, args.sequence, args.count)
    while is_working and (request_counter < limit or limit==0):
      query = v.next()
      if not query:
        break
      if "result" in query:
        result = jrp.request(query['method'], query['params'])
        if args.check :
          check_result(query, result)
      else:
        jrp.notify(query['method'], query['params'])
      call_count+=1
      request_counter+=1
      
      # speed limit
      stat.show(timeout=1)
      if rate_max != 0 and call_count == rate_max:
        call_count=0
        now = datetime.datetime.now()
        delta = now - start
        if delta.seconds == 0:
          microseconds = delta.seconds*1000000 + delta.microseconds
          time.sleep( (1000000.0 - microseconds)/1000000.0)
        start = datetime.datetime.now()
        
  except IOError as e:
    print "I/O error({0}): {1}".format(e.errno, e.strerror)
  except:
    print "Unexpected error:", sys.exc_info()[0]
    raise
  if is_working!=0:
    is_working-=1


def do_test(args):
  global is_working

  if args.threads == 0:
    stat = PercentileMethods(0)
    do_thread(args,stat)
    return
  
  threads = []
  count = args.threads
  
  for t in xrange(count):
    stat = PercentileMethods(t)
    thread = Thread(target = do_thread, args = (args, stat))
    threads.append( thread )

  for t in xrange(count):
    threads[t].start()
  
  while is_working:
    time.sleep(1)

  for t in xrange(count):
    threads[t].join()
    

def do_probe(args):
  print("Текуще настройки:")
  for k, v in options.iteritems():
    if not k in ['modules', 'config', 'file']:
      print("\t{0:10}{1}".format(k,v) )
  f = args.file
  f.seek(0)
  v = evalator( f, args.sequence, 1)
  req = v.next()
  while req!=None:
    print( to_json(req) )
    req = v.next()

def do_list(args):
  print("Список доступных последовательностей:")
  e = evalator(args.file)
  for k, v in e.lists.iteritems():
    if k != "import":
      print(u"\t{0} - {1}".format(k, v[-1]))
  print(u"\tall - объединение всех последовательностей")

def signal_handler(signal, frame):
  global is_working
  print("STOP")
  is_working = 0
  
  
def main(args):
  
  signal.signal(signal.SIGINT, signal_handler)
  test_type = args.type

  if test_type == "list":
    do_list(args)
  elif test_type == "probe":
    do_probe(args)
  elif test_type == "test":
    do_test(args)


if __name__ == '__main__':

  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  
  parser.add_argument('-f', '--file',  help="config file", type=argparse.FileType('r'), default=options['file'])
  parser.add_argument('-s', '--sequence', help="бла-бла", default=options['sequence'])
  parser.add_argument('-a', '--addr',  help="Имя или Интернет адрес сервера", default=options['addr'])
  parser.add_argument('-p', '--port',  help="Номер порта", type=int, default=options['port'])
  parser.add_argument('-P', '--pconn', help="persistent connect", action='store_true')
  parser.add_argument('-u', '--udp',   help="Использовать udp-протокол", default=options['udp'], action='store_true') 
  parser.add_argument('-r', '--rate',  help="Ограничение скорости (запросов в секунду)", type=int, default=options['rate'])
  parser.add_argument('-c', '--count', help="количество повторений на каждый поток", type=int, default=options['count'])
  parser.add_argument('-l', '--limit', help="Ограничение на общее количество запросов", type=int,  default=options['limit'])
  parser.add_argument('-t', '--threads', help="число потоков", type=int,  default=options['threads'])

  parser.add_argument('-C', '--check', help="check results for requests", action='store_true')
  parser.add_argument('-T', '--trace', help="trace", action='store_true')
  parser.add_argument('mode',   help="Режим работы (см. документацию)", nargs='?', 
                      choices=["list", "probe", "ping", "test", "init", "bench", "stress"], 
                      default=options['mode'])

  args = parser.parse_args()
  
  options['addr']     = args.addr
  options['port']     = args.port
  options['udp']      = args.udp
  options['pconn']    = args.pconn
  options['trace']    = args.trace
  options['rate']     = args.rate
  options['file']     = args.file.name
  options['sequence'] = args.sequence
  options['type']     = args.type
  options['limit']    = args.limit
  options['check']    = args.check
  options['count']    = args.count
  options['threads']  = args.threads
  
  main(args)
  sys.exit(result_code)
