#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <malloc.h>
#include <termios.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

/////////////////////////////////////
//Variable
const char * SPName = "/dev/sda6";

/////////////////////////////////////
//FUNCTION
int ouvertureSerialPort(char * name);
int * ouvertureSocket();
struct sockaddr_in * setServer();
int connectToServeur(int socket, struct sockaddr_in * server);
int sendData(int socket, char * message);

//void setOptions(struct termios, int USB);

/////////////////////////////////////
int main() {
    /* Error Handling */
    int * socket = ouvertureSocket();
    if(*socket > 0)
        puts("Erreur de creation Socket");
    struct sockaddr_in * serveur =  setServer();
    int connect = connectToServeur(socket, serveur);
    if(connect)
    {
        puts("Connect error");
    }
    puts("Connected");
    /*
    struct termios toptions;
    int USB = ouverturePort(SPName);
    if ( USB < 0 ) {
        puts("Erreur sur l'ouverture du port");
    }
     */
    return 0;
}

int ouvertureSerialPort(char * name)
{
    int USB = open( "/dev/ttyUSB0", O_RDWR| O_NOCTTY);
    return USB;
}
int * ouvertureSocket()
{
    int * socket_desc = (int *)malloc(sizeof(int));
    *socket_desc = socket(AF_INET, SOCK_STREAM,0);
    return socket_desc;
}
struct sockaddr_in* setServer()
{
    struct sockaddr_in * server = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    server->sin_addr.s_addr = inet_addr("127.0.0.1");
    server->sin_family = AF_INET;
    server->sin_port = htons(5000);
    return server;
}
int connectToServeur(int socket, struct sockaddr_in * server)
{
    if(connect(socket , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        return 1;
    }
    return 0;
}
int sendData
/*void setOptions(struct termios toptions, int USB)
{
    tcgetattr(USB,&amp;amp;toptions);
}*/