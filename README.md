# KYTKENTÄOHJEET

## ``` mbed_app.json -tiedostoon omat wifi/MQTT -asetukset ```

``` Connect L432KC - PmodALS with lines: ```

[Kytkentäkaavio](https://digilent.com/reference/pmod/pmodals/start)

L432KC D13 - ALS 4 SCK   
L432KC D12 - ALS 3 MISO  
L432KC A6  - ALS 1 CS  
L432KC GND - ALS 5 GND
L432KC Vcc - ALS 6 Vcc

``` Connect L432KC - ESP8266 with lines: ```

[Kytkentäkaavio](https://www.tme.com/Document/9142f73717083c9f098432c2e25cca2d/MOD-WIFI-ESP8266.pdf)

L432KC GND - ESP8266 1 GND  
L432KC VCC - ESP8266 2 VCC  
L432KC D1  - ESP8266 3 RX   
L432KC D0  - ESP8266 4 TX   

``` PMOD OledRGB with lines: ```

[Kytkentäkaavio](https://digilent.com/reference/pmod/pmodoledrgb/start)

Pinnit edestä päin näyttöylös päin vasemmalta päin ylärivi 1-6 ja alarivi 7-12. 

L432KC D9   - PMOD OLEDRGB 1  CS  
L432KC D11  - PMOD OLEDRGB 2  MOSI  
L432KC      - PMOD OLEDRGB 3  EI KYTKETÄ (NC)  
L432KC D13  - PMOD OLEDRGB 4  SCK  
L432KC GND  - PMOD OLEDRGB 5  GND  
L432KC 3V3  - PMOD OLEDRGB 6  VCC  
L432KC D10  - PMOD OLEDRGB 7  D/C  
L432KC D6   - PMOD OLEDRGB 8  RES  
L432KC 3V3  - PMOD OLEDRGB 9  VCCEN  
L432KC 3V3  - PMOD OLEDRGB 10 PMODEN  
L432KC      - PMOD OLEDRGB 11 EI KYTKETÄ (GND)  
L432KC      - PMOD OLEDRGB 12 EI KYTKETÄ (VCC)   
