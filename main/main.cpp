/*  Main file
 *
 */
#include "Arduino.h"
#include <WiFi.h>
#include "config.h"


// Arduino like analogWrite
// value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  // calculate duty
  uint32_t duty = (LEDC_BASE_FREQ / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}


void setup() {
  // Setup timer and attach timer to a led pins
  ledcSetup(LEDC_CHANNEL_0_R, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN_R, LEDC_CHANNEL_0_R);
  ledcSetup(LEDC_CHANNEL_1_G, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN_G, LEDC_CHANNEL_1_G);
  ledcSetup(LEDC_CHANNEL_2_B, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN_B, LEDC_CHANNEL_2_B);
  ledcSetup(LEDC_CHANNEL_3_BOD, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN_BOD, LEDC_CHANNEL_3_BOD);
  ledcSetup(LEDC_CHANNEL_4_TABLE, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_table, LEDC_CHANNEL_4_TABLE);

  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(LED_PIN_BOD_toggle, OUTPUT);
  pinMode(FRIDGE_PIN, OUTPUT);
  // Set outputs to LOW
  digitalWrite(LED_PIN_BOD_toggle, LOW);
  digitalWrite(FRIDGE_PIN, LOW);

  // Connect to Wi-Fi network with SSID and password
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

  // PWM
  ledcAnalogWrite(LEDC_CHANNEL_0_R, 0);
  ledcAnalogWrite(LEDC_CHANNEL_1_G, 0);
  ledcAnalogWrite(LEDC_CHANNEL_2_B, 0);
  ledcAnalogWrite(LEDC_CHANNEL_3_BOD, 255);
  ledcAnalogWrite(LEDC_CHANNEL_4_TABLE, 0);
}

void loop(){
  delay(1000);

  // WiFi
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
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
            if (header.indexOf("GET /21/on") >= 0) {
              Serial.println("GPIO 21 on");
              output21State = "on";
              digitalWrite(LED_PIN_BOD_toggle, HIGH);
            } else if (header.indexOf("GET /21/off") >= 0) {
              Serial.println("GPIO 21 off");
              output21State = "off";
              digitalWrite(LED_PIN_BOD_toggle, LOW);
            } else if (header.indexOf("GET /RGB/on") >= 0) {
              Serial.println("RGB on");
              outputRGBState = "on";
              ledcAnalogWrite(LEDC_CHANNEL_0_R, 255);
              ledcAnalogWrite(LEDC_CHANNEL_1_G, 255);
              ledcAnalogWrite(LEDC_CHANNEL_2_B, 255);
            } else if (header.indexOf("GET /RGB/off") >= 0) {
              Serial.println("RGB off");
              outputRGBState = "off";
              ledcAnalogWrite(LEDC_CHANNEL_0_R, 0);
              ledcAnalogWrite(LEDC_CHANNEL_1_G, 0);
              ledcAnalogWrite(LEDC_CHANNEL_2_B, 0);
            } else if (header.indexOf("GET /table/on") >= 0) {
              Serial.println("Table on");
              outputTableState = "on";
              ledcAnalogWrite(LEDC_CHANNEL_4_TABLE, 255);
            } else if (header.indexOf("GET /table/off") >= 0) {
              Serial.println("Table off");
              outputTableState = "off";
              ledcAnalogWrite(LEDC_CHANNEL_4_TABLE, 0);
            } else if (header.indexOf("GET /fridge/on") >= 0) {
              Serial.println("Fridge on");
              outputFridgeState = "on";
              digitalWrite(FRIDGE_PIN, HIGH);
            } else if (header.indexOf("GET /fridge/off") >= 0) {
              Serial.println("Fridge off");
              outputFridgeState = "off";
              digitalWrite(FRIDGE_PIN, LOW);
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");

            // Display current state, and ON/OFF buttons for GPIO 21
            client.println("<p>Main " + output21State + "</p>");
            // If the output21State is off, it displays the ON button
            if (output21State=="off") {
              client.println("<p><a href=\"/21/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/21/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("<p>RGB " + outputRGBState + "</p>");
            // If the output21State is off, it displays the ON button
            if (outputRGBState=="off") {
              client.println("<p><a href=\"/RGB/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/RGB/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("<p>Table " + outputTableState + "</p>");
            // If the output21State is off, it displays the ON button
            if (outputTableState=="off") {
              client.println("<p><a href=\"/table/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/table/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            client.println("<p>Fridge " + outputFridgeState + "</p>");
            // If the output21State is off, it displays the ON button
            if (outputFridgeState=="off") {
              client.println("<p><a href=\"/fridge/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/fridge/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

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
}
