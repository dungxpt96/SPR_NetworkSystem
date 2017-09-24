#include "receiver.h"
#include "sender.h"
/* Add */
int sock;
struct sockaddr_in server_addr;
int ret;
int numReceiver;
char data_recv[1000][1024];
int numOfPacketRecv = 0;

int main(int argc, char *argv[]) 
{
	if (argc < 2) {
		printf("Error: Missing argument\n");
		return -1;
	}
	else if (argc == 2) {
		numReceiver = atoi(argv[1]);
	}
	else {
		printf("Warning: Too many argument\n");
		numReceiver = atoi(argv[1]);
	}

	printf("--------- RECEIVER %d ----------\n", numReceiver);
	sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
	{
		perror("Could not create socket: ");
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (ret < 0)
	{
		perror("Connect to Server failed: ");
		return -1;
	}

	printf("Connect to ip %s: %d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
	char *flag_client;
	if (numReceiver == 1) {
		flag_client = "r1";
	}
	else {
		flag_client = "r2";
	}
	write(sock, flag_client, strlen(flag_client));

	unsigned char packet_buf[1030];
	int nread;
	while ( true ) {
		memset(&(packet_buf), 0, 1030);
		nread = read(sock, packet_buf, 1030);
		int flag = write(sock, "is active", 9);
		/*if (flag < 0) {
			printf("Proxy is Disconnected\n");
			break;
		}*/
		if (nread < 0) {
			perror("Error in read from Proxy: ");
			break;
		}
		packet_t *packet = decap_packet(packet_buf);
		if (packet->len > 0) {
			strcpy(data_recv[numOfPacketRecv], packet->value);
			numOfPacketRecv ++;
			sort_packet_value();
		}

		if (packet->len == 0) {
			printf("%d - %d - %d\n", packet->header, packet->type, packet->len);
			numReceiver > 1 ? output_data("Out2.txt") : output_data("Out1.txt");
		}
	}
	close(sock);
	return 0;
}

void sort_packet_value() {
	int i, j;
	for (i = 0; i < numOfPacketRecv; i++) {
		for (j = i + 1; j < numOfPacketRecv; j++) {
			if (strlen(data_recv[i]) < strlen(data_recv[j])) {
				char temp[1024];
				strcpy(temp, data_recv[i]);
				strcpy(data_recv[i], data_recv[j]);
				strcpy(data_recv[j], temp);
			}
		}
	}
}

void output_data(char *filename) {
	FILE *fp = fopen(filename, "w");
	int i;
	for (i = 0; i < numOfPacketRecv; i++) {
		fprintf(fp, "Packet: %d\tLength: %d\nData:\n", i + 1, (int)strlen(data_recv[i]));
		fwrite(data_recv[i], 1, strlen(data_recv[i]), fp);
		fprintf(fp, "\n --------------------------------------------------- \n");
	}
	fclose(fp);
	printf("Output data into file %s successful\n", filename);
	printf("Continue WAITTING data from Proxy\n");
	numOfPacketRecv = 0;
}
