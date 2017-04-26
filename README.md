# IP-Forwarding
This program takes in a binary file of packets called ip_packets. This file is read and the header information is processed to decide what to do with the packets. The header is the first 20 bytes of data from each packet so the first header is found and then the datagram length is found using the information. This is how each packet is distinguished from eachother. 

A packet is dropped if the TTL = 0. Although it is not implemented in this program, it would check the checksum and then subtract one from the TTL and then see if it was 0. But with this we do not check the checksum, just subtract from TTL no matter what. 

If the packet does not get dropped, the destination IP address is then read and compared with the forwarding table given as forwarding_table.txt. The forwarding table consists of a net-id, mask and next hop addresses respectivley to compare with shown below:

```
128.15.0.0    255.255.0.0     177.14.23.1
137.34.0.0    255.255.0.0     206.15.7.2 
137.34.192.0  255.255.192.0   138.27.4.3
137.34.128.0  255.255.128.0   115.12.4.4 
137.34.0.0    255.255.224.0   139.34.12.5 
201.17.34.0   255.255.255.0   192.56.4.6 
27.19.54.0    255.255.255.0   137.7.5.7 
0.0.0.0       0.0.0.0         142.45.9.8
```

The program uses longest prefix matching to decide which route to take if there are two that net-id's that match one destination IP. 

Once the next hop is found then that specific packet will be written to a binary out file called ip_packets_out. This file will be created for you based on what you call it when running the program. 

Even though a sample forwarding table and ip_packets binary file are given, this program can run with any given forwarding table in dotted format as above and any ip_packets file and will correctly drop or write to the out file and tell the next hop. 

## USAGE

In the terminal this can be ran using the makefile or using gcc:

```
make

OR

gcc ipforward.c -o ipforward
```

And the file can be ran as:

```
Usage: ./ipforward <forwarding_table.txt> <ip_packets> <ip_packets_out>
```

## Testing

When ran with the sample Forwarding table and ip_packets file given the output would be as follows:

```
./ipforward forwarding_table.txt ip_packets ip_packets_out

Source IP: 154.15.10.3
Destination IP: 128.15.10.38
Packet Length: 40
Dropped: Yes, TTL = 0


Source IP: 133.34.11.61
Destination IP: 137.34.11.1
Packet Length: 120
Dropped: No
Hop: 139.34.12.5


Source IP: 21.17.34.5
Destination IP: 201.17.34.35
Packet Length: 52
Dropped: No
Hop: 192.56.4.6


Source IP: 144.12.9.7
Destination IP: 137.34.160.17
Packet Length: 90
Dropped: No
Hop: 115.12.4.4


Source IP: 207.19.54.3
Destination IP: 27.19.54.12
Packet Length: 75
Dropped: No
Hop: 137.7.5.7
```

This ip_packet file had five packets of different lengths. Information about each packet is printed. 

The ip_packets file can be read using the following command in the terminal:

```
xxd -b ip_packets
```

This will allow the user to see each packet. Once the program is ran and an output file is created the same command can be used and if any packets have been dropped they will not show up in that file. 
