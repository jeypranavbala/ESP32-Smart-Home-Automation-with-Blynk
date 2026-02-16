# ESP32 Smart Home Automation with Blynk

ESP32-based IoT home automation and monitoring system integrating environmental sensing, security detection, and remote appliance control via the Blynk IoT platform.

## Features

- Remote light and fan control from mobile app  
- Temperature & humidity monitoring (DHT11)  
- Ambient light sensing with automatic indicator  
- Motion-based solenoid door lock  
- Gas leakage detection with buzzer alert  
- Real-time telemetry to Blynk dashboard  
- Adjustable gas threshold via app  

## Hardware Components

- ESP32 Dev Board  
- DHT11 Sensor  
- LDR Module  
- HC-SR04 Ultrasonic Sensor  
- MQ135 Gas Sensor  
- Relay Module (2-channel)  
- Solenoid Lock  
- Buzzer  
- LED  

## Pin Connections

| Device | ESP32 Pin |
|--------|----------|
| DHT11 Data | GPIO 4 |
| Relay Light | GPIO 26 |
| Relay Fan | GPIO 27 |
| LDR Analog | GPIO 34 |
| LED Indicator | GPIO 14 |
| Ultrasonic TRIG | GPIO 32 |
| Ultrasonic ECHO | GPIO 33 |
| Solenoid Lock | GPIO 25 |
| MQ135 Analog | GPIO 35 |
| Buzzer | GPIO 2 |

## Blynk Virtual Pins

| Function | Virtual Pin |
|----------|------------|
| Light Control | V0 |
| Fan Control | V1 |
| Temperature | V2 |
| Humidity | V3 |
| Gas Level | V4 |
| Lock Status | V5 |
| LDR Level | V6 |
| Gas Threshold | V7 |

## Setup

1. Install Arduino libraries:
   - Blynk
   - DHT sensor library

2. Open `home_automation_esp32.ino`

3. Replace credentials:

#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN "YOUR_BLYNK_AUTH_TOKEN"



4. Upload to ESP32.

## System Overview

Sensors → ESP32 → Blynk Cloud → Mobile App  
ESP32 → Relays / Lock / Buzzer (Actuation)

