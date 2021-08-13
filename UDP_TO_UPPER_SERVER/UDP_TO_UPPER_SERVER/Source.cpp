#include "network_libaries.h"
#include <iostream>

int handle(SOCKET socket)
{
	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	char read[1024];
	int bytes_recieved = recvfrom(socket, read, 1024, 0, (struct sockaddr*)&client_address, &client_len);
	if (bytes_recieved < 1)
	{
		std::cout << "connection closed: " << GETSOCKETERRNO() << "\n";
		return 1;
	}
	std::string send_data = "";
	for (unsigned j = 0; j < bytes_recieved; j++)
	{
		send_data += toupper(read[j]);
	}
	sendto(socket, send_data.c_str(), sizeof(send_data), 0, (struct sockaddr*)&client_address, client_len);
}

int main()
{
#if defined (_WIN32)
	WSAData d;
	WORD VERSION = MAKEWORD(2, 2);

	if (WSAStartup(VERSION, &d))
	{
		std::cout << "WSAStartup() failed. " << GETSOCKETERRNO() << "\n"; return 1;
	}
#endif

	const char* address = 0;
	const char* port = "8080";
	const char* port2 = "8081";

	//set up listning port 1
	//getaddrinfo
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	
	struct addrinfo* bind_address;
	if (getaddrinfo(address, port, &hints, &bind_address))
	{
		std::cout << "getaddrinfo() failed. " << GETSOCKETERRNO() << "\n"; return 1;
	}

	//create sockets
	SOCKET socket_listen;
	socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen))
	{
		std::cout << "socket() failed. " << GETSOCKETERRNO() << "\n"; return 1;
	}
	
	//clear ipv6only flag for support for ipv4
	bool opt = false;

	if (setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&opt, sizeof(opt)))
	{
		std::cout << "setsockopt() for clear ipv6only flag failed: " << GETSOCKETERRNO() << " . Quitting...\n";
		return -1;
	}

	//bind sockets
	if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		std::cout << "bind() failed. " << GETSOCKETERRNO() << "\n"; return 1;
	}
	freeaddrinfo(bind_address);

	//setting up listning port 2
	//getaddrinfo
	if (getaddrinfo(address, port2, &hints, &bind_address))
	{
		std::cout << "getaddrinfo()2 failed. " << GETSOCKETERRNO() << "\n"; return 1;
	}
	//create socket
	SOCKET socket_listen2;
	socket_listen2 = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_listen2))
	{
		std::cout << "socket()2 failed. " << GETSOCKETERRNO() << "\n"; return 1;
	}
	//clear ipv6 only option
	if (setsockopt(socket_listen2, IPPROTO_IPV6, IPV6_V6ONLY, (char*)&opt, sizeof(opt)))
	{
		std::cout << "setsockopt()2 for clear ipv6only flag failed: " << GETSOCKETERRNO() << "\n";return -1;
	}
	//bind socket
	if (bind(socket_listen2, bind_address->ai_addr, bind_address->ai_addrlen))
	{
		std::cout << "bind()2 failed. " << GETSOCKETERRNO() << "\n"; return 1;
	}
	freeaddrinfo(bind_address);


	//multiplexing using select()
	//adding listning socket to fdd_set master
	fd_set master;
	FD_ZERO(&master);
	FD_SET(socket_listen, &master);
	FD_SET(socket_listen2, &master);
	SOCKET max_socket = socket_listen2;
	

	std::cout << "Listening on port: " << port << " and port: " << port2 << " for connections...\n";
	//main loop
	while (true)
	{
		fd_set reads = master;
		if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
		{
			std::cout << "select(). " << GETSOCKETERRNO() << "\n"; return 1;
		}

		if (FD_ISSET(socket_listen, &reads))
		{
			if (handle(socket_listen))
			{
				return 1;
			}
		}
		else if (FD_ISSET(socket_listen2, &reads))
		{
			if (handle(socket_listen2))
			{
				return 1;
			}
		}
	}


#if defined (_WIN32)
	WSACleanup();
#endif
	CLOSESOCKET(socket_listen);
	CLOSESOCKET(socket_listen2);
	std::cout << "Finished.\n";
	return 0;
}

