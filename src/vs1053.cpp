#include "vs1053.h"

void vsReset() {                                                          // reset code
  digitalWrite(VS1053_xCS, HIGH);
  digitalWrite(VS1053_xDCS, HIGH);
  vsWriteWram(0xC017, 3);                                                 // GPIO register for pin directions, setting up MP3 mode
  vsWriteWram(0xC019, 0);
  delay(100);
  vsWriteRegister(VS1053_REG_MODE, VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_RESET);
  delay(200);
  vsWriteRegister(VS1053_REG_MODE, VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_LINE1);
  vsWriteRegister(VS1053_REG_CLOCKF, 0x6000);                             // clockfrequency
  vsWriteRegister(VS1053_REG_AUDATA, 44100 + 1);                          // 44.1kHz + stereo
  vsWriteRegister(VS1053_REG_VOLUME, 30);                                 // volume
}

uint16_t vsReadRegister(uint8_t vsRegister) {
  uint16_t result;
  SPI.beginTransaction(SPISettings(200000, MSBFIRST, SPI_MODE0));         // Begin exclusive SPI transaction (low clock)
  digitalWrite(VS1053_xCS, LOW);                                          // chip select
  SPI.write(3);                                                           // 3 = Read
  SPI.write(vsRegister);                                                  // from vsRegister
  result = SPI.transfer16(0xFFFF);                                        // read (write part of transfer is ignored by vs1053)
  digitalWrite(VS1053_xCS, HIGH);                                         // deselect
  SPI.endTransaction();                                                   // close this transaction
  return result;
}

void vsWriteRegister(uint8_t vsRegister, uint16_t value) {
  SPI.beginTransaction(SPISettings(200000, MSBFIRST, SPI_MODE0));         // Begin exclusive SPI transaction (low clock)
  digitalWrite(VS1053_xCS, LOW);                                          // chip select
  SPI.write(2);                                                           // 2 = Write
  SPI.write(vsRegister);                                                  // to vsRegister
  SPI.write16(value);
  while (!digitalRead(VS1053_DREQ));                                      // vs1053 datasheet says we should wait until DREQ is high again
  digitalWrite(VS1053_xCS, HIGH);                                         // deselect
  SPI.endTransaction();                                                   // close this transaction
}

void vsWriteBuffer(uint8_t *buffer, uint8_t size) {
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));        // Begin exclusive SPI transaction (high clock)
  digitalWrite(VS1053_xDCS, LOW);                                         // chip select (data)
  while (size--) {                                                        // write buffer byte by byte
    SPI.write(buffer[0]);
    buffer++;
  }
  digitalWrite(VS1053_xDCS, HIGH);                                        // deselect (data)
  SPI.endTransaction();                                                   // close this transaction
}

void vsWriteWram(uint16_t address, uint16_t data) {                       // write Wram (used in reset to configere chip)
  vsWriteRegister(VS1053_REG_WRAMADDR, address);
  vsWriteRegister(VS1053_REG_WRAM, data);
}

void vsSineTest() {                                                       // is it alive?
  uint16_t mode = vsReadRegister(VS1053_REG_MODE);                        // turn on tests (bit 5)
  mode |= 0x0020;
  vsWriteRegister(VS1053_REG_MODE, mode);
  while (!digitalRead(VS1053_DREQ));                                      // wait until DREQ is high again
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));        // Begin exclusive SPI transaction (high clock)
  digitalWrite(VS1053_xDCS, LOW);                                         // chip select (data)
  SPI.write(0x53);                                                        // sine test
  SPI.write(0xEF);
  SPI.write(0x6E);
  SPI.write(0x44);
  SPI.write(0x00);
  SPI.write(0x00);
  SPI.write(0x00);
  SPI.write(0x00);
  digitalWrite(VS1053_xDCS, HIGH);                                        // deselect (data)
  SPI.endTransaction();                                                   // close this transaction
  delay(500);
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));        // Begin exclusive SPI transaction (high clock)
  digitalWrite(VS1053_xDCS, LOW);                                         // chip select (data)
  SPI.write(0x45);
  SPI.write(0x78);
  SPI.write(0x69);
  SPI.write(0x74);
  SPI.write(0x00);
  SPI.write(0x00);
  SPI.write(0x00);
  SPI.write(0x00);
  digitalWrite(VS1053_xDCS, HIGH);                                        // deselect (data)
  SPI.endTransaction();                                                   // close this transaction
}
