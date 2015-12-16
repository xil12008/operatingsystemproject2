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

s.send("STORE abcede.txt 25820\n\r") 
time.sleep(0.1)
buffer = "-" * 25820
s.send(buffer)
data = s.recv(1024)
print "recv:", data

s.close()
