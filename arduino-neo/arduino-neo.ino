/*
 * 使用说明：
 *    上点后LED闪烁，尝试连接WIFI（1Hz频率闪烁）
 *    连接上后尝试连接MQTT，如果连接失败LED将快速闪烁（2.5Hz闪烁），这个错误不可恢复，只能尝试重启
 *    正常连接到MQTT服务器后保持LED亮状态
 *    之后每收到一条信息，LED将变换状态（从亮变暗，或，从暗变亮，不一定）
 */

//#define DEBUG_NO_WIFI_ONLY_LED

#include "neo_PubSubClient.h"
#include "neo_exec.h"
#include <Adafruit_NeoPixel.h>
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <interrupts.h>
#define NEO_PIN D1
#endif
#ifdef ESP32
#include <WiFi.h>
#define NEO_PIN 17
#endif

#define TIME_INTV 20  // ms，每一帧的间隔

#define WLAN_SSID       "geeklab"
#define WLAN_PASS       "ai@geeklab"

#define AIO_SERVER      "onedawn.top"
#define AIO_SERVERPORT  8883                   // 8883 for MQTTS
#define AIO_USERNAME    "sVyd16qv"
#define AIO_KEY         "iIJQUrn5SkwpaCqG4omyW3Tb6z7RPvBY"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEO_N, NEO_PIN, NEO_GRB + NEO_KHZ800);
WiFiClient client;
void callback(char* topic, byte* payload, unsigned int length);
PubSubClient mqtt(AIO_SERVER, AIO_SERVERPORT, callback, client);

const char* test1 = "1 default\n128 0 13 -1\nfg:0 FF0000;5 0000FF;13 00FFFF\nsg:5000\nfg:0 00FF00;13 0000FF\nsg:5000\nfg:0 FF0000;5 0000FF;13 00FFFF\n";
int ms, fidx, startms, todelay;
#define BUF_LEN 512
char buf[BUF_LEN];

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  if (strcmp("iot/" AIO_USERNAME "/ask", topic) == 0) {
    mqtt.publish("iot/" AIO_USERNAME "/info", "esp8266-neoLight, see https://github.com/wuyuepku/esp8266-neoLight\n"
      "usage:\n"
      "./neo/ask <null>:see the procedures in this machine\n"
      "./neo/delete <num>: delete a procedure start from index 0\n"
      "./neo/add <procedure> add a procedure using compiled string\n");
  } else if (strcmp("iot/" AIO_USERNAME "/neo/add", topic) == 0) {  // 添加一个procedure，用./neo/add_ret返回
    payload[length] = '\0';
    sprintf(buf, "%d", neo_exec_load((const char*)payload));
    mqtt.publish("iot/" AIO_USERNAME "/neo/add_ret", buf);
  } else if (strcmp("iot/" AIO_USERNAME "/neo/delete", topic) == 0) {  // 强行delete一个procedure，用./neo/delete_ret返回
    int slot2delete;
    if (length < 10) {
      payload[length] = '\0';
      sscanf((const char*)payload, "%d", &slot2delete);
      neo_exec_delete(slot2delete);
      mqtt.publish("iot/" AIO_USERNAME "/neo/delete_ret", "0");
    } else mqtt.publish("iot/" AIO_USERNAME "/neo/delete_ret", "-1");
  } else if (strcmp("iot/" AIO_USERNAME "/neo/ask", topic) == 0) {
    neo_info(buf);
    mqtt.publish("iot/" AIO_USERNAME "/neo/info", buf);
  }
//  Serial.println(length);
}

void setup() {

  Serial.begin(115200);
  delay(100);
  Serial.println("esp8266-neoLight with MQTT support");
  delay(1000);

#ifndef DEBUG_NO_WIFI_ONLY_LED
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
#endif
  
  strip.begin();
  {
    #ifdef ESP8266
    InterruptLock lock;
    #endif
    #ifdef ESP32
    delay(1);
    portDISABLE_INTERRUPTS();
    #endif 
    strip.show();
    #ifdef ESP32
    portENABLE_INTERRUPTS();
    #endif 
  }

  // initialize neo_exec
  neo_exec_init();
  
#ifdef DEBUG_NO_WIFI_ONLY_LED
  neo_exec_load(test1);
#endif

#ifndef DEBUG_NO_WIFI_ONLY_LED
  // 连接MQTTs
  randomSeed(micros());
  Serial.print("Connecting to ");
  Serial.println(AIO_SERVER);
#endif
  
  Serial.println(millis());
  startms = -1;
  fidx = 0;
}

void loop() {
#ifndef DEBUG_NO_WIFI_ONLY_LED
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();
#endif

  if (startms < 0) startms = millis();
  fidx += 1;
  ms = millis();
  todelay = TIME_INTV * fidx + startms - ms;
  sprintf(buf, "% 3d: delay %d\n", fidx, todelay);
  Serial.print(buf);
  if (todelay > 0) delay(todelay);
  neo_exec_draw(TIME_INTV);
  show_frame(frame);
}

void show_frame(struct neo_color *frame) {
  for(uint16_t i=0; i<NEO_N; i++) {
    strip.setPixelColor(i, frame[i].r, frame[i].g, frame[i].b);
  }
  {
    #ifdef ESP8266
    InterruptLock lock;
    #endif
    #ifdef ESP32
    delay(1);
    portDISABLE_INTERRUPTS();
    #endif 
    strip.show();
    #ifdef ESP32
    portENABLE_INTERRUPTS();
    #endif 
  }
}

void reconnect() {
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqtt.connect(clientId.c_str(), AIO_USERNAME, AIO_KEY, "iot/" AIO_USERNAME "/offline", 0, 0, "esp82660-neoLight")) {
      Serial.println("connected");
      mqtt.subscribe("iot/" AIO_USERNAME "/ask");
      mqtt.subscribe("iot/" AIO_USERNAME "/neo/ask");
      mqtt.subscribe("iot/" AIO_USERNAME "/neo/delete");
      mqtt.subscribe("iot/" AIO_USERNAME "/neo/add");
      mqtt.publish("iot/" AIO_USERNAME "/online","esp82660-neoLight");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000); digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
  }
}


