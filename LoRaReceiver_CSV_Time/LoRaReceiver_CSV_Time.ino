
#include <LoRa.h>
#include "boards.h"
#include "SparkFun_Ublox_Arduino_Library.h"
#include <MicroNMEA.h> //https://github.com/stevemarple/MicroNMEA

/*
This code is intended to run with putty or other SERIAL readed saving the information to a csv.
The time for the CSV is obtained form the GPS signal-> Need to have GPS look
*/
SFE_UBLOX_GPS myGPS;

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    
    if (myGPS.begin(Serial1) == false) {
        Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
        while (1);
    }
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DI0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    LoRa.setSpreadingFactor(12);           // ranges from 6-12,default 7 see API docs

    Serial.println("SAT,LATITUDE,LONGITUDE,ALTITUDE,SPEED,COURSE,RSSI,SNR,DATERX,TIMERX");

}

void loop()
{
    myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

    
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        // received a packet
        //Serial.println("Received packet ");

        String recv = "";
        // read packet
        while (LoRa.available()) {
            recv += (char)LoRa.read();
        }

        Serial.print(recv);
        // print RSSI of packet
        Serial.print(",");
        Serial.print(LoRa.packetRssi());
        Serial.print(",");
        Serial.print(String(LoRa.packetSnr()));
            
      if (nmea.isValid() == true) {
          Serial.print(",");
          Serial.print(nmea.getYear());
          Serial.print("-");
          Serial.print(int(nmea.getMonth()));
          Serial.print("-");
          Serial.print(int(nmea.getDay()));
          Serial.print(",");
          Serial.print(int(nmea.getHour()));
          Serial.print(":");
          Serial.print(int(nmea.getMinute()));
          Serial.print(":");
          Serial.print(int(nmea.getSecond()));
          Serial.println();  // END OF CSV LINE
      }else{
        Serial.print(",0-0-0,00:00:00");
        Serial.println();  // END OF CSV LINE
      }
    }

    delay(250); //Don't pound too hard on the I2C bus
}


//This function gets called from the SparkFun Ublox Arduino Library
//As each NMEA character comes in you can specify what to do with it
//Useful for passing to other libraries like tinyGPS, MicroNMEA, or even
//a buffer, radio, etc.
void SFE_UBLOX_GPS::processNMEA(char incoming)
{
    //Take the incoming char from the Ublox I2C port and pass it on to the MicroNMEA lib
    //for sentence cracking
    nmea.process(incoming);
}
