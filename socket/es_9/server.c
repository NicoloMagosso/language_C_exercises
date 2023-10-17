#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERVER_PORT 40000
#define DIMBUFF 512

int main(int argc, char* argv[])
{
	struct sockaddr_in servizio, rem_indirizzo;
	int pid, soa, socketfd, on = 1, fromlen = sizeof(servizio);

	memset((char*)&servizio, 0, sizeof(servizio));

	servizio.sin_family = AF_INET;
	servizio.sin_addr.s_addr = htonl(INADDR_ANY);
	servizio.sin_port = htons(SERVER_PORT);

	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketfd == -1)
	{
		perror("Errore nella creazione del socket");
		exit(1);
	}

	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if (bind(socketfd, (struct sockaddr*)&servizio, sizeof(servizio)) == -1)
	{
		perror("Errore nel binding");
		close(socketfd);
		exit(1);
	}

	if (listen(socketfd, 10) == -1)
	{
		perror("Errore nella listen");
		close(socketfd);
		exit(1);
	}

	for (;;)
	{
		printf("\n\nServer in ascolto...\n");
		fflush(stdout);

		soa = accept(socketfd, (struct sockaddr*)&rem_indirizzo, &fromlen);

		if (soa == -1)
		{
			perror("Errore nella accept");
			continue;
		}

		pid = fork();

		if (pid == 0)
		{
			char nome_file[20];
			close(socketfd);
			read(soa, nome_file, sizeof(nome_file));
			printf("invio nome file: %s\n", nome_file);
			fflush(stdout);
			close(1);
			dup(soa);
			close(soa);
			execl("/usr/bin/cat", "cat", nome_file, (char*)0);
			perror("Errore nell'esecuzione di execl");
			exit(1);
		}

		close(soa);
	}

	return 0;
}
