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
from client import client
from jsonrpc import jsonrpc
from percentile import PercentileMethods

is_working = 0
request_counter = 0

def do_thread(args, stat):
  global is_working
  global request_counter
  is_working+=1
  stat.show(timeout=1)
  try:
    rate_max = options["rate"]
    limit = options["limit"]
    call_count = rate_max-1
    start = datetime.datetime.now()
    
    cli = client( options['addr'], options['port'], options['pconn'] , options['udp'])
    jrp = jsonrpc( cli, stat = stat, trace=options['trace'] )
    options['file'].seek(0)
    v = evalator( options['file'], options['sequence'], options['count'])
    while is_working and (request_counter < limit or limit==0):
      query = v.next()
      if not query:
        break
      if "result" in query:
        result = jrp.request(query['method'], query['params'])
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

  if options["threads"] == 0:
    stat = PercentileMethods(0)
    do_thread(args,stat)
    return
  
  threads = []
  count = options["threads"]
  
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
    

def show_json(obj):
  print( json.dumps(obj, ensure_ascii=False, sort_keys=True) )


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
    show_json(req)
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
  parser.add_argument("type", nargs='?',  help="бла-бла", choices=["probe", "test", "list"], default=options['type'])

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
  
  main(args)
  