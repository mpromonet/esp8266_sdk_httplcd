
/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** mcp23008lcd.ino
** 
** Interface to an LCD 16x2 though a MCP23008 GPIO expander 
**
** -------------------------------------------------------------------------*/

/* Wiring 
#         LCD                |    MCP23008
# 1 : GND
# 2 : 3.3V
# 3 : Contrast (0-5V)*
# 4 : RS (Register Select) <-------- PIN5
# 5 : R/W (Read Write)       - GROUND THIS PIN
# 6 : Enable or Strobe     <-------- PIN6
# 7 : Data Bit 0             - NOT USED
# 8 : Data Bit 1             - NOT USED
# 9 : Data Bit 2             - NOT USED
# 10: Data Bit 3             - NOT USED
# 11: Data Bit 4           <-------- PIN0
# 12: Data Bit 5           <-------- PIN1
# 13: Data Bit 6           <-------- PIN2
# 14: Data Bit 7           <-------- PIN3
# 15: LCD Backlight +5V**
# 16: LCD Backlight GND
*/


#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#include "lcd.h"

MDNSResponder mdns;
bool mdnsinit = false;
ESP8266WebServer server ( 80 );
const char magic[] = {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef};
int offsetSSID = sizeof(magic);
int offsetPassword = 48;
lcd disp;

int initStaFromEEPROM()
{
    int ret = 0;
    char head[sizeof(magic)];
    for (int i=0; i<sizeof(magic) ; ++i)
    {
        head[i] = EEPROM.read(i);
    }
    if (memcmp(head, magic, sizeof(magic)) != 0)
    {
        Serial.println("\nInit EEPROM");
        int i=0;
        for (i = 0; i < sizeof(magic); i++)
            EEPROM.write(i, magic[i]);  
        for (; i < 512; i++)
            EEPROM.write(i, 0); 
        EEPROM.commit(); 
    }
    else
    {
        Serial.println("\nRead EEPROM");
        String ssid;
        for (int i = 0; i<32; ++i)
        {
            ssid += char(EEPROM.read(offsetSSID+i));
        }    
        String password;
        for (int i=0; i<64 ; ++i)
        {
            password += char(EEPROM.read(offsetPassword+i));
        }

        WiFi.begin ( ssid.c_str(), password.c_str() );
        ret = 1; 
    }
    return ret;
}
 
void setup() 
{
    Serial.begin ( 115200 );
    Serial.println("\nSetup");
    
    // init LCD
    disp.begin(0x00, 0, 2);
    disp.print("Starting","");
    
    // init eeprom
    EEPROM.begin(512);
    
    // setup STA
    initStaFromEEPROM();
    
    // setup AP
    WiFi.softAP("esp8266");
    
    // setup HTTP server callbacks
    server.on ( "/", []() 
    { 
        String ssid = server.arg("ssid");
        String password = server.arg("password");
        if ( (ssid.length() > 0) && (password.length() > 0) )
        {
            for (int i=0; i<ssid.length() ; ++i)
            {
              EEPROM.write(i+offsetSSID,ssid[i]);
            }
            for (int i=0; i<password.length() ; ++i)
            {
              EEPROM.write(i+offsetPassword,password[i]);
            }
            EEPROM.commit();
            initStaFromEEPROM();
        }
        
        String msg = server.arg("msg");
        if (msg.length() > 0)
        {
            disp.print(NULL,msg.c_str());
        }

        String answer;
        answer += "<html><body><h2>ESP8266</h2>";
        answer += "<h3>ESP SDK "; answer += ESP.getSdkVersion(); answer += " chipid:"; answer += ESP.getChipId(); answer += " flashid:"; answer += ESP.getFlashChipId(); answer +=" flashsize:"; answer += ESP.getFlashChipSize(); answer += "</h3>";
        answer += "Uptime "; answer += (millis()/1000); answer += " s<br>";
        answer += "Battery "; answer += ESP.getVcc(); answer += " mV<br>";
        answer += "Heap Size:"; answer += ESP.getFreeHeap(); answer += " Bytes<br>";
        answer += "Reset Info:"; answer += ESP.getResetInfo(); answer += "<br>";
        answer += "AP  IP:";  answer += IPAddress2String(WiFi.softAPIP()); answer += " MAC:"; answer += WiFi.softAPmacAddress(); answer += "<br>";
        answer += "STA IP:";  answer += IPAddress2String(WiFi.localIP()); answer += " MAC:"; answer += WiFi.macAddress(); answer += "<br>";
        answer += "<form method=\"post\">Message:<input name=\"msg\"><button>Display</button></form>";
        answer += "<form method=\"post\">SSID:<input name=\"ssid\">Password:<input type=\"password\" name=\"password\"><button>Configure</button></form>";
        answer += "<form action=\"reboot\"><button>Reboot</button></form>";
        answer += "</body></html>";
        server.send ( 200, "text/html", answer.c_str() );
        
    } );
    server.on ( "/reboot", []() 
    { 
        server.send ( 200, "text/html", "<html><head><META HTTP-EQUIV=\"Refresh\" CONTENT=\"1; URL=/\"></head><body><h1>Restarting...</h1></html>" );
        ESP.restart();
    } );  
    server.begin();
    Serial.println ( "HTTP server started" );  
}

String IPAddress2String(const IPAddress & addr)
{
    String ip;
    for (int i=0; i<4; ++i)
    {
        if (i!=0)
            ip += ".";      
        ip += addr[i];      
    }
    return ip;
}
            
int oldStatus = WL_DISCONNECTED;
void loop() 
{
    int status = WiFi.status(); 
    if (status != oldStatus)
    {
        if (status == WL_CONNECTED)
        {
            // start mDNS
            if ( mdns.begin ( "esp8266" ) ) 
            {
                Serial.println ( "MDNS responder started" );
                mdnsinit = true;
            }
            IPAddress addr = WiFi.localIP();
            String ip(IPAddress2String(addr));
            disp.print("STA:ok",ip.c_str());
            Serial.print ( "IP address: " );
            Serial.println ( WiFi.localIP() );    
        }
        else if(status == WL_NO_SSID_AVAIL)
        {
            IPAddress addr = WiFi.softAPIP();
            String ip(IPAddress2String(addr));
            disp.print("STA:no_ssid",ip.c_str());
        }
        else if(status == WL_CONNECT_FAILED)
        {
            IPAddress addr = WiFi.softAPIP();
            String ip(IPAddress2String(addr));
            disp.print("STA:connect",ip.c_str());
        }
        oldStatus = status;
    }

    if (mdnsinit)
    {
        mdns.update();
    }
        
    server.handleClient();
}
