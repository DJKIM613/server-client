#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <set>

using namespace std;

#define BUF_SIZE 1024
#define MESSAGE_SIZE 3072
#define MAX_CLIENT_NUM 10

void *request_handler(void* arg);
void send_data(int clnt_sock, char *file_name);
void send_400error(int clnt_sock);
void send_404error(int clnt_sock);
void print_error(char *message);

set<int> used_socket, unused_socket;
int sock_number[MAX_CLIENT_NUM];
bool is_b_option;
int main(int argc, char *argv[]) {
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_size;
	char buf[BUF_SIZE];
	pthread_t t_id;
	if (argc != 2 && argc != 3) print_error(" echoserver <port> [-b]");
	if(argc == 3){
		if(strcmp(argv[2], "-b") == 0) is_b_option = true;
		else print_error(" echoserver <port> [-b]");
	}

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));
	if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) print_error("bind error\n");

	if (listen(serv_sock, 20) == -1) print_error("listen error\n");

	for(int i= 0 ; i < MAX_CLIENT_NUM ; i++) unused_socket.insert(i);

	while (1) {
		clnt_adr_size = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr, (socklen_t *)(&clnt_adr_size));
		if(unused_socket.size() == 0){
			send(clnt_sock, "client_full", strlen("client_full"), 0);
			close(clnt_sock);
			continue;
		}
		int index = *unused_socket.begin();
		sock_number[index] = clnt_sock;
		used_socket.insert(index);
		unused_socket.erase(index);

		pthread_create(&t_id, NULL, request_handler, &index);
		pthread_detach(t_id);
	}

	close(serv_sock);
	return 0;
}

void *request_handler(void *arg) {
	int index = *((int *)arg);
	int clnt_sock = sock_number[index];
	
	printf("-----------------------------------\n");
	printf("connect_success!! socket index : %d client number %d\n", clnt_sock, used_socket.size());
	printf("-----------------------------------\n");

	char message[MESSAGE_SIZE];
	//for(set<int> :: iterator it = used_socket.begin() ; it != used_socket.end() ; it++) printf("%d\n", sock_number[*it]);
	while(recv(clnt_sock, message, sizeof(message), 0) != 0){
		printf("%s\n", message);	memset(message, 0, sizeof(message));
		if(is_b_option) for(set<int> :: iterator it = used_socket.begin() ; it != used_socket.end() ; it++) send(sock_number[*it], message, strlen(message), 0);
		else send(clnt_sock, message, strlen(message), 0);
	}

	used_socket.erase(index);
	unused_socket.insert(index);
	close(clnt_sock);
}

void print_error(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}