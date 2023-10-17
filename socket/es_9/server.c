#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#define SERVER_PORT 40000
#define SOCKET_ERROR ((int)-1)
#define DIMBUFF 512

int main(int argc, char *argv[])
{

    struct sockaddr_in servizio, rem_indirizzo;
    int pid, nread, soa, socketfd, client_len, fd, on = 1, fromlen = sizeof(servizio);

    memset((char *)&servizio, 0, sizeof(servizio));

    servizio.sin_family = AF_INET;
    servizio.sin_addr.s_addr = htonl(INADDR_ANY);
    servizio.sin_port = htons(SERVER_PORT);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    bind(socketfd, (struct sockaddr *)&servizio, sizeof(servizio));

    listen(socketfd, 10);

    for (;;)
    {
        printf("\n\nServer in ascolto...\n");
        fflush(stdout);

        soa = accept(socketfd, (struct sockaddr *)&rem_indirizzo, &fromlen);

        if (pid == 0)
        {
            char nome_file[256];
            ssize_t nread = read(soa, nome_file, sizeof(nome_file));

            if (nread < 0)
            {
                perror("Errore nella lettura del nome del file");
                close(soa);
                exit(1);
            }

            nome_file[nread] = '\0';
            printf("Richiesta del file: %s\n", nome_file);

            close(socketfd);

            if (dup2(soa, 1) == -1)
            {
                perror("Errore nel redirect dell'output verso il socket");
                close(soa);
                exit(1);
            }

            close(soa);

            if (execl("/bin/cat", "cat", nome_file, NULL) == -1)
            {
                perror("Errore nell'esecuzione di execl");
                exit(1);
            }
        }

        close(socketfd);
    }

    return 0;
}