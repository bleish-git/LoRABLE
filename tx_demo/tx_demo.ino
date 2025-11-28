/*
*
*   TX sketch per Heltec LORAWAN v3
*   Invio pacchetti senza nessun protocollo via hardware punto-punto
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

double txNumber;

bool lora_idle=true;

static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnTxTimeout( void );
void drawText(String msg="Ciao.");

void setup() {
    Serial.begin(115200);
    Mcu.begin(HELTEC_BOARD,SLOW_CLK_TPYE);

    VextON();
    delay(100);

    // Inizializzazione display.
    display.init();
    display.setFont(ArialMT_Plain_24);
    drawText("\nModulo TX\nCiao...");
    delay(7000);


    txNumber=0;

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 ); 
   }


void drawText(String msg) {
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, msg);
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
  
    
	if(lora_idle == true)
	{
    delay(3000);
		txNumber += 0.01;
		sprintf(txpacket,"\nHello world number %0.2f",txNumber);  //prepara un pacchetto

		Serial.printf("\r\nsending packet \"%s\" , length %d\r\n",txpacket, strlen(txpacket));
    drawText(txpacket);

		Radio.Send( (uint8_t *)txpacket, strlen(txpacket) ); //invia il pacchetto	
    lora_idle = false;
	}
  Radio.IrqProcess( );
}

void OnTxDone( void )
{
	Serial.println("TX done......");
	lora_idle = true;
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
    Serial.println("TX Timeout......");
    lora_idle = true;
}
