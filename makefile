ipforward: ipforward.c
	gcc ipforward.c -o ipforward

clean:
	-rm -f ipforward
	-rm -f ip_packets_out
