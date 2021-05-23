#include "Sniffer.h"


int Sniffer::Sniffing()
{
	SOCKET sniffer;
	struct in_addr addr;
	int in;

	char hostname[100];
	struct hostent* local;
	WSADATA wsa;

	logfile = fopen("log.txt", "w");
	if (logfile == NULL)
	{
		printf("Unable to create file.");
	}

	//Initialise Winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup() failed.\n");
		return 1;
	}
	printf("Initialised");

	//Create a RAW Socket
	printf("\nCreating RAW Socket...");
	sniffer = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (sniffer == INVALID_SOCKET)
	{
		printf("Failed to create raw socket.\n");
		return 1;
	}
	printf("Created.");

	//Retrive the local hostname
	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
	{
		printf("Error : %d", WSAGetLastError());
		return 1;
	}
	printf("\nHost name : %s \n", hostname);

	//Retrive the available IPs of the local host
	local = gethostbyname(hostname);
	printf("\nAvailable Network Interfaces : \n");
	if (local == NULL)
	{
		printf("Error : %d.\n", WSAGetLastError());
		return 1;
	}

	for (i = 0; local->h_addr_list[i] != 0; ++i)
	{
		memcpy(&addr, local->h_addr_list[i], sizeof(struct in_addr));
		printf("Interface Number : %d Address : %s\n", i, inet_ntoa(addr));
	}

	printf("Enter the interface number you would like to sniff : ");
	//scanf("%d", &in);
	/*
	Choose the main interface by your own and insert it to in
	*/
	in = 2; //temporery



	memset(&dest, 0, sizeof(dest));
	memcpy(&dest.sin_addr.s_addr, local->h_addr_list[in], sizeof(dest.sin_addr.s_addr));
	dest.sin_family = AF_INET;
	dest.sin_port = 0;

	printf("\nBinding socket to local system and port 0 ...");
	if (bind(sniffer, (struct sockaddr*)&dest, sizeof(dest)) == SOCKET_ERROR)
	{
		printf("bind(%s) failed.\n", inet_ntoa(addr));
		return 1;
	}
	printf("Binding successful");

	//Enable this socket with the power to sniff : SIO_RCVALL is the key Receive ALL ;)

	j = 1;
	printf("\nSetting socket to sniff...");
	if (WSAIoctl(sniffer, SIO_RCVALL, &j, sizeof(j), 0, 0, (LPDWORD)&in, 0, 0) == SOCKET_ERROR)
	{
		printf("WSAIoctl() failed.\n");
		return 1;
	}
	printf("Socket set.");

	//Begin
	printf("\nStarted Sniffing\n");
	printf("Packet Capture Statistics...\n");
	StartSniffing(sniffer); //Happy Sniffing

	//End
	closesocket(sniffer);
	WSACleanup();

	return 0;
}



void Sniffer::StartSniffing(SOCKET sniffer)
{
	char* Buffer = (char*)malloc(65536); //Its Big!
	int mangobyte;

	if (Buffer == NULL)
	{
		printf("malloc() failed.\n");
		return;
	}

	do
	{
		mangobyte = recvfrom(sniffer, Buffer, 65536, 0, 0, 0); //Eat as much as u can

		if (mangobyte > 0)
		{
			ProcessPacket(Buffer, mangobyte);
		}
		else
		{
			printf("recvfrom() failed.\n");
		}
	} while (mangobyte > 0);

	free(Buffer);
}

void Sniffer::ProcessPacket(char* Buffer, int Size)
{
	iphdr = (IPV4_HDR*)Buffer;
	++total;

	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iphdr->ip_srcaddr;

	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iphdr->ip_destaddr;

	std::string src = inet_ntoa(source.sin_addr);
	std::string dst = inet_ntoa(dest.sin_addr);

	if (src.find(subnetMask) != std::string::npos && dst.find(subnetMask) != std::string::npos) //filter only inner subnet packets
	{
		switch (iphdr->ip_protocol) //Check the Protocol and do accordingly...
		{
		case 1: //ICMP Protocol
			++icmp;
			PrintIcmpPacket(Buffer, Size);
			break;

		case 2: //IGMP Protocol
			++igmp;
			break;

		case 6: //TCP Protocol
			++tcp;
			PrintTcpPacket(Buffer, Size);
			break;

		case 17: //UDP Protocol
			++udp;
			PrintUdpPacket(Buffer, Size);
			break;

		default: //Some Other Protocol like ARP etc.
			++others;
			break;
		}
		printf("TCP : %d UDP : %d ICMP : %d IGMP : %d Others : %d Total : %d\r", tcp, udp, icmp, igmp, others, total);
	}

}

void Sniffer::PrintIpHeader(char* Buffer)
{
	unsigned short iphdrlen;

	iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;



	fprintf(logfile, "\n");
	fprintf(logfile, "IP Header\n");
	fprintf(logfile, " |-IP Version : %d\n", (unsigned int)iphdr->ip_version);
	fprintf(logfile, " |-IP Header Length : %d DWORDS or %d Bytes\n", (unsigned int)iphdr->ip_header_len, ((unsigned int)(iphdr->ip_header_len)) * 4);
	fprintf(logfile, " |-Type Of Service : %d\n", (unsigned int)iphdr->ip_tos);
	fprintf(logfile, " |-IP Total Length : %d Bytes(Size of Packet)\n", ntohs(iphdr->ip_total_length));
	fprintf(logfile, " |-Identification : %d\n", ntohs(iphdr->ip_id));
	fprintf(logfile, " |-Reserved ZERO Field : %d\n", (unsigned int)iphdr->ip_reserved_zero);
	fprintf(logfile, " |-Dont Fragment Field : %d\n", (unsigned int)iphdr->ip_dont_fragment);
	fprintf(logfile, " |-More Fragment Field : %d\n", (unsigned int)iphdr->ip_more_fragment);
	fprintf(logfile, " |-TTL : %d\n", (unsigned int)iphdr->ip_ttl);
	fprintf(logfile, " |-Protocol : %d\n", (unsigned int)iphdr->ip_protocol);
	fprintf(logfile, " |-Checksum : %d\n", ntohs(iphdr->ip_checksum));
	fprintf(logfile, " |-Source IP : %s\n", inet_ntoa(source.sin_addr));
	fprintf(logfile, " |-Destination IP : %s\n", inet_ntoa(dest.sin_addr));
}

/*
sorts all the packets to application layer protocols
due all application protocols that we need r using tcp we dont check udp at all
*/
void Sniffer::sortPackets(char* Buffer)
{
	iphdr = (IPV4_HDR*)Buffer;
	unsigned short iphdrlen = iphdr->ip_header_len * 4;
	tcpheader = (TCP_HDR*)(Buffer + iphdrlen);

	//ntohs(tcpheader->source_port)
	//ntohs(tcpheader->dest_port)

	/*
	Metholadgy -> we know that all the requests are from the same subnet so we gotta check if the source port or the dest port has the port number of the protocol that we wanna check
	we need both src and dest and not only dest because the service that uses the protocl is or on our machine or another machine in the subnet and both are vuln.
	NOTE !!!
	I had to use if statements because its way faster than switch case AND I couldnt use map
	because I had to do smth with the protocol when I find it, so if I found a port to be ftp
	I need to create an object of an FTP class and use it later to the module so map isnt the solution here.
	*/
	if (ntohs(tcpheader->source_port) == 21 || ntohs(tcpheader->dest_port) == 21) { std::cout << "FTP FTP FTP !!" << std::endl; }
	else if (ntohs(tcpheader->source_port) == 22 || ntohs(tcpheader->dest_port) == 22) { std::cout << "SSH SSH SSH !!" << std::endl; }
	else if (ntohs(tcpheader->source_port) == 23 || ntohs(tcpheader->dest_port) == 23) { std::cout << "TELNET TELNET TELNET !!" << std::endl; }


}

void Sniffer::PrintTcpPacket(char* Buffer, int Size)
{
	unsigned short iphdrlen;

	iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	tcpheader = (TCP_HDR*)(Buffer + iphdrlen);

	fprintf(logfile, "\n\n***********************TCP Packet*************************\n");

	PrintIpHeader(Buffer);

	fprintf(logfile, "\n");
	fprintf(logfile, "TCP Header\n");
	fprintf(logfile, " |-Source Port : %u\n", ntohs(tcpheader->source_port));
	fprintf(logfile, " |-Destination Port : %u\n", ntohs(tcpheader->dest_port));
	fprintf(logfile, " |-Sequence Number : %u\n", ntohl(tcpheader->sequence));
	fprintf(logfile, " |-Acknowledge Number : %u\n", ntohl(tcpheader->acknowledge));
	fprintf(logfile, " |-Header Length : %d DWORDS or %d BYTES\n"
		, (unsigned int)tcpheader->data_offset, (unsigned int)tcpheader->data_offset * 4);
	fprintf(logfile, " |-CWR Flag : %d\n", (unsigned int)tcpheader->cwr);
	fprintf(logfile, " |-ECN Flag : %d\n", (unsigned int)tcpheader->ecn);
	fprintf(logfile, " |-Urgent Flag : %d\n", (unsigned int)tcpheader->urg);
	fprintf(logfile, " |-Acknowledgement Flag : %d\n", (unsigned int)tcpheader->ack);
	fprintf(logfile, " |-Push Flag : %d\n", (unsigned int)tcpheader->psh);
	fprintf(logfile, " |-Reset Flag : %d\n", (unsigned int)tcpheader->rst);
	fprintf(logfile, " |-Synchronise Flag : %d\n", (unsigned int)tcpheader->syn);
	fprintf(logfile, " |-Finish Flag : %d\n", (unsigned int)tcpheader->fin);
	fprintf(logfile, " |-Window : %d\n", ntohs(tcpheader->window));
	fprintf(logfile, " |-Checksum : %d\n", ntohs(tcpheader->checksum));
	fprintf(logfile, " |-Urgent Pointer : %d\n", tcpheader->urgent_pointer);
	fprintf(logfile, "\n");
	fprintf(logfile, " DATA Dump ");
	fprintf(logfile, "\n");

	fprintf(logfile, "IP Header\n");
	PrintData(Buffer, iphdrlen);

	fprintf(logfile, "TCP Header\n");
	PrintData(Buffer + iphdrlen, tcpheader->data_offset * 4);

	fprintf(logfile, "Data Payload\n");
	PrintData(Buffer + iphdrlen + tcpheader->data_offset * 4
		, (Size - tcpheader->data_offset * 4 - iphdr->ip_header_len * 4));

	fprintf(logfile, "\n###########################################################");

	sortPackets(Buffer);
}

void Sniffer::PrintUdpPacket(char* Buffer, int Size)
{
	unsigned short iphdrlen;

	iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	udpheader = (UDP_HDR*)(Buffer + iphdrlen);

	fprintf(logfile, "\n\n***********************UDP Packet*************************\n");

	PrintIpHeader(Buffer);

	fprintf(logfile, "\nUDP Header\n");
	fprintf(logfile, " |-Source Port : %d\n", ntohs(udpheader->source_port));
	fprintf(logfile, " |-Destination Port : %d\n", ntohs(udpheader->dest_port));
	fprintf(logfile, " |-UDP Length : %d\n", ntohs(udpheader->udp_length));
	fprintf(logfile, " |-UDP Checksum : %d\n", ntohs(udpheader->udp_checksum));

	fprintf(logfile, "\n");
	fprintf(logfile, "IP Header\n");

	PrintData(Buffer, iphdrlen);

	fprintf(logfile, "UDP Header\n");

	PrintData(Buffer + iphdrlen, sizeof(UDP_HDR));

	fprintf(logfile, "Data Payload\n");

	PrintData(Buffer + iphdrlen + sizeof(UDP_HDR), (Size - sizeof(UDP_HDR) - iphdr->ip_header_len * 4));

	fprintf(logfile, "\n###########################################################");
}

void Sniffer::PrintIcmpPacket(char* Buffer, int Size)
{
	unsigned short iphdrlen;

	iphdr = (IPV4_HDR*)Buffer;
	iphdrlen = iphdr->ip_header_len * 4;

	icmpheader = (ICMP_HDR*)(Buffer + iphdrlen);

	fprintf(logfile, "\n\n***********************ICMP Packet*************************\n");
	PrintIpHeader(Buffer);

	fprintf(logfile, "\n");

	fprintf(logfile, "ICMP Header\n");
	fprintf(logfile, " |-Type : %d", (unsigned int)(icmpheader->type));

	if ((unsigned int)(icmpheader->type) == 11)
	{
		fprintf(logfile, " (TTL Expired)\n");
	}
	else if ((unsigned int)(icmpheader->type) == 0)
	{
		fprintf(logfile, " (ICMP Echo Reply)\n");
	}

	fprintf(logfile, " |-Code : %d\n", (unsigned int)(icmpheader->code));
	fprintf(logfile, " |-Checksum : %d\n", ntohs(icmpheader->checksum));
	fprintf(logfile, " |-ID : %d\n", ntohs(icmpheader->id));
	fprintf(logfile, " |-Sequence : %d\n", ntohs(icmpheader->seq));
	fprintf(logfile, "\n");

	fprintf(logfile, "IP Header\n");
	PrintData(Buffer, iphdrlen);

	fprintf(logfile, "UDP Header\n");
	PrintData(Buffer + iphdrlen, sizeof(ICMP_HDR));

	fprintf(logfile, "Data Payload\n");
	PrintData(Buffer + iphdrlen + sizeof(ICMP_HDR), (Size - sizeof(ICMP_HDR) - iphdr->ip_header_len * 4));

	fprintf(logfile, "\n###########################################################");
}

/*
	Print the hex values of the data
*/
void Sniffer::PrintData(char* data, int Size)
{
	char a, line[17], c;
	int j;

	//loop over each character and print
	for (i = 0; i < Size; i++)
	{
		c = data[i];

		//Print the hex value for every character , with a space. Important to make unsigned
		fprintf(logfile, " %.2x", (unsigned char)c);

		//Add the character to data line. Important to make unsigned
		a = (c >= 32 && c <= 128) ? (unsigned char)c : '.';

		line[i % 16] = a;

		//if last character of a line , then print the line - 16 characters in 1 line
		if ((i != 0 && (i + 1) % 16 == 0) || i == Size - 1)
		{
			line[i % 16 + 1] = '\0';

			//print a big gap of 10 characters between hex and characters
			fprintf(logfile, "          ");

			//Print additional spaces for last lines which might be less than 16 characters in length
			for (j = strlen(line); j < 16; j++)
			{
				fprintf(logfile, "   ");
			}

			fprintf(logfile, "%s \n", line);
		}
	}

	fprintf(logfile, "\n");
}