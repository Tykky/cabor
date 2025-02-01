import socket
import json

sockets = []
r = 2

for i in range(0, r):
    data = {}
    if (i < r - 1):
        data = {
            "command": "ping",
            "code": "source code text" + str(i)
        }
    elif i == r - 1:
        data = {
            "command": "shutdown",
        }


    json_data = json.dumps(data)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("127.0.0.1", 4120))
    sock.sendall(json_data.encode())
    sock.shutdown(socket.SHUT_WR)
    sockets.append(sock)

for i in range(0, r):
    response = sockets[i].recv(1024)
    if len(response) > 0:
        json_response = json.loads(response)
        print(json_response)
    sockets[i].close()