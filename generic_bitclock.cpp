#include <Arduino.h>
#include <SPI.h>

void FlashPump_Generic( const uint32_t Length ) {
  uint32_t i = 0;

  for ( i = 0; i < Length; i++ ) {
    SPI.transfer( 0x00 );
  }
}
