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
from evalator import Evalator
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
    
    
# ------------------------------------
# ------------------------------------

def send_request(jrp, query, check):
  if "result" in query:
    result = jrp.request(query['method'], query['params'])
    if check:
      check_result( query, result )
  else:
    jrp.notify(query['method'], query['params'])


def work_thread(args, stat):
  global is_working
  global request_counter
  
  is_working+=1
  if stat and stat.id==0:
    stat.show_head()
    stat.show(timeout=1)
  try:
    rate_max = args.rate
    limit = args.limit
    call_count = rate_max-1
    start = datetime.datetime.now()
    cli = Client( args.addr, args.port, args.pconn , args.udp)
    jrp = jsonrpc( cli, stat = stat, trace=args.trace )
    args.file.seek(0)
    v = Evalator( args.file, args.name, args.count)
    while is_working and (request_counter < limit or limit==0):
      query = v.next()
      if not query:
        break
      send_request(jrp, query, args.check)
      call_count+=1
      request_counter+=1
      
      # speed limit
      if stat:
        stat.show(timeout=args.showtime, minitems=args.minshow)
      
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
    raise
  except:
    print "Unexpected error:", sys.exc_info()[0]
    raise
  if is_working!=0:
    is_working-=1

# ------------------------------------

def do_list(args):
  print("Список доступных последовательностей:")
  e = Evalator(args.file)
  for k, v in e.lists.iteritems():
    if k != "import":
      print(u"\t{0} - {1}".format(k, v[-1]))
  print(u"\tall - объединение всех последовательностей")

# ------------------------------------    

def do_probe(args):
  print("Текуще настройки:")
  for k, v in options.iteritems():
    if not k in ['modules', 'config', 'file']:
      print("\t{0:10}{1}".format(k,v) )
  f = args.file
  f.seek(0)
  v = Evalator( f, args.name, 1)
  req = v.next()
  while req!=None:
    print( to_json(req) )
    req = v.next()

  
# ------------------------------------

def do_test(args):
  global is_working
  stat = None
  if args.threads == 0:
    if args.stat != 0:
      stat = PercentileMethods(id=0, limit=args.stat)
    work_thread(args,stat)
    return
  
  threads = []
  count = args.threads
  
  for t in xrange(count):
    if args.stat != 0:
      stat = PercentileMethods(id=t, limit=args.stat)
    thread = Thread(target = work_thread, args = (args, stat))
    threads.append( thread )

  for t in xrange(count):
    threads[t].start()
  
  while is_working:
    time.sleep(1)

  for t in xrange(count):
    threads[t].join()


# ------------------------------------    

def do_ping(args):
  try:
    args.stat = 0
    args.rate = 0
    args.limit = 1
    args.check = False
    args.pconn = False
    args.trace = True
    args.treads = 0
    do_test(args)
    print("Ping OK")
    return
  except Exception as e:
    print("Ping FAIL")
    print(e.message)
  print("Ping FAIL")

# ------------------------------------

def do_init(args):
  args.count = 1
  args.treads = 0
  do_test(args)

# ------------------------------------

def do_bench(args):
  args.count = 0
  args.limit = 0
  args.trace = False
  args.check = False
  do_test(args)

# ------------------------------------

def do_stress(args):
  args.count = 0
  args.limit = 0
  args.stat  = 0
  args.trace = False
  args.check = False
  do_test(args)
  
# ------------------------------------



def signal_handler(signal, frame):
  global is_working
  print("STOP")
  is_working = 0
  
# ------------------------------------

def main(args):
  
  signal.signal(signal.SIGINT, signal_handler)
  test_type = args.mode

  if test_type == "list":
    do_list(args)
  elif test_type == "probe":
    do_probe(args)
  elif test_type == "ping":
    do_ping(args)
  elif test_type == "test":
    do_test(args)
  elif test_type == "init":
    do_init(args)
  elif test_type == "bench":
    do_bench(args)
  elif test_type == "stress":
    do_stress(args)

# ------------------------------------

if __name__ == '__main__':

  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)

  parser.add_argument('mode',   help="Режим работы (см. документацию)", nargs='?', 
                      choices=["list", "probe", "ping", "test", "init", "bench", "stress"], 
                      default=options['mode'])
  
  parser.add_argument('-f', '--file',     help="Файл конфигурации", type=argparse.FileType('r'), default=options['file'])
  parser.add_argument('-n', '--name',     help="Имя последовательности", default=options['name'])
  parser.add_argument('-a', '--addr',     help="Имя или Интернет адрес сервера", default=options['addr'])
  parser.add_argument('-p', '--port',     help="Номер порта", type=int, default=options['port'])
  parser.add_argument('-u', '--udp',      help="Использовать udp-протокол", default=options['udp'], action='store_true') 
  parser.add_argument('-P', '--pconn',    help="Постоянное подключение", action='store_true')

  parser.add_argument('-t', '--threads', help="Число потоков", type=int,  default=options['threads'])
  parser.add_argument('-r', '--rate',    help="Ограничение скорости (запросов в секунду)", type=int, default=options['rate'])
  parser.add_argument('-c', '--count',   help="Количество повторений на каждый поток", type=int, default=options['count'])
  parser.add_argument('-l', '--limit',   help="Ограничение на общее количество запросов", type=int,  default=options['limit'])

  parser.add_argument('-C', '--check',    help="check results for requests", action='store_true')
  parser.add_argument('-T', '--trace',    help="trace", action='store_true')

  parser.add_argument('-S', '--stat',     help="Размер массива статистики. 0 - отключить",    type=int, default=options['stat'])
  parser.add_argument('-W', '--showtime', help="Таймаут отображения статистики", type=float,  default=options['showtime'])
  parser.add_argument('-M', '--minshow',  help="Минимальный набор данных для отображения статистики", type=int,  default=options['minshow'])
  

  args = parser.parse_args()
  
  options['mode']     = args.mode
  options['file']     = args.file.name
  options['name']     = args.name
  options['addr']     = args.addr
  options['port']     = args.port
  options['udp']      = args.udp
  options['pconn']    = args.pconn

  options['threads']  = args.threads
  options['rate']     = args.rate
  options['count']    = args.count
  options['limit']    = args.limit

  options['check']    = args.check
  options['trace']    = args.trace
  
  options['stat']     = args.stat
  options['showtime'] = args.showtime
  options['minshow'] =  args.minshow
  
  if args.stat < args.minshow:
    print("Значение параметра --stat меньше --minshow. Сатистика отображена не будет.")
  
  main(args)
  sys.exit(result_code)
