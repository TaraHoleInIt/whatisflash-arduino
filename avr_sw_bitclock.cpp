#include <Arduino.h>

// Set this to the port where your SPI lines are hooked up to
#define SPI_PORT PORTB

// Set pins here, NOTE THAT THESE ARE PIN NUMBERS FOR THAT PORT AND NOT ARDUINO PIN NUMBERS
#define SPI_CLK_PIN _BV( 2 )
#define SPI_DO_PIN _BV( 1 )

#if defined __AVR_ARCH__
  void FlashPump_AVRSW( const uint32_t Length ) {
    uint32_t i = 0;
  
    // Make sure this is zero so we don't bother the flash chip with
    // anything other than zero bytes.
    // This might not matter but eh, better to be safe.
    SPI_PORT &= ~SPI_DO_PIN;

    noInterrupts( );
      for ( i = 0; i < Length; i++ ) {
        // Pulse the clock line 8 times
        SPI_PORT |= SPI_CLK_PIN;
        SPI_PORT &= ~SPI_CLK_PIN;
        
        SPI_PORT |= SPI_CLK_PIN;
        SPI_PORT &= ~SPI_CLK_PIN;
        
        SPI_PORT |= SPI_CLK_PIN;
        SPI_PORT &= ~SPI_CLK_PIN;
        
        SPI_PORT |= SPI_CLK_PIN;
        SPI_PORT &= ~SPI_CLK_PIN;
        
        SPI_PORT |= SPI_CLK_PIN;
        SPI_PORT &= ~SPI_CLK_PIN;
        
        SPI_PORT |= SPI_CLK_PIN;
        SPI_PORT &= ~SPI_CLK_PIN;
        
        SPI_PORT |= SPI_CLK_PIN;
        SPI_PORT &= ~SPI_CLK_PIN;
        
        SPI_PORT |= SPI_CLK_PIN;
        SPI_PORT &= ~SPI_CLK_PIN;    
      }
    interrupts( );
  }
#endif
