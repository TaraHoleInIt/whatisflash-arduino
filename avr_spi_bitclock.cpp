#include <Arduino.h>
#include <avr/io.h>

#if defined SPCR
void FlashPump_AVRSPI( const uint32_t Length ) {
  uint8_t OldSPCR = SPCR;
  uint8_t OldSPSR = SPSR;
  uint32_t i = 0;

  noInterrupts( );
    SPSR = _BV( SPI2X );
    SPCR = _BV( SPE ) | _BV( MSTR );
    
    for ( i = 0; i < Length; i++ ) {
      SPDR = 0x00;
  
      while ( bit_is_clear( SPSR, SPIF ) )
      ;
    }
  
    SPCR = OldSPCR;
    SPSR = OldSPSR;
  interrupts( );
}
#endif
