
#include <ESPAsyncWebServer.h>
#include "ServerEndpoints.h"
#include "MotorFactors.h"
#include <SPIFFS.h>

void setupServerEndpoints()
{
    // Define endpoints
    server.on("/straight", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    motorCommand = "straight";
    request->send(200, "text/plain", "Going straight"); });

    server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    motorCommand = "right";
    request->send(200, "text/plain", "Turning right"); });

    server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    motorCommand = "left";
    request->send(200, "text/plain", "Turning left"); });

    server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    motorCommand = "stop";
    request->send(200, "text/plain", "Stopping"); });

    server.on("/setMotorFactor", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if (request->hasParam("right") && request->hasParam("left")) {
            rightMotorFactor = request->getParam("right")->value().toFloat();
            leftMotorFactor = request->getParam("left")->value().toFloat();
            request->send(200, "text/plain", "Motor factors updated");
        } else {
            request->send(400, "text/plain", "Missing parameters");
        } });

    // 両方のモーターのfactorを取得するエンドポイント
    server.on("/getMotorFactors", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        String response = "Right Motor Factor: " + String(rightMotorFactor) + "\n";
        response += "Left Motor Factor: " + String(leftMotorFactor);
        request->send(200, "text/plain", response); 
    });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
        if (SPIFFS.exists("/index.html"))
        {
            request->send(SPIFFS, "/index.html", "text/html");
        }else{
            request->send(404, "text/plain", "index.html does not exist!");
        } });

    // Start server
    server.begin();
}
