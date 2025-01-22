import socket
import json

sockets = []
r = 1000

for i in range(0, r):
    data = {
        "command": "compile",
        "code": "source code text" + str(i)
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