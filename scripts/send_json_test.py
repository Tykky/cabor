import socket
import json

sockets = []
r = 1


data = {
    "command": "compile",
    "code": "true"
}

json_data = json.dumps(data)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("127.0.0.1", 3000))
sock.sendall(json_data.encode())
sock.shutdown(socket.SHUT_WR)
sockets.append(sock)

response = sockets[0].recv(100 * 1024 * 1024)
if len(response) > 0:
    json_response = json.loads(response)
    print(json_response)
sockets[0].close()
