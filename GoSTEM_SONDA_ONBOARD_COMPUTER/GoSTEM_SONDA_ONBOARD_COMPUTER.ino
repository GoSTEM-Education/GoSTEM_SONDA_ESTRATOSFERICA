/*
Aquest codi està dissenyat per correr en una T-BEAM V1.1 
Onboard Computer per sonda estratosfèrica amb Barometre & Temp, SD logger, GPS and LoRa Transmitter
Desenvolupat per GoSTEM
*/

//LLIBRERIES
//Aquestes es poden trobar al repositori de GoSTEM o buscant en el repositori de GitHub original (recomanable)
#include "SparkFun_Ublox_Arduino_Library.h"
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <axp20x.h>   
#include <LoRa.h>
#include "board_LoRa.h"
#include <MicroNMEA.h> //https://github.com/stevemarple/MicroNMEA


// Definicions inicials
SFE_UBLOX_GPS myGPS;
Adafruit_BMP280 bmp; // I2C
AXP20X_Class axp; // For the LEDS
#define AXP192_SLAVE_ADDRESS 0x34



//Connexió HSPI pel modul SD, definició dels pins
#define HSPI_SCLK 14
#define HSPI_MISO 2
#define HSPI_MOSI 15
#define HSPI_CS 13

//Connexió pel moduls BME280, definició dels pins 
#define SDA 21
#define SCL 22

TwoWire I2Cone = TwoWire(1);

//El valor de la presió de referènca es important ajustar-lo per tal de tenir uns valor més ajustats.
#define SEALEVELPRESSURE_HPA (1019.25)

unsigned long delayTime;
int current_size = 0;
SPIClass * hspi = NULL;
String dataString =""; // Emagatzema les dades que es guarden a la SD
File GoSTEM_LOG;

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));

//Definició de les variables que fa servir el GPS
long latitude_mdeg;
long longitude_mdeg;
long speed;
long course;

void setup()
{
    initBoard();
    // Deixem un delay inicial quant la placa es iniciada
    delay(1500);
    Serial.begin(115200);
    Wire.begin(SDA, SCL);

    Serial.println("GOSTEM ONBOARD COMPUTER");
    if (myGPS.begin(Serial1) == false) {
        Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
        // Si surt aquest missatge pot ser que faci falta reiniciar l'alimentació de la placa
        while (1);
    }

     hspi = new SPIClass(HSPI);
    hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_CS); //SCLK, MISO, MOSI, SS
    pinMode(HSPI_CS, OUTPUT); //HSPI SS
  
    unsigned status;
    status = bmp.begin(0x76);
      /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

      if (!status) {
      Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                        "try a different address!"));
      Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
      Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
      Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
      Serial.print("        ID of 0x60 represents a BME 280.\n");
      Serial.print("        ID of 0x61 represents a BME 680.\n");
      }
    // Mirem si la SD està present i pot ser inicialitzada
  if (!SD.begin(HSPI_CS,*hspi)) {
  Serial.println("Card failed, or not present");
  }

    if (SD.exists("/data.csv")) { // El nom per defecte del nostre fixer de dades es data.csv
      Serial.println("data.csv exists.");
      GoSTEM_LOG = SD.open("/data.csv", FILE_WRITE);
      GoSTEM_LOG.seek(GoSTEM_LOG.size());
      current_size = GoSTEM_LOG.size();
      GoSTEM_LOG.close(); // Es important obrir i despres tancar el modul SD
    }
    else {
      Serial.println("data.csv doesn't exist.");
        // open a new file and immediately close it:
          Serial.println("Creating data.csv...");
          GoSTEM_LOG = SD.open("/data.csv", FILE_WRITE);
          GoSTEM_LOG.println("--- LOG Begin ---");
          current_size = GoSTEM_LOG.size();
          GoSTEM_LOG.close();
    }

    // Mirem si el fitxer s'ha creat adequadament:
    if (SD.exists("/data.csv")) {
      Serial.println("data.csv exists.");
    }
    else {
      Serial.println("data.csv still doesn't exist.");
      // Si aquest misatge surt de forma reiterada s'ha de mirar que les connexions de ls SD estiguin ben fetes i mirar de fer un restet a la placa (amb el boto)
    }


    axp.begin(Wire, AXP192_SLAVE_ADDRESS);
    axp.setDCDC1Voltage(3300); // Get power to the charging LED
    axp.setChgLEDMode(AXP20X_LED_OFF); // Per defecte volem el LED apagat
    delayTime = 5000;

    // LoRa Configuration, definició dels pins que fa servir el chip de LoRa
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DI0_PIN);

        if (!LoRa.begin(LoRa_frequency)) {
          Serial.println("Starting LoRa failed!");
          while (1);
        }
      LoRa.setSpreadingFactor(8);           // ranges from 6-12,default 7 see API docs
      // Busqeu que es el Spreding Factor en les comunicacions LoRa, a major nombre més abast, però ull s'ha de complir la normativa europea!


//Aquest loop només s'executarà un sol cop
}

void loop()
{//Loop pincipal on s'executen de forma repetida totes les funcions principals

    printValues();
    StoreValues();
    SendLoRaData();

    delay(delayTime); //Aquest detlay ens permetrà tenir major o menor volum de dades en funció de la frequència que es seleccioni
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

void printValues() {
  //Aquesta funció ens imprimeix per Serial el que està calculat la placa, ens serervirà per quant la tinguem connectada al PC
  
  myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

    if (nmea.isValid() == true) {
        long latitude_mdeg = nmea.getLatitude();
        long longitude_mdeg = nmea.getLongitude();
        long speed = nmea.getSpeed();
        long course =  nmea.getCourse();

        Serial.print("Latitude (deg): ");
        Serial.println(latitude_mdeg / 1000000., 6);
        Serial.print("Longitude (deg): ");
        Serial.println(longitude_mdeg / 1000000., 6);
        Serial.print("Speed: ");
        Serial.println(speed / 1000., 1);
        Serial.print("Course: ");
        Serial.println(course / 1000., 2);
        delay(1000);
    } else {
        Serial.print("No Fix - ");
        Serial.print("Num. satellites: (Minimum 5)");
        Serial.println(nmea.getNumSatellites());
    }
    
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" °C");

    Serial.print("Pressure = ");

    Serial.print(bmp.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.println();




}


void StoreValues(){

     // Aqeusta funció ens guarda les dades a la targeta SD

    if (SD.exists("/data.csv")) {
      
        myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.

    if (nmea.isValid() == true) {
         latitude_mdeg = nmea.getLatitude();
         longitude_mdeg = nmea.getLongitude();
         speed = nmea.getSpeed();
         course =  nmea.getCourse();

        (latitude_mdeg / 1000000., 6);
        (longitude_mdeg / 1000000., 6);
        (speed / 1000., 1);
        (course / 1000., 2);
    } else {
         latitude_mdeg = 0.000000;
         longitude_mdeg = 0.000000;
         speed = 0.0;
         speed = 0.00;
    }
      // Temperatura,Pressio,Altitude,Latitud,Temps
      GoSTEM_LOG = SD.open("/data.csv", FILE_WRITE);
      GoSTEM_LOG.seek(current_size); //Desplaçem el cursos a la última posició del fitxer
      Serial.println("Data Logged in SD Card");
      GoSTEM_LOG.println("");  // BEGIN OF CSV LINE, JUMP TO NEW LINE
      GoSTEM_LOG.print(bmp.readTemperature());
      GoSTEM_LOG.print(",");
      GoSTEM_LOG.print(bmp.readPressure() / 100.0F);
      GoSTEM_LOG.print(",");
      GoSTEM_LOG.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
      GoSTEM_LOG.print(",");
      GoSTEM_LOG.print(latitude_mdeg / 1000000., 6);
      GoSTEM_LOG.print(",");
      GoSTEM_LOG.print(longitude_mdeg / 1000000., 6);
      GoSTEM_LOG.print(",");
      GoSTEM_LOG.print(speed / 1000., 1);
      GoSTEM_LOG.print(",");
      GoSTEM_LOG.print(course / 1000., 1);
      GoSTEM_LOG.print(",");
      GoSTEM_LOG.print(int(nmea.getHour()));
      GoSTEM_LOG.print(":");
      GoSTEM_LOG.print(int(nmea.getMinute()));
      GoSTEM_LOG.print(":");
      GoSTEM_LOG.print(int(nmea.getSecond()));
      GoSTEM_LOG.print(",");
      current_size = current_size + 68; // Cal modificar-ho a la llargada del string que es guardi cada cop      
      GoSTEM_LOG.close();
      axp.setChgLEDMode(AXP20X_LED_BLINK_4HZ); // Activem els leed per msotrar que s'ha guardat correctament a la SD
      delay(500);
      axp.setChgLEDMode(AXP20X_LED_OFF);

    } else {Serial.println("error writing in data.csv"); axp.setChgLEDMode(AXP20X_LED_OFF); } // Si no tenim pampalluges algo no va bé.

}

void SendLoRaData() {
  // Aquesta funció el que ens fa es enviar a través del modum LoRa les nostres dades que també s'han guardat a la SD
  if (nmea.isValid() == true) {
      latitude_mdeg = nmea.getLatitude();
      longitude_mdeg = nmea.getLongitude();
      speed = nmea.getSpeed();
      course =  nmea.getCourse();

      (latitude_mdeg / 1000000., 6);
      (longitude_mdeg / 1000000., 6);
      (speed / 1000., 1);
      (course / 1000., 2);
  } else {
      latitude_mdeg = 0.000000;
      longitude_mdeg = 0.000000;
      speed = 0.0;
      speed = 0.00;
  }
       // Define LoRa packet - Start
       // Aquesta es la forma com enviem les dades, les separem per comes per així poder despres tenir el CSV ordenat.
      LoRa.beginPacket();
      LoRa.print(bmp.readTemperature());
      LoRa.print(",");
      LoRa.print(bmp.readPressure() / 100.0F);
      LoRa.print(",");
      LoRa.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
      LoRa.print(",");
      LoRa.print(latitude_mdeg / 1000000., 6);
      LoRa.print(",");
      LoRa.print(longitude_mdeg / 1000000., 6);
      LoRa.print(",");
      LoRa.print(speed / 1000., 1);
      LoRa.print(",");
      LoRa.print(course / 1000., 1);
      LoRa.print(",");
      LoRa.print(int(nmea.getHour()));
      LoRa.print(":");
      LoRa.print(int(nmea.getMinute()));
      LoRa.print(":");
      LoRa.print(int(nmea.getSecond()));
      LoRa.endPacket();
        // Define LoRa packet - End
      Serial.println("LoRa Paquet Send Succesfully");

}


