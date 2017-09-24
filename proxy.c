#include "proxy.h"

int listenfd, connectfd;
int err, ret;
pthread_t th_pid[1024];
struct sockaddr_in server_addr, cli_addr;
socklen_t size;

int main() {
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	if (listenfd < 0) {
		perror("Error in create socket: ");
		exit(1);
	}
	printf("===> Socket retrieve success\n");

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);

	ret = bind(listenfd, (struct sockaddr*) &server_addr, sizeof(server_addr));
	if (ret < 0) {
		perror("Error in bind socket to Network Interface: ");
		exit(1);
	}

	ret = listen(listenfd, 10);
	if (ret < 0) {
		perror("Error in listen socket: ");
		return -1;
	}
	printf("===> Waiting a client connect to...\n");

	while(true) {
		size = sizeof(cli_addr);

		connectfd = accept(listenfd, (struct sockaddr*) &cli_addr, &size);

		if (connectfd < 0) {
			perror("Error in accept: ");
			continue;
		}
		/* Confirm Sender or Receiver */
		char flag_client[3];
		read(connectfd, flag_client, 10);
		flag_client[2] = 0;
		if (strcmp(flag_client, "r1") == 0) {
			printf("Connected to Receiver 1\n");
			receiver[1] = connectfd;
		}
		else if (strcmp(flag_client, "r2") == 0) {
			receiver[0] = connectfd;
			printf("Connected to Receiver 2\n");
		}
		else {
			sender[numSender] = connectfd;
			printf("Connected to Sender %d\n", numSender + 1);
			err = pthread_create(&th_pid[numSender], NULL, &handle_data_receiver, &connectfd);
			numSender++;
			if (err != 0) {
				perror("Can't create thread: ");
			}	
		}
	}

	int i;
	for (i = 0; i < numSender; i++) {
		pthread_join(th_pid[i], 0);
	}

	return 0;
}

void *handle_data_receiver(void *arg) {
	int conn = *((int *) arg);
	unsigned char packet_buf[1030];
	int nread;
	memset(&(packet_buf), 0, 1030);
	while ( (nread = read(conn, packet_buf, 1030)) > 0) {
		packet_t *packet = decap_packet(packet_buf);
		packet = hanle_recv_packet(packet);
		char *packet_data = encap_packet_to_data(packet);
		
		if ((packet->header % 2) == 0) { // Send Receiver 1
			write(receiver[1], packet_data, 1030);
		}
		else {
			write(receiver[0], packet_data, 1030);
		}

		if (packet->type == 0x04) {
			close(conn);
			printf("Disconneted to Client %d on server\n", conn - 5);
		}
		memset(&(packet_buf), 0, 1030);
	}
}