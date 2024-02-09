import socket
from pynput.mouse import Button, Controller
from screeninfo import get_monitors

# 마우스 컨트롤러 객체 생성
mouse = Controller()

# HOST와 PORT 설정
HOST = '192.168.0.7'  # 이곳에 서버의 IP 주소를 입력하세요.
PORT = 12345

# 화면 해상도 가져오기
monitor = get_monitors()[0]
width, height = monitor.width, monitor.height

# 소켓 생성
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(1)

print('Waiting for connection...')
conn, addr = s.accept()
print('Connected by', addr)

while True:
    data = conn.recv(1024)
    if not data: break

    # 받은 데이터를 줄바꿈으로 분리
    events = data.decode('utf-8').split('\n')  
    for event in events:
        if event:  # 이벤트 데이터가 비어 있지 않은 경우에만 처리
            event_type, *args = event.split(',')
            x, y = map(float, args[:2])
            # 화면 해상도에 따른 마우스 좌표 변환
            x = x * width
            y = y * height
            mouse.position = (x, y)  # 마우스 위치 설정

            if event_type == 'click':
                button = args[2]
                pressed = args[3] == 'True'
                if button == 'left':
                    button = Button.left
                elif button == 'right':
                    button = Button.right
                elif button == 'middle':
                    button = Button.middle
                if pressed:
                    mouse.press(button)
                else:
                    mouse.release(button)
            elif event_type == 'drag':
                button = args[2]
                if button == 'left':
                    button = Button.left
                elif button == 'right':
                    button = Button.right
                elif button == 'middle':
                    button = Button.middle
                mouse.press(button)
                mouse.move(x, y)
                mouse.release(button)

conn.close()
