#include <ESP8266WiFi.h>
#include <ArduinoHA.h>

// Include WiFi credentials
#ifdef __has_include
#  if __has_include("credentials.h")
#    include "credentials.h"
#  else
const char* ssid = "default_ssid";
const char* password = "default_password";
const char* mqttServer = "192.168.0.17"; // Default MQTT server IP
const int mqttPort = 1883;               // Default MQTT port
#  endif
#endif

// Define the GPIO pin connected to the SSR
const int ssrPin = D1;

// WiFi and MQTT client objects
WiFiClient wifiClient;
HADevice device("esp8266_heater");
HAMqtt mqtt(wifiClient, device);

// Define the HANumber object
HANumber number("heaterPower", HANumber::PrecisionP0); // Full numbers, no decimals

// Heating control variables
int powerLevel = 0; // Use integer values for power level

// PWM Frequency and Resolution
const int pwmFrequency = 500;  // PWM frequency in Hz
const int pwmResolution = 1023; // Resolution (10-bit PWM)

// Function to update heating rod based on the power level
void updateHeatingRod() {
    if (powerLevel <= 0) {
        // Turn off SSR
        analogWrite(ssrPin, 0);
    } else if (powerLevel >= 100) {
        // Turn on SSR (no PWM)
        analogWrite(ssrPin, pwmResolution);
    } else {
        // Set PWM value based on power level
        int pwmValue = (int)(pwmResolution * (powerLevel / 100.0));
        analogWrite(ssrPin, pwmValue);
    }
}
// Callback function for handling number input from Home Assistant
void onNumberCommand(HANumeric receivedNumber, HANumber* sender) {
    if (!receivedNumber.isSet()) {
        // Reset command received from Home Assistant
        powerLevel = 0;
    } else {
        // Update the power level based on input from Home Assistant
        powerLevel = receivedNumber.toInt32(); // Convert to integer
    }
    // Update heating rod output
    updateHeatingRod();
    
    // Report the state back to Home Assistant
    sender->setState(receivedNumber);
}


void setup() {
    // Set up the SSR pin
    pinMode(ssrPin, OUTPUT);
    analogWriteFreq(pwmFrequency);

    // Start serial communication for debugging
    Serial.begin(115200);
    delay(10);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected to WiFi");

    // Set up device details
    device.setName("Heater Controller");
    device.setSoftwareVersion("1.0.0");

    // Configure the number input from Home Assistant
    number.setName("Heater Power Level");
    number.setIcon("mdi:thermometer");
    number.setMin(0);
    number.setMax(100);
    number.setStep(1);  // Use integer steps

    // Register the callback for number commands
    number.onCommand(onNumberCommand);

    // Start the MQTT connection
    mqtt.begin(mqttServer, mqttPort);
}

void loop() {
    mqtt.loop();
}
