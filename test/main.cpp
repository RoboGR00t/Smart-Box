#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>


const int LED = 0;
int websockMillis = 50 ;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(88);
String webSite, JSONtxt;
boolean LEDonoff = true;

const char webSiteCont[] PROGMEM =
R"=====(

  <!DOCTYPE html>
  <html lang="en" dir="ltr">
    <head>
      <meta charset="utf-8">
      <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
      <title>Home.io</title>

      <script>

        InitWebSocket()
        function InitWebSocket(){
          websock = new WebSocket('ws://'+window.location.hostname+':88/');
          websock.onmessage = function(evt){
            JSONobj = JSON.parse(evt.data);
            document.getElementById('btn').innerHTML = JSONobj.LEDonoff;

            if(JSONobj.LEDonoff == 'ON'){
              document.getElementById('btn').style.background = 'FFA200';
            }
            else{
              document.getElementById('btn').style.background = 'FFA200';
            }
          } // end of onmessage
        } // end off init

        function button(){
          btn = 'LEDonoff=ON';
          if(document.getElementById('btn').innerHTML === 'ON'){
            btn = 'LEDonoff=OFF';
          }
          websock.send(btn);
        }

      </script>

    </head>
    <body>
<!--      <a href="#" id="btn" onclick='button()'></a>-->
      <input type="checkbox" id="btn" onclick='button()'>

    </body>
  </html>


)=====";

void WebSite(){

  server.send(200, "text/html", webSiteCont);

}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t welength){

  String payloadString = (const char *) payload;
  Serial.print("payloadString = ");
  Serial.println(payloadString);

  if(type == WStype_TEXT){
    byte separator = payloadString.indexOf('=');
    String var = payloadString.substring(0,separator);
    Serial.print("var = ");
    Serial.println(var);
    String val = payloadString.substring(separator+1);
    Serial.print("val = ");
    Serial.println(val);
    Serial.println(" ");

    if (var == "LEDonoff") {
      LEDonoff = false;
      if(val == "ON") {
        LEDonoff = true;
      }
    }

  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  // WiFi.disconnect();
  delay(1000);

  WiFiManager wifiManager;
  wifiManager.autoConnect("Smart Box","Smart Box");

  /* Print local IP address and start web server */
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  WiFi.mode(WIFI_STA);
  Serial.print(" Start ESP ");
  server.on("/",WebSite);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

}

void loop() {

  webSocket.loop();
  server.handleClient();

  if( LEDonoff == true){
    digitalWrite(LED, LOW);
  }
  else{
    digitalWrite(LED,HIGH);
  }

  String LEDswitch = "OFF";

  if(LEDonoff == true) LEDswitch = "ON";
  JSONtxt = "{\"LEDonoff\":\"" + LEDswitch + "\"}";
  webSocket.broadcastTXT(JSONtxt);



}
