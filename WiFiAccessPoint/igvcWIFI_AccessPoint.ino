/*
  WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.

  1. Connect to the access point "IGVC_WIFI"
  2. Point your web browser to http://192.168.4.1/ to connect to server

                               http://192.168.4.1/FORWARD               // to issue commands  
                               http://192.168.4.1/BACKWARD
                               http://192.168.4.1/RIGHT
                               http://192.168.4.1/LEFT
*/

#include <WiFi.h>
#include <NetworkClient.h>
#include <WiFiAP.h>


#define GPIO_PIN2 2     // These will be the output 4 output signals to change speed of motors
#define GPIO_PIN19 19


// Set these to your desired credentials.
const char *ssid = "IGVC_WIFI";
const char *password = "igvcpassword";

NetworkServer server(80);

void setup() {
  pinMode(GPIO_PIN2, OUTPUT);
  pinMode(GPIO_PIN19, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");


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
            client.print("Click <a href=\"/BACKWARD\">here</a> to go Reverse.<br>");
            client.print("Click <a href=\"/RIGHT\">here</a> to turn Right.<br>");
            client.print("Click <a href=\"/LEFT\">here</a> to turn Left.<br>");
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

        // Check to see if the client request was "GET /R" or "GET /L" "GET /F" or "GET /B":
        if (currentLine.endsWith("GET /FORWARD")) {
          digitalWrite(GPIO_PIN2, HIGH);  // GET /F turns the LED ON
          digitalWrite(GPIO_PIN19, HIGH);  // GET /F turns the LED2 ON
        }
        if (currentLine.endsWith("GET /BACKWARD")) {
          digitalWrite(GPIO_PIN2, LOW);  // GET /B turns the LED OFF
          digitalWrite(LED_BUILTIN2, LOW);  // GET /B turns the LED OFF       
          }
        if (currentLine.endsWith("GET /RIGHT")) {
          digitalWrite(GPIO_PIN2, LOW);  // GET /R turns the LED OFF
          digitalWrite(GPIO_PIN19, HIGH);  // GET /R turns the LED ON        
          }
        if (currentLine.endsWith("GET /LEFT")) {
          digitalWrite(GPIO_PIN2, HIGH);  // GET /L turns the LED ON
          digitalWrite(GPIO_PIN19, LOW);  // GET /L turns the LED2 OFF        
          }
      }
    
        }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
