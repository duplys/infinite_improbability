#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "Seeed_BMP280.h"
#include "Air_Quality_Sensor.h"
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

BMP280 bmp280;
AirQualitySensor sensor(A0);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// By default 'pool.ntp.org' is used with 60 seconds update interval and
// no offset
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


//const char broker[]  = "test.mosquitto.org";
const IPAddress broker(192, 168, 178, 85);
int        port      = 1883;
const char topic1[]  = "temperature";
const char topic2[]  = "pressure";
const char topic3[]  = "air_quality_value";
const char topic4[]  = "air_quality_level";

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  if(!bmp280.init()){
    Serial.println("BMP280 init ERROR!");
  } else {
    Serial.println("BMP280 ready.");
  }

  Serial.println("Waiting for Air Quality Sensor v1.3 to init...");
    delay(20000);

  if (sensor.init()) {
    Serial.println("Air Quality Sensor v1.3 ready.");
  } else {
    Serial.println("Air Quality Sensor v1.3 init ERROR!");
  }

  timeClient.begin();
}

void loop() {
  float temperature;
  float pressure;
  int air_quality_value;
  int quality = sensor.slope();

  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;

    timeClient.update();

    Serial.println(timeClient.getFormattedTime());

    //get temperature, print it and send via MQTT
    temperature = bmp280.getTemperature();
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.println("C"); // The unit for  Celsius because original arduino don't support speical symbols

    Serial.print("Sending ");
    Serial.print(temperature);
    Serial.print(" to topic ");
    Serial.println(topic1);

    mqttClient.beginMessage(topic1);
    mqttClient.print(timeClient.getFormattedTime() + ": " + String(temperature));
    //mqttClient.print(temperature);
    mqttClient.endMessage();

    //get atmospheric pressure data, print it and send via MQTT
    pressure = bmp280.getPressure();
    Serial.print("Pressure: ");
    Serial.print(pressure);
    Serial.println("Pa");

    Serial.print("Sending ");
    Serial.print(pressure);
    Serial.print(" to topic ");
    Serial.println(topic2);

    mqttClient.beginMessage(topic2);
    mqttClient.print(pressure);
    mqttClient.endMessage();

    //print altitude calculated from pressure
    Serial.print("Calculated altitude: ");
    Serial.print(bmp280.calcAltitude(pressure));
    Serial.println("m");

    //get air quality value, print it and send via MQTT
    air_quality_value = sensor.getValue();
    Serial.print("Air Quality Sensor v1.3 value: ");
    Serial.println(air_quality_value);

    Serial.print("Sending ");
    Serial.print(air_quality_value);
    Serial.print(" to topic ");
    Serial.println(topic3);

    mqttClient.beginMessage(topic3);
    mqttClient.print(air_quality_value);
    mqttClient.endMessage();

    //print air quality level and send it via MQTT

    mqttClient.beginMessage(topic4);
    if (quality == AirQualitySensor::FORCE_SIGNAL) {
      Serial.println("High pollution! Force signal active");
      mqttClient.print("High pollution! Force signal active");
    } else if (quality == AirQualitySensor::HIGH_POLLUTION) {
      Serial.println("High pollution!");
      mqttClient.print("High pollution!");
    } else if (quality == AirQualitySensor::LOW_POLLUTION) {
      Serial.println("Low pollution!");
      mqttClient.print("Low pollution!");
    } else if (quality == AirQualitySensor::FRESH_AIR) {
      Serial.println("Fresh air");
      mqttClient.print("Fresh air");
    }
    mqttClient.endMessage();

    Serial.println("\n");//add a line between output of different times.
  }

}