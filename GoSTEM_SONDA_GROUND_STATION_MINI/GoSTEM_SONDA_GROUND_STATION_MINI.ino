
#include <LoRa.h>
#include "boards.h"

long Waiting_timer; // Ens indica que està esperant per nous paquets, podem treurel ja que es un simple contador
long Packet_Count = 0; // Conta el total de paquets que em rebut
void setup()
{
    initBoard();
    // When the power is turned on, a delay is required.
    delay(1500);

    Serial.println("LoRa Receiver");

    // Definició dels pins pel modul LoRa
    LoRa.setPins(RADIO_CS_PIN, RADIO_RST_PIN, RADIO_DIO0_PIN);
    if (!LoRa.begin(LoRa_frequency)) {
        Serial.println("Starting LoRa failed!");
        while (1);
    }
    LoRa.setSpreadingFactor(8);      // ranges from 6-12, default 7 see API doc
    // Aquest valor ha de ser el mateix en el modul RX que en el TX

    Serial.println("TEMP,PRES,ALTIUDE,LATITUDE,LONGITUDE,SPEED,COURSE,TIMETX,RSSI,SNR");
   // Aquest codi esta pensat per anar acompanyat d'un programa com PuTTy, aquest programes llegeixen el ports serial que la PCB esta imprimit i ho guarda en una SD
   // D'aquesta manera tot el que llegeixi la modul Ground Station serà guardat automaticament en un fitxer csv en el ordinador en questió.  
}

void loop()
{
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
        Packet_Count=Packet_Count+1;

#ifdef HAS_DISPLAY
        if (u8g2) {
            u8g2->clearBuffer();
            char buf[256];
            snprintf(buf, sizeof(buf),"Received OK! %i", Packet_Count);
            u8g2->drawStr(0, 12, buf);
            u8g2->drawStr(0, 26, recv.c_str());
            snprintf(buf, sizeof(buf), "RSSI:%i", LoRa.packetRssi());
            u8g2->drawStr(0, 40, buf);
            snprintf(buf, sizeof(buf), "SNR:%.1f", LoRa.packetSnr());
            u8g2->drawStr(0, 56, buf);
            u8g2->sendBuffer();
        }
        sleep(5);
        Waiting_timer=0;
#endif
    } else {
#ifdef HAS_DISPLAY
        if (u8g2) {
            u8g2->clearBuffer();
            char buf[256];
            u8g2->drawStr(0, 12, "Waiting Packets");
            snprintf(buf, sizeof(buf), "Waiting for: %i", Waiting_timer);
            u8g2->drawStr(0, 50, buf);
            u8g2->sendBuffer();
        }
        Waiting_timer=Waiting_timer+1;
#endif


    }
}
