/* THI NGUYEN CS475 F2017*/
#include <stdio.h>
#include "csapp.h"
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */

static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *hostHeader = "Host: %s\r\n";

void *thread(void *vargp);

void doit(int connfd);
void parse_uri(char *uri,char *hostname,char *path,int *port);
void clienterror(int fd, char *cause, char *errnum, 
         char *shortmsg, char *longmsg);

int main(int argc,char **argv)
{
    int listenfd, *connfd;
    char hostname[MAXLINE],port[MAXLINE];
    socklen_t  clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    
    /* Check command line args */
    if(argc != 2){
        fprintf(stderr,"Usage :%s <port> \n",argv[0]);
        exit(1);
    }

    /* The open_listenfd function returns a listening descriptor 
        that is ready to receive connection requests on port argv[1].*/
    listenfd = Open_listenfd(argv[1]);
    while(1){
        clientlen = sizeof(clientaddr);
        connfd = Malloc(sizeof(int));
        *connfd = Accept(listenfd,(SA *)&clientaddr,&clientlen);

        /*print accepted message*/
        Getnameinfo((SA*)&clientaddr,clientlen,hostname,MAXLINE,
                    port,MAXLINE,0);
        printf("Accepted connection from (%s %s).\n",hostname,port);
        Pthread_create(&tid,NULL,thread,connfd);
    }
    return 0;
}

void *thread(void *vargp) {
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    doit(connfd);
    Close(connfd);
    return NULL;
}

/*handle the client HTTP transaction */
void doit(int connfd)
{
    char buf[MAXLINE],method[MAXLINE],uri[MAXLINE],version[MAXLINE], responseBuf[MAXLINE];
    /*store the request line arguments*/
    char hostname[MAXLINE],path[MAXLINE];
    int port, serverfd; /*the end server file descriptor*/

    rio_t client_rio,server_rio;

    Rio_readinitb(&client_rio,connfd);
    Rio_readlineb(&client_rio,buf,MAXLINE);

    /*
        At this point, if the request header from client in the right format,
        the buf should have this similar structure:
        GET http://cs.gmu.edu(:port)/contact/index.html HTTP/1.1
    */

    /* parse the method, uri, version from buf after reading the client request line */
    sscanf(buf,"%s %s %s",method,uri,version); 
    /*
        method = GET, uri = http://cs.gmu.edu/contact/index.html , version = HTTP/1.1
    */ 

    /* If method is not GET, send error message to client */
    if(strcasecmp(method,"GET")){
        clienterror(connfd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    }
    printf("method %s\n", method );
    printf("uri %s\n", uri);
    printf("version %s\n", version); 

    /* parse the uri to get hostname, file path, and port */ 
    parse_uri(uri,hostname,path,&port);
    printf("After parse_uri\n");
    printf("uri %s\n", uri);
    printf("hostname %s\n", hostname);
    printf("path %s\n", path);
    printf("port%d\n", port);

     /* The open_clientfd function establishes a connection with a server running on
        host hostname and listening for connection reqests on port number port */
    /*connect to the end server*/
    char portNumber[20];
    sprintf(portNumber,"%d",port);
    serverfd = Open_clientfd(hostname,portNumber);
    if(serverfd<0){
        printf("connection error\n");
        return;
    }

    Rio_readinitb(&server_rio,serverfd);
    /*write the http requests to endserver*/
    char line[MAXLINE], host_hdr[MAXLINE];
    sprintf(line,"GET %s HTTP/1.0\r\n",path);
    sprintf(host_hdr,hostHeader,hostname);
    Rio_writen(serverfd,line,strlen(line));
    while (Rio_readlineb(&client_rio, line, MAXLINE) > 2)
    {
        if (!strncasecmp(line,"Proxy-Connection",strlen("Proxy-Connection")))
            strcpy(line, "Proxy-Connection: close\r\n");
        if (!strncasecmp(line,"Connection",strlen("Connection")))
            strcpy(line, "Connection: close\r\n");
        if(!strncasecmp(line,"User-Agent",strlen("User-Agent")))
            strcpy(line, user_agent_hdr);
        if (!strncasecmp(line,"Host",strlen("Host")))
            strcpy(host_hdr,line);

        printf("header from client: %s", line);
        
        Rio_writen(serverfd, line, strlen(line));

    }
    Rio_writen(serverfd, "\r\n", 2);

    /*receive message from end server and send to the client*/
    size_t n;
    while((n=Rio_readlineb(&server_rio,responseBuf,MAXLINE))!=0)
    {
        printf("proxy received %d bytes from server and then send to client\n", (int)n);
        Rio_writen(connfd,responseBuf,n);
    }
    Close(serverfd);
}


/* parse the uri with this format http://cs.gmu.edu/contact/index.html
        If sucessully, 
        hostname: cs.gmu.edu
        file path: /contact/index.html

*/

void parse_uri(char *uri,char *hostname,char *path,int *port)
{
    printf("In parse_uri \n");
    // Ex URI:  http://cs.gmu.edu:8080/contact/index.html 
    *port = 80; // default port if the port optional field is not provided
    /* 
        doubleSlashPos = the pointer to the first occurrence of // in uri 
        if doubleSlashPos = NULL, // is not found
    */
    char* doubleSlashPos = strstr(uri,"//");
    if (doubleSlashPos) {
        // if there is //, then get the first postion of cs.gmu.edu:8080/contact/index.html 
        doubleSlashPos = doubleSlashPos+2;
    }
    else {
        doubleSlashPos = uri;
    }
    printf("portOrPathPos %s\n", doubleSlashPos);
    /*  
        get the port in case of the uri in this format 
        http://cs.gmu.edu:8080/contact/index.html 
        search for the first occurence of : in string after doubleSlashPos
     */
    char* portOrPathPos = strstr(doubleSlashPos,":");

    // if uri includes port 
    if(portOrPathPos!=NULL) {
        printf("1. portOrPathPos %s\n", portOrPathPos);
        *portOrPathPos = '\0';
        printf("2. portOrPathPos %s\n", portOrPathPos);
        sscanf(doubleSlashPos,"%s",hostname); // E.x: get hostname = cs.gmu.edu from cs.gmu.edu:8080/contact/index.html 
        
        sscanf(portOrPathPos+1,"%d%s",port,path); // E.x: get port = 8080, path = /contact/index.html 
    }
    else {
        // if uri doesn't have port but have path 
        portOrPathPos = strstr(doubleSlashPos,"/");
        if(portOrPathPos!=NULL) {
            printf("1. portOrPathPos %s\n", portOrPathPos);
            *portOrPathPos = '\0';
            printf("2. portOrPathPos %s\n", portOrPathPos);
            sscanf(doubleSlashPos,"%s",hostname);
            printf("hostname %s\n", hostname);
            *portOrPathPos = '/';
            sscanf(portOrPathPos,"%s",path);
        }
        else {
            // if uri just have hostname
            printf("uri just have hostname\n");
            printf("%s\n", doubleSlashPos);
            sscanf(doubleSlashPos,"%s",hostname);
        }
    }
     // check if the hostname contain www., if so, remove it
    printf("hostname before check www %s\n", hostname);
    char www[4];
    strcpy(www, "www.");
    char toCheck[5]; 
    strncpy(toCheck, hostname, 4);
    toCheck[4] ='\0';
    printf("toCheck %s\n", toCheck);
    printf("www %s\n", www);
    if (strcmp(www, toCheck) == 0) {
        sscanf(hostname+4,"%s",hostname);
    }
    printf("hostname after check %s\n", hostname);
    return;
}

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
         char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Proxy Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Proxy</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}
/* $end clienterror */











