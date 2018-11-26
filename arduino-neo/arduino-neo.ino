/*
 * 使用说明：
 *    上点后LED闪烁，尝试连接WIFI（1Hz频率闪烁）
 *    连接上后尝试连接MQTT，如果连接失败LED将快速闪烁（2.5Hz闪烁），这个错误不可恢复，只能尝试重启
 *    正常连接到MQTT服务器后保持LED亮状态
 *    之后每收到一条信息，LED将变换状态（从亮变暗，或，从暗变亮，不一定）
 */

#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "neo_exec.h"
#define TIME_INTV 20  // ms，每一帧的间隔

#define WLAN_SSID       "geeklab"
#define WLAN_PASS       "ai@geeklab"

#define AIO_SERVER      "onedawn.top"
#define AIO_SERVERPORT  8883                   // 8883 for MQTTS
#define AIO_USERNAME    "sVyd16qv"
#define AIO_KEY         "iIJQUrn5SkwpaCqG4omyW3Tb6z7RPvBY"

#define NEO_PIN D1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_N, NEO_PIN, NEO_GRB + NEO_KHZ800);
//WiFiClientSecure client;
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish mqtt_info = Adafruit_MQTT_Publish(&mqtt, "iot/" AIO_USERNAME "/info");
Adafruit_MQTT_Subscribe mqtt_ask = Adafruit_MQTT_Subscribe(&mqtt, "iot/" AIO_USERNAME "/ask");
void MQTT_connect();

const char* test1 = "1 default\n128 0 100 1\nfg:0 FF0000;10 0000FF;90 FF00FF\nsg:10000\nfg:0 FF0000;90 0000FF\n";
int ms, fidx, startms, todelay;

void setup() {

  Serial.begin(115200);
  delay(100);
  Serial.println("esp8266-neoLight with MQTTs support");

  // 连接WIFI
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println(); Serial.println();
  Serial.print("Connecting to "); Serial.println(WLAN_SSID);
  delay(500); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  delay(500); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(500); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.print(".");
  }
  Serial.println();
  delay(500);
  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // 连接MQTTs
  Serial.print("Connecting to ");
  Serial.println(AIO_SERVER);
  if (! client.connect(AIO_SERVER, AIO_SERVERPORT)) {
    Serial.println("Connection failed. Halting execution.");
    while(1) { delay(200); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); }
  }
  digitalWrite(LED_BUILTIN, LOW);  // 这个状态是LED点亮
  mqtt.subscribe(&mqtt_ask);
  
  // End of trinket special code
  // strip.setBrightness(BRIGHTNESS);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // initialize neo_exec
  neo_exec_init();

  Serial.println(millis());
  neo_exec_load(test1);
  Serial.print(test1);
  neo_exec_load(test1);
  neo_exec_load(test1);
  neo_exec_load(test1);
  neo_exec_load(test1);

  Serial.println(millis());
  neo_exec_draw(TIME_INTV);

  Serial.println(millis());
  show_frame(frame);
  
  Serial.println(millis());
  startms = millis();
  fidx = 0;
}

char buf[256];
Adafruit_MQTT_Subscribe *subscription;
void loop() {
  MQTT_connect();
  while ((subscription = mqtt.readSubscription(10))) {  // 只给10ms的时间去读取，不然就返回
    if (subscription == &mqtt_ask) {
      Serial.print(F("Got: "));
      Serial.println((char *)mqtt_ask.lastread);
      if (! mqtt_info.publish("hello from esp8266")) {
        Serial.println(F("Failed"));
      } else {
        Serial.println(F("OK!"));
      }
    }
  }
//  fidx += 1;
//  ms = millis();
//  todelay = TIME_INTV * fidx + startms - ms;
//  sprintf(buf, "f(% 3d): sleep %d ms\n", fidx, todelay);
//  Serial.print(buf);
//  if (todelay > 0) delay(todelay);
//  neo_exec_draw(TIME_INTV);
//  show_frame(frame);
  delay(1000);
  Serial.print('*');
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void show_frame(struct neo_color *frame) {
  for(uint16_t i=0; i<NEO_N; i++) {
    strip.setPixelColor(i, frame[i].r, frame[i].g, frame[i].b);
  }
  strip.show();
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while(1) { delay(200); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); }
       }
  }

  Serial.println("MQTT Connected!");
}

