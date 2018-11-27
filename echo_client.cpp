#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <pthread.h>
#define BUF_SIZE 4096
#define MESSAGE_SIZE 3072

void print_error(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

void *sending_section(void *arg) {
    int serv_sock = *(int *)arg;
    char message[MESSAGE_SIZE];
    
    while(1){
        scanf("%s", message);
	    send(serv_sock, message, strlen(message), 0);
    }
}

void *receiving_section(void *arg) {
    int serv_sock = *(int *)arg;
    char message[MESSAGE_SIZE];
    
    while(recv(serv_sock, message, sizeof(message), 0) != 0) {
        printf("%s\n", message);
        memset(message, 0, sizeof(message));
    }
}

int main(int argc, char *argv[]) {
	int serv_sock;
	struct sockaddr_in serv_addr;
    
	char message[BUF_SIZE];

	if (argc != 3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1) print_error("socket error()\n");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) print_error("connect() error!");
    printf("-----------------------------------\n");
	printf("connect_success!!\n");
	printf("-----------------------------------\n");

    pthread_t t_id1, t_id2;
    pthread_create(&t_id1, NULL, sending_section, &serv_sock);
    pthread_create(&t_id2, NULL, receiving_section, &serv_sock);

    pthread_join(t_id1, 0);
    pthread_join(t_id2, 0);

	close(serv_sock);
	return 0;
}

