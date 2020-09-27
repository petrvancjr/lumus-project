const char* ssid = "free-LHOTKA";
const char* password = "12Lhotka5";
// web server port
WiFiServer server(80);
// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


// The PWM config
// use first 3 channels of 16 channels (started from zero)
#define LEDC_CHANNEL_0_R  0
#define LEDC_CHANNEL_1_G  1
#define LEDC_CHANNEL_2_B  2
#define LEDC_CHANNEL_3_BOD  3
#define LEDC_CHANNEL_4_TABLE 4

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  13

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

// Auxiliar variables to store the current output state
String output21State = "off";
String outputRGBState = "off";
String outputTableState = "off";
String outputFridgeState = "off";

// LED PINs
#define LED_PIN_R   0
#define LED_PIN_G   16
#define LED_PIN_B   17
#define LED_PIN_BOD 15
#define LED_PIN_BOD_toggle 21
#define LED_table 4
#define FRIDGE_PIN 22
