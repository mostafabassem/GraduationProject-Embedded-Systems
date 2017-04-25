#Echo server program
import socket

HOST = ''                 # Symbolic name meaning all available interfaces
PORT = 1234              # Arbitrary non-privileged port
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((HOST, PORT))
while True:
    data = s.recv(1024)
    if not data: break
    print(data)
conn.close()
