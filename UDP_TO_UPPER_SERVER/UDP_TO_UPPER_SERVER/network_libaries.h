#pragma once
#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment (lib, "ws2_32.lib")

#else //for Berkeley sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

//abstraction of types across diffrent operating systems

//for unix systems
#if !defined (_WIN32)
#define SOCKET int
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define GETSOCKETERRNO() (errno)
#endif 

//for windows
#if defined (_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())
#endif

//definition for ipv6
#if !defined (IPV6_6ONLY)
#define IPV6_V6ONLY 27
#endif