#include <Arduino.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
// #include "../lib/functions.h"


/* NUMBER OF CONTROLED RELAYS */
#define RELAYS 4
/**/


/* CREATE WEB SERVER OBJECT */
ESP8266WebServer server(80);
/**/

/* CREATE WEB SOCKET SERVER OBJECT*/
WebSocketsServer socket = WebSocketsServer(88);
/**/

/* RELAY PINS VALUES AND INITIALIZE */
int BUTTONS_VAL[RELAYS] = {0,0,0,0};
/**/

/* RELAY PINS */
const int BUTTONS[RELAYS] = {14,12,13,15};
/**/

/* WEB PAGE*/
const char webpage[] PROGMEM =
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

            var received_data = evt.data.split(' ');
            var button = received_data[1];
            var value = received_data[0];

            if(value == '1'){
              document.getElementById(button).innerHTML = 'OFF';
            }
            else if(value == '0'){
              document.getElementById(button).innerHTML = 'ON';
            }

          } // end of onmessage
        } // end off init

        function button(id){
          if(document.getElementById(id).innerHTML === 'OFF'){
            msg = '0' + ' ' + id;
          }
          else if(document.getElementById(id).innerHTML === 'ON'){
            msg = '1' + ' ' + id;
          }
          websock.send(msg);
        }

      </script>

    </head>
    <body>
      <h1> SMART BOX UNDER CONSTRACTION </h1>
      <h3>Yellow</h3>
      <a id="0" onclick='button(0)'></a>
      <h3>Green</h3>
      <a id="1" onclick='button(1)'></a>
      <h3>Red</h3>
      <a id="2" onclick='button(2)'></a>
      <h3>Blue</h3>
      <a id="3" onclick='button(3)'></a>
    </body>
  </html>


)=====";

void event(uint8_t num, WStype_t type, uint8_t * payload, size_t welength) {

  String msg = (const char*)  payload;
  Serial.print("MESSAGE : ");
  Serial.println(msg);

  if(type == WStype_TEXT){
    byte separator = msg.indexOf(' ');
    String value = msg.substring(0,separator);
    String button = msg.substring(separator+1);
    Serial.print("BUTTON : ");
    Serial.println(button);
    Serial.print("VALUE : ");
    Serial.println(value);
    Serial.println(" ");

    BUTTONS_VAL[button.toInt()] = value.toInt();

  }

}

void website(){
  server.send(200, "text/html", webpage);
}

void setup() {
  Serial.begin(115200);

  /* INITIALIZE ALL BUTTONS AS OUTPUTS */
  for (unsigned int i = 0; i < RELAYS; i++) {
    pinMode(BUTTONS[i], OUTPUT);
  }
  /**/

  /* RESET WIFI PASSWORD */
  // WiFi.disconnect();
  // delay(1000);
  /**/

  /* START WIFI MANAGER FOR CONNECTING TO WIFI*/
  WiFiManager wifiManager;
  wifiManager.autoConnect("Smart Box","Smart Box");
  /**/

  /* PRINT LOCAL IP ADDRESS AND START WEB SERVER */
  Serial.println("");
  Serial.println("WIFI CONNECTED.");
  Serial.print("IP ADDRESS: ");
  Serial.println(WiFi.localIP());

  WiFi.mode(WIFI_STA);
  Serial.print(" START WEB SERVER ");
  server.on("/",website);
  server.begin();
  /**/

  /* START WEB SOCKET SERVER */
  socket.begin();
  socket.onEvent(event);
  /**/

}

void loop(){
  for (unsigned int i = 0; i < RELAYS; i++) {

    socket.loop();
    server.handleClient();


    if( BUTTONS_VAL[i] == 1){
      digitalWrite(BUTTONS[i], LOW);
    }
    else{
      digitalWrite(BUTTONS[i],HIGH);
    }

    socket.broadcastTXT(String(BUTTONS_VAL[i])+' '+String(i));
  }
}
