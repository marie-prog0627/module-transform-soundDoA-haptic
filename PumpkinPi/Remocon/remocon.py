#! /usr/bin/python
# for Pumpkin Pi
# (C)Copyright 2016 All rights reserved by Y.Onodera
# http://einstlab.web.fc2.com


from __future__ import print_function
import os
import sys
import socket
from contextlib import closing

def main():
  host = '127.0.0.1'
  port = 10500
  bufsize = 4096
  b = ""
  index = -1

  sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
  with closing(sock):
    sock.connect((host, port))
    while True:
#      line = sys.stdin.readline().rstrip()
#      if len(line) == 0:
#        break

      a = sock.recv(bufsize)
      if "<RECOGOUT>" in a:
        b = ""
      b = b + a
      if "</RECOGOUT>" in a:
# for debug
        print(b)
        index = b.find("CM=",110)
        score = float(b[index+4:index+9])
        print(score)
# check score
        if score > 0.9:
          if "STOP" in b:
            print("STOP")
#            break
          if "REBOOT" in b:
            os.system("./REBOOT.sh")
          if "POWEROFF" in b:
            os.system("./POWEROFF.sh")
          if "TVon" in b:
            os.system("./TVon.sh")
          if "TVoff" in b:
            os.system("./TVoff.sh")
          if "AIRCONon" in b:
            os.system("./AIRCONon.sh")
          if "AIRCONoff" in b:
            os.system("./AIRCONoff.sh")
          if "LEDon" in b:
            os.system("./LEDon.sh")
          if "LEDoff" in b:
            os.system("./LEDoff.sh")

  return

if __name__ == '__main__':
  main()


