#include <arpa/inet.h> 
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>

// make the IP header struct with pointer p
struct IPheader
{
	unsigned short vht;
	unsigned short datagram_len;
	unsigned short identifier;
	unsigned short flag_offset;
	unsigned char TTL;
	unsigned char ULP;
	unsigned short checksum;
	unsigned int src_IP;
	unsigned int dest_IP;
}*p;

int main(int argc, char* argv[])
{
	// initialize variables and arrays
	char forwarding_table[1024];
	char ip_packets[1024];
	char ip_packets_out[1024];
	char packet_header[160];
	struct in_addr address;
	struct sockaddr_in ip_conv;
	int file_start = 0;
	unsigned int comp_dest;
	unsigned int new_net;
	char* netid;
	char* mask;
	char* hop;
	char new_hop[128];
	unsigned int old_mask;
	unsigned int temp;
	unsigned int new_temp;
	unsigned int new_mask;

	// set argument count, exit on error
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s <forwarding_table.txt> <ip_packets> <ip_packets_out>\n", argv[0]);
		exit(1);
	}

	// copy argument values into proper file names
	strcpy(forwarding_table, argv[1]);
	strcpy(ip_packets, argv[2]);
	strcpy(ip_packets_out, argv[3]);

	// Open the three files
	FILE* forward_file;
	forward_file = fopen(forwarding_table, "r");

	FILE* packets_in;
	packets_in = fopen(ip_packets, "rb");

	FILE* packets_out;
	packets_out = fopen(ip_packets_out, "wb");

	// send error messages if files do not open
	if(forward_file == NULL)
	{
		fprintf(stderr, "Cannot open file: %s\n", argv[1]);
	}

	if(packets_in == NULL)
	{
		fprintf(stderr, "Cannot open file: %s\n", argv[2]);
	}

	// create a while loop to loop until ip packet file ends
	int c = fgetc(packets_in);
	while(c != EOF)
	{
		// seek to where the new packet starts each loop
		fseek(packets_in, file_start, SEEK_SET);

		// read the first 20 bytes from the packet being read
		fread(packet_header, 20, 1, packets_in);

		// point the struct at the packet header buffer
		p = (struct IPheader*) packet_header;

		// print out source IP in dotted notation
		address.s_addr = p->src_IP;
		char* source_IP_dot = inet_ntoa(address);
		printf("Source IP: %s\n", source_IP_dot);

		// print out destination IP in dotted notation
		address.s_addr = p->dest_IP;
		char* destination_IP_dot = inet_ntoa(address);
		printf("Destination IP: %s\n", destination_IP_dot);

		// print packet length
		printf("Packet Length: %d\n", ntohs(p->datagram_len));

		// subtract 1 from the TTl
		p->TTL -= 1;

		// create a buffer for just the data with the data size
		char data[(ntohs(p->datagram_len) * 8)];

		// check to see if the TTL is less than 0 and drop if it is
		// do not write to the file if it is dropped
		if(ntohs(p->TTL) <= 0)
		{
			printf("Dropped: Yes, TTL = 0\n\n");

			file_start = file_start + ntohs(p->datagram_len);

			memset(packet_header, 0, 20);
			memset(data, 0, ntohs(p->datagram_len));
			continue;
		}

		// read the data into the data buffer
		fread(data, ntohs(p->datagram_len) - 20, 1, packets_in);

		printf("Dropped: No\n");

		// write the packet header and the data to the out file if it is not dropped
		fwrite(packet_header, 20, 1, packets_out);
		fwrite(data, ntohs(p->datagram_len) - 20, 1, packets_out);

		char line[1024];
		int get_in = 0;
		temp = 0;

		// run a while loop over the forwarding table to check where the next hop is
		while(fgets(line, sizeof(line), forward_file))
		{
			// get the netid, mask and hop from each line
			netid = strtok(line, " ");
			mask = strtok(NULL, " ");
			hop = strtok(NULL, " ");

			// turn the mask into a binary address
			old_mask = inet_addr(mask);
			
			// "and" the destination IP with the mask to compare with a net id
			ip_conv.sin_addr.s_addr = p->dest_IP & old_mask;
			comp_dest = ip_conv.sin_addr.s_addr;

			// get the netid in binary
			new_net = inet_addr(netid);

			// compare to see where the masked value and netid are the same
			if(comp_dest == new_net)
			{
				// make if statement to set temp = to the masked number
				// set new hop to the hop at that net id
				if(get_in == 0)
				{
					get_in = get_in + 1;
					temp = old_mask;
					strcat(new_hop, hop);
					continue;
				}

				// this gets the length of the number of bits
				new_temp = floor(log2(temp)) + 1;
				new_mask = floor(log2(old_mask)) + 1;

				// if the temp holding the masked value is smaller than the new masked value set new temp
				// set new hop as well
				if(new_temp < new_mask)
				{
					temp = old_mask;
					memset(new_hop, 0, 128);
					strcat(new_hop, hop);
				}
			}
		}
		// print out the next hop if packet is not dropped
		printf("Hop: %s\n\n", new_hop);

		// go to the beginning of the forward file
		// set the file start number to know where the beginning of the next packet is
		fseek(forward_file, 0, SEEK_SET);
		file_start = file_start + ntohs(p->datagram_len);

		// reset the memory for the buffers and hop address
		memset(packet_header, 0, 20);
		memset(data, 0, ntohs(p->datagram_len));
		memset(new_hop, 0, 128);

		c = fgetc(packets_in);
	}

	// close the files
	fclose(forward_file);
	fclose(packets_in);
	fclose(packets_out);
}