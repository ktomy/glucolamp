#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#include <FastLED.h>

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <string>

using namespace std;

int rgb[190][3] = {
    {84, 0, 159}, {84, 0, 163}, {84, 0, 168}, {79, 0, 177}, {78, 0, 182}, {77, 0, 186}, {76, 0, 191}, {70, 0, 200}, {68, 0, 204}, {66, 0, 209}, {60, 0, 214}, {58, 0, 218}, {55, 0, 223}, {46, 0, 232}, {43, 0, 236}, {40, 0, 241}, {36, 0, 245}, {33, 0, 250}, {25, 0, 255}, {16, 0, 255}, {12, 0, 255}, {4, 0, 255}, {0, 0, 255}, {0, 4, 255}, {0, 16, 255}, {0, 21, 255}, {0, 25, 255}, {0, 29, 255}, {0, 38, 255}, {0, 42, 255}, {0, 46, 255}, {0, 51, 255}, {0, 63, 255}, {0, 67, 255}, {0, 72, 255}, {0, 84, 255}, {0, 89, 255}, {0, 93, 255}, {0, 97, 255}, {0, 106, 255}, {0, 110, 255}, {0, 114, 255}, {0, 119, 255}, {0, 127, 255}, {0, 135, 255}, {0, 140, 255}, {0, 152, 255}, {0, 157, 255}, {0, 161, 255}, {0, 165, 255}, {0, 174, 255}, {0, 178, 255}, {0, 182, 255}, {0, 187, 255}, {0, 195, 255}, {0, 199, 255}, {0, 216, 255}, {0, 220, 255}, {0, 225, 255}, {0, 229, 255}, {0, 233, 255}, {0, 242, 255}, {0, 246, 255}, {0, 250, 255}, {0, 255, 255}, {0, 255, 246}, {0, 255, 242}, {0, 255, 238}, {0, 255, 225}, {0, 255, 216}, {0, 255, 212}, {0, 255, 203}, {0, 255, 199}, {0, 255, 195}, {0, 255, 191}, {0, 255, 187}, {0, 255, 178}, {0, 255, 174}, {0, 255, 170}, {0, 255, 157}, {0, 255, 152}, {0, 255, 144}, {0, 255, 135}, {0, 255, 131}, {0, 255, 127}, {0, 255, 123}, {0, 255, 114}, {0, 255, 110}, {0, 255, 106}, {0, 255, 102}, {0, 255, 89}, {0, 255, 84}, {0, 255, 80}, {0, 255, 76}, {0, 255, 63}, {0, 255, 59}, {0, 255, 55}, {0, 255, 46}, {0, 255, 42}, {0, 255, 38}, {0, 255, 25}, {0, 255, 21}, {0, 255, 16}, {0, 255, 12}, {0, 255, 8}, {0, 255, 0}, {8, 255, 0}, {12, 255, 0}, {21, 255, 0}, {25, 255, 0}, {29, 255, 0}, {42, 255, 0}, {46, 255, 0}, {51, 255, 0}, {55, 255, 0}, {63, 255, 0}, {67, 255, 0}, {72, 255, 0}, {76, 255, 0}, {89, 255, 0}, {93, 255, 0}, {97, 255, 0}, {110, 255, 0}, {114, 255, 0}, {119, 255, 0}, {123, 255, 0}, {131, 255, 0}, {135, 255, 0}, {140, 255, 0}, {144, 255, 0}, {153, 255, 0}, {161, 255, 0}, {165, 255, 0}, {178, 255, 0}, {182, 255, 0}, {187, 255, 0}, {191, 255, 0}, {199, 255, 0}, {203, 255, 0}, {208, 255, 0}, {212, 255, 0}, {221, 255, 0}, {225, 255, 0}, {242, 255, 0}, {246, 255, 0}, {250, 255, 0}, {255, 255, 0}, {255, 250, 0}, {255, 242, 0}, {255, 238, 0}, {255, 233, 0}, {255, 229, 0}, {255, 221, 0}, {255, 216, 0}, {255, 212, 0}, {255, 199, 0}, {255, 191, 0}, {255, 187, 0}, {255, 178, 0}, {255, 174, 0}, {255, 170, 0}, {255, 165, 0}, {255, 161, 0}, {255, 153, 0}, {255, 148, 0}, {255, 144, 0}, {255, 131, 0}, {255, 127, 0}, {255, 119, 0}, {255, 110, 0}, {255, 106, 0}, {255, 102, 0}, {255, 97, 0}, {255, 89, 0}, {255, 85, 0}, {255, 80, 0}, {255, 76, 0}, {255, 63, 0}, {255, 59, 0}, {255, 55, 0}, {255, 51, 0}, {255, 38, 0}, {255, 34, 0}, {255, 29, 0}, {255, 21, 0}, {255, 17, 0}, {255, 12, 0}, {255, 0, 0}, {255, 255, 255}, {0, 0, 0}};

void (*resetFunc)(void) = 0; // declare reset function @ address 0

time_t lastGoodGlucose = time(nullptr);

const uint16_t PixelCount = 8;

#define magenta 0
#define blue 52
#define green 105
#define yellow 146
#define red 187
#define white 188
#define black 189

#define very_low_color magenta
#define low_color blue
#define target_color green
#define high_color yellow
#define very_high_color red

WiFiClientSecure client;
WiFiManager wifiManager;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

//#define TRIGGER_PIN 0
#define BUTTON_PIN 5

#define GLUCOSE_TOO_OLD 11

char nightscout_server[128];
char high_point[4] = "200";
char low_point[4] = "90";
char target[4] = "110";

int high_threshold = -1;
int low_threshold = -1;
int target_value = -1;
#define very_low_threshold 35
#define very_high_threshold 300

int brightness = 0;
int currentColorIndex = -1;

long lastDebounceTimeValue = 0;
long *lastDebounceTime = &lastDebounceTimeValue;
const int debounceDelay = 100;
volatile int state;

bool shouldSaveConfig = false;

int shared_sgv = 0;

CRGB leds[PixelCount];

void saveConfigCallback()
{
    Serial.println("Should save config");
    shouldSaveConfig = true;
}

void showOnStrip(CRGB color, bool insist)
{
    int ledBrightness = 0;
    switch (brightness)
    {
    case 0:
        ledBrightness = 255;
        break;
    case 1:
        ledBrightness = 128;
        break;
    case 2:
        ledBrightness = 64;
        break;
    case 3:
        ledBrightness = 32;
        break;
    case 4:
        ledBrightness = 16;
        break;
    case 5:
        ledBrightness = 4;
        break;
    }

    // well, this is bad, but... sometimes instead of 8 leds just 1 is shown,
    // but if I insist on showing it for more times, it just works
    if (insist)
    {
        for (int i = 0; i < 10; i++)
        {
            FastLED.showColor(color, ledBrightness);
        }
    }
    FastLED.showColor(color, ledBrightness);
}

void fadeInColor(int colorIndex)
{
        CRGB color(rgb[colorIndex][0], rgb[colorIndex][1], rgb[colorIndex][2]);
    for (int i = 0; i < 256; i++)
    {
        FastLED.showColor(color, i);
        delay(3);
    }
}

void fadeOutColor(int colorIndex)
{
    CRGB color(rgb[colorIndex][0], rgb[colorIndex][1], rgb[colorIndex][2]);
    for (int i = 255; i >= 0; i--)
    {
        FastLED.showColor(color, i);
        delay(3);
    }
    currentColorIndex = black;
}

void showColor(int colorIndex)
{
    // For brightness change, color does not change
    if (currentColorIndex == colorIndex)
    {
        CRGB color(rgb[colorIndex][0], rgb[colorIndex][1], rgb[colorIndex][2]);
        showOnStrip(color, true);
        return;
    }

    // fade-in from black
    if ( currentColorIndex == -1 || currentColorIndex == black)
    {
        fadeInColor(colorIndex);
    }
    else  if (colorIndex == black || colorIndex == white ||
        currentColorIndex == white)
    {
        CRGB color(rgb[colorIndex][0], rgb[colorIndex][1], rgb[colorIndex][2]);
        showOnStrip(color, true);
    }
    else
    {
        for (int i = currentColorIndex; i != colorIndex; currentColorIndex > colorIndex ? i-- : i++)
        {
            CRGB color(rgb[i][0], rgb[i][1], rgb[i][2]);

            showOnStrip(color, false);
            delay(20);
        }
    }
    currentColorIndex = colorIndex;
}

void FlashColor(int colorIndex)
{
    fadeInColor(colorIndex);
    fadeOutColor(colorIndex);
}

void checkButton()
{
    // check for button press
    if (digitalRead(BUTTON_PIN) == HIGH)
    {
        // poor mans debounce/press-hold, code not ideal for production
        delay(50);
        if (digitalRead(BUTTON_PIN) == HIGH)
        {
            Serial.println("Button Pressed");
            // still holding button for 3000 ms, reset settings, code not ideaa for production
            delay(3000); // reset delay hold
            if (digitalRead(BUTTON_PIN) == HIGH)
            {
                FlashColor(magenta);
                Serial.println("Button Held");
                Serial.println("Erasing Config, restarting");
                wifiManager.resetSettings();
                WiFi.disconnect(true);
                delay(300);
                SPIFFS.format();
                delay(300);
                ESP.restart();
                delay(300);
            }
        }
    }
}

void showIfNoDataForLongTime()
{
    if (shared_sgv != 0 && difftime(time(nullptr), lastGoodGlucose) > 1 * 60) // 10 minutes
    {
        Serial.printf("Not connected\n");
        FlashColor(white);
        brightness = 4;
        showOnStrip(CRGB(255, 255, 255), true);
        currentColorIndex = white;
        shared_sgv = 0;
        lastGoodGlucose = time(nullptr);
    }
}

IRAM_ATTR void triggerInterrupt()
{
    int reading = digitalRead(BUTTON_PIN);
    if (reading == state)
        return;
    bool debounce = false;
    if ((millis() - *lastDebounceTime) <= debounceDelay)
    {
        debounce = true;
    }
    *lastDebounceTime = millis();
    if (debounce)
        return;
    state = reading;
    if (reading == false)
        return;

    Serial.println((string("Brightness is: ") + to_string(brightness)).c_str());
    if (brightness < 5)
    {
        ++brightness;
    }
    else
    {
        brightness = 0;
    }

    CRGB color(rgb[currentColorIndex][0], rgb[currentColorIndex][1], rgb[currentColorIndex][2]);
    showOnStrip(color, true);
}

bool GetJsonValue(DynamicJsonDocument json, const char *node, char *buf)
{
    if (json.containsKey(node))
    {
        strcpy(buf, json[node]);
        return true;
    }
    Serial.println(String("JSON does not contain ") + String(node));
    return false;
}

bool ReadJsonConfigFile()
{
    if (SPIFFS.exists("/config.json"))
    {
        Serial.println("reading config file");
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile)
        {
            Serial.println("opened config file");
            size_t size = configFile.size();
            std::unique_ptr<char[]> buf(new char[size]);

            configFile.readBytes(buf.get(), size);

            DynamicJsonDocument json(1024);
            auto deserializeError = deserializeJson(json, buf.get());
            if (!deserializeError)
            {
                Serial.println("\nparsed json");
                if (
                    GetJsonValue(json, "nightscout_server", nightscout_server) && GetJsonValue(json, "high_point", high_point) && GetJsonValue(json, "low_point", low_point) && GetJsonValue(json, "target", target))
                {
                    configFile.close();

                    Serial.printf(
                        "Config file parsed. Values:\nnightscout_server: %s\nhigh_point: %s\nlow_point: %s\ntarget: %s\n", nightscout_server, high_point, low_point, target);
                    return true;
                }
            }
            else
            {
                Serial.println("failed to load json config");
            }
            configFile.close();
        }
    }
    return false;
}

void SaveConfigFile(
    WiFiManagerParameter &custom_nightscout_server,
    WiFiManagerParameter &custom_high_point,
    WiFiManagerParameter &custom_low_point,
    WiFiManagerParameter &custom_target)
{
    Serial.println("saving config");
    // read updated parameters
    strcpy(nightscout_server, custom_nightscout_server.getValue());
    strcpy(high_point, custom_high_point.getValue());
    strcpy(low_point, custom_low_point.getValue());
    strcpy(target, custom_target.getValue());
    Serial.println("Parameters from capture portal are: ");
    Serial.println("\tnightscout_server : " + String(nightscout_server));
    Serial.println("\thigh_point : " + String(high_point));
    Serial.println("\tlow_point : " + String(low_point));
    Serial.println("\ttarget : " + String(target));

    DynamicJsonDocument json(1024);

    json["nightscout_server"] = nightscout_server;
    json["high_point"] = high_point;
    json["low_point"] = low_point;
    json["target"] = target;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
        Serial.println("failed to open config file for writing");
    }
    else
    {
        serializeJson(json, Serial);
        serializeJson(json, configFile);

        configFile.close();
    }
}

void setup()
{
    WiFi.mode(WIFI_STA);
    Serial.begin(115200);
    Serial.setDebugOutput(false);
    Serial.println();
    Serial.println();

    FastLED.addLeds<NEOPIXEL, 3>(leds, PixelCount);

    pinMode(BUTTON_PIN, INPUT);
    FlashColor(green);
    Serial.println();
    Serial.println();
    checkButton();
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), triggerInterrupt, CHANGE);

    Serial.println("mounting FS...");

    if (SPIFFS.begin())
    {
        Serial.println("mounted file system");
        if (!ReadJsonConfigFile())
        {
            Serial.println("config file does not exist, resetting wifi...");
            wifiManager.resetSettings();
        }
    }
    else
    {
        Serial.println("failed to mount FS");
        SPIFFS.format();
        delay(300);
        ESP.restart();
        delay(300);
    }

    WiFiManagerParameter custom_nightscout_server("server", "Nightscout server", "", 128, "><p>Nightscout server name (e.g. test1.nsromania.ro or myname.herokuapp.com)</p><hr/");
    WiFiManagerParameter custom_high_point("high", "High threshold", "", 3, "><p> High threshold in mg/dl (NOT IN MMOL/L) (e.g. 200)</p><hr/");
    WiFiManagerParameter custom_low_point("low", "Low threshold", "", 3, "><p>Low thrashold in mg/dl (NOT IN MMOL/L,), (e.g. 70)</p><ht/");
    WiFiManagerParameter custom_target("target", "Target value", "", 3, "><p>Target value in mg/dl (NOT IN MMOL/L,), (e.g. 110)</p><ht/");

    WiFiManager wifiManager;
    wifiManager.setDebugOutput(false);

    wifiManager.setSaveConfigCallback(saveConfigCallback);

    wifiManager.addParameter(&custom_nightscout_server);
    wifiManager.addParameter(&custom_high_point);
    wifiManager.addParameter(&custom_low_point);
    wifiManager.addParameter(&custom_target);

    if (!wifiManager.autoConnect("Glucolamp"))
    {
        FlashColor(yellow);
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        ESP.reset();
        delay(5000);
    }

    Serial.println("connected to WiFi");

    // save the custom parameters to FS
    if (shouldSaveConfig)
    {
        SaveConfigFile(
            custom_nightscout_server,
            custom_high_point,
            custom_low_point,
            custom_target);
    }

    high_threshold = atoi(high_point);
    low_threshold = atoi(low_point);
    target_value = atoi(target);

    Serial.println("local ip");
    Serial.println(WiFi.localIP());
    
    timeClient.begin();
    timeClient.update();
    lastGoodGlucose = time(nullptr);
    FlashColor(white);
}

int getGlucoseValue(String nsInput)
{
    DynamicJsonDocument json(1024);

    auto deserializeError = deserializeJson(json, nsInput);
    if (!deserializeError)
    {
        long long date = json[0]["date"];
        long long glucoseEpoch = date / 1000;
        unsigned long nowEpoch = timeClient.getEpochTime();
        int glucoseAge = nowEpoch - glucoseEpoch;
        if (glucoseAge < 60 * GLUCOSE_TOO_OLD)
        {
            int sgv = json[0]["sgv"];
            lastGoodGlucose = time(nullptr);
            return sgv;
        }
        Serial.printf("Glucose too old (%d seconds)\n", glucoseAge);
        if (shared_sgv == 0)
        {
            FlashColor(yellow);
            shared_sgv = -1;
        }
    }
    else
    {
        Serial.println("Cannot deserialize string");
    }
    return -1;
}

String GetApiResponse()
{
    Serial.println();
    Serial.println(String("Starting connection to server ") + String(nightscout_server));
    client.setInsecure();
    if (!client.connect(String(nightscout_server), 443))
    {
        Serial.println("Connection failed!");
        if (shared_sgv == 0)
        {
            FlashColor(red);
            shared_sgv = -1;
        }
        return String("");
    }
    Serial.println("Connected to server!");
    client.println("GET /api/v1/entries/sgv?count=1 HTTP/1.1");
    client.println("Host: " + String(nightscout_server));
    client.println("accept: application/json");
    client.println();

    while (client.connected())
    {
        String line = client.readStringUntil('\n');
        if (line == "\r")
        {
            break;
        }
    }

    String apiResponse = client.readString();
    client.stop();
    return apiResponse;
}

int GetColorIndexByGlucose(int sgv)
{
    int colorIndex = 0;

    if (sgv <= very_low_threshold)
    {
        colorIndex = very_low_color;
    }
    else if (sgv <= low_threshold)
    {
        colorIndex = (sgv - very_low_threshold) * (low_color - very_low_color) / (low_threshold - very_low_threshold) + very_low_color;
    }
    else if (sgv <= target_value)
    {
        colorIndex = (sgv - low_threshold) * (target_color - low_color) / (target_value - low_threshold) + low_color;
    }
    else if (sgv <= high_threshold)
    {
        colorIndex = (sgv - target_value) * (high_color - target_color) / (high_threshold - target_value) + target_color;
    }
    else if (sgv <= very_high_threshold)
    {
        colorIndex = (sgv - high_threshold) * (very_high_color - high_color) / (very_high_threshold - high_threshold) + high_color;
    }
    else
    {
        colorIndex = very_high_color;
    }

    return colorIndex;
}

void ShowGlucoseColor(int sgv)
{
    int colorIndex = GetColorIndexByGlucose(sgv);
    showColor(colorIndex);
}

void PollNightscout()
{

    String apiResponse = GetApiResponse();
    if (apiResponse != String(""))
    {
        int sgv = getGlucoseValue(apiResponse);
        if (sgv > 0)
        {
            Serial.println((string("Glucose value is: ") + to_string(sgv)).c_str());
            ShowGlucoseColor(sgv);
            shared_sgv = sgv;
        }
    }
}

void rainbow()
{
    showColor(magenta);
    showColor(red);
}

// bool test1 = false;

void loop()
{
    //rainbow();

    timeClient.update();
    PollNightscout();
    showIfNoDataForLongTime();
    delay(30000);
}