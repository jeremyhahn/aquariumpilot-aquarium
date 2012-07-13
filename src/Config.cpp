#include "Arduino.h"
#include "Config.h"

Config::Config() {

	//_mac = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

	_address[0] = 192;
	_address[1] = 168;
	_address[2] = 11;
	_address[3] = 51;

	_port = 80;
}

Config::~Config() {

}

uint8_t* Config::getMac() {
	return _mac;
}

uint8_t* Config::getAddress() {
	return _address;
}

uint8_t* Config::getNetmask() {
	return _netmask;
}

uint8_t* Config::getGateway() {
	return _gateway;
}

uint8_t Config::getPort() {
	return _port;
}
