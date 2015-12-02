import threading
import pdb
import sys
import select
import os
import socket
import time
import re
import array
from memory import Memory


mem = Memory()
letter = 'A'
file2letter = {}

def store(filename, bytes, contents, s):
    printthreadid()
    print "Rcvd: ADD", filename, bytes 
    global letter
    filename = (str)(filename)
    if filename in file2letter.keys():
        s.send("ERROR: FILE EXISTS\n")  
        printthreadid()
        print "Sent: ERROR: ERROR: FILE EXISTS"
        return
    else:
        printthreadid()
        if not mem.allocate( letter, bytes): 
            s.send("ERROR: INSUFFICIENT DISK SPACE\n")
            print "Sent: ERROR: INSUFFICIENT DISK SPACE"
            return

        if not os.path.exists(".storage"):
            os.makedirs(".storage")

        f = open('.storage/' + filename, 'wb+')
        f.write(contents)

        file2letter[filename] = {"letter":letter, "length": int(bytes)} 
        letter = chr(ord(letter) + 1)
        printthreadid()
        print "Simulated Clustered Disk Space Allocation:"
        mem.printmem()
        s.send("ACK\n")
        printthreadid()
        print "Sent: ACK"

def printthreadid():
    print "[thread %d]" % threading.current_thread().ident,

def read(filename, offset, length, s):
    printthreadid()
    print "Rcvd: READ", filename, offset, length 
    offset = int(offset)
    length = int(length)
    if not filename in file2letter.keys():
        s.send("ERROR: NO SUCH FILE\n")  
        printthreadid()
        print "Sent: ERROR: NO SUCH FILE" 
        return
    if offset + length > file2letter[filename]["length"]:
        s.send("ERROR: INVALID BYTE RANGE\n")
        printthreadid()
        print "Sent: ERROR: INVALID BYTE RANGE"
        return

    with open(".storage/" +filename, "rb") as f:
        bytecount = 0
        retlist = []
        byte = f.read(1)
        while byte != "":
            if bytecount >= offset and bytecount < offset + length:
                retlist.append(byte)
            bytecount += 1 
            byte = f.read(1)
    s.send("ACK "+ str(length) + "\n" + ''.join(retlist) )
    printthreadid()
    print "Sent %d bytes (from ** '%c' blocks) from offset %d" %( int(length), file2letter[filename]["letter"], int(offset) ) 

def delete(filename, s):
    printthreadid()
    print "Rcvd: Rcvd: DELETE", filename
    if not filename in file2letter.keys():
        s.send("ERROR: NO SUCH FILE\n")  
        printthreadid()
        print "Sent: ERROR: NO SUCH FILE" 
        return
    printthreadid()
    mem.deallocate(file2letter[filename]["letter"])
    s.send("ACK\n")
    printthreadid()
    print "Sent: ACK"
    return

def dir(s):
    printthreadid()
    print "Rcvd: Rcvd: DIR" 
    count = 0
    mystr = str(len(file2letter.items())) + "\n"
    for key, value in sorted(file2letter.items()): # Note the () after items!
        mystr += key + "\n"
    s.send(mystr)

def handle(input, s):
    inputs = input.split() 
    inputs.append(s)
    if inputs[0] == "STORE":
        filename = inputs[1]
        bytes = inputs[2]
        content = inputs[3]
        store(filename, bytes, content, s)
    elif inputs[0] == "READ":
        filename = inputs[1]
        offset = inputs[2]
        length = inputs[3]
        read(filename, offset, length, s)
    elif inputs[0] == "DELETE":
        filename = inputs[1]
        delete(filename, s)
    elif inputs[0] == "DIR":
        dir(s)
    else:
        s.send("Unknown command\n")

