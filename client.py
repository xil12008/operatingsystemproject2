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

buffer = "0123456789" * (100* 25820/10)
s.send("STORE abcede.txt 2582000\n\r") 
time.sleep(0.1)
print "len", len(buffer)
s.send(buffer)
data = s.recv(1024)
print "recv:", data
    
time.sleep(10)

s.send("READ abc.txt 4090 5000\n")
data = s.recv(6000)
print "recv:", data

s.close()
