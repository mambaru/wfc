#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import argparse
import socket


class Client:

  def __init__(self, addr, port, pconn = False, udp=False):
    self.addr = addr
    self.port = port
    self.udp = udp
    self.buff = ""
    self.pconn = pconn
    if self.pconn:
      self.connect()
  
  def connect(self):
    
    if self.udp:
      self.cli = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    else:
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
  

if __name__ == '__main__':
  parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
  
  parser.add_argument('-a', '--addr',  help="Имя или Интернет адрес сервера", default="0.0.0.0")
  parser.add_argument('-p', '--port',  help="Номер порта", type=int, default=12345)
  parser.add_argument('-u', '--udp',   help="Использовать udp-протокол", default=False, action='store_true') 
  parser.add_argument('-c', '--count',   type=int,help="Количество повторений запроса", default=1)
  parser.add_argument('-P', '--pconn', help="Использовать постоянное подключение", action='store_true')
  args = parser.parse_args()

  cli = Client(args.addr, args.port, pconn=args.pconn, udp=args.udp)
  
  while True:
    try:
      req = raw_input()
    except:
      break
    for i in range(args.count):
      cli.send(req)
      result = cli.recv()
      print(result)
  
