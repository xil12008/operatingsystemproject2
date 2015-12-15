import threading
import pdb
import sys
import select
import socket
import time

TCP_IP = ''
TCP_PORT = 8765

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

s.send("STORE def.txt 79112\nabcdefghijklmnopqrstuvwxyz")
data = s.recv(1024)
print "recv:", data

s.close()
