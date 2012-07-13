#define DEBUG true

#include "Arduino.h"
#include "SPI.h"
#include "Ethernet.h"

//--------------------------------------------------------------------------------------
// Configuration: Global
//--------------------------------------------------------------------------------------
int sdcardPin = 4;       // Arduino SD card pin
int ethernetPin = 53;    // Arduino ethernet pin

byte mac[] = {  0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAB };   // Local ethernet MAC address
byte ip[] = { 192,168,11,51 };                          // Local IP address

EthernetServer server(80);

int main(void)
{
	init();

	setup();

	for (;;)
		loop();

	return 0;
}

void setup()
{
#if DEBUG
  //  turn on serial (for debuggin)
  Serial.begin(115200);
#endif

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
}

//  url buffer size
#define BUFSIZE 255

// Toggle case sensitivity
#define CASESENSE true


void loop() {

	  char clientline[BUFSIZE];
	  int index = 0;
	  // listen for incoming clients
	#if defined(ARDUINO) && ARDUINO >= 100
	  EthernetClient client = server.available();
	#else
	  Client client = server.available();
	#endif
	  if (client) {

	    //  reset input buffer
	    index = 0;

	    while (client.connected()) {
	      if (client.available()) {
	        char c = client.read();

	        //  fill url the buffer
	        if(c != '\n' && c != '\r' && index < BUFSIZE){ // Reads until either an eol character is reached or the buffer is full
	          clientline[index++] = c;
	          continue;
	        }

	#if DEBUG
	        Serial.print("client available bytes before flush: "); Serial.println(client.available());
	        Serial.print("request = "); Serial.println(clientline);
	#endif

	        // Flush any remaining bytes from the client buffer
	        client.flush();

	#if DEBUG
	        // Should be 0
	        Serial.print("client available bytes after flush: "); Serial.println(client.available());
	#endif

	        //  convert clientline into a proper
	        //  string for further processing
	        String urlString = String(clientline);

	        //  extract the operation
	        String op = urlString.substring(0,urlString.indexOf(' '));
	        Serial.print("op: ");
	        Serial.println(op);

	        //  we're only interested in the first part...
	        urlString = urlString.substring(urlString.indexOf('/'), urlString.indexOf(' ', urlString.indexOf('/')));
	        Serial.print("urlString: ");
	        Serial.println(urlString);

	        //  put what's left of the URL back in client line
	#if CASESENSE
	        urlString.toUpperCase();
	#endif
	        urlString.toCharArray(clientline, BUFSIZE);

	        //  get the first two parameters
	        char *pin = strtok(clientline,"/");
	        char *value = strtok(NULL,"/");

	        Serial.print("*pin: ");
	        Serial.println(pin);

	        Serial.print("*value: ");
	        Serial.println(value);

	        //  this is where we actually *do something*!
	        char outValue[10] = "MU";
	        String jsonOut = String();

	        if(pin != NULL){
	          if(value != NULL){

	#if DEBUG
	            //  set the pin value
	            Serial.println("setting pin");
	#endif

	            //  select the pin
	            int selectedPin = atoi (pin);
	#if DEBUG
	            Serial.println(selectedPin);
	#endif

	            //  set the pin for output
	            pinMode(selectedPin, OUTPUT);

	            //  determine digital or analog (PWM)
	            if(strncmp(value, "HIGH", 4) == 0 || strncmp(value, "LOW", 3) == 0){

	#if DEBUG
	              //  digital
	              Serial.println("digital");
	#endif

	              if(strncmp(value, "HIGH", 4) == 0){
	#if DEBUG
	                Serial.println("HIGH");
	#endif
	                digitalWrite(selectedPin, HIGH);
	              }

	              if(strncmp(value, "LOW", 3) == 0){
	#if DEBUG
	                Serial.println("LOW");
	#endif
	                digitalWrite(selectedPin, LOW);
	              }

	            }
	            else {

	#if DEBUG
	              //  analog
	              Serial.println("analog");
	#endif
	              //  get numeric value
	              int selectedValue = atoi(value);
	#if DEBUG
	              Serial.println(selectedValue);
	#endif
	              analogWrite(selectedPin, selectedValue);

	            }

	            //  return status
	            client.println("HTTP/1.1 200 OK");
	            client.println("Content-Type: text/html");
	            client.println("X-Powered-By: AquariumPilot v1.0");
	            client.println();

	          }
	          else {
	#if DEBUG
	            //  read the pin value
	            Serial.println("reading pin");
	#endif

	            //  determine analog or digital
	            if(pin[0] == 'a' || pin[0] == 'A'){

	              //  analog
	              int selectedPin = pin[1] - '0';

	#if DEBUG
	              Serial.println(selectedPin);
	              Serial.println("analog");
	#endif

	              sprintf(outValue,"%d",analogRead(selectedPin));

	#if DEBUG
	              Serial.println(outValue);
	#endif

	            }
	            else if(pin[0] != NULL) {

	              //  digital
	              int selectedPin = pin[0] - '0';

	#if DEBUG
	              Serial.println(selectedPin);
	              Serial.println("digital");
	#endif

	              pinMode(selectedPin, INPUT);

	              int inValue = digitalRead(selectedPin);

	              if(inValue == 0){
	                sprintf(outValue,"%s","LOW");
	                //sprintf(outValue,"%d",digitalRead(selectedPin));
	              }

	              if(inValue == 1){
	                sprintf(outValue,"%s","HIGH");
	              }

	#if DEBUG
	              Serial.println(outValue);
	#endif
	            }

	            //  assemble the json output
	            jsonOut += "{\"";
	            jsonOut += pin;
	            jsonOut += "\":\"";
	            jsonOut += outValue;
	            jsonOut += "\"}";

	            //  return value with wildcarded Cross-origin policy
	            client.println("HTTP/1.1 200 OK");
	            client.println("Content-Type: text/html");
	            client.println("Access-Control-Allow-Origin: *");
	            client.println("X-Powered-By: AquariumPilot v1.0");
	            client.println();
	            client.println(jsonOut);
	          }
	        }
	        else {

	          //  error
	#if DEBUG
	          Serial.println("erroring");
	#endif
	          client.println("HTTP/1.1 404 Not Found");
	          client.println("Content-Type: text/html");
	          client.println("X-Powered-By: AquariumPilot v1.0");
	          client.println();

	        }
	        break;
	      }
	    }

	    // give the web browser time to receive the data
	    delay(1);

	    // close the connection:
	    client.stop();
	  }
}
