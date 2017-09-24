#include "sender.h"

int sock;
struct sockaddr_in server_addr;
int ret;
int numOfChunk = 0;
int numSender;
char data[1025];

int main(int argc, char *argv[]) 
{
	if (argc < 2) {
		printf("Error: Missing argument\n");
		return -1;
	}
	else if (argc == 2) {
		numSender = atoi(argv[1]);
	}
	else {
		printf("Warning: Too many argument\n");
		numSender = atoi(argv[1]);
	}

	printf("--------- SENDER %d ----------\n", numSender);
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
	char *flag_client = "s";
	write(sock, flag_client, strlen(flag_client));

	handle_data("In.txt");
	printf("Send data sucessful. Terminating.\n");
	sleep(2);
	close(sock);
	return 0;
}

void handle_data(char *filename) {
	FILE *fp = fopen(filename, "r");
	fseek(fp, 1024 * (numSender - 1), SEEK_CUR);
	while (!feof(fp)) {
		if (  (numOfChunk % 16) == 0 ) {
			memset(&data, 0, 1025);
			fread(data, 1, 1024, fp);
			data[1025] = '\0';
			packet_t *newPacket = calloc ( (sizeof(packet_t) + strlen(data)), 1 );
			newPacket->header = numSender;
			if ( (newPacket->len = strlen(data)) > 0) {
				newPacket->type = transfer_data;
				newPacket->value = data;
			}
			else {
				newPacket->type = end_of_transfer;
			}
			unsigned char *packet = encap_packet_to_data(newPacket);
			write(sock, packet, 1030);
		}
		else {
			fseek(fp, 1024, SEEK_CUR);
		}
		numOfChunk ++;
	}
	fclose(fp);
}
