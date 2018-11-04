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

void transferAvailableTTSMP3Data() {                                      // transfer waiting tts data to vs1053
  char encbuf[129] = {0};                                                 // buffer for encoded data (we get base64 encoded data)
  if (https.available()) {                                                // is there actual data waiting?
    bool validResponseData = https.find("\"audioContent\": \"");          // find the audioContent token
    while (https.available() && validResponseData) {                      // is token found assume data is valid
      int r = https.readBytes(encbuf, 128);                               // read 4 blocks of 32 bytes encoded data
      encbuf[r] = 0;                                                      // put a delimiter at the end for if we need it for logging
      int decodedLength = Base64.decodedLength(encbuf, r);                // wat will be the length of the decoded data?
      char decodedString[decodedLength];                                  // reserve room for it
      Base64.decode(decodedString, encbuf, r);                            // decode it
      int offset = 0;
      while (offset < decodedLength) {                                    // write blocks of 32 bytes (probably 3 but to be shure this way)
        uint8_t w = decodedLength - offset > 32 ? 32 : decodedLength - offset; // 32 bytes is wat the vs1053 can swallow at once
        while (!digitalRead(VS1053_DREQ)) {}                              // first wait until the vs1053 is ready for this next bite
        vsWriteBuffer((uint8_t *) decodedString + offset, w);             // and feed it to the VS1053
        offset += w;                                                      // next bite
      }
    }
  }
}

void transferAvailableMP3Data() {
  if (digitalRead(VS1053_DREQ)) {                                         // if the VS1053 is hungry
    if (http.available() > 0) {                                           // and we have food
      uint8_t bytesread = http.read(mp3IOBuffer, 32);                     // get some food
      vsWriteBuffer(mp3IOBuffer, bytesread);                              // and feed it to the VS1053
    }
  }
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