import heapq
import threading
import operator
from collections import deque
import pdb
import sys
import Queue as Q
import logging
logging.basicConfig(stream=sys.stderr, level=logging.CRITICAL)

class Memory():
    def __init__(self):
        self.n_blocks = 128
        self.cap = self.n_blocks
        self.blocksize = 4096 #in bytes
        self.mem = ['.' for _ in range(self.cap)]

    def printmem(self):
        print "=" * 32
        for i in range(self.cap/32):
            for j in range(32):
                sys.stdout.write(self.mem[i*32+j])
            sys.stdout.write("\n")
        print "=" * 32
 
    def deallocate(self, filename, ID):
        count = 0
        for i in range(self.cap):
            if self.mem[i] == ID: 
                self.mem[i] = '.'
                count += 1
        print "Deleted", filename, "file '%c' (deallocated %d blocks)" %( ID, count)
    
    def allocate(self, ID, size):
        size = (int)(size)
        blocks = (int)( ( size + self.blocksize - 1) / self.blocksize )
        remainblock = 0
        for i in range( 0, self.cap): 
            if self.mem[i] == '.':
                remainblock += 1
        if remainblock < blocks:
            return False
        else:
            clusters = 1 
            bb = blocks
            for i in range( 0, self.cap): 
                if self.mem[i] == '.' and blocks > 0:
                    self.mem[i] = ID
                    blocks -= 1
                if i != self.cap - 1 :
                    if blocks > 0 and self.mem[i] == ID and self.mem[i+1] != '.':
                        clusters += 1
            if clusters == 1:
                tmpstr = "cluster"
            else:
                tmpstr = "clusters"
            print "Stored file '%c' (%d bytes; %d blocks; %d %s)" %(ID, size, bb, clusters, tmpstr) 
            return True


def test():
    mem = Memory()
    mem.allocate("A", 192 * 4096)
    mem.printmem()
    mem.allocate("B", 32 * 4096)
    mem.allocate("C", 32 * 4096)
    mem.deallocate("A")
    print mem.allocate("D", 32 * 4096)
    mem.printmem()

#test()
