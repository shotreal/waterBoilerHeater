#include <ESP8266WiFi.h>
#include <ArduinoHA.h>
#include <RBDdimmer.h>  // Include RBDDimmer library

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

// Define the GPIO pins for the TRIAC module
const int dimmerPin = D1;  // GPIO pin connected to the TRIAC module
const int zerocrossPin = D2; // GPIO pin connected to the zero-cross detection

// Initialize the RBDDimmer object
dimmerLamp dimmer(dimmerPin, zerocrossPin);  // Create dimmer object

// WiFi and MQTT client objects
WiFiClient wifiClient;
HADevice device("esp8266_heater");
HAMqtt mqtt(wifiClient, device);

// Define the HANumber object
HANumber number("heaterPower", HANumber::PrecisionP0); // Full numbers, no decimals

// Heating control variables
int powerLevel = 0; // Use integer values for power level

// Function to update heating rod based on the power level
void updateHeatingRod() {
    if (powerLevel <= 0) {
        // Turn off TRIAC
        dimmer.setPower(0);
        dimmer.setState(OFF);
    } else if (powerLevel >= 100) {
        // Turn on TRIAC at full power (100%)
        dimmer.setPower(100);
        dimmer.setState(ON);
    } else {
        // Set TRIAC power level based on powerLevel
        dimmer.setPower(powerLevel);
        dimmer.setState(ON);
    }
    Serial.print("Power Level: ");
    Serial.println(powerLevel); 
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
    // Set up the TRIAC module
    dimmer.begin(NORMAL_MODE, ON);  // Initialize dimmer in normal mode, with the TRIAC on
    
    // Start serial communication for debugging
    Serial.begin(115200);
    delay(10);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(10);
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
