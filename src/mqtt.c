/*!
 * @file     router.c
 * @brief    Input command will come here.
 *
 * This module contains router type definitions used in various applications.
 *
 * @copyright Copyright (C) 2020 FeiG(SI China MAC).\n
 *            All rights reserved.
 */

#define HAVE_STRUCT_TIMESPEC

#if !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include "pahoc/MQTTClient.h"
#include "../include/pthread/pthread.h"
#include "../include/itag/config.h"
#include "../lib/cJSON/cJSON.h"
#include "defs.h"
int CONNECT = 1;
volatile MQTTClient_deliveryToken deliveredtoken;
void msgDelivered(void *context, MQTTClient_deliveryToken dt)
{
  printf("Message with token value %d delivery confirmed\n", dt);
  deliveredtoken = dt;
}

int msgArriving(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
  int i;
  char* payload;
  printf("Message arrived\n");
  printf(" topic: %s\n", topicName);
  printf(" message: ");
  payload = message->payload;
  if(strcmp(payload, DISCONNECT) == 0) {
    printf(" \n out!!");
    CONNECT = 0;
  }
  for(i=0; i<message->payloadlen; i++) {
    putchar(*payload++);
  }
  printf("\n");
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

void connLost(void *context, char *cause) {
  fprintf(stderr, "Connection lost, cause: %s\n", cause);
}

void *subClient(void *threadId) {
  long tid;
  tid = (long)threadId;
  fprintf(stdout, "subClient thread #%ld!\n", tid);

  MQTTClient client;
  MQTTClient_connectOptions optsSub = MQTTClient_connectOptions_initializer;
  int rc;
  int ch;
  MQTTClient_create(&client, ADDRESS, SUB_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  optsSub.keepAliveInterval = 20;
  optsSub.cleansession = 1;
  optsSub.username = USERNAME;
  optsSub.password = PASSWORD;
  MQTTClient_setCallbacks(client, NULL, connLost, msgArriving, msgDelivered);
  if ((rc = MQTTClient_connect(client, &optsSub)) != MQTTCLIENT_SUCCESS) {
    printf("Failed to connect, return code %d\n", rc);
    exit(EXIT_FAILURE);
  }
  printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
  "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
  MQTTClient_subscribe(client, TOPIC, QOS);
  do {
    ch = getchar();
  } while(ch!='Q' && ch != 'q');
  MQTTClient_unsubscribe(client, TOPIC);
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
  pthread_exit(NULL);
}

void *pubClient(void *threadid) {
  long tid;
  tid = (long)threadid;
  int count = 0;
  fprintf(stdout, "pubClient thread #%ld!\n", tid);

  //声明一个MQTTClient
  MQTTClient client;
  //初始化MQTT Client选项
  MQTTClient_connectOptions optsPub = MQTTClient_connectOptions_initializer;
  //使用参数创建一个client，并将其赋值给之前声明的client
  MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  optsPub.keepAliveInterval = 20;
  optsPub.cleansession = 1;
  optsPub.username = USERNAME;
  optsPub.password = PASSWORD;

  MQTTClient_message msgPub = MQTTClient_message_initializer;
  //声明消息token
  MQTTClient_deliveryToken token;
  int rc;
  //使用MQTTClient_connect将client连接到服务器，使用指定的连接选项。成功则返回MQTTCLIENT_SUCCESS
  if ((rc = MQTTClient_connect(client, &optsPub)) != MQTTCLIENT_SUCCESS) {
    printf("Failed to connect, return code %d\n", rc);
    exit(EXIT_FAILURE);
  }
  msgPub.payload = PAYLOAD;
  msgPub.payloadlen = strlen(PAYLOAD);
  msgPub.qos = QOS;
  msgPub.retained = 0;
  while(CONNECT){
    MQTTClient_publishMessage(client, TOPIC, &msgPub, &token);
    printf("Waiting for up to %d seconds for publication of %s\n"
      "on topic %s for client with ClientID: %s\n",
      (int)(TIMEOUT/1000), PAYLOAD, TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d msgDelivered\n", token);
#if defined(_WIN32)
	Sleep(100);
#else
	usleep(3000000L);
#endif

  }
  MQTTClient_disconnect(client, 10000);
  MQTTClient_destroy(&client);
}

int runThreads(int argc, char* argv[], char **env) {
  pthread_t threads[NUM_THREADS];
  long t;
  pthread_create(&threads[0], NULL, subClient, (void *)0);
  pthread_create(&threads[1], NULL, pubClient, (void *)1);
  pthread_exit(NULL);  
  getchar();
  return 0;
}

struct args{
  char key[2];
  char val[255];
};

int main(int argc, char *argv[]) {
  int i;
  struct args* list;
  for(i = 0; i < argc; i = i + 1) {
    printf("Argument %d is %s\n", i, argv[i]);
    struct args* current = (struct args*)malloc(sizeof(struct args));
    //key
    if(argv[i][0] == '-') {
      switch(argv[i][1]) {
        case 'h': 
          snprintf(current->key, 5, "host");
          break;
        case 'p': 
          snprintf(current->key, 5, "port");
          break;
        case 's': 
          snprintf(current->key, 5, "slav");
          break;
        case 'f': 
          snprintf(current->key, 5, "func");
          break;
        case 'r': 
          snprintf(current->key, 5, "regs");
          break;
        case 'c':
          snprintf(current->key, 5, "cunt");
          break;

        default:
          break;
      }
    }

    //value
    snprintf(current->val, strlen(argv[i]) + 1, argv[i]);

    // link to list
    // list.link(current);
  }

  if (3 < argc) {
    char*res = strstr(argv[1], "onboard");
    if (NULL != res) {
      //hit
    }  
  }
  for (;;Sleep(1));
  exit(EXIT_SUCCESS);
}
