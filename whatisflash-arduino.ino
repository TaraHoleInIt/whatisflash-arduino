#include <Arduino.h>
#include <SPI.h>

// Uncomment the following line if you want to use Arduino's
// SPI implementation to run the flash pump.
// This should work everywhere.
//#define Config_SPI_Pump_Generic

// Uncomment the following line if you want to use software
// bitbanging to drive the SPI clock on AVR microcontrollers.
//#define Config_SPI_Pump_AVRSW

// Uncomment the following line if you want to use the USI
// peripheral found on some AVR microcontrollers to drive
// the SPI clock.
//#define Config_SPI_Pump_AVRUSI

// Uncomment the following line if you want to use the hardware
// SPI peripheral found on most AVR microcontrollers.
#define Config_SPI_Pump_AVRSPI

#define Config_Data_FrameDelay_MS 16
#define Config_Data_FrameCount 29
#define Config_Data_FrameSize ( 96 * 64 )
#define Config_Init_Length 7
#define Config_Data_Offset 7

// Uncomment this if you're setting Config_Pin_LCD_RST
// and you're going to use an IO line to control LCD reset.
// Leave commented if you're using an external reset circuit
#define Config_LCD_Reset_Time_MS 50

#define Config_LCD_HIGH_Wait 500

#define Config_Pin_Flash_CS 10
#define Config_Pin_LCD_CS 9
#define Config_Pin_LCD_RST 8
#define Config_Pin_LCD_DC 7

#define BIT( n ) ( 1 << n )

extern void FlashPump_AVRSW( const uint32_t Length );
extern void FlashPump_AVRUSI( const uint32_t Length );
extern void FlashPump_AVRSPI( const uint32_t Length );

extern void FlashPump_Generic( const uint32_t Length );

void FlashRead( const uint32_t Address, const uint32_t Length ) {
  uint32_t i = 0;
  
  SPI.beginTransaction( SPISettings( ) );
    // Select SPI flash chip
    digitalWrite( Config_Pin_Flash_CS, LOW );

      // Start read from address
      SPI.transfer( 0x03 ); // Flash read
      SPI.transfer( ( Address >> 16 ) & 0xFF );
      SPI.transfer( ( Address >> 8 ) & 0xFF );
      SPI.transfer( Address & 0xFF );

#if defined Config_Pin_LCD_CS
      // Select LCD
      digitalWrite( Config_Pin_LCD_CS, LOW );
#endif

#if defined Config_SPI_Pump_Generic
      FlashPump_Generic( Length );
#elif defined Config_SPI_Pump_AVRSW
      FlashPump_AVRSW( Length );
#elif defined Config_SPI_Pump_AVRUSI
      FlashPump_AVRUSI( Length );
#elif defined Config_SPI_Pump_AVRSPI
      FlashPump_AVRSPI( Length );
#else
      FlashPump_Generic( Length );
#endif
      
#if defined Config_Pin_LCD_CS
      digitalWrite( Config_Pin_LCD_CS, HIGH );
#endif
    digitalWrite( Config_Pin_Flash_CS, HIGH );
  SPI.endTransaction( );
}

void LCD_Init( void ) {
#if defined Config_Pin_LCD_DC
  // DC line controlled by software
  digitalWrite( Config_Pin_LCD_DC, LOW );
    FlashRead( 0, Config_Init_Length );
  digitalWrite( Config_Pin_LCD_DC, HIGH );
#else
  // Init entirely by flash chip
  // Only a few displays support this
  FlashRead( 0, Config_Init_Length );
  
  delay( Config_LCD_HIGH_Wait );
#endif
}

void setup( void ) {
  SPI.begin( );

#if defined Config_Pin_LCD_RST
  // Configure reset pin and do a generic low->wait->high->wait reset
  pinMode( Config_Pin_LCD_RST, OUTPUT );

  // Low to reset
  digitalWrite( Config_Pin_LCD_RST, LOW );
  delay( Config_LCD_Reset_Time_MS );

  // Recovery
  digitalWrite( Config_Pin_LCD_RST, HIGH );
  delay( Config_LCD_Reset_Time_MS );
#else
  // External reset circuit used
  // Wait (time) for LCD reset to finish
  #if defined Config_LCD_Reset_Time_MS
    delay( Config_LCD_Reset_Time_MS );
  #endif
#endif

#if defined Config_Pin_LCD_DC
  pinMode( Config_Pin_LCD_DC, OUTPUT );
  digitalWrite( Config_Pin_LCD_DC, LOW );
#endif

#if defined Config_Pin_LCD_CS
  pinMode( Config_Pin_LCD_CS, OUTPUT );
  digitalWrite( Config_Pin_LCD_CS, HIGH );
#endif

  pinMode( Config_Pin_Flash_CS, OUTPUT );
  digitalWrite( Config_Pin_Flash_CS, HIGH );

  LCD_Init( );
}

void loop( void ) {
  uint32_t TimeTaken = 0;
  uint32_t TimeStart = 0;
  uint32_t i = 0;

  while ( true ) {
    for ( i = 0; i < Config_Data_FrameCount; i++ ) {
      TimeStart = millis( );
#if defined Config_Pin_LCD_CS
      FlashRead( Config_Data_Offset + ( i * Config_Data_FrameSize ), Config_Data_FrameSize );
#else
      // If we don't have control over the LCD's chip select pin and it's always tied low then
      // the commands to read from SPI flash will be interpreted as data.
      // This just fixes the offsets and length a bit but there will still be a tiny amount of garbage
      // transferred to the screen.
      FlashRead( Config_Data_Offset + 4 + ( i * Config_Data_FrameSize ), Config_Data_FrameSize - 4 );
#endif
      TimeTaken = millis( ) - TimeStart;

      // Only run the delay between frames if we were fast enough to have time left over
      if ( TimeTaken < Config_Data_FrameDelay_MS && ( Config_Data_FrameDelay_MS - TimeTaken ) > 0 ) {
        delay( Config_Data_FrameDelay_MS - TimeTaken );
      }
    }
  }
}
