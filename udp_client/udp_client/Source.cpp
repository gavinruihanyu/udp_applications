#include "../../../udp_server/networking_libaries.h"
#include <iostream>

int main()
{
#if defined (_WIN32)
	WSAData data_winsock;
	WORD VERSION = MAKEWORD(2, 2);
	if (WSAStartup(VERSION, &data_winsock))
	{
		std::cout << "WSAStartup() failed: " << GETSOCKETERRNO() << " . quitting...\n"; return 1;
	}
#endif
	std::string ADDR;
	std::string PORT;
	std::cout << "Remote address: ";
	std::cin >> ADDR;
	std::cout << "Remote port: ";
	std::cin >> PORT;
	
	std::cout << "Attempting to connect to: " << ADDR << " on port: " << PORT << "\n";
	
	//getaddrinfo
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;

	struct addrinfo* peeraddress;
	if (getaddrinfo(ADDR.c_str(), PORT.c_str(), &hints, &peeraddress))
	{
		std::cout << "getaddrinfo() failed: " << GETSOCKETERRNO() << " . quitting...\n"; return 1;
	}

	//print target
	std::cout << "Connected to remote address : ";
	char address_buffer[100];
	char service_buffer[100];
	getnameinfo(peeraddress->ai_addr, peeraddress->ai_addrlen, address_buffer, sizeof(address_buffer), service_buffer, sizeof(service_buffer), NI_NUMERICHOST | NI_NUMERICSERV);
	std::cout << address_buffer << " on port: " << service_buffer << "\n";

	//create socket
	SOCKET socket_peer;
	socket_peer = socket(peeraddress->ai_family, peeraddress->ai_socktype, peeraddress->ai_protocol);
	if (!ISVALIDSOCKET(socket_peer))
	{
		std::cout << "socket() failed: " << GETSOCKETERRNO() << " . quitting...\n"; return 1;
	}

	char buf[] = "Hello World!";

	int bytes_sent = sendto(socket_peer, buf, sizeof(buf), 0, peeraddress->ai_addr, peeraddress->ai_addrlen);
	std::cout << "sent " << bytes_sent << " of " << sizeof(buf) << "\n";

	struct sockaddr_storage client_address;
	socklen_t client_len = sizeof(client_address);
	char buf_recv[1024];

	int bytes_recieved = recvfrom(socket_peer, buf_recv, 1024, 0, (struct sockaddr*)&client_address, &client_len);
	/*for (unsigned i = 0; i < bytes_recieved; i++)
	{
		std::cout << buf_recv[i];
	}*/
	std::cout << buf_recv;
	std::cout << "\n";



#if defined (_WIN32)
	WSACleanup();
#endif

	return 0;
}