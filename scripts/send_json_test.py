import socket
import json

data = {
    "command": "compile",
    "code": "source code text source code text source code text source code text source code text source code text source code text source code text source code text source code text source code text source code text "
}

for i in range(1, 100):
    data["code " + str(i)] = "source code text " + str(i)

json_data = json.dumps(data)
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(("127.0.0.1", 4120))
sock.sendall(json_data.encode())
sock.shutdown(socket.SHUT_WR)

response = sock.recv(1024)
#json_response = json.loads(response)
#print(json_response)
sock.close()