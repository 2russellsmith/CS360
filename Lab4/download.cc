

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <sys/epoll.h>

using namespace std;

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define HOST_NAME_SIZE      255
#define PATH_NAME_SIZE      255
#define MAX_GET             1024


int  main(int argc, char* argv[])
{
    int hSocket[NCONNECTIONS];
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
    char filePath[PATH_NAME_SIZE];
    int dflag = 0;
    int cflag = 0;
    int cvalue = 1;
    int index;
    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "dc:")) != -1){
        switch (c)
        {
            case 'd':
                dflag = 1;
                break;
            case 'c':
                cvalue = atoi(optarg);
                cflag = 1;
                break;
            case '?':
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                "Unknown option character `\\x%x'.\n", optopt);
                return 1;
        }
    }
    if(argv[optind] == NULL || argv[optind + 1] == NULL || argv[optind + 2] == NULL || argv[optind + 3] != NULL){
        printf("\nUsage: download [-d or -c number] host-name host-port file-path\n");
        return 0;
    }
    strcpy(strHostName,argv[optind]);
    nHostPort=atoi(argv[optind + 1]);
    strcpy(filePath, argv[optind + 2]);

    int epollfd = epoll_create(1);
    for(int i = 0; i < cvalue; i++){
        /* make a socket */
        hSocket[i]=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(hSocket[i] == SOCKET_ERROR)
        {
            printf("\nCould not make a socket. \n");
            return 0;
        }

        /* get IP address from name */
        pHostInfo=gethostbyname(strHostName);


        if(pHostInfo == NULL){
            printf("Could not resolve host. Please check you have a valid domain name.");
            return 0;
        }
        /* copy address into long */
        memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);
        
        
        /* fill address struct */
        Address.sin_addr.s_addr=nHostAddress;
        Address.sin_port=htons(nHostPort);
        Address.sin_family=AF_INET;
        if(dflag == 1)
            printf("\nConnecting to %s (%lx) on port %d",strHostName, nHostAddress,nHostPort);
    

        /* connect to host */
        if(connect(hSocket[i],(struct sockaddr*)&Address,sizeof(Address))
        == SOCKET_ERROR)
        {
            printf("\nCould not connect to host. Please check you have a valid address and port number.\n");
            return 0;
        }
        
        /* read from socket into buffer
        ** number returned by read() and write() is the number of bytes
        ** read or written, with -1 being that an error occured */
        //Create HTTP message
        char* message = (char*)malloc(MAX_GET);
        sprintf(message, "GET %s HTTP/1.1\r\nHost:%s:%d\r\n\r\n", filePath, strHostName, nHostPort);

        //EPOLLL STUFF
        struct epoll_event event;
        event.data.fd = hSocket[o];
        event.events = EPOLLIN;
        int ret = epoll_ctl(epollfd, EPOLL_CTRL_ADD, hSocket[i], &event);

        //Send Message
        write(hSocket[i], message, strlen(message));

        if(dflag == 1)
            printf("\nRequest: \n%s\n", message);
        memset(pBuffer, 0, BUFFER_SIZE);
        //Read response
        
        int len;
        string data = "";
        while( (len = read(hSocket[i], pBuffer, BUFFER_SIZE)) != 0 ){ 
            if( len == -1 ) {
                perror( "Read failed" );
                exit(1);
            }
            data.append(pBuffer);
            memset(pBuffer, 0, BUFFER_SIZE);
        }

        int endOfHeader = data.find("\r\n\r\n") + 4;
        string header = data.substr(0, endOfHeader);
        if(dflag == 1)
            printf("Response Header: \n%s\n", header.c_str());
        int index1 = header.find("Content-Len", 0) + 16;
        int index2 = header.find('\n', index1);
        int contentLength = atoi(header.substr(index1, index2 - index1).c_str());

        char *body = (char*)malloc(contentLength);
        sprintf(body, "%s", data.substr(endOfHeader, contentLength).c_str());   
        if(cflag != 1)
            printf("\n%s\n", body); 
        else
            printf("\n%s%d%s", "Successfully downloaded ", i + 1, " time(s).");
        if(dflag == 1)
            printf("\nClosing socket\n");
        /* close socket */
        if(close(event.data.fd) == SOCKET_ERROR)
        {
            printf("\nCould not close socket\n");
            return 0;
        }
        free(message);
        free(body);
    }
}
