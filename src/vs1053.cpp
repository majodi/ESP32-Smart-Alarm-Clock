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
  volumeDesired = VOL_LOW;                                                // set desired volume to low as we force real volume to low also
  blynkSyncState();                                                       // update app state
  vsSetVolume(VOL_LOW);                                                   // set volume low immediately (no graduate)
}

void vsSetVolume(uint8_t vol) {
  uint16_t v;                                                             // prepare high/low bits
  v = vol;
  v <<= 8;
  v |= vol;
  noInterrupts();                                                         // turn off interrupts
  vsWriteRegister(VS1053_REG_VOLUME, v);                                  // write value
  interrupts();                                                           // turn on interrupts again
  volume = vol;                                                           // keep actual volume up-to-date (if not done already)
}

void regulateVolume() {
  if (volumeOverrule > 0) {
    vsSetVolume(volumeOverrule);
  } else {
    if (abs(volume - volumeDesired) > 1) {                                // if needed catch-up real volume to desired volume
      if (volume < volumeDesired) {                                       // volume too low?
        volume += (volumeDesired - volume) / 2;                           // slowly turn up to desired level
      }
      if (volume > volumeDesired) {                                       // volume too high?
        volume -= (volume - volumeDesired) / 2;                           // slowly turn down to desired level
      }
      vsSetVolume(volume);                                                // set new level
    }
  }
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

void startRadio(char *host, int port, char *path) {
  if (!radioPlaying) {                                                    // turn on when not playing
    httpConnect(host, port, false);                                       // connect to host
    httpGetRequest(host, path);                                           // request stream
    streamType = MP3_STREAM;                                              // set stream type to MP3 data
    digitalWrite (MUTE_PIN, HIGH);                                        // un-mute
    if (volumeOverrule > 0) {
      vsSetVolume(volumeOverrule);                                        // if needed overrule volume immediately (no regulation)
    }
    radioPlaying = true;                                                  // radio is playing
    radioTimePlayed = 0;                                                  // reset time played
    blynkSyncState();                                                     // update state to app
  }
}

void stopRadio() {
  if (radioPlaying) {                                                     // turn off only when playing
    digitalWrite (MUTE_PIN, LOW);                                         // mute
    volumeOverrule = 0;                                                   // reset volume overrule
    if (streamType == MP3_STREAM) {
      streamType = NO_ACTIVE_STREAM;                                      // set stream type to none
      httpEnd();
    }
    radioPlaying = false;                                                 // radio is not playing
    radioPlayTime = 0;                                                    // reset play time (continuous) for next start
    blynkSyncState();                                                     // update state to app
  }
}

void transferAvailableTTSMP3Data() {                                      // transfer waiting tts data to vs1053
  char encbuf[129] = {0};                                                 // buffer for encoded data (we get base64 encoded data)
  if (httpWaitAvailable(2000)) {                                          // is there actual data waiting?
    handleTouch();                                                        // first handle possible touch (to detect a snooze request)
    bool validResponseData = https.find("\"audioContent\": \"");          // find the audioContent token
    TTSPlaying = true;                                                    // assume we have content, so TTS is playing
    slog("TTSPlaying (data available), audiocontent %d, alarmstate %d, endrequest %d", validResponseData, alarmState, TTSEndRequest);
    // !! Blocking until full text reading is done !!
    while (httpWaitAvailable(2000) && validResponseData && alarmState != ALARM_SNOOZED && !TTSEndRequest) { // is token found assume data is valid, but check if we want to snooze first or if there was an end request
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
      handleTouch();                                                      // handle possible touch
    }
    volumeOverrule = 0;                                                   // reset possible volume overrule
  }
  httpEnd();                                                              // assume stream has ended (time-out was 2sec)
  streamType = NO_ACTIVE_STREAM;                                          // set stream to none
  TTSPlaying = false;                                                     // TTS is not playing anymore
  digitalWrite (MUTE_PIN, LOW);                                           // mute
  if (alarmState == ALARM_ACTIVE) {                                       // if in alarm state (INFO_TTS)
    nextAlarmSection(false);                                              // start next alarm section (force = false, not needed, TTS always full ending on itself)
  }
}

void transferAvailableMP3Data() {
  if (digitalRead(VS1053_DREQ)) {                                         // if the VS1053 is hungry
    handleTouch();                                                        // first handle possible touch (to detect a snooze request)
    if (httpWaitAvailable(2000) && alarmState != ALARM_SNOOZED) {         // hungry, we have food and we don't want to snooze
      uint8_t bytesread = secureConnect ? https.read(mp3IOBuffer, 32) : http.read(mp3IOBuffer, 32); // get some food
      if (bytesread) {
        vsWriteBuffer(mp3IOBuffer, bytesread);                              // and feed it to the VS1053
      } else {
        slog("could not read from MP3 stream");
      }
    } else {
      stopRadio();                                                        // stop radio
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
  delay(3500);
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
