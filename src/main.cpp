#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <config.h>

IPAddress host(192,168,178,1);
const int httpPort = 49000;

struct sr {
  int sent;
  int receive;
};

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

sr parseData(String xml){
  String sent     = xml.substring(xml.indexOf("<NewByteSendRate>")+17,xml.indexOf("</NewByteSendRate>"));
  String receive  = xml.substring(xml.indexOf("<NewByteReceiveRate>")+20,xml.indexOf("</NewByteReceiveRate>"));
  Serial.println(sent);
  Serial.println(receive);

  sr parsed = {sent.toInt(),receive.toInt()};
  return parsed;
}

void getFritzData(){
  WiFiClient client;
  if (client.connect(host, httpPort)) {

    String soapaction = "SoapAction:urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1#GetAddonInfos";
    String url      = "/igdupnp/control/WANCommonIFC1";
    String hostport = host.toString() + ":" + String(httpPort);
    String body     = getSoapXML();
    String length   = String(body.length());
    String header   = getSoapHeader(url, hostport, soapaction, length);


    Serial.print("Requesting URI: ");
    Serial.println(hostport+url);
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
    parseData(lines);
  }
  else{
    Serial.println("no connection to fritzbox");
  }
}

void setup(void){
  pinMode(LED, OUTPUT);
  //digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(SSID, PWD);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(5000);
  Serial.println("Start");
  getFritzData();
  delay(5000);
}
