#pragma once

#include "IPlugin.h"
#include "framework.h"

#if defined(_WIN32)
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

namespace Roar {

class INetBuffer {
  public:
    virtual ~INetBuffer() = default;
    virtual void WriteUInt8(uint8_t value) = 0;
    virtual void WriteUInt16(uint16_t value) = 0;
    virtual void WriteUInt32(uint32_t value) = 0;
    virtual void WriteInt32(int32_t value) = 0;
    virtual void WriteFloat(float value) = 0;
    virtual void WriteBytes(const void *data, size_t length) = 0;
    virtual uint8_t ReadUInt8() = 0;
    virtual uint16_t ReadUInt16() = 0;
    virtual uint32_t ReadUInt32() = 0;
    virtual int32_t ReadInt32() = 0;
    virtual float ReadFloat() = 0;
    virtual void ReadBytes(void *dest, size_t length) = 0;
    virtual void Clear() = 0;
    virtual void ResetRead() = 0;
    virtual uint8_t *GetData() = 0;
    virtual const uint8_t *GetData() const = 0;
    virtual size_t GetSize() const = 0;
    virtual size_t GetReadPos() const = 0;
    virtual bool CanRead(size_t bytes) const = 0;
    virtual void LoadData(const uint8_t *data, size_t size) = 0;
};

class INetSocket {
  public:
    virtual ~INetSocket() = default;
    virtual bool Bind(uint16_t port) = 0;
    virtual bool SendTo(const INetBuffer &buffer, const sockaddr_in &dest) = 0;
    virtual bool SendTo(const INetBuffer &buffer, const char *ip, uint16_t port) = 0;
    virtual int ReceiveFrom(INetBuffer &buffer, sockaddr_in &from) = 0;
    virtual SOCKET_TYPE GetFd() const = 0;
    virtual bool IsBound() const = 0;
};

class INetServer {
  public:
    virtual ~INetServer() = default;
    virtual bool Start() = 0;
    virtual int Receive(INetBuffer &buffer, sockaddr_in &from) = 0;
    virtual bool SendTo(const INetBuffer &buffer, const sockaddr_in &dest) = 0;
    virtual SOCKET_TYPE GetFd() const = 0;
};

class INetClient {
  public:
    virtual ~INetClient() = default;
    virtual bool Connect(const char *ip, uint16_t port) = 0;
    virtual bool Send(const INetBuffer &buffer) = 0;
    virtual int Receive(INetBuffer &buffer) = 0;
    virtual SOCKET_TYPE GetFd() const = 0;
    virtual bool IsConnected() const = 0;
};

class INetwork : public IPlugin {
  public:
    virtual INetClient *NewClient() = 0;
    virtual INetServer *NewServer(uint16_t port) = 0;
    virtual INetBuffer *NewBuffer() = 0;
};

} // namespace Roar
