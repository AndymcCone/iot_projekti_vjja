
#include "mbed.h"
#include "ESP8266Interface.h"   // included in the OS6
#include "ntp-client/NTPClient.h"
#include "MQTTClientMbedOs.h"
//#include <cstdio>
//#include <cstdlib>
//#include <ctime>
#include <time.h>
#include <string>
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1331.h"

Adafruit_SSD1331 OLED(D9, D6, D10, D11, NC, D13); // cs, res, dc, mosi, (nc), sck  
ESP8266Interface esp(MBED_CONF_APP_ESP_TX_PIN, MBED_CONF_APP_ESP_RX_PIN);
SPI spi(D11, D12, D13); // mosi, miso, sck
DigitalOut alsCS(A6);        // chip select for sensor SPI communication

#define Black 0x0000
#define White 0xFFFF
#define ntpAddress "time.mikes.fi"  // The VTT Mikes in Helsinki
#define ntpPort 123     // Typically 123 for every NTP server

int getALS()
{
    char alsByte0 = 0; //8bit data from sensor board, char is the unsigned 8bit
    char alsByte1 = 0; // 8bit data from sensor board
    char alsByteSh0 = 0;
    char alsByteSh1 = 0;
    char als8bit = 0;
    unsigned short alsRaw = 0;   // unsigned 16 bit
    float alsScaledF = 0;       // 32 bit floating point
    
    // Begin the conversion process and serial data output
    alsCS.write(0); 
    // Reading two 8bit bytes by writing two dymmy 8bit bytes
    ThisThread::sleep_for(1ms);
    alsByte0 = spi.write(0x00);
    alsByte1 = spi.write(0x00);
    // End of serial data output and back to tracking mode
    alsCS.write(1);
    ThisThread::sleep_for(1ms);
    // Check the http://www.ti.com/lit/ds/symlink/adc081s021.pdf
    // shifting bits to get the number out
    alsByteSh0 = alsByte0 << 4;
    alsByteSh1 = alsByte1 >> 4;
    
    als8bit =( alsByteSh0 | alsByteSh1 );
        
    alsScaledF = (float(als8bit))*(float(6.68)); 
    // The value 6.68 is 64 bit double precision floating point of type double.
    // Conversions to 32 bit floating point of type float.
        
    return (int)alsScaledF; 
}

// WLAN security
const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}
     
   

int AmbientLightSensor(){

    int alsScaledI; 
    int payload;

    spi.format(8,0);           
    spi.frequency(2000000);

    alsScaledI = getALS();
   
         
        //tulostus testi, kommentoidaan pois
        printf("Ambient light scaled to LUX =  '%0d' \r\n",alsScaledI);
            
        if (alsScaledI > 30){ 
            printf("Tarpeeksi valoisaa, ei tarvetta yövalolle.\n\n");
            payload = 0;
            return 0;
            }
        else{
            printf("Hämärää, sytytetään yövalo.\n\n");
            payload = 1;
            return 1;
            }
    }



int main()
{ 

//Store device and broker IP 
    SocketAddress deviceIP;
    SocketAddress MQTTBroker;
    TCPSocket socket;
    MQTTClient client(&socket);

    char buffer[128];

    ThisThread::sleep_for(500ms); // waiting for the ESP8266 to wake up.
    
    printf("\r\nConnecting...\r\n");
    int ret = esp.connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("\r\nConnection error\r\n");
        //return -1;
    }

    printf("Success\n\n");
    esp.get_ip_address(&deviceIP);
    printf("IP: %s\n", deviceIP.get_ip_address());

    NTPClient ntp(&esp);
    ntp.set_server(ntpAddress, ntpPort);

    esp.gethostbyname(MBED_CONF_APP_MQTT_BROKER_HOSTNAME, &MQTTBroker, NSAPI_IPv4, "esp");
    MQTTBroker.set_port(MBED_CONF_APP_MQTT_BROKER_PORT);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
    data.MQTTVersion = 3;
    data.clientID.cstring = MBED_CONF_APP_MQTT_CLIENT_ID;
    data.username.cstring = MBED_CONF_APP_MQTT_AUTH_METHOD;
    data.password.cstring = MBED_CONF_APP_MQTT_AUTH_TOKEN;
 
    
    sprintf(buffer, "%d", AmbientLightSensor());
                    
    MQTT::Message msg;
    msg.qos = MQTT::QOS0;
    msg.retained = false;
    msg.dup = false;
    msg.payload = (void*)buffer;
    msg.payloadlen = strlen(buffer);
    
    ThisThread::sleep_for(2s);  
                                
    // Connecting mqtt broker
    if (client.isConnected() == false){
        printf("Connecting %s ...\n", MBED_CONF_APP_MQTT_BROKER_HOSTNAME);
        socket.open(&esp);
        socket.connect(MQTTBroker);
        client.connect(data);
    }
 
   //Tässtä led näyttö kun ohjelma käynnistyy
    OLED.begin();
    OLED.clearScreen();
    OLED.fillScreen(Black);
    OLED.setTextColor(White);
    OLED.setCursor(30,25);
    OLED.setTextSize(1);
    OLED.printf("Booting up");

    int valosensorilukema = 0;
 
    while (true) {

        time_t timestamp = ntp.get_timestamp();
        timestamp += (60*60*3);  //  GMT +3  for Finland for the summer time.  
        
        if (timestamp < 0) {
            printf("An error occurred when getting the time. Code: %u\r\n", timestamp);
        } else {
            
            printf("Current time is %s\r\n", ctime(&timestamp));
            char *ptr = ctime(&timestamp);
            char *aika = (char*)malloc(sizeof(char)*6);
            memset(aika, '\0', 6*sizeof(char));
            memcpy(aika,ptr+11, 5*sizeof(char));
            
            OLED.clearScreen();  
            OLED.fillScreen(Black); 
            OLED.setTextColor(White); 
            OLED.setTextSize(3); 
            OLED.setCursor(4,20); 
            OLED.printf("%s", aika);
            free(aika);
        }

        int lampuntarve = AmbientLightSensor();

        if (valosensorilukema == lampuntarve){}
        
        else {
            valosensorilukema = lampuntarve;
            sprintf(buffer, "%d", lampuntarve);
            printf("Publishing with payload: %d\n", lampuntarve);
            client.publish(MBED_CONF_APP_MQTT_TOPIC, msg);
            
        }

        ThisThread::sleep_for(10000ms);
        
    }
}