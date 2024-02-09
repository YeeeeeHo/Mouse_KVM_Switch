import socket

# 호스트와 포트를 정의합니다. 여기서 'localhost'는 같은 컴퓨터를 의미합니다.
HOST = 'localhost'  
PORT = 12345  

# 소켓 객체를 생성합니다.
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 서버에 연결합니다.
s.connect((HOST, PORT))

# 서버에 메시지를 전송합니다.
s.sendall(b'Hello, server')

# 응답을 수신합니다.
data = s.recv(1024)

# 수신한 데이터를 출력합니다.
print('Received', repr(data))

# 연결을 종료합니다.
s.close()
