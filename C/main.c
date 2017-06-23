#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <cjson/cJSON.h>
#include <cjson/cJSON_Utils.h>
//Include serial port
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

const char *port = "/dev/ttyUSB0";
char buffer[255];
int STOP = 0;

char *toJson();

int main (int argc, char *argv[]) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl == NULL) {
        return 128;
    }

    char* jsonObj = "{\"limoPrix\" : \"1\" , \"prodLimo\" : \"2\"}";
    //char * tmp = toJson();
    //malloc(sizeof(tmp));
    //char * jsonObj = toJson();
    //strcpy(jsonObj,toJson());
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "charsets: utf-8");
    //https://webservice-deploy.herokuapp.com/create
    curl_easy_setopt(curl, CURLOPT_URL, "172.30.1.119:5000/simplePost");

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonObj);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");

    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return res;
}
char *toJson()
{
    cJSON *root = malloc(sizeof(cJSON));
    cJSON *fld = malloc(sizeof(cJSON));
    char * message = malloc(sizeof(char *));
    char * toSend = malloc(sizeof(char *));
    //root  = cJSON_CreateArray();
    //int index = 0, oldIndex = 0;
    char nameField[4][4] = {"d","w","v","t"};
    int beginString = 1;
    printf("<<<%c>>>", *(buffer+1));
    if(*buffer == '*') {
        for (int z = 0; z < 4; z++) {
            //char * pos = strstr(buf,"/");
            //puts("|1|");

            char *posSlash = strchr(buffer + beginString, '/');
            //printf("<TMP %s >\n",posSlash);

            int endString = (int) (posSlash - buffer);
            //printf("<<endString %d >\n", endString);

            //printf("<beginString %d >\n",beginString);

            //puts("|memcpy|");
            memcpy(toSend, (buffer + beginString), (size_t) (endString - beginString));
            beginString = endString + 1;
            char *target = NULL;
            target = malloc(strlen(toSend) + 1);
            strcpy(target, toSend);
            //(toSend)= '\0';
            //      (endString-beginString) = '\0';
            //if(z == 3 && isdigit(toSend))
            //printf("< %s >", target);
            //printf("|t %s %s t|",&buffer,tmp);
            // printf("|t %s %s t|",buffer-tmp,tmp);
            /*while ( < pos ) {
                index++;
                printf("%d",index);
        }*/
            if (posSlash != NULL) {
                //    puts("|2|");

                root = cJSON_CreateObject();
                //puts("|3|");
                char * field = nameField[z];
                cJSON_AddStringToObject(root, field, target);
                //puts("|4|");
                message = cJSON_Print(root);
                //printf("<<<<<< %s >>>>>>",message);
                //cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
                //char *tmp;
                //concat(tmp,buf,index-oldIndex);

                //char field= nameField[z];
                //unsigned long e = (buffer - tmp);
                //puts("|5|");
                //printf("<%s>",test);
                //cJSON_AddStringToObject(fld, &field, &e);
                //  puts("|5|");
                //oldIndex = index;

            }
            //puts("|5|");
        }
    }
    // puts("|6|");
    return message;
}
