#include <Arduino.h>

#if defined USICR
  void FlashPump_AVRUSI( const uint32_t Length ) {
    uint8_t r16 = _BV( USIWM0 ) | _BV( USITC );
    uint8_t r17 = _BV( USIWM0 ) | _BV( USITC ) | _BV( USICLK );
    uint32_t i = 0;

    noInterrupts( );
      // Transmit only null bytes, remember, the flash is still active
      USIDR = 0;
      
      for ( i = 0; i < Length; i++ ) {
        // Clock out using USI
        // Source: ATTiny 25/45/85 datasheet
        USICR = r16;
        USICR = r17;
        
        USICR = r16;
        USICR = r17;
        
        USICR = r16;
        USICR = r17;
        
        USICR = r16;
        USICR = r17;
        
        USICR = r16;
        USICR = r17;
        
        USICR = r16;
        USICR = r17;
        
        USICR = r16;
        USICR = r17;
        
        USICR = r16;
        USICR = r17;
      }
    interrupts( );
  }
#endif
