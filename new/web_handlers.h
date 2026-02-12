#ifndef WEB_HANDLERS_H
#define WEB_HANDLERS_H

#include <Arduino.h>
#include <WebServer.h>
#include "gateway_config.h"

void setupWebServer(WebServer& server);
void handleRoot();
void handleStatus();
void handleGetConfig();
void handleSetConfig();
void handleSaveConfig();
void handleResetConfig();
void handleCaptiveRedirect();
void handleNotFound();

#endif
