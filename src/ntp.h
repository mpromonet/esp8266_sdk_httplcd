/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ntp.h
** 
** NTP client
**
** -------------------------------------------------------------------------*/

#ifndef _NTP_H_
#define _NTP_H_

#include <Arduino.h>
#include <WiFiUdp.h>

class ntpclient
{
	public:
		ntpclient(const IPAddress & server);
	
		void begin(unsigned int port);
		unsigned long sendNTPpacket();
		long parseNTPpacket();
	
	private:
		static const int NTP_PACKET_SIZE = 48;
		byte      packetBuffer[NTP_PACKET_SIZE];
		WiFiUDP   udp;
		IPAddress timeServer;
};

#endif
