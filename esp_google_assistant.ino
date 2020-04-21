#define IO_USERNAME  "Shrey_786"
#define IO_KEY       "671fcbc09bd5426991bff60e1046f46d"
#define WIFI_SSID       "AndroidAP"
#define WIFI_PASS       "chuche1223"
#include "AdafruitIO_WiFi.h"
//#include "AdafruitIO_ESP8266.h"
#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

/************************ Main Program Starts Here *******************************/
#include <ESP8266WiFi.h>
#include <AdafruitIO.h>
#include <Adafruit_MQTT.h>
#include <ArduinoHttpClient.h>
AdafruitIO_Feed *command = io.feed("voice commands");
const int output4 = 5;
WiFiServer server(80);

const char* ssid     = "AndroidAP";

const char* password = "chuche1223";


// Variable to store the HTTP request

String header;
unsigned long currentTime = millis();

// Previous time

unsigned long previousTime = 0; 

// Define timeout time in milliseconds (example: 2000ms = 2s)

const long timeoutTime = 2000;
String output5State = "off";
void setup() {
  // put your setup code here, to run once:
  pinMode(output4,OUTPUT);
  digitalWrite(output4,LOW);
  Serial.begin(9600);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'command' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  command->onMessage(handleMessage);  

  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(50);
  }
  
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
    Serial.print("Connecting to ");

  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");

  }

  // Print local IP address and start web server

  Serial.println("");

  Serial.println("WiFi connected.");

  Serial.println("IP address: ");

  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  WiFiClient client = server.available();   // Listen for incoming clients



  if (client) {                             // If a new client connects,

    Serial.println("New Client.");          // print a message out in the serial port

    String currentLine = "";                // make a String to hold incoming data from the client

    currentTime = millis();

    previousTime = currentTime;

    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected

      currentTime = millis();         

      if (client.available()) {             // if there's bytes to read from the client,

        char c = client.read();             // read a byte, then

        Serial.write(c);                    // print it out the serial monitor

        header += c;

        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.

          // that's the end of the client HTTP request, so send a response:

          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)

            // and a content-type so the client knows what's coming, then a blank line:

            client.println("HTTP/1.1 200 OK");

            client.println("Content-type:text/html");

            client.println("Connection: close");

            client.println();

            

            // turns the GPIOs on and off

            if (header.indexOf("GET /5/on") >= 0) {

              Serial.println("GPIO 5 on");

              output5State = "on";

              digitalWrite(output4, HIGH);

            } else if (header.indexOf("GET /5/off") >= 0) {

              Serial.println("GPIO 5 off");

              output5State = "off";

              digitalWrite(output4, LOW);

            } 

            

            // Display the HTML web page

            client.println("<!DOCTYPE html><html>");

            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");

            client.println("<link rel=\"icon\" href=\"data:,\">");

            // CSS to style the on/off buttons 

            // Feel free to change the background-color and font-size attributes to fit your preferences

            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");

            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");

            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");

            client.println("</style></head>");

            

            // Web Page Heading

            client.println("<body><h1>ESP8266 Web Server</h1>");

            

            // Display current state, and ON/OFF buttons for GPIO 5  

            client.println("<p>GPIO 5 - State " + output5State + "</p>");

            // If the output5State is off, it displays the ON button       

            if (output5State=="off") {

              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");

            } else {

              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");

            } 

               

            // Display current state, and ON/OFF buttons for GPIO 4  

           /* client.println("<p>GPIO 4 - State " + output4State + "</p>");

            // If the output4State is off, it displays the ON button       

            if (output4State=="off") {

              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");

            } else {

              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");

            }*/

            client.println("</body></html>");

            

            // The HTTP response ends with another blank line

            client.println();

            // Break out of the while loop

            break;

          } else { // if you got a newline, then clear currentLine

            currentLine = "";

          }

        } else if (c != '\r') {  // if you got anything else but a carriage return character,

          currentLine += c;      // add it to the end of the currentLine

        }

      }

    }

    // Clear the header variable

    header = "";

    // Close the connection

    client.stop();

    Serial.println("Client disconnected.");

    Serial.println("");

  }
io.run();
}


void handleMessage(AdafruitIO_Data *data) {
  
  String commandStr = data->toString(); // store the incoming commands in a string
  
  // received message
  Serial.print("received <- ");
  Serial.println(commandStr);
   if (commandStr.equalsIgnoreCase("high")){    

    digitalWrite(output4,HIGH);
   }
   else if (commandStr.equalsIgnoreCase("low")){    

    digitalWrite(output4,LOW);
   }
   }
