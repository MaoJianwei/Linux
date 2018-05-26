#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>


void print_family(struct addrinfo *aip){
    printf(" family ");
    switch(aip->ai_family){
        case AF_INET:
        printf("inet");
        break;
        case AF_INET6:
        printf("inet6");
        break;
        case AF_UNIX:
        printf("unix");
        break;
        case AF_UNSPEC:
        printf("unspecified");
        break;
        default:
        printf("unknown");
    }
}

void print_type(struct addrinfo *aip){
    printf(" type ");
    switch (aip->ai_socktype){
        case SOCK_STREAM:
            printf("stream");
            break;
        case SOCK_DGRAM:
            printf("datagram");
            break;
        case SOCK_SEQPACKET:
            printf("seqpacket");
            break;
        case SOCK_SOCK_RAW:
            printf("raw");
            break;
        default:
            printf("unknown (%d)", aip->ai_socktype);
    }
}

void print_protocol(struct addrinfo *arp){
    printf(" protocol ");
    switch (aip->ai_protocol){
        case 0:
            printf("default");
            break;
        case IPPROTO_TCP:
            printf("TCP");
            break;
        case IPPROTO_UDP:
            printf("UDP");
            break;
        case IPPROTO_RAW:
            printf("raw");
            break;
        default:
            printf("unknown (%d)", aip->ai_protocol);
    }
}

void printf_flags(struct addrinfo *aip){
    printf("flags");
    if(aip->ai_flags==0)
        printf(" 0 ");
    else{
        if(aip->flags & AI_PASSIVE)
            printf(" passvie");
        if(aip->flags & AI_CANONNAME)
            printf(" canon");
        if(aip->flags & AI_NUMERICHOST)
            printf(" numhost");
        if(aip->flags & AI_NUMERICSERV)
            printf(" numserv");
        if(aip->flags & AI_V4MAPPED)
            printf(" v4mapped");
        if(aip->flags & AI_ALL)
            printf(" all");
    }
}

int main(int agrc, char *argv[]){
    struct addrinfo     *ailist, *aip;
    struct addrinfo     hints;
    struct sockaddr_in  *sinp;
    const char          *addr;
    int                 err;
    char                abuf[INET_ADDRSTRLEN];

    if(argc != 3)
        perror("usage: %s nodename service", argv[0]);
    hints.ai
}