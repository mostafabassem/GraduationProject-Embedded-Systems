# Echo client program
import socket

HOST = '192.168.1.6'     # The remote host
PORT = 9761              # Same Port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
while True:
    x=input('Enter anything:\n')
    s.sendall(x.encode()) ##call me maybe
    data = s.recv(1024)
    print('test2',repr(data))
