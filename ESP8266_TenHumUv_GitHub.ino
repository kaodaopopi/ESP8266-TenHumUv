#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);

// 設定無線基地台SSID跟密碼
const char* ssid    = "WiFi名稱";     
const char* password = "WiFi密碼";     

void setup(void)
{
  Serial.begin(115200);  // 設定速率 感測器
  dht.begin();           // 初始化

  // 連接無線基地台
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  // 等待連線，並從 Console顯示 IP
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(A0, INPUT);
}
 
void loop(void)
{
  delay(30000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int UV=analogRead(A0); //讀取UV感測器回傳值
  if (isnan(h) || isnan(t)|| isnan(UV) ) {
    Serial.println(F("感測器讀取失敗"));
    return;
  }
  Serial.print(F("濕度: "));
  Serial.print(h);
  Serial.print(F("% 溫度: "));
  Serial.print(t);
  Serial.println("°C");
  Serial.print("UV:");
  Serial.println(UV);

  //執行API Request
  WiFiClient client;
  const uint16_t port = 80;
  const char* host = "IP位置"; 
  client.connect(host, port);
  
  // 檢查連線是否成功
  if (!client.connect(host, port)) {
  Serial.println("connection failed!");
  return;
  }else{  
    String postStr = "";
    postStr +="TemHumCode=";
    postStr += String("1");
    postStr +="&Temperature=";
    postStr += String(t);
    postStr +="&Humidity=";
    postStr += String(h);
    postStr += "&UVCode=";
    postStr += String("1");
    postStr += "&UVData=";
    postStr += String(UV);
      
    String url = "/api/ESP8266TemHumUv?" + postStr;
    client.print(String("GET ")  + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" +"Connection:close\r\n\r\n"); 
     //查看回傳值
     while(client.available()){
     String line = client.readStringUntil('\r');
     Serial.print(line);
    }
  }
  Serial.println("連線關閉");
  client.stop();

} 
