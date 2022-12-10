/*
  Read NMEA sentences over sERIAL using Ublox module SAM-M8Q, NEO-M8P, etc
  Base on SparkFun_Ublox_Arduino_Library //https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
*/

#include "SparkFun_Ublox_Arduino_Library.h"
#include <LoRa.h>
#include "board_LoRa.h"

SFE_UBLOX_GPS myGPS;

#include <MicroNMEA.h> //https://github.com/stevemarple/MicroNMEA

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

int LoRa_Delay = 2*60*1000; // Time dalay in ms betwen each message minuts*segons*milsegons

void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    Serial.println("GOSTEM GPS + LoRa Example");

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

}
/*
 DATA STRUCUTURE FOR THE CSV
 Serial.println("DATA,SAT,LONGITUDE,LATITUDE,ALTITUDE,SPEED,COURSE,RSSI,SNR,TIMERX");
*/

void loop()
{
    myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

    if (nmea.isValid() == true) {
        long latitude_mdeg = nmea.getLatitude();
        long longitude_mdeg = nmea.getLongitude();
        long altitude = nmea.getAltitude(altitude);
        long speed = nmea.getSpeed();
        long course =  nmea.getCourse();



         // Define LoRa packet - Start
         //("SAT,LATITUDE,LONGITUDE,ALTITUDE,SPEED,COURSE,RSSI,SNR,TIMERX");
        LoRa.beginPacket();
        LoRa.print(nmea.getNumSatellites());
        LoRa.print(",");
        LoRa.print(latitude_mdeg / 1000000., 4);
        LoRa.print(",");
        LoRa.print(longitude_mdeg / 1000000., 4);
        LoRa.print(","); //Altitude en metres
        LoRa.print(altitude / 1000., 1);
        LoRa.print(",");
        LoRa.print(speed / 1000., 1);
        LoRa.print(",");
        LoRa.print(course / 1000., 1);
        LoRa.endPacket();
        // Define LoRa packet - End

        Serial.print("Lat: ");
        Serial.println(latitude_mdeg / 1000000., 4);
        Serial.print("Lon: ");
        Serial.println(longitude_mdeg / 1000000., 4);

        Serial.print("Alt: "); //Altitude en metres
        Serial.println(altitude / 1000., 1);

        Serial.print("Speed: ");
        Serial.println(speed / 1000., 1);
        Serial.print("Course: ");
        Serial.println(course / 1000., 2);

        delay(LoRa_Delay);

    } else {
       //("SAT,LATITUDE,LONGITUDE,ALTITUDE,SPEED,COURSE,RSSI,SNR,TIMERX");
        LoRa.beginPacket();
        LoRa.print(nmea.getNumSatellites());
        LoRa.endPacket();

        Serial.print("No Fix - Sat ");
        Serial.println(nmea.getNumSatellites());
        
        delay(LoRa_Delay);
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
