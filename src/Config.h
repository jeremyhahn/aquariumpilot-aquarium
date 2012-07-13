#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"
#include "Ethernet.h"

class Config {

	private:
		uint8_t _mac[6];         //= { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
		uint8_t _address[4]; //= { 192, 168, 11, 51 };
		uint8_t _netmask[4];    //= { 255, 255, 255, 0 };
		uint8_t _gateway[4]; //= { 192, 168, 11, 1 };
		uint8_t _port;      //= 80;

	public:
		Config();
		~Config();

		uint8_t* getMac();
		void setMac(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);

		uint8_t* getAddress();
		void setAddress(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);

		uint8_t* getNetmask();
		uint8_t* getGateway();
		uint8_t getPort();
};

#endif
