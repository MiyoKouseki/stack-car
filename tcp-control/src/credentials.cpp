#include "credentials.h"
#include <SD.h>

std::tuple<String, String> getCredentials() {
    if (!SD.begin()) {
        Serial.println("Failed to initialize SD card");
        return {"", ""};
    }

    File file = SD.open("/credentials.txt", "r");
    if (!file) {
        Serial.println("Failed to open credentials.txt");
        return {"", ""};
    }

    String ssid = file.readStringUntil('\n');
    String password = file.readStringUntil('\n');
    file.close();

    return {ssid, password};
}
