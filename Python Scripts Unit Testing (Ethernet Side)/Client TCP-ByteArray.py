# Echo client program
import socket

HOST = '192.168.1.6'     # The remote host
PORT = 9761              # Same Port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
#x=input('give me shit:\n')
my_bytes = bytearray()
my_bytes.append(2)
my_bytes.append(57)
my_bytes.append(0)
my_bytes.append(40)
my_bytes.append(0)
my_bytes.append(138)
my_bytes.append(10)
my_bytes.append(15)
s.send(my_bytes)
#s.sendall(x.encode()) ##call me maybe

