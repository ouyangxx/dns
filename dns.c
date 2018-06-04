#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif
#include "dns.h"

int gethostbydomain(const char *domain, char *ipAddrBuf, int size)
{
    int ret = -1;
    #ifdef WIN32
    WSADATA wsaData;
    int iResult = 0;
    struct hostent *host;
    struct in_addr addr;
    iResult = WSAStartup(MAKEWORD(2,2),&wsaData); /*init winsocket*/
    if (iResult!=0)
    {
        fprintf(stderr, "init winsocket failed\n");
        return -1;
    }
    host = gethostbyname(domain);
    if(NULL == host){
        fprintf(stderr, "gethostbyname error:%d" ,WSAGetLastError());
        return -1;
    }
    if (host->h_addrtype == AF_INET) {
        if (*(host->h_addr_list) != NULL)
        {
            addr.s_addr = *(u_long*)(*(host->h_addr_list));
            char *ipAddr = inet_ntoa(addr);
            memset(ipAddrBuf, 0, size);
            strncpy(ipAddrBuf, ipAddr, size);
            ipAddrBuf[size-1] = '\0';
            ret = 0;
        }
    }
    #else
    struct addrinfo hints;
    struct addrinfo *result, *result_pointer;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_CANONNAME;
    hints.ai_protocol = 0;  /* any protocol */
    ret = getaddrinfo(domain, NULL, &hints, &result);
    if (ret != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return -1;
    }
    /* traverse the returned list and output the ip addresses */
    for (result_pointer = result; result_pointer != NULL; result_pointer = result_pointer->ai_next)
    {
        if (AF_INET == result_pointer->ai_family)
        {
            //struct sockaddr_in *sockaddr_ipv4 = (struct sockaddr_in *)result_pointer->ai_addr;
            //char *ipAddr = inet_ntoa(sockaddr_ipv4->sin_addr);
            char ipAddr[1024+1] = "";
            ret = getnameinfo(result_pointer->ai_addr, result_pointer->ai_addrlen, ipAddr, sizeof(ipAddr), NULL, 0, NI_NUMERICHOST);
            if (ret != 0)
            {
                fprintf(stderr, "error in getnameinfo: %s \n", gai_strerror(ret));
                continue;
            }
            memset(ipAddrBuf, 0, size);
            strncpy(ipAddrBuf, ipAddr, size);
            ipAddrBuf[size-1] = '\0';
            ret = 0;
            break;
        }
    }
    freeaddrinfo(result);
    #endif
    return ret;
}

#if 0
int main()
{
    char ipAddr[1024];
    if (gethostbydomain("www.baidu.com", ipAddr, 1024) == 0)
    {
        printf("ip: %s\n", ipAddr);
    }
}
#endif
