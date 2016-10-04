#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid     = "WLAN";
const char* password = "1234567890";

IPAddress host(192,168,178,1);
const int httpPort = 49000;

#define LED D0 // Led in NodeMCU at pin GPIO16 (D0).

String getSoapXML(){
  String xml = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
  xml += "<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">";
  xml += "<s:Body>";
  xml += "<u:GetCommonLinkProperties xmlns:u=\"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1\" />";
  xml += "</s:Body>";
  xml += "</s:Envelope>";

  return xml;
}

String getSoapHeader(String url, String host, String soapaction, String clength){
  String header = String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Content-Type: text/xml; charset=\"utf-8\"" + "\r\n" +
                          soapaction + "\r\n" +
                          "Content-Length: "+ clength +"\r\n\r\n";
                          //"Connection: close\r\n\r\n";

  return header;
}

void getFritzData(){
  WiFiClient client;
  if (client.connect(host, httpPort)) {

    String soapaction = "SoapAction:urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetAddonInfos";
    String url  = "/igdupnp/control/WANCommonIFC1";
    String host = "192.168.178.1:49000";
    String body = getSoapXML();
    String length = String(body.length());
    String header = getSoapHeader(url, host, soapaction, length);

    Serial.print("Requesting URI: ");
    Serial.println(host+url);
    Serial.println("length: "+length);

    client.print(header+body);

    int i=0;
    while((!client.available()) && (i<1000)){
      delay(100);
      Serial.print(".");
      digitalWrite(LED, !digitalRead(LED));
      i++;
    }

    // Read all the lines of the reply from server and print them to Serial
    String lines = "";
    while(client.available()){
      Serial.print(".");
      digitalWrite(LED, !digitalRead(LED));
      lines += client.readStringUntil('\r');
    }

    Serial.println(lines);
  }
  else{
    Serial.println("no connection to fritzbox");
  }
}

void setup(void){
  pinMode(LED, OUTPUT);
  //digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(5000);
  Serial.println("Start");
  getFritzData();
  delay(5000);
}
