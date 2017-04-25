# Echo client program
import socket

HOST = '192.168.1.6'     # The remote host
PORT = 9762              # Same Port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#s.connect((HOST, PORT))
while True:
    x=input('give me shit:\n')
    s.sendto(x.encode(),(HOST,PORT)) ##call me maybe
    data = s.recv(1024)
    print('test2',repr(data))
