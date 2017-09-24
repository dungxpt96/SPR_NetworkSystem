#ifndef PROXY_H
#define PROXY_H

#include "spr.h"

int sender[1024];
int receiver[2];
int numSender = 0;

void *handle_data_receiver(void *);

#endif 
