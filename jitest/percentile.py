#!/usr/bin/python
# -*- coding: utf-8 -*-

import datetime
import time
from threading import Lock

import curses


class Percentile:
  show_mutex = Lock()
  def __init__(self, id, name, limit=1024):
    self.id = id
    self.name = name
    self.limit = limit
    self.start = None
    self.count = 0
    self.data = []
    
  def reset(self, last=None):
    if last!=None:
      self.data=[last]
    self.count = 0
    self.start = datetime.datetime.now()
    
  def add(self, microseconds):
    if self.start == None:
      self.reset()
    if self.count < self.limit:
      self.data += [microseconds]
    self.count+=1
    
  def time(self, perc):
    if len(self.data)==0:
      return 0
    if perc==0:
      return self.data[0]
    if perc==100:
      return self.data[-1]
    pos = ( len(self.data)*perc ) / 100 
    return self.data[pos]
  
  def rate(self, perc):
    time = self.time(perc)
    if time==0 or self.count==0:
      return 0
    rate = 1000000.0 / time
    
    if rate < 100:
      return int(rate*100.0)/100.0
    return int(rate)
  
  def show(self, reset = True, minitems=10):
    
    if minitems!=0 and len(self.data) < minitems :
      return
    last = None
    if len(self.data) > 0:
      last = self.data[-1]
      self.data.sort()
    
    rate=0
    if self.count > 0 :
      finish = datetime.datetime.now()
      delta = finish - self.start
      microseconds = delta.seconds*1000000 + delta.microseconds
      rate = self.count*1000000/microseconds
    
    Percentile.show_mutex.acquire()
    showstr = "{:>2} {:>6} ".format(self.id,self.count)
    namerate = "{:<24}".format( "{0}:{1}".format(self.name, rate) )
    if len( namerate ) > 24:
      namerate=".."+namerate[-22:]
      
    showstr +=  namerate
    for i in [0,50,80,90,95,99,100]:
      cur="{0}:{1}".format(self.time(i), self.rate(i))
      showstr += "{:>14}".format(cur)
    print(showstr)
      
    '''
    print(str(self.id)+": {:>2}|{:>7},{:>7}|{:>7}({:>7})|{:>7}({:>7})|{:>7}({:>7})|{:>7}({:>7})|{:>7}({:>7})|{:>7}({:>7})".format(
      self.name, rate, self.count,
      self.time(0), self.rate(0), 
      self.time(50), self.rate(50), 
      self.time(80), self.rate(80), 
      self.time(95), self.rate(95), 
      self.time(99), self.rate(99), 
      self.time(100), self.rate(100))
    )
    '''
    Percentile.show_mutex.release()

    if reset:
      self.reset(last)
  
  @staticmethod
  def show_head(limit):
    print("µ - microsecond. array limit={0}".format(limit) )
    print("id  count method:rate                0% µs:rate   50% µs:rate   80% µs:rate   90% µs:rate   95% µs:rate   99% µs:rate  100% µs:rate")


class PercentileMethods:
  
  def __init__(self, id, limit=1024):
    self.methods = {}
    self.id = id
    self.limit = limit
    self.showtime = None
    #self.methods["[total]"]=Percentile(self.id, "[total]", self.limit)
    
  def add(self, method, microseconds):  
    if not method in self.methods:
      self.methods[method]=Percentile(self.id, method, self.limit)
    self.methods[method].add(microseconds)
    #self.methods["[total]"].add(microseconds)
    
  def show(self, timeout=0, minitems=10, reset=True):
    if timeout!=0:
      if self.showtime == None:
        self.showtime = datetime.datetime.now()
      else:
        now = datetime.datetime.now()
        delta = now - self.showtime 
        #if delta.seconds < timeout:
        if (delta.seconds + delta.microseconds/1000000.0) < timeout:
          return
        self.showtime = now
    for m in self.methods:
      self.methods[m].show(reset=reset, minitems=minitems)
      
  def show_head(self):
    Percentile.show_head(self.limit)

if __name__ == '__main__':
  import random
  p = PercentileMethods(0)
  p.show_head()
  for i in range(20):
    p.add("test", random.randint(1000, 10000))
  p.show()
  