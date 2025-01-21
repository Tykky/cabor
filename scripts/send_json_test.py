import socket
import json

data = {
    "command": "ping",
    "code": "source code text"
}

json_data = json.dumps(data)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("127.0.0.1", 4120))
sock.sendall(json_data.encode())
sock.shutdown(socket.SHUT_WR)

response = sock.recv(1024)
if len(response) > 0:
    json_response = json.loads(response)
    print(json_response)
sock.close()