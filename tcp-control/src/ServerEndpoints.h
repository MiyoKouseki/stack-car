// ServerEndpoints.h

#ifndef SERVER_ENDPOINTS_H
#define SERVER_ENDPOINTS_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern AsyncWebServer server;
extern String motorCommand;

void setupServerEndpoints();

#endif // SERVER_ENDPOINTS_H
