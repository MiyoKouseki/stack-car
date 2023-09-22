
#include <ESPAsyncWebServer.h>
#include "ServerEndpoints.h"

void setupServerEndpoints() {


// Define endpoints
server.on("/straight", HTTP_GET, [](AsyncWebServerRequest *request){
    motorCommand = "straight";
    request->send(200, "text/plain", "Going straight");
});

server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request){
    motorCommand = "right";
    request->send(200, "text/plain", "Turning right");
});

server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request){
    motorCommand = "left";
    request->send(200, "text/plain", "Turning left");
});

server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request){
    motorCommand = "stop";
    request->send(200, "text/plain", "Stopping");
});

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", 
        "<!DOCTYPE html>"
        "<html lang=\"en\">"
        "<head>"
            "<meta charset=\"UTF-8\">"
            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
            "<title>M5Stack Controller</title>"
            "<style>"
                "body {"
                    "font-family: Arial, sans-serif;"
                    "background-color: #f4f4f4;"
                    "display: flex;"
                    "height: 100vh;"
                    "justify-content: center;"
                    "align-items: center;"
                "}"

                ".button-container {"
                    "background-color: #fff;"
                    "padding: 20px;"
                    "border-radius: 10px;"
                    "box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);"
                "}"

                "button {"
                    "background-color: #007BFF;"
                    "color: #fff;"
                    "border: none;"
                    "border-radius: 5px;"
                    "padding: 15px 30px;"
                    "font-size: 20px;"
                    "cursor: pointer;"
                    "margin: 10px;"
                    "transition: background-color 0.3s;"
                "}"

                "button:hover {"
                    "background-color: #0056b3;"
                "}"

                ".direction-row {"
                    "display: flex;"
                    "justify-content: center;"
                "}"
            "</style>"
        "</head>"
        "<body>"
            "<div class=\"button-container\">"
                "<div class=\"direction-row\">"
                    "<button onclick=\"sendCommand('/straight')\">↑</button>"
                "</div>"
                "<div class=\"direction-row\">"
                    "<button onclick=\"sendCommand('/left')\">←</button>"
                    "<button onclick=\"sendCommand('/stop')\">STOP</button>"
                    "<button onclick=\"sendCommand('/right')\">→</button>"
                "</div>"
            "</div>"
            "<script>"
                "function sendCommand(path) {"
                    "fetch(path).then(response => response.text()).then(data => {"
                        "console.log(data);"
                    "});"
                "}"
            "</script>"
        "</body>"
        "</html>"
    );
});


  // Start server
  server.begin();


}