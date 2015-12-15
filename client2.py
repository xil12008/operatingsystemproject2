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

#s.send("STORE abc.txt 25842\nabcdefghijgklmnopqrstvwxyz")
s.send("STORE abc.txt 524288\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

s.send("READ abc.txt 23 4\n")
data = s.recv(1024)
print "recv:", data

s.send("DELETE abc.txt\n")
data = s.recv(1024)
print "recv:", data

s.send("STORE linhui.txt 4096\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

s.send("STORE xiaoyan.txt 4096\nabcdef")
data = s.recv(1024)
print "recv:", data

s.send("DELETE linhui.txt\n")
data = s.recv(1024)
print "recv:", data

s.close()
