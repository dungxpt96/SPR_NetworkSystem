#ifndef SPR_H
#define SPR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>

#define PORT 8888
#define SIZE_CHUNK 1024 // Max of chunk data in file
unsigned char buffer_h[1030];

struct packet {
	uint16_t header;
	uint8_t type;
	uint16_t len;
	char *value;
};
typedef struct packet packet_t;

enum type_of_packet {
	transfer_data = 0x02,
	end_of_transfer = 0x4 
}; 

/*packet_t *create_packet(uint16_t header, uint8_t type, uint16_t len, char *data);*/
/* data == value */
unsigned char *encap_packet_to_data(packet_t *packet) {
	char data[1024];
	if (packet->value != NULL)
		strcpy(data, packet->value);
	else
		strcpy(data, "");
	memset(&buffer_h, 0, 1030);
	buffer_h[0] = packet->header >> 8;
	buffer_h[1] = packet->header;
	buffer_h[2] = packet->type;
	buffer_h[3] = packet->len >> 8;
	buffer_h[4] = packet->len;
	memcpy(buffer_h + 5, data, packet->len);
	buffer_h[5 + packet->len] = '\0';
	return buffer_h;
}

void print_data(unsigned char *data) {
	int i ;
	for (i = 0; i < 5; i++) {
		printf("%d\n", data[i]);
	}
}

void print_packet(packet_t *packet) {
	printf(" ----- PACKET ----- \n");
	printf("Header: %d\n", packet->header);
	printf("Type: %d\n", packet->type);
	printf("Len: %d\n", packet->len);
	printf("Value: %s\n", packet->value);
}

packet_t *decap_packet(unsigned char *data) {
	packet_t *packet = calloc ((sizeof(packet_t) + 1024), 1);
	packet->header = data[0] * 256 + data[1];
	packet->type = data[2];
	packet->len = data[3] * 256 + data[4];
	packet->value = data + 5;
	return packet;
}

char *exchange_data(char *data) {
	memset(&buffer_h, 0, 1030);
	int i, order = 0;

	for (i = 0; i <= strlen(data); i ++) {
		if (data[i] != '0') {
			buffer_h[order ++] = data[i];
		}
	}
	buffer_h[order] = '\0';
	return buffer_h;
}

packet_t *hanle_recv_packet(packet_t *packet) {
	packet->value = exchange_data(packet->value);
	packet->len = strlen(packet->value);
	return packet;
}


void handle_data(char *);

#endif