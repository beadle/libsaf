#-*- coding: utf-8 -*-
from socket import *

class TcpClient:
    HOST='127.0.0.1'
    PORT=5000
    BUFSIZ=1024
    ADDR=(HOST, PORT)
    def __init__(self):
        self.client=socket(AF_INET, SOCK_DGRAM)
        self.client.connect(self.ADDR)

      	while True:
            pass

            
if __name__ == '__main__':
    client=TcpClient()
