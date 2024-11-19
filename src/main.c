#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

#define SERVICE_PORT "6980"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_BUF_LEN 1048


int main(void) {
    struct addrinfo hints, *res;
    int sockfd, newfd;
    struct sockaddr_storage their_addr;

    // first, load up address structs with getaddrinfo():
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int exit_code = getaddrinfo(NULL, SERVICE_PORT, &hints, &res);
    if (exit_code != 0) {
      perror("failed to get addrinfo");
      return errno;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("failed socket creation...\n");
        return -1;
    }

    exit_code = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if (exit_code != 0) { 
      return errno;
    }

    exit_code = listen(sockfd, BACKLOG);
    if (exit_code == -1) {
        perror("failed to listen");
        return -1;
    }

    socklen_t addr_size = sizeof their_addr;
    newfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (newfd == -1) {
        perror("failed to accept new connection");
        return -1;
    }


    while (1==1) {
        char *buf[MAX_BUF_LEN];
        socklen_t addr_len = sizeof their_addr;

        if (recv(newfd, &buf, MAX_BUF_LEN-1, 0) == -1) {
            perror("recvfrom failed");
            return -1;
        }

        printf("received message: %s\n", *buf);
    };

    shutdown(sockfd, 2);

    return 0;
}
