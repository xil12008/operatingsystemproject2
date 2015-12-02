#!/usr/bin/env python 

""" 
An echo server that uses threads to handle multiple clients at a time. 
Entering any line of input at the terminal will exit the server. 
""" 

import select 
import socket 
import sys 
import threading 
from handle import handle, printthreadid

class Server: 
    def __init__(self): 
        self.host = '' 
        self.port = 8765 
        self.backlog = 5 
        self.size = 1024 
        self.server = None 
        self.threads = [] 

    def open_socket(self): 
        try: 
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
            self.server.bind((self.host,self.port)) 
            self.server.listen(5) 
        except socket.error, (value,message): 
            if self.server: 
                self.server.close() 
            print "Could not open socket: " + message 
            sys.exit(1) 

    def run(self): 
        self.open_socket() 
        input = [self.server,sys.stdin] 
        running = 1 
        while running: 
            inputready,outputready,exceptready = select.select(input,[],[]) 

            for s in inputready: 

                if s == self.server: 
                    # handle the server socket 
                    c = Client(self.server.accept()) 
                    c.setDaemon(True)
                    c.start() 
                    self.threads.append(c) 

        # close all threads 
        self.server.close() 
        for c in self.threads: 
            c.join() 

class Client(threading.Thread): 
    def __init__(self,(client,address)): 
        threading.Thread.__init__(self) 
        self.client = client 
        self.address = address 
        self.size = 1024 

    def run(self): 
        running = 1 
        while running: 
            data = self.client.recv(self.size) 
            if data: 
                #self.client.send(data) 
                handle(data, self.client)
            else: 
                self.client.close() 
                printthreadid()
                print "Client closed its socket....terminating"
                running = 0 

if __name__ == "__main__": 
    print "Block size is 4096"
    print "Number of blocks is 128"
    print "Listening on port 8765"
    s = Server() 
    s.run()
