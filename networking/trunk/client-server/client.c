#include "client.h"
#include "tools.h"

void start_client(char * server, char * port) {
	struct addrinfo hints, *serverinfo, *p;
	char serverIP[INET6_ADDRSTRLEN];
	char sendbuf[MAXLEN], recvbuf[MAXLEN];
	int sockfd;
	int ret;
	
	memset(&hints,0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	
	if((ret = getaddrinfo(server, port, &hints, &serverinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
		exit(1);
	}

	for (p = serverinfo; p !=NULL; p = p->ai_next ) {

		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if ((connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		exit(2);
	}
	
	inet_ntop(p->ai_family,get_in_addr((struct sockaddr*)p->ai_addr),serverIP,sizeof(serverIP));

	freeaddrinfo(serverinfo);

	/* Sending test data */
	/* test_data(sockfd); */

	/* TODO: Add the ability to detect server disconnects */
	while (fgets(sendbuf, MAXLEN, stdin)) {
		int r;
		if ((send(sockfd, sendbuf, strlen(sendbuf), 0)) == -1) {
			perror("send() call failed.");
			continue;
		}
		if ((r = recv(sockfd, recvbuf, MAXLEN, 0)) == -1) {
			perror("recv call() failed.");
			continue;
		}
		recvbuf[r] = '\0';
		fprintf(stdout, "%s", recvbuf);
	}
}

void test_data(int socket) {
	char buf[1000000];
	char recvbuf[MAXLEN];
	int i, r;

	for (i = 0; i < 1000000; i++) {
		sprintf(buf, "%d\n", i);
		if ((send(socket, buf, strlen(buf), 0)) == -1) {
			perror("send() call failed");
			continue;
		}

		if ((r = recv(socket, recvbuf, MAXLEN, 0)) == -1) {
			perror("recv call() failed");
			continue;
		}
		recvbuf[r] = '\0';
		fprintf(stdout, "%s", recvbuf);
	}
	exit(0);
}
