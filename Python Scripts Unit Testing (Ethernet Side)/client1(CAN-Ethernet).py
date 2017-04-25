# Echo client program
import socket

HOST = '192.168.1.6'     # The remote host
PORT = 1234              # Same Port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
while True:
    x=s.recv(1024)
    print(x)
