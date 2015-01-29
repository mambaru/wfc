#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import datetime
import time

sys.path.insert(0, '../../jitest')

from evalator import evalator

def check_exit(N, left, right):
  if left!=right:
    print( "{0}!={1}".format(left,right) )
    print( "{0} ws {1}".format( type(left), type(right) ) )
    print( "Test {0} Fail".format(N) )
    sys.exit(1)
    
def test_once(N, v, right):
  tst = v.next()
  check_exit(1, tst, right)
  tst = v.next()
  check_exit(1, tst, None)

# ==========================================  
# ==========================================  
# ==========================================  

def test1(v):
  v.reset('test-01', 1)
  test_once(1, v, "01")

def test2(v):
  v.reset('test-02', 1)
  test_once(1, v, 2)

def test3(v):
  v.reset('test-03', 1)
  test_once(1, v, [3])

def test4(v):
  v.reset('test-04', 1)
  test_once(1, v, {"4":4})

def test5(v):
  v.reset('test-05', 1)
  for i in ["01", 2, [3], {"4":4}]:
    tst = v.next()
    check_exit(1, tst, i)
  tst = v.next()
  check_exit(1, tst, None)
  
# ==========================================  

def test6(v):
  v.reset('test-06', 2)
  N = 0 
  P = 0
  I = 0
  C1 = 0
  C2 = 0
  # два прохода
  for i in range(2):
    # каждый элемент
    for j in range(2):
      # повторы
      for g in range(2):
        # "$N-$P-$C-$C1-$C2"
        chk = "{0}-{1}-{2}-{3}-{4}".format(N,P,I,C1,C2)
        tst = v.next()
        check_exit(6, tst, chk)
        N+=1
        C2+=1
        C1+=1
      P+=1
      C2=0
    I+=1
    P=0
    C1=0
    
def test7(v):
  v.reset('test-07', 2)
  now = datetime.datetime.now()
  unix_now = int(time.mktime(now.timetuple()))
  tst = v.next()
  check_exit(7, tst["startstamp"], unix_now)
  check_exit(7, tst["timestamp"], unix_now)
  check_exit(7, tst["timespan"], 0)
  time.sleep(1)
  tst = v.next()
  check_exit(7, tst["startstamp"], unix_now)
  check_exit(7, tst["timestamp"], unix_now + 1)
  check_exit(7, tst["timespan"], 1)


    

if __name__ == '__main__':
  f = open('./test_evalator.json', 'r')
  v = evalator(f)
  check_exit(0,"test","test")
  test1(v)
  test2(v) 
  test3(v)
  test4(v)
  test5(v)
  test6(v)
  test7(v)
  print("Done")