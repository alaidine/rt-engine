#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef int socklen_t;
#define CLOSE_SOCKET(s) closesocket(s)
#define SOCKET_TYPE SOCKET
#define INVALID_SOCK INVALID_SOCKET
#else
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define CLOSE_SOCKET(s) close(s)
#define SOCKET_TYPE int
#define INVALID_SOCK (-1)
#endif

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

class NetBuffer {
  private:
    uint8_t *_data;
    size_t _size;
    size_t _capacity;
    size_t _readPos;

  public:
    NetBuffer(size_t capacity = 4096) : _size(0), _capacity(capacity), _readPos(0) { _data = new uint8_t[capacity]; }

    ~NetBuffer() { delete[] _data; }

    // Write operations
    void WriteUInt8(uint8_t value) {
        if (_size + sizeof(uint8_t) > _capacity)
            return;
        _data[_size++] = value;
    }

    void WriteUInt16(uint16_t value) {
        if (_size + sizeof(uint16_t) > _capacity)
            return;
        memcpy(_data + _size, &value, sizeof(uint16_t));
        _size += sizeof(uint16_t);
    }

    void WriteUInt32(uint32_t value) {
        if (_size + sizeof(uint32_t) > _capacity)
            return;
        memcpy(_data + _size, &value, sizeof(uint32_t));
        _size += sizeof(uint32_t);
    }

    void WriteInt32(int32_t value) {
        if (_size + sizeof(int32_t) > _capacity)
            return;
        memcpy(_data + _size, &value, sizeof(int32_t));
        _size += sizeof(int32_t);
    }

    void WriteFloat(float value) {
        if (_size + sizeof(float) > _capacity)
            return;
        memcpy(_data + _size, &value, sizeof(float));
        _size += sizeof(float);
    }

    void WriteBytes(const void *data, size_t length) {
        if (_size + length > _capacity)
            return;
        memcpy(_data + _size, data, length);
        _size += length;
    }

    // Read operations
    uint8_t ReadUInt8() {
        if (_readPos + sizeof(uint8_t) > _size)
            return 0;
        return _data[_readPos++];
    }

    uint16_t ReadUInt16() {
        if (_readPos + sizeof(uint16_t) > _size)
            return 0;
        uint16_t value;
        memcpy(&value, _data + _readPos, sizeof(uint16_t));
        _readPos += sizeof(uint16_t);
        return value;
    }

    uint32_t ReadUInt32() {
        if (_readPos + sizeof(uint32_t) > _size)
            return 0;
        uint32_t value;
        memcpy(&value, _data + _readPos, sizeof(uint32_t));
        _readPos += sizeof(uint32_t);
        return value;
    }

    int32_t ReadInt32() {
        if (_readPos + sizeof(int32_t) > _size)
            return 0;
        int32_t value;
        memcpy(&value, _data + _readPos, sizeof(int32_t));
        _readPos += sizeof(int32_t);
        return value;
    }

    float ReadFloat() {
        if (_readPos + sizeof(float) > _size)
            return 0.0f;
        float value;
        memcpy(&value, _data + _readPos, sizeof(float));
        _readPos += sizeof(float);
        return value;
    }

    void ReadBytes(void *dest, size_t length) {
        if (_readPos + length > _size)
            return;
        memcpy(dest, _data + _readPos, length);
        _readPos += length;
    }

    // Utility
    void Clear() {
        _size = 0;
        _readPos = 0;
    }

    void ResetRead() { _readPos = 0; }

    uint8_t *GetData() { return _data; }
    const uint8_t *GetData() const { return _data; }
    size_t GetSize() const { return _size; }
    size_t GetReadPos() const { return _readPos; }
    bool CanRead(size_t bytes) const { return _readPos + bytes <= _size; }

    // Load data from external buffer
    void LoadData(const uint8_t *data, size_t size) {
        if (size > _capacity)
            return;
        memcpy(_data, data, size);
        _size = size;
        _readPos = 0;
    }
};

class NetSocket {
  private:
    SOCKET_TYPE _sockfd;
    sockaddr_in _addr;
    bool _bound;

  public:
    NetSocket() : _sockfd(INVALID_SOCK), _bound(false) {
#if defined(_WIN32) || defined(_WIN64)
        // Initialize Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            perror("WSAStartup failed");
            return;
        }
#endif
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd == INVALID_SOCK) {
            perror("Socket creation failed");
        }
    }

    ~NetSocket() {
        if (_sockfd != INVALID_SOCK) {
            CLOSE_SOCKET(_sockfd);
        }
#if defined(_WIN32) || defined(_WIN64)
        WSACleanup();
#endif
    }

    bool Bind(uint16_t port) {
        memset(&_addr, 0, sizeof(_addr));
        _addr.sin_family = AF_INET;
        _addr.sin_addr.s_addr = INADDR_ANY;
        _addr.sin_port = htons(port);

        // Set socket to non-blocking
#if defined(_WIN32) || defined(_WIN64)
        u_long mode = 1;
        ioctlsocket(_sockfd, FIONBIO, &mode);
#else
        int flags = fcntl(_sockfd, F_GETFL, 0);
        fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK);
#endif

        if (bind(_sockfd, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) {
            perror("Bind failed");
            return false;
        }

        _bound = true;
        return true;
    }

    bool SendTo(const NetBuffer &buffer, const sockaddr_in &dest) {
        int sent =
            sendto(_sockfd, (const char *)buffer.GetData(), (int)buffer.GetSize(), 0, (struct sockaddr *)&dest, sizeof(dest));
        return sent > 0;
    }

    bool SendTo(const NetBuffer &buffer, const char *ip, uint16_t port) {
        sockaddr_in dest;
        memset(&dest, 0, sizeof(dest));
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);
        inet_pton(AF_INET, ip, &dest.sin_addr);

        return SendTo(buffer, dest);
    }

    int ReceiveFrom(NetBuffer &buffer, sockaddr_in &from) {
        char temp[4096];
        socklen_t fromLen = sizeof(from);

        int received = recvfrom(_sockfd, temp, sizeof(temp), 0, (struct sockaddr *)&from, &fromLen);

        if (received > 0) {
            buffer.LoadData((const uint8_t *)temp, received);
            return received;
        }

        return received; // 0 or -1
    }

    SOCKET_TYPE GetFd() const { return _sockfd; }
    bool IsBound() const { return _bound; }
};

class NetServer {
  private:
    NetSocket _socket;
    uint16_t _port;

  public:
    NetServer(uint16_t port) : _port(port) {}

    bool Start() { return _socket.Bind(_port); }

    int Receive(NetBuffer &buffer, sockaddr_in &from) { return _socket.ReceiveFrom(buffer, from); }

    bool SendTo(const NetBuffer &buffer, const sockaddr_in &dest) { return _socket.SendTo(buffer, dest); }

    SOCKET_TYPE GetFd() const { return _socket.GetFd(); }
};

class NetClient {
  private:
    NetSocket _socket;
    sockaddr_in _serverAddr;
    bool _connected;

  public:
    NetClient() : _connected(false) {}

    bool Connect(const char *ip, uint16_t port) {
        // Bind to any port
        if (!_socket.Bind(0)) {
            return false;
        }

        memset(&_serverAddr, 0, sizeof(_serverAddr));
        _serverAddr.sin_family = AF_INET;
        _serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, ip, &_serverAddr.sin_addr);

        _connected = true;
        return true;
    }

    bool Send(const NetBuffer &buffer) {
        if (!_connected)
            return false;
        return _socket.SendTo(buffer, _serverAddr);
    }

    int Receive(NetBuffer &buffer) {
        sockaddr_in from;
        return _socket.ReceiveFrom(buffer, from);
    }

    SOCKET_TYPE GetFd() const { return _socket.GetFd(); }
    bool IsConnected() const { return _connected; }
};
