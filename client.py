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

s.send("STORE 1.txt 25849\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

s.send("STORE 22adf.txt 99999\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

#s.send("READ abc.txt 25000 843\nabcdefghijgklmnopqrstvwxyz")
#data = s.recv(1024)
#print "recv:", data

s.send("READ abc.txt 25000 842\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

s.send("DIR\n")
data = s.recv(1024)
print "recv:", data

#s.send("DELETE abc.txt\n")
s.close()

