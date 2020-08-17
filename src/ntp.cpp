/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ntp.cpp
** 
** NTP client
**
** -------------------------------------------------------------------------*/

#include "ntp.h"

ntpclient::ntpclient(const IPAddress & server) : timeServer(server)
{
}

void ntpclient::begin(unsigned int port)
{
    udp.begin(port);
}


unsigned long ntpclient::sendNTPpacket()
{
    Serial.println("sending NTP packet...");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    
    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(timeServer, 123); //NTP requests are to port 123
    size_t size = udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
    return size;
}

long ntpclient::parseNTPpacket()
{
    long epoch = -1;
    int cb = udp.parsePacket();
    if (cb)
    {
        Serial.println ( "Receiving NTP answer" );
      
        udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
        
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord  = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        
        // now convert NTP time into everyday time:
        const unsigned long seventyYears = 2208988800UL;
        epoch = secsSince1900 - seventyYears;
        
        // print Unix time:
        Serial.print("Unix time = ");
        Serial.println(epoch);
    }
    return epoch;
}
