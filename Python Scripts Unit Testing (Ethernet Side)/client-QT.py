# Echo client program
import socket

HOST = '192.168.1.2'     # The remote host
PORT = 1234              # Same Port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
while True:
    x=input('give me shit:\n')
    s.sendall(x.encode()) ##call me maybe

