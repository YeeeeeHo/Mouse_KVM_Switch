#include <winsock2.h> #
#include <windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#pragma comment(lib, "ws2_32.lib")

// 문자열을 구분자로 분리하여 벡터로 반환하는 함수
std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    char buffer[1024];
    int clientAddrSize = sizeof(clientAddr);

    // Winsock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed." << std::endl;
        return -1;
    }

    // 서버 소켓 생성
    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create server socket." << std::endl;
        return -1;
    }

    // 서버 주소 설정
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(12345);

    // 서버 소켓에 주소 할당
    if (bind(serverSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind server socket." << std::endl;
        return -1;
    }

    // 클라이언트 연결 대기
    if (listen(serverSocket, 5) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen." << std::endl;
        return -1;
    }

    // 클라이언트 연결 수락
    clientSocket = accept(serverSocket, (SOCKADDR *)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to accept client." << std::endl;
        return -1;
    }

    // 클라이언트로부터 데이터 수신
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int len = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0)
            break;

        // 받은 데이터 처리
        std::string data(buffer);
        std::vector<std::string> events = split(data, '\n');
        for (const auto &event : events)
        {
            std::vector<std::string> args = split(event, ',');
            if (args.size() < 2)
                continue; // argument가 2개 미만인 경우 무시

            std::string event_type = args[0];

            if (event_type == "keypress") // 키보드 이벤트인 경우
            {
                BYTE key = std::stoi(args[1]);      // 키 코드
                bool pressed = (args[2] == "True"); // 키 상태

                // 키 이벤트 생성
                keybd_event(key, 0, pressed ? 0 : KEYEVENTF_KEYUP, 0);
            }
            else if (event_type == "move" || event_type == "click" || event_type == "scroll")
            {
                int x = std::stoi(args[1]);
                int y = std::stoi(args[2]);

                // 마우스 위치 설정
                SetCursorPos(x, y);

                if (event_type == "click" && args.size() >= 5)
                {
                    // 마우스 클릭 이벤트 생성
                    DWORD event_down, event_up;
                    if (args[3] == "left")
                    {
                        event_down = MOUSEEVENTF_LEFTDOWN;
                        event_up = MOUSEEVENTF_LEFTUP;
                    }
                    else if (args[3] == "right")
                    {
                        event_down = MOUSEEVENTF_RIGHTDOWN;
                        event_up = MOUSEEVENTF_RIGHTUP;
                    }
                    else
                    { // button == "middle"
                        event_down = MOUSEEVENTF_MIDDLEDOWN;
                        event_up = MOUSEEVENTF_MIDDLEUP;
                    }

                    mouse_event(args[4] == "True" ? event_down : event_up, 0, 0, 0, 0);
                }
                else if (event_type == "scroll" && args.size() >= 5)
                {
                    // 스크롤 이벤트
                    mouse_event(MOUSEEVENTF_WHEEL, 0, 0, std::stoi(args[4]) * WHEEL_DELTA, 0);
                }
            }
        }
    }

    // 클라이언트 소켓 종료
    closesocket(clientSocket);

    // 서버 소켓 종료
    closesocket(serverSocket);

    // Winsock 종료
    WSACleanup();

    return 0;
}
