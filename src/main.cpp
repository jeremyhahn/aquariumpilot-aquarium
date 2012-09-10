#include "Arduino.h"
#include "SPI.h"
#include "Ethernet.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "EEPROM.h"
#include "IpAddress.h"
#include "Netmask.h"
#include "Gateway.h"
#include "Wire.h"
#include "Chronodot.h"

extern int  __bss_end;
extern int  *__brkval;
extern void sendNotification(String subject, String body);

#define DEBUG true

// Data wire is plugged into pin 5 on the Arduino
#define ONE_WIRE_BUS 5

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

DeviceAddress aquariumTemp = { 0x28, 0x59, 0x57, 0x71, 0x03, 0x00, 0x00, 0xA1 };
DeviceAddress surfaceTemp = { 0x28, 0x86, 0x54, 0xEB, 0x03, 0x00, 0x00, 0xF5 };
DeviceAddress roomTemp = { 0x28, 0x43, 0x2D, 0x28, 0x04, 0x00, 0x00, 0xD4 };

int sdcardPin = 4; // Arduino SD card pin
int ethernetPin = 53; // Arduino ethernet pin

// REST server - url buffer size
#define BUFSIZE 255

// REST server - API key required to communicate with the web service
String apiKey = "ABC123";

byte mac[] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAD }; // Local ethernet MAC address
byte mailServer[] = { 172, 16, 201, 2 };
IpAddress ipAddress;
Netmask netmask;
Gateway gateway;
EthernetServer httpServer(80);
EthernetClient httpClient;
EthernetClient smtpClient;

Chronodot RTC;
/*
int seconds; //00-59;
int minutes; //00-59;
int hours;//1-12 - 00-23;
int day;//1-7
int date;//01-31
int month;//01-12
int year;//0-99;
*/
DateTime now;
int yesterday;
String dateString;
String timeString;
String dateTimeString;

bool maintenanceInProgress;

String pH;
int phLastDayAlarm = -1;
char serialChar;

/**
 * C main program execution. Only needed when developing
 * outside the Arduino programming environment / no bootloader.
 *
 * Calls setup and then calls loop() indefinitely
 */
int main(void) {

	init();

	setup();

	for (;;)
		loop();

	return 0;
}

/**
 * The arduino setup
 */
void setup() {

	Serial.begin(115200);

	//int sdcardPin = 4;
	//int ethernetPin = 53;

	//for(int i = 0; i < 512; i++)
		//EEPROM.write(i, 0);

	//ipAddress.set(192, 168, 11, 51);
	//netmask.set(255, 255, 255, 0);
	//gateway.set(192, 168, 11, 1);
	//config.erase();

	Serial.print("Address: ");
	Serial.println(ipAddress.toString());

	Serial.print("Netmask: ");
	Serial.println(netmask.toString());

	Serial.print("Gateway: ");
	Serial.println(gateway.toString());

	// mac, ip, dns, gateway, subnet
	Ethernet.begin(mac, ipAddress.getBytes(), gateway.getBytes(), gateway.getBytes(), netmask.getBytes());
	httpServer.begin();

	Wire.begin();
	RTC.begin();
	//setSyncProvider(RTC.get); // set sync to use the ChronoDot
	//setSyncInterval(10); // sync every 10 seconds if possible

	// Set the real-time clock
	// uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec, int tempF, float tempC
	//RTC.adjust(DateTime((uint16_t) 2012, (uint8_t) 8, (uint8_t) 30, (uint8_t) 15, (uint8_t) 38, (uint8_t) 0, 0, (float) 0));

	// Temperature
	sensors.begin();
	sensors.setResolution(aquariumTemp, 12);
	sensors.setResolution(surfaceTemp, 12);
	sensors.setResolution(roomTemp, 12);

	// PH
	Serial3.begin(38400);
	Serial3.print("C\r");
	pH.reserve(5);
}

void debug(String message) {

	Serial.print("[");
	Serial.print(dateTimeString);
	Serial.print("] ");
	Serial.println(message);
}

void checkPh() {

	#ifdef DEBUG
	debug("checkPh()");
	#endif

	if(maintenanceInProgress) return;

	// Inform the pH stamp of the water temperature (for more accurate readings)
	Serial3.print(sensors.getTempF(aquariumTemp));
	Serial3.print("\r");

	// Take one pH reading
	//Serial3.print("R\r");

	pH = "";

	// Parse the response
    while(Serial3.available()) {

       serialChar = (char)Serial3.read();
	   if(String(serialChar) == "\r") break;

	   pH += serialChar;

	   #ifdef DEBUG
	   Serial.print("Reading: ");
	   Serial.println(serialChar);
	   #endif
    }

    // Put the pH stamp back into continuous mode
    Serial3.print("C\r");

    // Apply Offset
    /*
    char cPh[5];
    pH.toCharArray(cPh, 5);

    float fPh = atof(cPh) - .4;
    dtostrf(fPh, 2, 2, cPh);
    pH = String(cPh);
    (/

	// Send notification (one per day) if pH is < 8.0
    /*
    if(fPh < 8 && phLastDayAlarm != now.day()) {

       sendNotification("Low pH", "The pH is too low. Reading: " + pH);
       phLastDayAlarm = now.day();
    }*/
}

void sendHtmlHeader() {

	httpClient.println("<h5>AquariumPilot v1.0</h5>");
}

void send404() {

	#ifdef DEBUG
	debug("send404()");
	#endif

	httpClient.println("HTTP/1.1 404 Not Found");
	httpClient.println("Content-Type: text/html");
	httpClient.println("X-Powered-By: AquariumPilot v1.0");
	httpClient.println();

	sendHtmlHeader();
	httpClient.println("<h1>Not Found</h1>");
}

void reply(int statusCode, String statusMessage, String body) {

	#ifdef DEBUG
	debug("reply()");
	#endif

	httpClient.print("HTTP/1.1 ");
	httpClient.print(statusCode);
	httpClient.print(" ");
	httpClient.print(statusMessage);
	httpClient.println("Content-Type: text/html");
	httpClient.println("X-Powered-By: AquariumPilot v1.0");
	httpClient.println();

	sendHtmlHeader();
	httpClient.println(body);
}

void sendNotification(String subject, String body) {

	#ifdef DEBUG
	debug("sendNotification()");
	#endif

	int wait = 1500;

	if (smtpClient.connect(mailServer, 25)) {

		smtpClient.println("EHLO aquariumpilot.bostonsaltwater.com");
		delay(wait);

		smtpClient.println("MAIL FROM:<jeremy.hahn@makeabyte.com>");
		delay(wait);

		smtpClient.println("RCPT TO:<jeremy.hahn@makeabyte.com>");
		delay(wait);

		smtpClient.println("DATA");
		delay(wait);

		#ifdef DEBUG
		smtpClient.print("Subject:[AquariumPilot] ");
		#else
		smtpClient.print("Subject:[AquariumPilot TESTING!!!] ");
		#endif
		smtpClient.println(subject);
		smtpClient.println("");

		smtpClient.println(body);
		smtpClient.println("\r\n\r\n- AquariumPilot");
		smtpClient.println(".");

		smtpClient.println("QUIT");
		delay(wait);

		smtpClient.println();
		smtpClient.stop();
	}
}

int availableMemory() {

	int free_memory;

	if ((int) __brkval == 0)
		free_memory = ((int) &free_memory) - ((int) &__bss_end);
	else
		free_memory = ((int) &free_memory) - ((int) __brkval);

	return free_memory;
}
/*
void factoryReset() {

	ipAddress.erase();
	config.erase();

	// Erase EERPOM
	//for(int i = 0; i < 512; i++)
	//    EEPROM.write(i, 0);
}*/

void startNewDay() {

	phLastDayAlarm = -1;
}

void loop() {

	// Date/Time handling
	now = RTC.now();
	dateString = String(now.month()) + "/" + String(now.day()) + "/" + String(now.year());
	timeString = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
	dateTimeString = dateString + " " + timeString;

	if(now.hour() == 0 && now.minute() == 0) {
		startNewDay();
	}

	// Start listening for web requests
	httpClient = httpServer.available();

	char clientline[BUFSIZE];
	int index = 0;

	if (httpClient) {

		//reset input buffer
		index = 0;

		while (httpClient.connected()) {

			if (httpClient.available()) {

				char c = httpClient.read();

				if (c != '\n' && c != '\r' && index < BUFSIZE) {
					clientline[index++] = c;
					continue;
				}

				httpClient.flush();

				String urlString = String(clientline);
				String op = urlString.substring(0, urlString.indexOf(' '));
				urlString = urlString.substring(urlString.indexOf('/'), urlString.indexOf(' ', urlString.indexOf('/')));
				urlString.toCharArray(clientline, BUFSIZE);

				char *key = strtok(clientline, "/");
				char *resource = strtok(NULL, "/");
				char *param1 = strtok(NULL, "/");
				char *param2 = strtok(NULL, "/");

				if (!apiKey.equals(key)) {
					reply(403, "Forbidden", "<h1>Forbidden</h1>");
					break;
				}

				char outValue[20] = "";
				String jsonOut = String();
				int address = atoi(param1);

				if (param1 != NULL) {  // Pin / id

					//
					// REST resources that use *param1 ONLY
					//

					// /ph/?
					if(strncmp(resource, "ph", 2) == 0) {

						// Set the PH stamp temp variable to the current room
						// temperature if this is a calibration request.
						if(strncmp(param1, "F", 1) == 0) {

							Serial3.print(sensors.getTempF(roomTemp));
							Serial3.print("\r");
						}

						Serial3.print(param1);
						Serial3.print("\r");
					}
					// /maintenance/?
					else if (strncmp(resource, "maintenance", 11) == 0) {
						maintenanceInProgress = atoi(param1);
					}

					//
					// REST resources that use both "pin" and "value" parameters
					//

					if (param2 != NULL) { // This is a write operation

						pinMode(address, OUTPUT);

						if (strncmp(resource, "digital", 7) == 0) {

							if (strncmp(param2, "HIGH", 4) == 0) {
								digitalWrite(address, HIGH);
							}

							if (strncmp(param2, "LOW", 3) == 0) {
								digitalWrite(address, LOW);
							}
						}
						else if (strncmp(resource, "analog", 6) == 0) {
							analogWrite(address, atoi(param2));
						}
						else if (strncmp(resource, "eeprom", 6) == 0) {
							EEPROM.write(address, atoi(param2));
						}

						httpClient.println("HTTP/1.1 200 OK");
						httpClient.println("Content-Type: text/html");
						httpClient.println("X-Powered-By: AquariumPilot v1.0");
						httpClient.println();
						break;
					}
					else { // No value specified -- read operation

						if(strncmp(resource, "ph", 2) == 0) {
							checkPh();
						    pH.toCharArray(outValue, 20);
						}
						else if (strncmp(resource, "eeprom", 6) == 0) {
							sprintf(outValue, "%d", EEPROM.read(address));
						}
						else if (strncmp(resource, "temp", 4) == 0) {

							sensors.requestTemperatures();

							if(address == 1) {
							   float fTemp = sensors.getTempF(aquariumTemp);
							   dtostrf(fTemp, 2, 2, outValue);
							}
							else if(address == 2) {
							   float fTemp = sensors.getTempF(surfaceTemp);
							   dtostrf(fTemp, 2, 2, outValue);
							}
							else { //if(address == 3) {
							   float fTemp = sensors.getTempF(roomTemp);
							   dtostrf(fTemp, 2, 2, outValue);
							}
						}
						else if (strncmp(resource, "analog", 6) == 0) {
							sprintf(outValue, "%d", analogRead(address));
						}
						else if (strncmp(resource, "digital", 7) == 0) {

							int selectedPin = atoi(param1);
							int inValue = digitalRead(selectedPin);

							if (inValue <= 0) {
								sprintf(outValue, "%s", "LOW");
								//sprintf(outValue,"%d",digitalRead(selectedPin));
							}

							if (inValue >= 1) {
								sprintf(outValue, "%s", "HIGH");
							}
						}

						// assemble the json response
						jsonOut += "{\"";
						jsonOut += param1;
						jsonOut += "\":\"";
						jsonOut += outValue;
						jsonOut += "\"}";

						//  return value with wildcarded Cross-origin policy
						httpClient.println("HTTP/1.1 200 OK");
						httpClient.println("Content-Type: text/html");
						httpClient.println("Access-Control-Allow-Origin: *");
						httpClient.println("X-Powered-By: AquariumPilot v1.0");
						httpClient.println();
						httpClient.println(jsonOut);
						break;
					}
				}
				//
				// REST resources that use neither "pin" or "value" parameters...
				//
				// These resources are not related to the arduino, but rather the
				// purpose / implementation itself.
				//
				// /ph
				if (strncmp(resource, "ph", 2) == 0) {

					checkPh();

					jsonOut += "{";
						jsonOut += "\"pH\":\"" + pH + "\" ";
					jsonOut += "}";
				}
				else if (strncmp(resource, "maintenance", 11) == 0) {

					jsonOut += "{";
						jsonOut += "\"maintenance\":\"" + String(maintenanceInProgress) + "\", ";
					jsonOut += "}";
				}
				// /system
				else if (strncmp(resource, "system", 6) == 0) {

					jsonOut += "{";
						jsonOut += "\"ipAddress\":\"" + ipAddress.toString() + "\", ";
						jsonOut += "\"netmask\":\"" + netmask.toString() + "\", ";
						jsonOut += "\"gateway\":\"" + gateway.toString() + "\", ";
						jsonOut += "\"availableMemory\":\"" + String(availableMemory()) + "\", ";
						jsonOut += "\"uptime\":\"" + String(millis()) + "\" ";
						/*
						jsonOut += "\"hours\":\"" + String(hours) + "\", ";
						jsonOut += "\"minutes\":\"" + String(minutes) + "\", ";
						jsonOut += "\"seconds\":\"" + String(seconds) + "\", ";
						jsonOut += "\"dayOfWeek\":\"" + String(date) + "\", ";
						jsonOut += "\"day\":\"" + String(day) + "\", ";
						jsonOut += "\"month\":\"" + String(month) + "\", ";
						jsonOut += "\"year\":\"" + String(year) + "\", ";
						*/
						//jsonOut += "\"tempF\":\"" + String(tempF) + "\" ";
						//jsonOut += "\"lastError\":\"" + lastError + "\" ";
					jsonOut += "}";
				}
				else if (strncmp(resource, "status", 6) == 0) {

					sensors.requestTemperatures();
					checkPh();

					char aquariumTempF[6];
					float fTemp = sensors.getTempF(aquariumTemp);
					dtostrf(fTemp, 2, 2, aquariumTempF);

					char surfaceTempF[8];
					float fTemp2 = sensors.getTempF(surfaceTemp);
					dtostrf(fTemp2, 2, 2, surfaceTempF);

					char roomTempF[6];
					float fTemp3 = sensors.getTempF(roomTemp);
					dtostrf(fTemp3, 2, 2, roomTempF);

					jsonOut += "{";
						jsonOut += "\"aquariumTemp\":\"" + String(aquariumTempF) + "\", ";
						jsonOut += "\"surfaceTemp\":\"" + String(surfaceTempF) + "\", ";
						jsonOut += "\"roomTemp\":\"" + String(roomTempF) + "\", ";
						jsonOut += "\"pH\":\"" + pH + "\", ";
						jsonOut += "\"phLastDayAlarm\":\"" + String(phLastDayAlarm) + "\", ";
						jsonOut += "\"maintenanceInProgress\":\"" + String(maintenanceInProgress) + "\" ";
					jsonOut += "}";
				}
				else if (strncmp(resource, "pinout", 6) == 0) {

					jsonOut += "{";
						jsonOut += "\"SDA\":\"" + String(SDA) + "\", ";
						jsonOut += "\"SCL\":\"" + String(SCL) + "\" ";
					jsonOut += "}";
				}
				else if (strncmp(resource, "config", 6) == 0) {

					jsonOut += "{";
						//jsonOut += "\"autoWaterChangesEnabled\":\"" + String(config.isAutoWaterChangesEnabled()) + "\", ";
					jsonOut += "}";
				}
				else if (strncmp(resource, "reset", 12) == 0) {
					//factoryReset();
				}
				else {
					send404();
				}

				httpClient.println("HTTP/1.1 200 OK");
				httpClient.println("Content-Type: text/html");
				httpClient.println("Access-Control-Allow-Origin: *");
				httpClient.println("X-Powered-By: AquariumPilot v1.0");
				httpClient.println();
				httpClient.println(jsonOut);

				break;
			}
		}

		// give the web browser time to receive the data
		delay(100);

		// close the connection:
		httpClient.stop();
	}
}
