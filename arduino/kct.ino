/***************************************************************************
  This is a library for the AMG88xx GridEYE 8x8 IR camera

  This sketch tries to read the pixels from the sensor

  Designed specifically to work with the Adafruit AMG88 breakout
  ----> http://www.adafruit.com/products/3538

  These sensors use I2C to communicate. The device's I2C address is 0x69

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Dean Miller for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include <Wire.h>
#include <Adafruit_AMG88xx.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Jcmi";
const char* password = "xxx";
const char* mqtt_server = "xxxx.xxxx.com";
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];

String subscribe_t = "/home/sub/kitchenTemp";

Adafruit_AMG88xx amg;

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String str;
  for (int i = 0; i < length; i++) {
    str += String((char)payload[i]);
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.print(str);
  // Switch on the LED if an 1 was received as first character
  if (str == "red") {

    Serial.print("redred");
  } else if (str == "blue") {

    Serial.print("blueblue");
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), "admin", "zxASqw12")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("AMG88xx pixels"));

  bool status;

  // default settings
  status = amg.begin();
  if (!status) {
    Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
    while (1);
  }

  Serial.println("-- Pixels Test --");

  Serial.println();

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(100); // let sensor boot up
}


void getTemp(){
  amg.readPixels(pixels);
  
  int j = 0;
  String pixelArr = String(j)+"_";
    for (int i = 1; i <= AMG88xx_PIXEL_ARRAY_SIZE; i++) {
      pixelArr += pixels[i - 1];
      pixelArr += "_";
      if( i%8 == 0 ){
        Serial.println(pixelArr.length());
        pixelArr.toCharArray(msg, pixelArr.length() + 1); 
        client.publish("/home/r/kitchenTemp", msg);
        j ++;
        pixelArr = String(j)+"_";
        
      }
    }
}
void loop() {
  //read all the pixels
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

unsigned long now = millis();
if (now - lastMsg > 1000) {
    lastMsg = now;
    getTemp();
  }
}
