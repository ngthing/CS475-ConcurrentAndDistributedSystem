#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);

void clienterror(int fd, char *cause, char *errnum, 
         char *shortmsg, char *longmsg);
void parse_uri(char *uri,char *hostname,char *path,int *port);

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    /* Check command line args */
    if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //line:netp:tiny:accept
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, 
                    port, MAXLINE, 0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);                                             //line:netp:tiny:doit
    Close(connfd);                                            //line:netp:tiny:close
    }
}

/*
 * doit - handle one HTTP request/response transaction
 */
/* $begin doit */
void doit(int fd) 
{
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    /*store the request line arguments*/
    char hostname[MAXLINE],path[MAXLINE];
    int port;
    rio_t rio;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))  //line:netp:doit:readrequest
        return;
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);       //line:netp:doit:parserequest
    if (strcasecmp(method, "GET")) {                     //line:netp:doit:beginrequesterr
        clienterror(fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    } 
    printf("method %s\n", method );
    printf("uri %s\n", uri);
    printf("version %s\n", version);                                                   //line:netp:doit:endrequesterr
     /* parse the uri to get hostname, file path, and port */ 
    parse_uri(uri,hostname,path,&port);
    printf("After parse_uri\n");
    printf("uri %s\n", uri);
    printf("hostname %s\n", hostname);
    printf("path %s\n", path);
    printf("port%d\n", port);
    read_requesthdrs(&rio);                              //line:netp:doit:readrequesthdrs

}
/* $end doit */

/*
 * read_requesthdrs - read HTTP request headers
 */
/* $begin read_requesthdrs */
void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {          //line:netp:readhdrs:checkterm
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    }
    return;
}
void parse_uri(char *uri,char *hostname,char *path,int *port)
{
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
            sscanf(portOrPathPos,"%s",hostname);
        }
    }
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
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

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

