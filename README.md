# ESP32 Smart Alarm Clock

ESP32 based "Smart" alarm clock. Talking alarm clock with wake-up sequence (NeoPixel light animation, wake-up sounds, information like news, weather and personal calendar). Touch control. Internet radio. Sleep (activity) monitoring using radar for motion detection. Smart phone app for control and settings. OTA updating and remote console (telnet).

For a short impression see [YouTube](https://www.youtube.com/watch?v=J7KHj_QfIIQ).

## Technology

ESP32 based

ESP32 Arduino Core

FreeRTOS

VS1053

NeoPixel

Google TTS

...

## Release notes

** July 22, 2019**
Minor adjustments, tested and stable for several months. 
**1.01 April 27, 2019**
Temporary adjustments to facilitate new release of RemoteDebug that doesn't play along nicely with PlatformIO. See [GitHub](https://github.com/JoaoLopesF/RemoteDebug/issues/48).
Added time adjust option (both APP and ESP code) for summer/winter time.
Fixed adjustment to Zulu time when fetching new alarm time. Currently hardcoded to 1 hour Amsterdam time-zone. In the future this could be another option.
Added 1 minute time window for early alrm trigger to avoid collision with new alrm time fetch.

