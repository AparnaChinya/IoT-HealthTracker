// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Please use an Arduino IDE 1.6.8 or greater

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
#include <AzureIoTHub.h>
#include <AzureIoTProtocol_MQTT.h>
#include <AzureIoTUtility.h>

#include "config.h"

static bool messagePending = false;
static bool messageSending = true;

static char *connectionString;
static char *ssid;
static char *pass;

static int interval = INTERVAL;

#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

MAX30105 particleSensor;
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
int beatAvg;


const int buttonPin = 4;     // the number of the pushbutton pin
int buttonState = 0;     

void blinkLED()
{
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
}

void initWifi()
{
    ssid = "UCInet Mobile Access";
    pass = "";
    // Attempt to connect to Wifi network:
    Serial.printf("Attempting to connect to SSID: %s.\r\n", ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Get Mac Address and show it.
        // WiFi.macAddress(mac) save the mac address into a six length array, but the endian may be different. The huzzah board should
        // start from mac[0] to mac[5], but some other kinds of board run in the oppsite direction.
        uint8_t mac[6];
        WiFi.macAddress(mac);
        Serial.printf("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.\r\n",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
        WiFi.begin(ssid, pass);
        delay(10000);
    }
    Serial.printf("Connected to wifi %s.\r\n", ssid);
}

void initTime()
{
    time_t epochTime;
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (true)
    {
        epochTime = time(NULL);

        if (epochTime == 0)
        {
            Serial.println("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        }
        else
        {
            Serial.printf("Fetched NTP epoch time is: %lu.\r\n", epochTime);
            break;
        }
    }
}
void initSensorTest()
{
  //  dht.begin();
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Sensor initialized!");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
int count = 100;
void setup()
{
    pinMode(LED_PIN, OUTPUT);

    initSerial();
    delay(2000);
    readCredentials();

    initWifi();
    initTime();
    initSensorTest();

     pinMode(buttonPin, INPUT);
     pinMode(0,OUTPUT);
     digitalWrite(0,LOW);

       // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
     lcd.print("Pills Remaining : ");
     lcd.setCursor(0, 1);
     lcd.print(count);
    /*
    * Break changes in version 1.0.34: AzureIoTHub library removed AzureIoTClient class.
    * So we remove the code below to avoid compile error.
    */
    // initIoThubClient();
    connectionString = "HostName=apchinnewiothub.azure-devices.net;DeviceId=apchinnewiothub;SharedAccessKey=tNz7sSI5WCQBz2qyKYbcBvEZCg30fu7F3eTaPy38y4Y=";
    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (iotHubClientHandle == NULL)
    {
        Serial.println("Failed on IoTHubClient_CreateFromConnectionString.");
        while (1);
    }

    IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);
    IoTHubClient_LL_SetDeviceMethodCallback(iotHubClientHandle, deviceMethodCallback, NULL);
    IoTHubClient_LL_SetDeviceTwinCallback(iotHubClientHandle, twinCallback, NULL);
}

static int messageCount = 1;

void SendSensorData() {


    buttonState = digitalRead(buttonPin);
    if(buttonState == HIGH)
   {
    byte rateSpot = 0;
   lastBeat = 0; //Time at which the last beat occurred
    beatsPerMinute =0;
     beatAvg=0;
    count--;
    lcd.begin(16, 2);
     lcd.print("Pills Remaining : ");
     lcd.setCursor(0, 1);
     lcd.print(count);
     digitalWrite(0,LOW);
     Serial.println("Button Pressed");
     uint32_t sensorSendTime = 80000L ;// 1 minutes
     for( uint32_t tStart = millis();  (millis()-tStart) < sensorSendTime;  ){
        
           if (!messagePending && messageSending)
                {
                    char messagePayload[MESSAGE_MAX_LEN];
                    bool temperatureAlert = readMessage(messageCount, messagePayload);
                    sendMessage(iotHubClientHandle, messagePayload, temperatureAlert);
                    messageCount++;
                    delay(interval);
                }
          IoTHubClient_LL_DoWork(iotHubClientHandle);
     }
     lcd.begin(16, 2);
     lcd.print("Remove finger");
     delay(5000);
     lcd.print("Pills Remaining : ");
     lcd.setCursor(0, 1);
     lcd.print(count);
   }

}
void loop()
{
    SendSensorData();         

    IoTHubClient_LL_DoWork(iotHubClientHandle);
    delay(10);
}
