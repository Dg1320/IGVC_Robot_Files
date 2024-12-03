/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  1. Connect to the access point "myWifi"
  2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off
     OR
     Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>


#define GPIO_PIN2 2     // These will be the output 4 output signals to change speed of motors
#define GPIO_PIN19 19
#define GPIO_PIN21 21
#define GPIO_PIN25 25


// Set these to your desired credentials.
const char *ssid = "NETGEAR96";
const char *password = "excitedowl661";

NetworkServer server(80);

void setup() {
  pinMode(GPIO_PIN2, OUTPUT);
  pinMode(GPIO_PIN19, OUTPUT);
  pinMode(GPIO_PIN21, OUTPUT);
  pinMode(GPIO_PIN25, OUTPUT);

    Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  if (!WiFi.softAP(ssid, password)) {
    log_e("Soft AP creation failed.");
    while (1);
  }
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();

  Serial.println("Server started");
}

void loop() {
  NetworkClient client = server.accept();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/FORWARD\">here</a> to go Forward.<br>");
            client.print("Click <a href=\"/RIGHT\">here</a> to turn Right.<br>");
            client.print("Click <a href=\"/LEFT\">here</a> to turn Left.<br>");
            client.print("Click <a href=\"/STOP\">here</a> to stop.<br>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /FORWARD" , "GET /LEFT" , "GET /RIGHT" , "GET /BACKWARD ", "GET /STOP ":
        if (currentLine.endsWith("GET /FORWARD")) {
              digitalWrite(GPIO_PIN2, !digitalRead(GPIO_PIN2));        
        }
        if (currentLine.endsWith("GET /RIGHT")) {
              digitalWrite(GPIO_PIN21, !digitalRead(GPIO_PIN21));        
          }
        if (currentLine.endsWith("GET /LEFT")) {
              digitalWrite(GPIO_PIN25, !digitalRead(GPIO_PIN25));        
          }
        if (currentLine.endsWith("GET /STOP")) {
              digitalWrite(GPIO_PIN19, !digitalRead(GPIO_PIN19));        
          }
      }
    
        }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}

