#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <config.h>

IPAddress host(192,168,178,1);
const int httpPort = 49000;

const long MAXVU = 80;
const long MINVU = 1;
const long MAXBYTEDN = 4000000;//32 mbit
const long MAXBYTEUP = 250000;//2 mbit

//holds receive/send data
struct sr {
  int sent;
  int receive;
};

//returns xml body for fritzbox request
String getSoapXML(){
  String xml = "";
  xml += "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
  xml += "<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">";
  xml += "<s:Body>";
  xml += "<u:GetCommonLinkProperties xmlns:u=\"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1\" />";
  xml += "</s:Body>";
  xml += "</s:Envelope>";

  return xml;
}

//returns SOAP POST header for fritzbox request
String getSoapHeader(String url, String host, String soapaction, String clength){
  String header = String("POST ") + url + " HTTP/1.1\r\n" +
                          "Host: " + host + "\r\n" +
                          "Content-Type: text/xml; charset=\"utf-8\"" + "\r\n" +
                          soapaction + "\r\n" +
                          "Content-Length: "+ clength +"\r\n\r\n";

  return header;
}

//parse Receive/Send tags form fritzbox xml
sr parseData(String xml){
  String sent     = xml.substring(xml.indexOf("<NewByteSendRate>")+17,xml.indexOf("</NewByteSendRate>"));
  String receive  = xml.substring(xml.indexOf("<NewByteReceiveRate>")+20,xml.indexOf("</NewByteReceiveRate>"));
  Serial.println(sent);
  Serial.println(receive);

  sr parsed = {sent.toInt(),receive.toInt()};
  return parsed;
}

//SOAP xml request to fritzbox and parse Receive/Send data
sr getFritzData(){
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
    return parseData(lines);
  }
  else{
    Serial.println("no connection to fritzbox");
  }
}

//move VU for startup
void startupVU(){
  for (size_t i = 1; i < MAXVU; i++) {
    analogWrite(VUDN,i);
    delay(15);
  }
  for (size_t i = MAXVU; i > 0; i--) {
    analogWrite(VUDN,i);
    delay(15);
  }

}

void setup(void){
  pinMode(LED, OUTPUT);
  pinMode(VUUP, OUTPUT);
  //digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(SSID, PWD);
  Serial.println("start vu");

  startupVU();

  Serial.println("start wlan");
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
  delay(10);
  sr data = getFritzData();
  long dn = map((long)data.receive,0,MAXBYTEDN,MINVU,MAXVU);
  long up = map((long)data.sent,0,MAXBYTEUP,MINVU,MAXVU);
  analogWrite(VUDN,(int) dn);
  analogWrite(VUUP,(int) up);

  Serial.println("down: "+String(dn)+" "+String(data.receive));
  Serial.println("up: "+String(up)+" "+String(data.sent));
}
