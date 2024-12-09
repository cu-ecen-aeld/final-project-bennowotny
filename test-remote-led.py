#!/usr/bin/env python3

import socket
import time
from math import sin, sqrt, pi
import sys

def red_func(t: float):
  return int(127*sin(3*t)+128)

def green_func(t: float):
  return int(127*sin(sqrt(2)*5*t)+128)

def blue_func(t: float):
  return int(127*sin(7*pi*t)+128)

if __name__ == '__main__':
  if len(sys.argv) < 2:
    print('ERR: need to supply an IP address for the target')
    exit()

  t = 0
  for _ in range(255):
    s = socket.socket()
    s.connect((sys.argv[1], 18658))
    s.send(bytes([red_func(t),green_func(t),blue_func(t)]))
    s.close()
    time.sleep(0.1)
    t += 0.1
