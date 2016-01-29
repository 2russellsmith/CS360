#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

string getFileExt(string filename) {
    string ext = filename.substr(filename.find_last_of(".") + 1);
    return ext;
}

string getFile(string rootPath, string argv, char *pBuffer, int hSocket)
{
    struct stat filestat;
    string fullPath = rootPath + argv;
    cout << endl << "Getting File " << fullPath << endl;
    if(stat(fullPath.c_str(), &filestat)) {
        cout << "ERROR in stat";
        sprintf(pBuffer, "%s", "HTTP/1.1 404 Page Not Found\r\n\r\n <html> I am sorry but we failed to find the page you are looking for. </html>");
        write(hSocket, pBuffer, strlen(pBuffer));
        return "";
    }
    FILE *fp;
    if(S_ISREG(filestat.st_mode)) {
        cout << fullPath.c_str() << " is a regular file \n";
        cout << "file size = " << filestat.st_size <<"\n";
        string result = "HTTP/1.1 200 OK\n";
        string fileType = getFileExt(fullPath);
        cout << "FILE TYPE: " << fileType << endl;

        if(fileType.compare("html") == 0){
            result.append("Content-Type: text/html\n");
            fp = fopen(fullPath.c_str(), "r");
        }else if(fileType.compare("jpg") == 0){
            result.append("Content-Type: image/jpg\n");
            fp = fopen(fullPath.c_str(), "rb");
        }else if(fileType.compare("gif") == 0){
            result.append("Content-Type: image/gif\n");
            fp = fopen(fullPath.c_str(), "rb");
        }else{
            result.append("Content-Type: text/plain\n");
            fp = fopen(fullPath.c_str(), "r");
        }

        result.append("Content-Length: ");
        result.append(to_string(filestat.st_size));
        result.append("\r\n\r\n");
        char *buffer = (char *)malloc(filestat.st_size);
        sprintf(pBuffer, "%s", result.c_str());
        fread(buffer, filestat.st_size, 1, fp);
        write(hSocket, pBuffer, strlen(pBuffer));
        write(hSocket, buffer, filestat.st_size);
        free(buffer);
        fclose(fp);
        return result;
    }
    if(S_ISDIR(filestat.st_mode)) {
        cout << fullPath.c_str() << " is a directory \n";
        DIR *dirp;
        struct dirent *dp;
        string result = "HTTP/1.1 200 OK\r\n\r\n <html><h1>" + fullPath + "</h1><ul>";
        dirp = opendir(fullPath.c_str());
        while ((dp = readdir(dirp)) != NULL){
            string name = dp->d_name;
            if(!strcmp(dp->d_name,"index.html")){
                return getFile(rootPath, argv + "/index.html", pBuffer, hSocket);
            }else if(!strcmp(dp->d_name,".")){
                result.append("<li><a href=\"/\">home</a></li>");
            }else if(strcmp(dp->d_name,"..")){
                if(dp->d_type == 0x8){
                    cout << "File " << fullPath;
                    result.append("<li><a href=\"" + argv + name + "\">" + dp->d_name + " </a></li>");
                }else{
                    cout << "Dir " << fullPath;
                    result.append("<li><a href=\"" + argv + name + "/\">" + dp->d_name + " </a></li>");
                }
            }
        }
        result.append("</ul></html>");
        (void)closedir(dirp);
        sprintf(pBuffer, "%s", result.c_str());
        write(hSocket, pBuffer, strlen(pBuffer));
        return result;
    }
    return "HTTP/1.1 500 Internal Server Error\r\n\r\n <html> I am sorry but we seem to be broken. </html>";

}


#define SOCKET_ERROR        -1
#define BUFFER_SIZE         500
#define MESSAGE             "This is the message I'm sending back and forth"
#define QUEUE_SIZE          5

int main(int argc, char* argv[])
{
    int hSocket,hServerSocket;  /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address; /* Internet socket address stuct */
    int nAddressSize=sizeof(struct sockaddr_in);
    char pBuffer[BUFFER_SIZE];
    int nHostPort;
    string rootPath;

    if(argc < 3)
      {
        printf("\nUsage: server host-port dir\n");
        return 0;
      }
    else
      {
        nHostPort=atoi(argv[1]);
        rootPath = argv[2];
      }

    printf("\nStarting server");

    printf("\nMaking socket");
    /* make a socket */
    hServerSocket=socket(AF_INET,SOCK_STREAM,0);

    if(hServerSocket == SOCKET_ERROR)
    {
        printf("\nCould not make a socket\n");
        return 0;
    }

    /* fill address struct */
    Address.sin_addr.s_addr=INADDR_ANY;
    Address.sin_port=htons(nHostPort);
    Address.sin_family=AF_INET;

    printf("\nBinding to port %d",nHostPort);

    /* bind to a port */
    if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address)) 
                        == SOCKET_ERROR)
    {
        printf("\nCould not connect to host\n");
        return 0;
    }
 /*  get port number */
    getsockname( hServerSocket, (struct sockaddr *) &Address,(socklen_t *)&nAddressSize);
    printf("opened socket as fd (%d) on port (%d) for stream i/o\n",hServerSocket, ntohs(Address.sin_port) );

        printf("Server\n\
              sin_family        = %d\n\
              sin_addr.s_addr   = %d\n\
              sin_port          = %d\n"
              , Address.sin_family
              , Address.sin_addr.s_addr
              , ntohs(Address.sin_port)
            );


    printf("\nMaking a listen queue of %d elements",QUEUE_SIZE);
    /* establish listen queue */
    if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
    {
        printf("\nCould not listen\n");
        return 0;
    }

    memset(pBuffer,0,sizeof(pBuffer));
    for(;;)
    {
        printf("\nWaiting for a connection\n");
        /* get the connected socket */
        memset(pBuffer,0,sizeof(pBuffer));
        hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);

        printf("\nGot a connection from %X (%d)\n",
              Address.sin_addr.s_addr,
              ntohs(Address.sin_port));
        int status = read(hSocket,pBuffer,BUFFER_SIZE);
        if(status == 0 || status == -1){

        }else{
            printf("Got from the browser: \n%s\n", pBuffer);
            int len;
            string data = "";
            data.append(pBuffer);
            int startOfFile = data.find("GET ") + 4;
            int endOfFile = data.find("HTTP") - 1;
            string fileName = data.substr(startOfFile, endOfFile - startOfFile);
            cout << "FileName\n" + fileName + "\n";
            cout << "rootPath\n" + rootPath + "\n";
            memset(pBuffer,0,sizeof(pBuffer));
            // sprintf(pBuffer, "%s", getFile(rootPath + fileName).c_str());
            getFile(rootPath, fileName, pBuffer, hSocket);
            // write(hSocket, pBuffer, strlen(pBuffer));
        }
        printf("\nClosing the socket");
        memset(pBuffer,0,sizeof(pBuffer));
        /* close socket */
        if(close(hSocket) == SOCKET_ERROR)
        {
         printf("\nCould not close socket\n");
         return 0;
        }
    }
}