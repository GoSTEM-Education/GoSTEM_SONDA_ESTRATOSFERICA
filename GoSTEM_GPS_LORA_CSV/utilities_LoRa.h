
#pragma once

/*
* arduinoLoRa Library just only support SX1276/Sx1278,Not support SX1262
* RadioLib Library supports also SX1262/SX1268 see https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/tree/master/examples/RadioLibExamples
* */

#define LILYGO_TBeam_V1_1


/*
* if you need to change it,
* please open this note and change to the frequency you need to test
* Option: 433E6,470E6,868E6,915E6
* */

#define LoRa_frequency      868E6


#define UNUSE_PIN                   (0)


#define GPS_RX_PIN                  34
#define GPS_TX_PIN                  12
#define BUTTON_PIN                  38
#define BUTTON_PIN_MASK             GPIO_SEL_38
#define I2C_SDA                     21
#define I2C_SCL                     22
#define PMU_IRQ                     35

#define RADIO_SCLK_PIN               5
#define RADIO_MISO_PIN              19
#define RADIO_MOSI_PIN              27
#define RADIO_CS_PIN                18
#define RADIO_DI0_PIN               26
#define RADIO_RST_PIN               23
#define RADIO_DIO1_PIN              33
#define RADIO_BUSY_PIN              32

#define BOARD_LED                   4
#define LED_ON                      LOW
#define LED_OFF                     HIGH

#define GPS_BAUD_RATE               9600
#define HAS_GPS
#define HAS_DISPLAY                 //Optional, bring your own board, no OLED !!









