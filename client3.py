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

s.send("STORE abc.txt 25842\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

s.send("READ xyz.jpg 5555 2000\n")
data = s.recv(1024)
print "recv:", data

s.close()


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

s.send("STORE def.txt 79112\nabcdefghijklmnopqrstuvwxyz")
data = s.recv(1024)
print "recv:", data

s.close()


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

s.send("READ abc.txt 4090 5000\n")
data = s.recv(1024)
print "recv:", data

s.send("DELETE abc.txt\n")
data = s.recv(1024)
print "recv:", data

s.close()


#s.send("DIR\n")
#data = s.recv(1024)
#print "recv:", data
