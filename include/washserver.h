#ifndef SERVER_H
#define SERVER_H

#include <WebServer.h>
#include <Preferences.h>

// Globals
extern bool autoMode;

void initServer();
void handleServer();
void saveState();
void loadState();

#endif
