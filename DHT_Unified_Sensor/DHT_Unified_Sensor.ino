#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 2     // Dijital pin, DHT sensörüne bağlı olan pin

#define DHTTYPE    DHT11     // DHT 11


DHT_Unified dht(DHTPIN, DHTTYPE); // DHT sensörü nesnesi oluşturuluyor
WiFiClient client;
 
String apiKey = "MEI6MR24WADUETGT"; 
const char *ssid = "-----"; // Wifi Ag Adiniz
const char *pass = "------"; // Wifi sifreniz
const char* server = "api.thingspeak.com";
 

const int analogGirisPini = A0;  // Toprak nem sensörünün bağlı olduğu analog pini tanımla

int sensorDegeri = 0;  
int cikisDegeri = 0; 

uint32_t delayMS;  //sensör okumaları arasındaki gecikme süresini saklamak

int dht11Pin = 2; //DHT11 sensörünün bağlı olduğu dijital pin numarasını saklamak için kullanılır

float nem, sicaklik;
String postStr = apiKey;

void setup() {
  Serial.begin(9600);
  dht.begin();

  delay(1000);

WiFiClient client;

delay(10);
dht.begin();

Serial.println("Connecting to ");
Serial.println(ssid);

// WiFi bağlantısı kuruluyor
WiFi.begin(ssid, pass);

while (WiFi.status() != WL_CONNECTED)
{
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");


  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
 
}

void loop() {

  delay(delayMS);


  // Toprak nem sensöründen veri okunuyor
sensorDegeri = analogRead(analogGirisPini);

cikisDegeri = map(sensorDegeri, 0, 1023, 0, 255);

  // okudugumuz ve cikis degerini Serial Monitor uzerine yazdiriyoruz 
  Serial.print("Analog sensor = ");
  Serial.print(sensorDegeri);
  Serial.print("\t cikis = ");
  Serial.println(cikisDegeri);


// DHT sensöründen sıcaklık ve nem değerleri okunuyor
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }


if (client.connect(server,80)) // “184.106.153.149” or api.thingspeak.com
{

postStr +="&field1=";
postStr += String(cikisDegeri);
postStr +="&field2=";
postStr += String(event.temperature);
postStr +="&field3=";
postStr += String(event.relative_humidity);
postStr += "\r\n\r\n";

client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);

Serial.println("%. Send to Thingspeak.");
}
client.stop();

Serial.println("Waiting…");

delay(1000);


}
