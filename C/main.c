#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <cjson/cJSON.h>
#include <strings.h>
#include <memory.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <curl/curl.h>
#include <pthread.h>

char * toJson(char **);
char ** parseToString(char * string);
void *readUSB(void *);
void curl(char * postthis, char * url);

const char *port = "/dev/ttyUSB0";
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
char buffer[255];
char usbData[255];
char oldBuffer[255];
int STOP = 0;
int main() {

    pthread_t arduino, conversion;
    int idth = 1;
    pthread_create(&arduino,NULL,readUSB,NULL);
    pthread_detach(arduino);
    strcpy(oldBuffer,usbData);
    while(1) {
        pthread_mutex_lock(&mtx);

        if(strcmp(oldBuffer,usbData)!=0) {
            puts("");
            char **parse = parseToString(usbData);
            if (parse[0] != "Error") {
                char *tmp = toJson(parse);
                curl(tmp, "https://webservice-deploy.herokuapp.com/create");
                free(tmp);
            } else {
                puts("Erreur de parse");
            }
        }
        strcpy(oldBuffer, usbData);
        pthread_mutex_unlock(&mtx);
    }
    pthread_mutex_destroy(&mtx);
}
char * toJson(char ** toConvert)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "timestamps", *(toConvert+4));

    cJSON * weather = cJSON_CreateArray();
    cJSON * weatherKind = cJSON_CreateObject();
    cJSON * weatherKind2 = cJSON_CreateObject();

    cJSON_AddStringToObject(weatherKind,"0",*(toConvert+1));
    cJSON_AddItemToArray(weather,weatherKind);

    cJSON_AddStringToObject(weatherKind2,"1",*(toConvert+3));
    cJSON_AddItemToArray(weather,weatherKind2);

    cJSON_AddItemToObject(root,"weather", weather);
    char * message = malloc(sizeof(root));
    message = cJSON_Print(root);
    return message;
}

char **parseToString(char * string)
{
    char **tab =  malloc(sizeof(char*)*5);

    int a,b,c;
    char d[255], e[255], ca[255], cb[255], cc[255];
    int res = sscanf(string,"%d %s %d %s %d",&a,d,&b,e,&c);
    //printf("RES : %d |", res);
    if(res > 0) {
        sprintf(ca, "%d", a);
        sprintf(cb, "%d", b);
        sprintf(cc, "%d", c);
        tab[0] = malloc(sizeof(ca));
        strcpy(tab[0], ca);

        tab[1] = malloc(sizeof(d));
        strcpy(tab[1], d);

        tab[2] = malloc(sizeof(cb));
        strcpy(tab[2], cb);

        tab[3] = malloc(sizeof(e));
        strcpy(tab[3], e);

        tab[4] = malloc(sizeof(cc));
        strcpy(tab[4], cc);
        return tab;
    }
    tab[0] = "Error";
    return tab;
}
void *readUSB(void * args){
    //////////////////////////////////////////////
    //Lire port serie, lecture canonique (Attend un NL ou CR)
    int USB = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);//O_NDELAY
    if (USB < 0)
        perror(USB);

    struct termios oldtoptions, toptions;

    //Option
    tcgetattr(USB, &oldtoptions);       //Save actual option
    bzero(&toptions, sizeof(toptions)); // clear strut for new port setting
    /*
     * B9600    --> Baudrate
     * CRTSCTS  --> Output hardware control flow ( only use if cable has all necessary lines)
     * CS8      --> 8N1 (8BIT, NO PARITY, 1 STOP BIT)
     * CLOCAL   --> Local connection, no modem control
     * CREAD    --> Enable receiving characters
     */

    toptions.c_cflag = B9600 | CRTSCTS | CS8 | CLOCAL | CREAD;

    /*
     * IGNPAR   --> Ignore bytes parity error
     * ICRNL    --> Map CR to NL (otherwise a CR input on the other computer will not terminate)
     */
    toptions.c_iflag = IGNPAR | ICRNL | IXON;

    /*
     * ICANON   --> Enable canoncical input
     */
    //toptions.c_oflag = ;

    /*
     * Initialize allcontrol character
     * Default values can be found in /usr/include/termios.h
     */
    toptions.c_cc[VINTR] = 0; //ctrl-c
    toptions.c_cc[VQUIT] = 0; //ctrl-v
    toptions.c_cc[VERASE] = 0; //del
    toptions.c_cc[VKILL] = 0; //@
    toptions.c_cc[VEOF] = 4; //ctrl-d
    toptions.c_cc[VTIME] = 0; // inter-character timer
    toptions.c_cc[VMIN] = 1; //block until read 1 character
    toptions.c_cc[VSWTC] = 0; //'\0'
    toptions.c_cc[VSTART] = '<'; //ctrl-q
    toptions.c_cc[VSTOP] = '>'; //ctrl-s
    toptions.c_cc[VSUSP] = 0; //ctrl-z
    toptions.c_cc[VEOL] = 0; //\'0'
    toptions.c_cc[VREPRINT] = 0; //ctrl-r
    toptions.c_cc[VDISCARD] = 0; //ctrl-u
    toptions.c_cc[VWERASE] = 0; //ctrl-w
    toptions.c_cc[VLNEXT] = 0; //ctrl-v
    toptions.c_cc[VEOL2] = 0; //'\0'

    //Clean the serial
    tcflush(USB, TCIFLUSH);
    tcsetattr(USB, TCSANOW, &toptions);

    int done = 1;
    char *string;
    while(1) {
        pthread_mutex_lock(&mtx);
        for (int i = 0; i < 255; i++)
            *(buffer + i) = NULL;
        while (STOP == 0) {
            ssize_t n = read(USB, buffer, 255);
            if (n < 0) {
                //printf("Length %d ",(int)n);
                //perror("Error ");
                //printf("errno = %d \n", errno);
            }
            if ((int) strlen(buffer) > 0) {

                strcpy(usbData,buffer);
                STOP = 1;
            }

            for (int i = 0; i < 255; i++)
                *(buffer + i) = NULL;
            pthread_mutex_unlock(&mtx);
            STOP = 0;
        }
    }

    tcsetattr(USB, TCSANOW, &oldtoptions);
    close(USB);
}
void curl(char * postthis, char * url)
{
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl == NULL) {
        return 128;
    }
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    //https://webservice-deploy.herokuapp.com/create
    //"172.30.1.119:5000/simplePost"
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}
