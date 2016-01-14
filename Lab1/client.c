#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define HOST_NAME_SIZE      255
#define PATH_NAME_SIZE      255

int  main(int argc, char* argv[])
{
    int hSocket;                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address stuct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    int nHostPort;
    char filePath[PATH_NAME_SIZE];

    if(argc < 3)
      {
        printf("\nUsage: client host-name host-port\n");
        return 0;
      }
    else
      {
  int dflag = 0;
  char *cvalue = NULL;
  int index;
  int c;

  opterr = 0;
  while ((c = getopt (argc, argv, "c:d::")) != -1){
    switch (c)
      {
      case 'd':
        dflag = 1;
        break;
      case 'c':
        cvalue = optarg;
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
  }
    if(cvalue != NULL){
	    int cval = atoi(cvalue);
  		printf ("dflag = %d, cvalue = %d\n",
          dflag, cval);
	}
  	int iteration = 0;
  	for (index = optind; index < argc; index++){
  		if(iteration == 0){
  			strcpy(strHostName,argv[index]);
  		}else if(iteration == 1){
		    nHostPort=atoi(argv[index]);
  		}else if(iteration == 2){
			strcpy(filePath, argv[index]);
  		}else{
        	printf("\nUsage: client host-name host-port\n");
  			return 0;
  		}
  		iteration++;
  	  	printf ("Non-option argument %s : %d\n", argv[index], index);
    }
}

    printf("\nMaking a socket");
    /* make a socket */
    hSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(hSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* get IP address from name */
    pHostInfo=gethostbyname(strHostName);
    /* copy address into long */
    memcpy(&nHostAddress,pHostInfo->h_addr,pHostInfo->h_length);

    /* fill address struct */
    Address.sin_addr.s_addr=nHostAddress;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);

    /* connect to host */
    if(connect(hSocket,(struct sockaddr*)&Address,sizeof(Address)) 
       == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }

    /* read from socket into buffer
    ** number returned by read() and write() is the number of bytes
    ** read or written, with -1 being that an error occured */
    nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
    printf("\nReceived \"%s\" from server\n",pBuffer);
    /* write what we received back to the server */
    write(hSocket,pBuffer,nReadAmount);
    printf("\nWriting \"%s\" to server",pBuffer);

    printf("\nClosing socket\n");
    /* close socket */                       
    if(close(hSocket) == SOCKET_ERROR)
    {
        printf("\nCould not close socket\n");
        return 0;
    }
}
