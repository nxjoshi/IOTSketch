/*
  For Experimenting with Oracle Products
*/


#include "DHT.h"
#define DHTPIN 2    
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);


float tValue;
float hValue;



#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>





const char* serverName = "http://192.168.1.14:3002";    
const char* Nodemcuserver = "http://192.168.31.190/header";
const char* OICServer = "https://oic-dev-orasenatdpltsecitom03-ia.integration.ocp.oraclecloud.com/ic/api/integration/v1/flows/rest/IOC_ATP_IOT/1.0/ATPInsert"
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <title>Simple login form</title>
    <link href="https://fonts.googleapis.com/css?family=Roboto:300,400,500,700" rel="stylesheet">
    <style>
      html, body {
      display: flex;
      justify-content: center;
      font-family: Roboto, Arial, sans-serif;
      font-size: 15px;
      }
      form {
      border: 5px solid #f1f1f1;
      }
      input[type=text], input[type=password] {
      width: 100%;
      padding: 16px 8px;
      margin: 8px 0;
      display: inline-block;
      border: 1px solid #ccc;
      box-sizing: border-box;
      }
      button {
      background-color: #8ebf42;
      color: white;
      padding: 14px 0;
      margin: 10px 0;
      border: none;
      cursor: grabbing;
      width: 100%;
      }
      h1 {
      text-align:center;
      fone-size:18;
      }
      button:hover {
      opacity: 0.8;
      }
      .formcontainer {
      text-align: left;
      margin: 24px 50px 12px;
      }
      .container {
      padding: 16px 0;
      text-align:left;
      }
      span.psw {
      float: right;
      padding-top: 0;
      padding-right: 15px;
      }
      /* Change styles for span on extra small screens */
      @media screen and (max-width: 300px) {
      span.psw {
      display: block;
      float: none;
      }
    </style>
  </head>
  <body background ="/Users/nitishjoshi/Documents/Arduino/Blockchain_IOT/Nodemcu_Blockchain/image/image.jpg">
  
    <form action="/action_page">
      <h1>MachineChain</h1>
      <div class="formcontainer">
      <hr/>
      <div class="container">
        <label for="temp"><strong>Temprature</strong></label>
        <input type="number" placeholder="Enter Temperature" name="temp" required>
        <label for="humid"><strong>Humidity</strong></label>
        <input type="number" placeholder="Enter Humidity" name="humid" required>
      </div>
      <button type="submit">Send Report</button>
      <div class="container" style="background-color: #eee">
       
      </div>
    </form>
  </body>
</html>
)=====";



const char* ssid = "Airtel_8454899945";
const char* password = "air47214";



ESP8266WebServer server(80); //Server on port 80

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
//===============================================================
// This routine is executed when you press submit
//===============================================================


void handleForm() {
 
  
// String temp = server.arg("temp"); 
// String humid = server.arg("humid");

 String temp = String(tValue, 2); 
 String humid = String(hValue, 2);

 StaticJsonDocument<200> doc;

  doc["temperature"] = temp;
  doc["humidity"] = humid;

if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      http.begin(client, serverName);
      http.addHeader("Content-Type", "application/json");
      char out[200];
      Serial.println( serializeJson(doc, out));
      String httpRequestData =   out   ;        
      int httpResponseCode = http.POST(httpRequestData);
      
      Serial.println(httpRequestData);
      Serial.print("HTTP Response code is: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      client.println("<p>GPIO 5 - State " + payload + "</p>");
      Serial.println(payload); 
      
      server.send(200, "text/html", "Blockchain Response : (" 
                                + payload + ")"
                                "<br><a href=\"/\">Return to Home Page</a>"); //Send web page
      http.end();
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }




}




void setup(void) {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

//==============================================================
//                  SETUP
//==============================================================


  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println("WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  server.on("/", handleRoot);      //Which routine to handle at root location
  server.on("/action_page", handleForm); 
  
  server.begin();                  //Start server
  Serial.println("HTTP server started");
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void) {
  server.handleClient();  

  float c = dht.readTemperature();// Read temperature as Celsius (the default)
  float h = dht.readHumidity();// Reading humidity 
  tValue = c;
  hValue=h;
  delay(10000);

  String temp = String(tValue, 2); 
 String humid = String(hValue, 2);
Serial.println(tValue);
 StaticJsonDocument<200> doc;

  doc["temperature"] = temp;
  doc["humidity"] = humid;

if ((millis() - lastTime) > timerDelay) {
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      http.begin(client, OICServer);
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Authorization", "Basic YXBpdXNlcjpFQlNpbnRlZ3JhdGVAMjAyMw==");
      char out[200];
      Serial.println( serializeJson(doc, out));
      String httpRequestData =   out   ;        
      int httpResponseCode = http.POST(httpRequestData);
      
      Serial.println(httpRequestData);
      Serial.print("HTTP Response code is: ");
      Serial.println(httpResponseCode);
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }

}
