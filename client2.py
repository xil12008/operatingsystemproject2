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

#s.send("STORE abc.txt 524288\nabcdefghijgklmnopqrstvwxyz")
#data = s.recv(1024)
#print "recv:", data

#s.send("STORE abc.txt 524288\nabcdefghijgklmnopqrstvwxyz")
#data = s.recv(1024)
#print "recv:", data
#
#s.send("READ abc.txt 23 4\n")
#data = s.recv(1024)
#print "recv:", data
#
#s.send("DELETE abc.txt\n")
#data = s.recv(1024)
#print "recv:", data

s.send("DIR\n")
data = s.recv(1024)
print "recv:", data

s.send("STORE linhui1.txt " + str( 2*4096 + 1) + "\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

s.send("STORE linhui2.txt " + str( 4*4096 - 1) + "\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

s.send("STORE linhui3.txt " + str( 2*4096 + 2000) + "\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

s.send("DIR\n")
data = s.recv(1024)
print "recv:", data

s.send("DELETE linhui2.txt\n")
data = s.recv(1024)
print "recv:", data

s.send("STORE H.txt " + str( 10*4096 + 1) + "\nabcdefghijgklmnopqrstvwxyz")
data = s.recv(1024)
print "recv:", data

while (1):
    s.send("DIR\n")
    data = s.recv(1024)
    print "recv:", data
    time.sleep(4)

#s.close()
