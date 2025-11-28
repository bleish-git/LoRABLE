/*
*
*   RX sketch per Heltec LORAWAN v3
*   Riceve un segnale LoRa con medesime caratteristiche 
*   Nessun protocollo, solo invio/ricezione via hardware punto-punto
*
*   https://github.com/bleish-git/LoRABLE/ - MIT License
*   bleish@gmail.com - 2025
*/

#include "LoRaWan_APP.h"
#include "Arduino.h"

// Display OLED integrato
#include <Wire.h>               
#include "HT_SSD1306Wire.h"

//Eventuale logo da mostrare all'avvio
//#include "images.h"

static SSD1306Wire  display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED); // addr , freq , i2c group , resolution , rst


#define RF_FREQUENCY                                865000000 // Hz

#define TX_OUTPUT_POWER                             5        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // payload size 

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

int16_t rssi,rxSize;

double txNumber;

bool lora_idle=true;

static RadioEvents_t RadioEvents;

void drawText(String);
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

void setup() {
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);

    VextON();
    delay(100);

    // Inizializzazione display.
    display.init();
    drawText("\nModulo RX\nCiao...");
    delay(7000);
	
    txNumber=0;
    rssi=0;
    rxSize=0;
    
    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
   }


void drawText(String msg="Ciao.\n\nRX mode...") {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 20, msg);
    //display.setFont(ArialMT_Plain_16);
    //display.drawString(0, 10, "Hello world");
    //display.setFont(ArialMT_Plain_24);
    //display.drawString(0, 26, "Hello world");
    display.display();  
}

void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
}

void loop()
{
  display.clear();
  Radio.IrqProcess( );

	if(lora_idle == true)
	{
    lora_idle = false;
    Serial.println("Rx mode...");
    Radio.Rx(0);
	}
  
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    rssi=rssi;
    rxSize=size;
    memcpy(rxpacket, payload, size );
    rxpacket[size]='\0';
    Radio.Sleep( );
    Serial.printf("\r\nreceived packet \"%s\" with rssi %d , length %d\r\n",rxpacket,rssi,rxSize);
    drawText(rxpacket);
    lora_idle = true;
}