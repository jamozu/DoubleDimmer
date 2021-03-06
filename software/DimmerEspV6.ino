/** **********************************************************************************************
   DIMMER
*********************************************************************************************** */
/*
  This software may be modified and distributed under the terms of the MIT license.

  LICENSE
  <https://mit-license.org/>.
  ------------------------------------------------------------------------------------------------
  Copyright © 2020 Jesus Amozurrutia
  
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
  and associated documentation files (the “Software”), to deal in the Software without 
  restriction, including without limitation the rights to use, copy, modify, merge, publish, 
  distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
  Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  
  ------------------------------------------------------------------------------------------------
  RRoble Duble Dimmer (Customized)
  ------------------------------------------------------------------------------------------------
  
  This project consists of an AC light dimmer for one or two incandescent light bulbs or dimmable 
  LED bulbs, with AC of 110 to 220 volts, 50 or 60Hz, using the WiFi enabled controller ESP-12E.
  
  The Dimmer function is achieved using the phase control method, using a TRIAC for each bulb, 
  varying the total power applied. This method works very well with incandescent light bulbs and 
  with LED lamps that have the Dimmable specification.
  
  Features:
  - Smart control and configuration using MQTT over WiFi. An MQTT broker is required.
  - One push button per light bulb to control the ON/OFF state and brightness.
  - ON/OFF state and brightness settings can be made via the integrated buttons or remotely via 
    WiFi/MQTT.
  - Lights can be dimmed from 10% to 100% power.
  - Supports transitions between brightness levels.
  - A LED indicator for each button/light bulb.
  - LED indicators change intensity if light is ON or OFF. 
  - LED indicators intensity can be set remotely or turned off for day / night mode.
  - Integrated into Home-Assistant or other MQTT compatible IoT Hubs, with auto discovery.
  - Setup Portal over WiFi, to configure WiFi/MQTT parameters, triggered by a double reset or
    optionally with multi-click of any of the light buttons.
  - On Power Up the device resets to the last known state, using MQTT retain or EEPROM.
  - Optional double click to trigger additional devices.
  - Advanced configuration through MQTT.

  ------------------------------------------------------------------------------------------------
  Definitions
  ------------------------------------------------------------------------------------------------

  - Device: Refers to the component as a whole. It is used to identify the status of the device 
    and to set the configuration.
  - Instance: It refers to each of the lights. The status of each of the instances can be set 
    individually.
  - Brightness: Relative brightness for each of the light bulbs. Brightness is scaled from 0, for 
    minimum brightness, to 100, for maximum brightness.
  - Zero Crossing: It is a pulse that detects every time the AC sinusoidal electrical voltage 
    signal crosses the zero volt zone. It is used to synchronize the phase control.
  - Power: The electrical power applied to the light bulb to achieve the desired brightness. Power 
    is regulated using phase control, by varying the time to trigger the TRIAC after the zero 
    crossing detection. The applied power is linear and proportional to the brightness value.
  - Minimum Power: The minimum power applied to the light bulb when the brightness has a value of 
    zero. The default value is 10% and can be configured to any value between 10% and 50%.
  - Duty Cycle: The time that the TRIAC is ON in relation to the duration of the AC period.
  - OTA: Over The Air updates. Allow the firmware to be updated over WiFi.
  
  ------------------------------------------------------------------------------------------------
  Pinout ESP-12E
  ------------------------------------------------------------------------------------------------

                 | RST            TXD0 | 
                 | ADC            RXD0 | 
                 | EN             IO5  |-----> TRIAC2 / TRIAC1
                 | IO16           IO4  |-----> TRIAC1
  Button 2 ----->| IO14           IO0  |-----> LED2 
  Button 1 ----->| IO12           IO2  |-----> LED1 
  ZCP      ----->| IO13           IO15 |
                 | VCC            GND  | 

  ------------------------------------------------------------------------------------------------
  Flashing
  ------------------------------------------------------------------------------------------------
  Board: "Generic ESP8266 Module"
  Built in LED: 2
  Upload Speed: 115200
  CPU Frequency: 80 MHz
  Crystal Frequency: 26 MHz
  Flash Size: 4M (1M LittleFS)
  Flash Mode: QIO
  Flash Frequency: 40 MHz
  Reset Method: ck
  Debug Port: Disabled
  Debug Level: None
  IwIP Varaint: v2 Lower Memory
  
  
  ------------------------------------------------------------------------------------------------
  Todo
  ------------------------------------------------------------------------------------------------

  - MQTT Signatures
  
  ------------------------------------------------------------------------------------------------
  ------------------------------------------------------------------------------------------------
  @author Jesus Amozurrutia Elizalde <jamozu@gmail.com>
  @version 0.7.10
  @date 2021/11/01
  @since Friday February 01 2019, 10:32:00
  @copyright MIT license
  @pre Device: ESP-12E (ESP8266)
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//
// Sketch configuration starts here
//
//              v            v            v            v            v 

/* --------------------------------------------------------------------------------------------
   Device identification
   -------------------------------------------------------------------------------------------- */
// Hardware version
#define HVERSION "02"
#define HVERSION_NUM 2

// Software version
#define SVERSION "0.7.10"

// Title for the application
#define DEVICE_TITLE "RRoble Light Dimmer"

// Title for WiFi/MQTT Setup Portal
#define PORTAL_PREFIX "RDimmer_"

// MQTT instance type. Used by the IoT Hub to identify the device properties and to construct
// the MQTT topics
#define INSTANCE_TYPE "light"


/* --------------------------------------------------------------------------------------------
   Features
   -------------------------------------------------------------------------------------------- */
// Enable second light bulb
#define DOUBLE_DIMMER

// Enable persistent state in EEPROM. Saves ligt state to EEPROM (if MQTT Retain flag is OFF)
#define PERSISTENT_STATE

// Enable double click events
#define DBL_CLICK

// Enable multiple clicks to trigger the Setup Portal
#define MULT_CLICK

// Enable MQTT discovery (HomeAssistant standard)
#define MQTT_DISCOVERY

// MQTT configuration enabled. Allows the device to be configured remotely trough MQTT
#define MQTT_CONFIG 

// Allow reset through MQTT message
#define MQTT_RESET 

// Enable OTA Updates
//#define OTA_UPDATES

// Save staus information on reset (useful for debugging network issues)
#define SAVE_ON_RESET

// Allow clearing saved status via MQTT message
#define SAVE_ON_RESET_CLEAR


/* -------------------------------------------------------------------------------------------- 
    Debug
   -------------------------------------------------------------------------------------------- */
// Enable debug messages to serial port
#define DEBUG_MODE

// Serial port speed
#define SERIAL_SPEED 74880


/* -------------------------------------------------------------------------------------------- 
    Direct OTA Updates
   -------------------------------------------------------------------------------------------- */
// OTA Port
//#define OTA_AR_PORT 8266

// MD5 Hash for secure OTA Arduino Updates
//#define OTA_AR_HASH "266ba2d990fc2ff1ef34c571e4ea49de"

// Password for secure OTA Arduino Updates (not recommended)
//#define OTA_AR_PASS "myOtaPass"


/* -------------------------------------------------------------------------------------------- 
    OTA through HTTP Server (Overrides Direct OTA Updates)
   -------------------------------------------------------------------------------------------- */
// Define the next 4 parameters to update from an HTTP Server
//#define OTA_HTTP_SERVER "domain.name"
//#define OTA_HTTP_PORT 80
//#define OTA_SCRIPT_NAME "/updateScript.php"

// Another option is to define the Http server in a single line
//#define OTA_HTTP_URL "http://domain.name/updateScript.php"


/* -------------------------------------------------------------------------------------------- 
   Network
   -------------------------------------------------------------------------------------------- */
// WiFi re-connect timer (ms)
#define WIFI_RECONNECT 8900

// Reboot after N consecutive WiFi connect attemps
#define WIFI_CONNECT_REBOOT 10

// Reboot after WiFi is restated N times without connectivity to the broker
#define WIFI_RESTART_REBOOT 3

// Network re-connect timer (ms)
#define NET_RECONNECT 2200

// MQTT subscription QOS [0, 1]
#define MQTT_SUB_QOS 1

// MQTT publish QOS [0, 1]
#define MQTT_PUB_QOS 1

// MQTT maximim number of unanswered events
#define MQTT_MAX_EVENTS 5

// MQTT Signature (Not implemented yet)
//#define MQTT_SIGN

// MQTT Buffer size (10 - 90)
#define BUFFER_SIZE 15

// MQTT Subscription Busy Time Out (ms)
#define MQTT_BUSY_TO 5000

// MQTT Buffer message timer (ms)
#define MQTT_BUFFER_TMR 350

// Re-subscribe to MQTT Topics (sec)
#define MQTT_RESUBSCRIBE 600

/* -------------------------------------------------------------------------------------------- 
   Messages
   -------------------------------------------------------------------------------------------- */
// Alive message
#define MQTT_MSG_ALIVE "{\"feedback\":\"online\"}"

// Go live sequence start message to hub/broker
#define MQTT_MSG_GOLIVE "{\"feedback\":\"golive\"}"

// Go live sequence start message to hub/broker
#define MQTT_MSG_RESET "{\"feedback\":\"golive\"}"

// Configuration saved
#define MQTT_MSG_SAVE "{\"feedback\":\"configuration saved\"}"

// Configuration save failed
#define MQTT_MSG_SAVE_FAIL "{\"feedback\":\"configuration not saved\"}"

// Configuration save failed
#define MQTT_MSG_SC "{\"feedback\":\"status cleared\"}"

// Configuration save failed
#define MQTT_MSG_SNC "{\"feedback\":\"status not cleared\"}"


/* -------------------------------------------------------------------------------------------- 
   Dimmer definitions
   -------------------------------------------------------------------------------------------- */
// Go live sequence max duration (sec)
#define GO_LIVE_DURATION 60

// Mimimum power applied to lamps (10% is a safe value, but some  LED lamps require between 
//  15% and 20% to be vissible and differentiate from the off state
#define MIN_POW  10

// Maximum value for Minimum power
#define MAX_MIN_POW  50

// Maximum dimm time (ms)
#define MAX_DIMM_TIME  20000

// Minimum dimm time (ms)
#define MIN_DIMM_TIME  500

// Maximum time at the edges of the dimmable range (ms). 
#define MAX_EDGE_TIME  3000

// Minimum time at the edges of the dimmable range (ms).
#define MIN_EDGE_TIME  100

// Button debounce timer
#define BTN_DEBOUNCE  50

// Button long press in ms
#define BTN_LONG_PRESS  800

// Button double click timer in ms
#define DBL_CLICK_INTERVAL  600

// Number of double clicks to trigger the Setup Portal
#define MULT_CLICK_COUNT 6

// Transition speed on wakeup (after a power outage or reset)
#define CONFIG_TRANSITION_WAKE 5


/* -------------------------------------------------------------------------------------------- 
   Default configuration values
   -------------------------------------------------------------------------------------------- */
#define CONFIG_TIMEOUT 300                // Setup Portal timeout

#define CONFIG_KEEPALIVE 30               // "keepAlive" configuration parameter default
#define CONFIG_MQTT_PORT 1883             // "mqttport" configuration parameter default
#define CONFIG_MQTT_RETAIN true           // "retain" configuration parameter default
#define CONFIG_MQTT_BROKER ""             // "mqttbroker" configuration parameter default
#define CONFIG_MQTT_USER ""               // "mqttuser" configuration parameter default
#define CONFIG_MQTT_PASS ""               // "mqttpass" configuration parameter default
#define CONFIG_MQTT_NAMESPACE "namespc"   // "namespc" configuration parameter default
#define CONFIG_MQTT_KEY ""                // "mqttkey" configuration parameter default
#define CONFIG_LGT_MODE true              // "LgtMode" configuration parameter default
#define CONFIG_LGT_DIMM true              // "LgtDimm" configuration parameter default
#define CONFIG_LGT_TRNS true              // "LgtTrns" configuration parameter default
#define CONFIG_LED_BRIGHT 100             // "LedBright" configuration parameter default
#define CONFIG_LED_DIMM 25                // "LedDimm" configuration parameter default
#define CONFIG_LED_DEFAULT true           // "LedDefault" configuration parameter default
#define CONFIG_DIMMING_TIME 2500          // "dimmTime" configuration parameter default
#define CONFIG_EDGE_TIME 800              // "edgeTime" configuration parameter default
#define CONFIG_TRANSITION_ON 0            // "transitionOn" configuration parameter default
#define CONFIG_TRANSITION_OFF 0           // "transitionOff" configuration parameter default
#define CONFIG_MIN_POW 10                 // "minPow" configuration parameter default
#define CONFIG_BRIGHTNESS 100             // "brightness" configuration parameter default


/* -------------------------------------------------------------------------------------------- 
   Double Reset
   -------------------------------------------------------------------------------------------- */
// Time between resets to detect a double reset
#define DRD_TIMEOUT 1.5

// Hardware address
#define DRD_ADDRESS 0x00


//              ^            ^            ^            ^            ^ 
//
// Sketch configuration ends here
//
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

/* --------------------------------------------------------------------------------------------
   Libraries
   LittleFS
   -------------------------------------------------------------------------------------------- */
//#include <FS.h>                 // This needs to be first, or it all crashes and burns...
#include "LittleFS.h" 
#include <ESP8266WiFi.h>        // Ver: 2.3.0 @ https://github.com/esp8266/Arduino
//                                 https://wiki.wemos.cc/tutorials:get_started:get_started_in_arduino
//                                 a) Start Arduino and open File -> Preferences window.
//                                 b) Enter the following URL into "Additional Board Manager URLs" field.
//                                    You can add multiple URLs, separating them with commas.
//                                    http://arduino.esp8266.com/stable/package_esp8266com_index.json
//                                    OLD: http://arduino.esp8266.com/versions/2.3.0/package_esp8266com_index.json
//                                 c) Open "Boards Manager" from Tools -> Board menu and install "esp8266" platform.
//                                 d) Patch the files @ https://github.com/esp8266/Arduino/commit/4dc4e75216dfe21ca1bc0ba4eebc2ec94d7b6be5
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>        // Ver: 0.14.0 @ https://github.com/tzapu/WiFiManager
#include <DoubleResetDetect.h>  // Ver: 1.0.0 @ https://github.com/jenscski/DoubleResetDetect
#include <ArduinoJson.h>        // Ver: 5.13.3 @ https://github.com/bblanchon/ArduinoJson/  
//                                 https://arduinojson.org
//                                 The Arduino Library Manager installs the ArduinoJson version 6 by default.
//                                 However, using version 5 is highly recommended because version 6 is still in beta stage.
#include <AsyncMqttClient.h>    // Ver: 0.8.1 @ https://github.com/marvinroger/async-mqtt-client/releases/tag/v0.8.1
                                // https://github.com/marvinroger/async-mqtt-client/blob/master/docs/1.-Getting-started.md
                                // Download the ZIP file
                                // Load the .zip with Sketch → Include Library → Add .ZIP Library
                                // Dependency: https://github.com/me-no-dev/ESPAsyncTCP/archive/master.zip
#include <Ticker.h>

/* -------------------------------------------------------------------------------------------- 
   General definitions
   -------------------------------------------------------------------------------------------- */
// System timer (us)
#define SYS_TIMER 1000

// Number of MQTT instances
#ifdef DOUBLE_DIMMER
  #define INSTANCE_NUM 2
#else
  #define INSTANCE_NUM 1
#endif

// LED base in us
#define LED_PWM 8000

// PACKETS IN TRANSIT
#define IN_TRANSIT 3

// TRANSIT TIMEOUT
#define TRANSIT_TO 5

// JSON Library
#define JSON6

#if (defined(OTA_HTTP_SERVER) && defined(OTA_HTTP_PORT) && defined(OTA_SCRIPT_NAME)) || (defined(OTA_HTTP_URL))
  #define OTA_HTTP
  #if INSTANCE_NUM == 3
    #define OTA_IMAGE_VERSION (SVERSION ".T" HVERSION)
  #elif INSTANCE_NUM == 2
    #define OTA_IMAGE_VERSION (SVERSION ".D" HVERSION)
  #else
    #define OTA_IMAGE_VERSION (SVERSION ".S" HVERSION)
  #endif
#endif

#ifdef SAVE_ON_RESET
  #define SAVE_ON_RESET_AT 10
#endif


/* -------------------------------------------------------------------------------------------- 
   Conditional libraries
   -------------------------------------------------------------------------------------------- */
#ifdef OTA_UPDATES
  #ifdef OTA_HTTP
    #include <ESP8266HTTPClient.h>
    #include <ESP8266httpUpdate.h>
  #else
    #include <ESP8266mDNS.h>
    #include <WiFiUdp.h>
    #include <ArduinoOTA.h>
  #endif
#endif

#if defined(SAVE_ON_RESET) || defined(PERSISTENT_STATE)
  #include <EEPROM.h>
#endif


/* -------------------------------------------------------------------------------------------- 
   Hardware
   -------------------------------------------------------------------------------------------- */
// Pins
#define PIN_ZCROSS 13        // Zero crossing signal pin
#if HVERSION_NUM == 1
  #if INSTANCE_NUM == 2
    #define PIN_TM1 4          // Triac 1 trigger signal (Right)
    #define PIN_TM2 5          // Triac 2 trigger signal (Left)
    #define PIN_BUTTON1 14     // Button 1 (Right)
    #define PIN_BUTTON2 12     // Button 2 (Left)
    #define PIN_LED1 2         // LED indicator 1 (Right)
    #define PIN_LED2 0         // LED indicator 2 (Left)
  #else
    #define PIN_TM1 5          // Triac 1 trigger signal
    #define PIN_BUTTON1 12     // Button 1
    #define PIN_LED1 4         // LED indicator 1
  #endif
#else
  #if INSTANCE_NUM == 2
    #define PIN_TM1 4          // Triac 1 trigger signal (Right)
    #define PIN_TM2 5          // Triac 2 trigger signal (Left)
    #define PIN_BUTTON1 12     // Button 1 (Right)
    #define PIN_BUTTON2 14     // Button 2 (Left)
    #define PIN_LED1 2         // LED indicator 1 (Right)
    #define PIN_LED2 0         // LED indicator 2 (Left)
  #else
    #define PIN_TM1 5          // Triac 1 trigger signal
    #define PIN_BUTTON1 12     // Button 1
    #define PIN_LED1 2         // LED indicator 1
  #endif
#endif


/* -------------------------------------------------------------------------------------------- 
   Dimmer AC Signal detection
   -------------------------------------------------------------------------------------------- */
// Segments in each half AC cycle
#define SEGMENTS 119
// Zero Cross Debounce time
#define ZC_DEBOUNCE 4165
// Number of tics in a half AC cycle @60Hz
#define TIC_60_CYCLE 41666
// Number of tics for the last portion of the 60Hz half cycle where the TRIAC signal should be off
#define TIC_60_OFF 3850
// Ticks per segment @60Hz (TIC_60_CYCLE / SEGMENTS)
#define TIC_60_SEGM 350
// Number of tics in a half AC cycle @50Hz
#define TIC_50_CYCLE 50000
// Number of tics for the last portion of the 50Hz half cycle where the TRIAC signal should be off
#define TIC_50_OFF 4620
// Ticks per segment @50Hz (TIC_50_CYCLE / SEGMENTS)
#define TIC_50_SEGM 420
// Min. time to detect a 60 Hz signal
#define TIM_60_LOW 7500
// Max. time to detect a 60 Hz signal
#define TIM_60_HIGH 9167
// Min. time to detect a 50 Hz signal
#define TIM_50_LOW 9167
// Max. time to detect a 50 Hz signal
#define TIM_50_HIGH 11000
// Number of events to detect frequency and Zero Cross pulse
#define SETUP_EVENTS 200


/* --------------------------------------------------------------------------------------------
   Compilation messages
   -------------------------------------------------------------------------------------------- */
#if INSTANCE_NUM == 3
  #warning "Image for triple light dimmer"
#elif INSTANCE_NUM == 2
  #warning "Image for double light dimmer"
#else
  #warning "Image for single light dimmer"
#endif
#if HVERSION_NUM < 2
  #warning "Hardware version 1"
#else
  #warning "Hardware version 2"
#endif


/* --------------------------------------------------------------------------------------------
   Device identification constants
   -------------------------------------------------------------------------------------------- */
// Title for the application
const char* TITLE = DEVICE_TITLE " Ver. " SVERSION;

// Title for WiFi/MQTT configuration portal
const char* PORTAL_SSID = PORTAL_PREFIX;

// MQTT Device type. Used by the IoT Hub to identify the device properties and to construct
// the MQTT topics
const char* INST_TYPE = INSTANCE_TYPE;


/* -------------------------------------------------------------------------------------------- 
   Configuration parameters. Stored in FS (File System)
   -------------------------------------------------------------------------------------------- */
struct iotConfig {
  int mqttport;                   // MQTT port
  char mqttbroker[65];            // Broker domain or IP address
  char mqttuser[41];              // MQTT User
  char mqttpass[41];              // MQTT Password
  char namespc[41];               // MQTT Namespace
  char mqttkey[41];               // MQTT signature key (not implemented yet)
  bool retain;                    // MQTT Retain enable/disable 
  int keepAlive;                  // MQTT Keepalive interval
  char myId[21];                  // MQTT namespace Unique ID
  bool LgtMode;                   // General mode (Dimmer or Switch)
  int LedBright;                  // LED Indicator level in bright mode (lights on)
  int LedDimm;                    // LED Indicator level in dimm mode level (lights off)
  boolean LedDefault;             // Default status for LED Indicators
  int dimmTime;                   // Dimming time (milliseconds)
  int edgeTime;                   // Time on the edge of the dimmable range (milliseconds)
  int transitionOn;               // Transition speed to turn ON
  int transitionOff;              // Transition speed to turn OFF
  char iName[INSTANCE_NUM][41];   // Name for light bulb 1
  bool iDimm[INSTANCE_NUM];       // Light mode (Dimmer or Switch)
  bool iTrns[INSTANCE_NUM];       // Light transitions enabled
  int iMinPow[INSTANCE_NUM];      // Min. Power % for Light
  int iBrightness[INSTANCE_NUM];  // Default brightness
};
  // Global configuration object
iotConfig dconfig;


/* -------------------------------------------------------------------------------------------- 
   Topics
   -------------------------------------------------------------------------------------------- */
// MQTT topic to subscribe for incomming SET messages: "NAMESPACE/INSTANCE_TYPE/DEVICE_ID/set"
char topicSet[81];
// MQTT topic to publish device status: "NAMESPACE/INSTANCE_TYPE/DEVICE_ID/state"
char topicState[81];
#ifdef DBL_CLICK
  // MQTT topic to publish button actions: "NAMESPACE/DEVICE_ID/action"
  char topicAction[81];
#endif
// MQTT Instance Topics
struct topicStruct {
  char uname[41];     // Light Name
  char uid[41];       // Light Unique ID
  char state[81];     // Light STATE topic
  char set[81];       // Light SET topic
};
topicStruct mqttTopics[INSTANCE_NUM];


/* -------------------------------------------------------------------------------------------- 
   Button definitions
   -------------------------------------------------------------------------------------------- */
struct pressButton {
  int pinId;
  int eventState;
  int features;
  int state;
  int lastState;
  int clkCtr;
  boolean lngFlag;
  unsigned int repInterval;
  unsigned long btnTimer;
  unsigned long repTimer;
  unsigned long dblTimer;
};
pressButton myButtons[INSTANCE_NUM];


/* -------------------------------------------------------------------------------------------- 
   Configuration and loop variables
   -------------------------------------------------------------------------------------------- */
// Flag for saving configuration data
bool shouldSaveConfig = false;
// Flag for indicating the file system is mounted
bool mounted = false;
// Millisecond counter for periodic system calls
unsigned long sysMillis = 0;
// Overflow detector  for system timer
unsigned long sysOvf = 0;
// Flag for system overflow
bool sysOvfFlg = false;
// Keep alive counter
int keepAlive = 0;
// Second counter (low precision)
unsigned long aliveCounter = 0;
// Temp variables to create messages and compound strings
char tmpMsg0[101];
char tmpMsg[341];
// Flag that controls when we go live. 
//  0 = Go Live cycle off. 1 = Go Live; 2 = Go Live with retain messages;
int goLive = 2;
// Counts subscriptions when switching from Go Live cycle to normal operation.
int goLiveSwitch = 0;
// Delay between fail detected and reset.
int resetDelay = 0;
#ifdef OTA_UPDATES
  // Enable OTA if WiFi is available on boot. Otherwise, disable OTA and continue
  // with normal operation
  boolean otaEnabled = false;
  // Update flag
  boolean otaUpdating = false;
#endif
#ifdef MQTT_DISCOVERY
  // Auto discovery sent flag
  boolean discovered = false;
  // Auto discovery Instance pointer
  int discInst = 0;
  // Auto discovery Action pointer
  int discActn = 0;
#endif


/* -------------------------------------------------------------------------------------------- 
   Network variables
   -------------------------------------------------------------------------------------------- */
// Flag for controling first connect cycle
bool netFirst = true;
// Enable system reboot only if a full network connection is detected and then goes down
bool canReboot = false;
// Flag for controling WiFi setup
bool wifiSetup = false;
// Flag for controling WiFi connectivity
bool wifiStatus = false;
// WiFi re-connect/timeout timer
unsigned long wifiTimer = 0;
// Count consecutive connection attemps
int wifiConnCtr = 0;
// WiFi restart counter (between successfull Broker connections)
int wifiRstCtr = 0;
// WiFi total restarts counter (between boot)
int wifiRstTtl = 0;
// Flag for controling MQTT setup
bool mqttSetup = false;
// Flag for controling MQTT connectivity
bool mqttStatus = false;
// Flag indicating we are waiting connection response from the MQTT broker
boolean mqttConnecting = false;
// MQTT event counter. Messages and connection attemps. Resets the count on callback functions
int mqttEventCtr = 0;
// Pending MQTT events
unsigned int mqttBusy0 = 0;
unsigned long mqttTmr0 = 0;
// MQTT Retain messge counter
int retainCnt = 0;
// Re-subscribe counter
int reSubscribe = 0;
// Millisecond counter for net connect retry
unsigned long netMillis = 0;
// Millisecond counter for net connect retry
unsigned long pubMillis = 0;
// Device ID (MAC Address)
char deviceId[21];
// Buffer to publish offline MQTT messages
char buffTop[BUFFER_SIZE][201];
char buffMsg[BUFFER_SIZE][201];
bool buffRet[BUFFER_SIZE];
int buffStat = 0;
int buffPub = 0;
int buffLim = BUFFER_SIZE;
// Messages in transit
uint16_t transit[IN_TRANSIT];
int transitTic[IN_TRANSIT];
bool transitBussy = false;


/* -------------------------------------------------------------------------------------------- 
   Light Configuration
   -------------------------------------------------------------------------------------------- */
// Dimm edges speed
int edgeSpeed = 10;

/* -------------------------------------------------------------------------------------------- 
   AC Frequency detection
   -------------------------------------------------------------------------------------------- */
// Flag indicating the dimmer is in setup mode
boolean inSetup = true;
// AC Frequency
int frec = 0;
// AC Frequency calculator counter (invalid)
int frecCntr0 = 0;
// AC Frequency calculator counter (50Hz)
int frecCntr50 = 0;
// AC Frequency calculator counter (60Hz)
int frecCntr60 = 0;
// Ticks per duty cycle segment (SEGMENTS divisions per half cycle)
int frecTic = TIC_60_SEGM;              
// Timer to get the duration of the Zero Crossing (ZC) pulse
unsigned long pulseT = 0;
// Mid point of the ZC pulse
unsigned long pulseH = 0;
// ZC pulse debounce. A Falling edge pulse must fowwlo a rising edge pulse
boolean pulseDbnc = false;
// Start of the +/- 5% range for the ZC pulse duration (range 1)
unsigned long pulseS1 = 0;
// End of the +/- 5% range for the ZC pulse duration (range 1)
unsigned long pulseE1 = 0;
// Start of the +/- 5% range for the ZC pulse duration (range 2)
unsigned long pulseS2 = 0;
// End of the +/- 5% range for the ZC pulse duration (range 2)
unsigned long pulseE2 = 0;
// Start of the +/- 5% range for the ZC pulse duration (range 3)
unsigned long pulseS3 = 0;
// End of the +/- 5% range for the ZC pulse duration (range 3)
unsigned long pulseE3 = 0;
// Start of the +/- 5% range for the ZC pulse duration (range 4)
unsigned long pulseS4 = 0;
// End of the +/- 5% range for the ZC pulse duration (range 4)
unsigned long pulseE4 = 0;
// Start of the +/- 5% range for the ZC pulse duration (range 5)
unsigned long pulseS5 = 0;
// End of the +/- 5% range for the ZC pulse duration (range 5)
unsigned long pulseE5 = 0;
// Counter of events off range
int pulseCnt0 = 0;
// Counter of events in the first range
int pulseCnt1 = 0;
// Counter of events in the second range
int pulseCnt2 = 0;
// Counter of events in the thirth range
int pulseCnt3 = 0;
// Counter of events in the fourth range
int pulseCnt4 = 0;
// Counter of events in the fifth range
int pulseCnt5 = 0;
// Current number of ranges used in the detector
int pulseNum = 0;
// Number of tics in a half AC cycle
unsigned long cycleTic = TIC_60_CYCLE;
// Number of tics for the last portion of the half cycle where the TRIAC signal should be off
unsigned long offTic = TIC_60_OFF;


/* -------------------------------------------------------------------------------------------- 
   Dimmer operation and control
   -------------------------------------------------------------------------------------------- */
// Trigger Timers; Trigger TRIACs
unsigned long tmr[INSTANCE_NUM];
// Trigger Timer 3; Turn off triacs
unsigned long tmrOff = 0;
// Trigger Timer 4; Overflow Timer till next cycle
unsigned long tmrOvf = 0;
// Trigger indicator (Indicates how many TRIACs will be triggered)
int trigger = 0;
// Trigger flag for each step
int trig[INSTANCE_NUM];
// Trigger sequence counter
int trigSeq = 0;
// Zero Cross Pulse debounce timer
static unsigned long zcLast = 0;
// Missed Zero Cross Pulse counter
int zcSkip = 0;


/* -------------------------------------------------------------------------------------------- 
   Dimmer Instance definitions and control
   -------------------------------------------------------------------------------------------- */
struct DimmerInst {
  boolean tState;     // State for Light
  boolean tDirection; // Dimming direction
  boolean iStatus;    // LED indicator status
  unsigned long tTmr; // Triger timer
  int tBright;        // Current brightness
  int tPower;         // Target power value (Min - 100)%
  int tValue;         // Current light power value
  int tCntr;          // Light transition speed control
  int tTransition;    // Transition speed
  int tPin;           // Triac Pin
  int iPin;           // Indicator Pin
  int bPin;           // Button Pin
  int eCntr;          // Light repetition counter at the edge of dimmable range
};
DimmerInst inst[INSTANCE_NUM];


/* -------------------------------------------------------------------------------------------- 
   LED Configuration
   -------------------------------------------------------------------------------------------- */
// LED bright level 
int LedBright = 100;
// LED dimm level
int LedDimm = 25;
// LEDs status
boolean ledsState = false;
// LED PWM brightness control
unsigned long LedPwm = 0;
// LED PWM counter overflow control
unsigned long LedOvf = 0;
// LED indicator brightness
boolean LedBrgt = true;

/* -------------------------------------------------------------------------------------------- 
   Debug information
   -------------------------------------------------------------------------------------------- */
#ifdef SAVE_ON_RESET
  // Count number of times the system boots
  int debugBoot = 0;
  // Count number of forced resets
  int debugRst = -1;
  // Reset timer, Wifi Restart, Wifi Restart Total, Alive counter
  int resetDelay1;
  int wifiRstCtr1;
  int wifiRstTtl1;
  unsigned long aliveCounter1;
  // Reset timer, Wifi Restart, Wifi Restart Total, Alive counter
  int resetDelay2;
  int wifiRstCtr2;
  int wifiRstTtl2;
  unsigned long aliveCounter2;
  // Reset timer, Wifi Restart, Wifi Restart Total, Alive counter
  int resetDelay3;
  int wifiRstCtr3;
  int wifiRstTtl3;
  unsigned long aliveCounter3;
#endif


/* -------------------------------------------------------------------------------------------- 
   Objects
   -------------------------------------------------------------------------------------------- */
// Reset detector
DoubleResetDetect drd(DRD_TIMEOUT, DRD_ADDRESS);

// Network Objects
AsyncMqttClient mqttClient;

// Timers
Ticker ticker;


/* --------------------------------------------------------------------------------------------
   Function definitions
   -------------------------------------------------------------------------------------------- */
#ifdef DEBUG_MODE
  void writeDebug (const char* message, int scope = 0, bool keepLine = false);
#endif

void mqttPublish (int tIndex, int instance, const char* payload, bool useBuffer = false);

/** **********************************************************************************************
   Dimmer setup Zero Cross pulse detector

   Detects the Zero Cross pulse (ZC), detects the AC frequency and the duration (width) of the 
   Zero Cross pulse.
   It also calculates the mid point of thre ZC pulse. The mid point is used as the reference to
   trigger TRIACs.
*********************************************************************************************** */
void IRAM_ATTR zeroCrossSetup () {
  ////////////////////////////////////////////////////////////////////////////////
  // Get timer and interrupt state
  ////////////////////////////////////////////////////////////////////////////////
  unsigned long zcT = micros();
  int zcpEdge = digitalRead(PIN_ZCROSS);
  ////////////////////////////////////////////////////////////////////////////////
  // End detection when we have frequency and ZC pulse width
  ////////////////////////////////////////////////////////////////////////////////
  if (pulseH > 0 && frec > 0) {
    inSetup = false;
    return;
  }
  ////////////////////////////////////////////////////////////////////////////////
  // Detect the frequency
  ////////////////////////////////////////////////////////////////////////////////
  // On the RISING edge of the pulse, calculate frequency
  if (zcpEdge == HIGH) {
    unsigned long cycleP = zcT - pulseT;
    // Debounce timer (a pulse too short is noise)
    if(cycleP < 100) {
      return;
    }
    // Set the sequence flag (a FALLING edge must follow a RISING edge
    pulseDbnc = true;
    // Detetc the frequency
    if (frec == 0) {
      // Define Frec.
      if (cycleP > TIM_50_LOW) {
        if (cycleP < TIM_50_HIGH) {
          ++frecCntr50;
        }
      } else if(cycleP > TIM_60_LOW) {
        ++frecCntr60;
      } else {
        ++frecCntr0;
      }
      // Reset detection counters if frequency does not match supported range
      if (frecCntr0 > SETUP_EVENTS) {
        // Reset timers
        frecCntr50 = 0;
        frecCntr60 = 0;
        frecCntr0 = 0;
        #ifdef DEBUG_MODE
          sprintf(tmpMsg0, "Bad frequency tics = %ld", cycleP);
          writeDebug(tmpMsg0, 3);
        #endif
      } 
      // Check if 50Hz has been detected
      else if (frecCntr50 > SETUP_EVENTS) {
        frec = 50;
        frecTic = TIC_50_SEGM;
        cycleTic = TIC_50_CYCLE;
        offTic = TIC_50_OFF;
        #ifdef DEBUG_MODE
          writeDebug("AC frequency = 50", 3);
        #endif
      } 
      // Check if 60Hz has been detected
      else if (frecCntr60 > SETUP_EVENTS) {
        frec = 60;
        frecTic = TIC_60_SEGM;
        cycleTic = TIC_60_CYCLE;
        offTic = TIC_60_OFF;
        #ifdef DEBUG_MODE
          writeDebug("AC frequency = 60", 3);
        #endif
      }
    }
    // Set the ZC pulse timer
    pulseT = zcT;
  }
  ////////////////////////////////////////////////////////////////////////////////
  // Detect ZC pulse duration
  ////////////////////////////////////////////////////////////////////////////////
  if (pulseH != 0) return;
  // On the FALLING edge of the pulse, calculate the pulse duration 
  if (zcpEdge == LOW && pulseDbnc && pulseT > 0) {
    // Get duration
    unsigned long zcW = zcT - pulseT;
    // Debounce timer
    if(zcW < 100) {
      return;
    }
    // Turn off sequence flag until the next RISING edge
    pulseDbnc = false;
    // Count pulse duration events in 5 ranges. This is done because in installations with a 
    // noisy AC signal and faulty or no ground, some of the ZC pulses may have small 
    // variations and may not be detected, as it looks like debounce noise.
    if (pulseNum < 1 || (zcW > pulseS1 && zcW < pulseE1)) {
      if (pulseNum < 1) {
        pulseS1 = zcW * 0.95;
        pulseE1 = zcW * 1.05;
        pulseNum = 1;
      }
      ++pulseCnt1;
    } else if(pulseNum < 2 || (zcW > pulseS2 && zcW < pulseE2)) {
      if (pulseNum < 2) {
        pulseS2 = zcW * 0.95;
        pulseE2 = zcW * 1.05;
        pulseNum = 2;
      }
      ++pulseCnt2;
    } else if(pulseNum < 3 || (zcW > pulseS3 && zcW < pulseE3)) {
      if (pulseNum < 3) {
        pulseS3 = zcW * 0.95;
        pulseE3 = zcW * 1.05;
        pulseNum = 3;
      }
      ++pulseCnt3;
    } else if(pulseNum < 4 || (zcW > pulseS4 && zcW < pulseE4)) {
      if (pulseNum < 4) {
        pulseS4 = zcW * 0.95;
        pulseE4 = zcW * 1.05;
        pulseNum = 4;
      }
      ++pulseCnt4;
    } else if(pulseNum < 5 || (zcW > pulseS5 && zcW < pulseE5)) {
      if (pulseNum < 5) {
        pulseS5 = zcW * 0.95;
        pulseE5 = zcW * 1.05;
        pulseNum = 5;
      }
      ++pulseCnt5;
    } else {
      ++pulseCnt0;
    }
    // If most pulses are detected as noise, restart detection
    if (pulseCnt0 > SETUP_EVENTS) {
      pulseNum = 0;
      pulseCnt0 = 0;
      pulseCnt1 = 0;
      pulseCnt2 = 0;
      pulseCnt3 = 0;
      pulseCnt4 = 0;
      pulseCnt5 = 0;
      #ifdef DEBUG_MODE
        sprintf(tmpMsg0, "Out of range ZC pulse = %ld ", zcW);
        writeDebug(tmpMsg0, 3);
      #endif
    } 
    // Check if enough pulses have been detected
    else if (pulseCnt1 > SETUP_EVENTS || pulseCnt2 > SETUP_EVENTS || pulseCnt3 > SETUP_EVENTS || pulseCnt4 > SETUP_EVENTS || pulseCnt5 > SETUP_EVENTS) {
      // Get the mid of the ZC pulse with the mean of the width
      pulseH = (zcW / 2 + (zcW * .1));
      #ifdef DEBUG_MODE
        sprintf(tmpMsg0, "ZC pulse width detected = %ld ", zcW);
        writeDebug(tmpMsg0, 3);
      #endif
    }
  }
}

/** **********************************************************************************************
   Zero Cross signal detector

   Detects the Zero Cross signal and resets the cycle timers to trigger the TRIACs.
*********************************************************************************************** */
void IRAM_ATTR zeroCross () {
  ////////////////////////////////////////////////////////////////////////////////
  // Debounce
  ////////////////////////////////////////////////////////////////////////////////
  unsigned long now = micros();
  if((unsigned long)(now - zcLast) < ZC_DEBOUNCE) return;
  ////////////////////////////////////////////////////////////////////////////////
  // Calculate TRIAC triggers
  ////////////////////////////////////////////////////////////////////////////////
  // Stop other interruptions during ZC calculations
  noInterrupts();
  zcLast = now;
  // Reset the missed ZC pulse counter
  zcSkip = 0;
  // Loop variables
  bool chg = false;
  bool cht = false;
  unsigned long ttm[INSTANCE_NUM];
  int tt[INSTANCE_NUM];
  int bt = 1;
  int i;
  ////////////////////////////////////////////////////////////////////////////////
  // Calc Triac
  ////////////////////////////////////////////////////////////////////////////////
  for (i = 0; i < INSTANCE_NUM; i++) {
    cht = false;
    // For transition 0, jump to the target brightness
    if (inst[i].tTransition == 0) {
      if (inst[i].tState) {
        if (inst[i].tValue != inst[i].tBright || inst[i].tPower == 0) {
          inst[i].tValue = inst[i].tBright;
          cht = true;
        }
      } else if (inst[i].tValue != 0 || inst[i].tPower != 0) {
        inst[i].tValue = 0;
        cht = true;
      }
    }
    // The current value is higher than the target brightness or the light is off
    else if (inst[i].tValue > inst[i].tBright || !inst[i].tState) {
      // Fade out
      if (inst[i].tValue > 0) {
        --inst[i].tCntr;
        if (inst[i].tCntr <= 0) {
          inst[i].tCntr = inst[i].tTransition;
          --inst[i].tValue;
          cht = true;
        }
      } else if (inst[i].tPower != 0) {
        inst[i].tPower = 0;
        cht = true;
      }
    }
    // The current value is lower than the target power
    else if (inst[i].tValue < inst[i].tBright) {
      // Fade in
      --inst[i].tCntr;
      if (inst[i].tCntr <= 0) {
        inst[i].tCntr = inst[i].tTransition;
        ++inst[i].tValue;
        cht = true;
      }
    } else if (inst[i].tPower == 0) {
      cht = true;
    }
    if (cht) {
      if (!inst[i].tState && inst[i].tValue <= 0) {
        inst[i].tPower = 0;
        inst[i].tTmr = SEGMENTS * frecTic + pulseH;
      } else {
        inst[i].tPower = getPower(inst[i].tValue, i);
        inst[i].tTmr = powerToSegment(inst[i].tPower) * frecTic + pulseH;
      }
      chg = true;
    }
    // Calculate timers
    ttm[i] = inst[i].tTmr;
    if (inst[i].tPower >= dconfig.iMinPow[i]) {
      tt[i] = bt;
    } else {
      tt[i] = 0;
    }
    bt <<= 1;
  }
  if (chg) {
    ////////////////////////////////////////////////////////////////////////////////
    // Sort
    ////////////////////////////////////////////////////////////////////////////////
    unsigned long m;
    int n;
    int j;
    for (i = 1; i < INSTANCE_NUM; i++) {
      m = ttm[i];
      n = tt[i];
      for (j = i - 1; (j >= 0) && (m < ttm[j]); j--) {
        ttm[j + 1] = ttm[j];
        tt[j + 1] = tt[j];
      }
      ttm[j + 1] = m;
      tt[j + 1] = n;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Calc duplicates
    ////////////////////////////////////////////////////////////////////////////////
    for (int i = (INSTANCE_NUM - 1); i > 0; i--) {
      if (ttm[i] != ttm[i - 1]) continue;
      tt[i] |= tt[i - 1];
      tt[i - 1] = 0;
    }
    ////////////////////////////////////////////////////////////////////////////////
    // Calc timers
    ////////////////////////////////////////////////////////////////////////////////
    unsigned long ltm = 0;
    trigger = 0;
    for (i = 0; i < INSTANCE_NUM; i++) {
      if (tt[i] > 0) {
        tmr[trigger] = ttm[i] - ltm;
        trig[trigger] = tt[i];
        ltm = ttm[i];
        ++trigger;
      }
    }
    for (i = trigger; i < INSTANCE_NUM; i++) {
      trig[i] = 0;
    }
    if (trigger > 0) {
      tmrOff = cycleTic - ltm - offTic;
    } else {
      tmr[0] = pulseH + frecTic;
      tmrOff = cycleTic - tmr[0] - offTic;
    }
    tmrOvf = offTic + tmr[0];
  }
  ////////////////////////////////////////////////////////////////////////////////
  // Restart interruptions and set the timer for the first step
  ////////////////////////////////////////////////////////////////////////////////
  trigSeq = 0;
  interrupts();
  timer1_write(tmr[0]);
}

/** **********************************************************************************************
   Trigger TRIACs

   Timer interrupt used to trigger TRIACs.
   Each half cycle of the AC signal is divided in steps:
   1 - Time between the middle of the Zero Cross pulse and the first set of TRIACs. If all 
       TRIACs have the same power level, the will be triggered.
   2 - Trigger the second set of TRIACs. Continue Triggering TRIACs.
   3 - Turn off trigger signal for all TRIACs / Sets the timer for the next half cycle.
   4 - Time between the Off time and the first set of TRIACs in the next half cycle (Step 1).
       This allows to continue even if a Zero Cross pulse is not detected, preventing flickering.
       Timer 4 depends on the AC frequency.
*********************************************************************************************** */
void IRAM_ATTR triggerTriacs () {
  // If lights are off, turn off TRIAC signals
  for (int i = 0; i < INSTANCE_NUM; i++) {
    if (inst[i].tPower > 0) continue;
    digitalWrite(inst[i].tPin, LOW);
  }
  // If all lights are off, we are done here.
  if (trigger == 0) return;
  // Last Step (Set timer for the next AC cycle)
  if (trigSeq == trigger) {
    // Set timer for step 1 in the next cycle
    timer1_write(tmrOvf);
    trigSeq = 0;
    // Turn off TRIACs
    for (int i = 0; i < INSTANCE_NUM; i++) {
      if (inst[i].tPower == 0) continue;
      digitalWrite(inst[i].tPin, LOW);
    }
  } 
  // Trigger TRIACs and set next timer
  else {
    int ctt = trigSeq;
    // Increase the trigger sequence
    ++trigSeq;
    // Trigger TRIACs
    int bt = 1;
    for (int i = 0; i < INSTANCE_NUM; i++) {
      if ((trig[ctt] & bt) > 0) {
        digitalWrite(inst[i].tPin, HIGH);
      }
      bt <<= 1;
    }
    // Set next timer
    if (trigSeq == trigger) {
      timer1_write(tmrOff);
    } else {
      timer1_write(tmr[trigSeq]);
    }
  }
  // Count the number of times this function is executed
  ++zcSkip;
  // If the counter is not cleared for a few cycles (the ZC pulse has not been detected)
  // turn off TRIACs
  if (zcSkip > 9) {
    trigger = 0;
    for (int i = 0; i < INSTANCE_NUM; i++) {
      inst[i].tPower = 0;
    }
  }
}

/** **********************************************************************************************
   Converts power % to trigger segment
   
   @param v Power value (0-100).
*********************************************************************************************** */
int powerToSegment (int v) {
  if (v < 53) {
    if (v < 24) {
      if (v <= 10) {
        if (v <= 0) return SEGMENTS;
        // Minimum Power is 10%
        return 105;
      }
      if (v < 16) {
        if (v < 14) {
          if (v < 12) {
            return 104;
          }
          return 103;
        }
        if (v < 15) {
          return 102;
        }
        return 101;
      }
      if (v < 20) {
        if (v < 18) {
          if (v < 17) {
            return 100;
          }
          return 99;
        }
        return 98;
      }
      if (v < 22) {
        if (v < 21) {
          return 97;
        }
        return 96;
      }
      return 95;
    }
    if (v < 39) {
      if (v < 32) {
        if (v < 28) {
          if (v < 26) {
            if (v < 25) {
              return 94;
            }
            return 93;
          }
          return 92;
        }
        if (v < 30) {
          if (v < 29) {
            return 91;
          }
          return 90;
        }
        return 89;
      }
      if (v < 36) {
        if (v < 35) {
          if (v < 33) {
            return 88;
          }
          return 87;
        }
        return 86;
      }
      if (v < 38) {
        return 85;
      }
      return 84;
    }
    if (v < 46) {
      if (v < 43) {
        if (v < 42) {
          if (v < 40) {
            return 83;
          }
          return 82;
        }
        return 81;
      }
      if (v < 45) {
        return 80;
      }
      return 79;
    }
    if (v < 50) {
      if (v < 49) {
        if (v < 48) {
          return 78;
        }
        return 77;
      }
      return 76;
    }
    if (v < 52) {
      return 75;
    }
    return 74;
  }
  //
  if (v < 81) {
    if (v < 69) {
      if (v < 61) {
        if (v < 57) {
          if (v < 56) {
            if (v < 55) {
              return 73;
            }
            return 72;
          }
          return 71;
        }
        if (v < 60) {
          if (v < 59) {
            return 70;
          }
          return 69;
        }
        return 68;
      }
      if (v < 65) {
        if (v < 64) {
          if (v < 63) {
            return 67;
          }
          return 66;
        }
        return 65;
      }
      if (v < 68) {
        if (v < 66) {
          return 64;
        }
        return 63;
      }
      return 62;
    }
    if (v < 76) {
      if (v < 72) {
        if (v < 71) {
          if (v < 70) {
            return 61;
          }
          return 60;
        }
        return 59;
      }
      if (v < 75) {
        if (v < 74) {
          return 58;
        }
        return 57;
      }
      return 56;
    }
    if (v < 79) {
      if (v < 78) {
        if (v < 77) {
          return 55;
        }
        return 54;
      }
      return 53;
    }
    if (v < 80) {
      return 52;
    }
    return 51;
  }
  if (v < 91) {
    if (v < 86) {
      if (v < 84) {
        if (v < 83) {
          if (v < 82) {
            return 50;
          }
          return 49;
        }
        return 48;
      }
      if (v < 85) {
        return 47;
      }
      return 46;
    }
    if (v < 89) {
      if (v < 88) {
        if (v < 87) {
          return 44;
        }
        return 43;
      }
      return 42;
    }
    if (v < 90) {
      return 40;
    }
    return 39;
  }
  if (v < 96) {
    if (v < 94) {
      if (v < 93) {
        if (v < 92) {
          return 37;
        }
        return 36;
      }
      return 34;
    }
    if (v < 95) {
      return 32;
    }
    return 30;
  }
  if (v < 99) {
    if (v < 98) {
      if (v < 97) {
        return 27;
      }
      return 24;
    }
    return 21;
  }
  if (v < 100) {
    return 14;
  }
  return 1;
}

/** **********************************************************************************************
   Get the Power %, from the brightness level
   
   @param bright Brightness level (0-100).
   @param idx Light index [0|1].
*********************************************************************************************** */
int getPower (int bright, int idx) {
  float sg = dconfig.iMinPow[idx];
  int res = bright * (100 - sg) / 100 + sg;
  return res;
}

#ifdef DEBUG_MODE
/** **********************************************************************************************
   Write a message to the Serial port

   @param message Message
   @param scope Scope.
     - 0 = Plain (Default)
     - 1 = System debug
     - 2 = Network
     - 3 = Dimmer
     - 4 = MQTT
   @param keepLine Set to "true" to print on the same line
*********************************************************************************************** */
void writeDebug (const char* message, int scope, bool keepLine) {
  if (scope == 1) {
    Serial.print("SYS: ");
  } else if (scope == 2) {
    Serial.print("NET: ");
  } else if (scope == 3) {
    Serial.print("DIM: ");
  } else if (scope == 4) {
    Serial.print("MQTT: ");
  }
  if (keepLine) {
    Serial.print(message);
  } else {
    Serial.println(message);
  }
}
#endif

/** **********************************************************************************************
   System timer

   Keep track of running time and execute periodic activities every second (aprox.)
*********************************************************************************************** */
void systemTimer () {
  /////////////////////////////////////////////
  // Validate second timer and overflow states
  /////////////////////////////////////////////
  unsigned long cTmr = millis();
  if (sysOvfFlg && cTmr >= sysOvf) return;
  sysOvfFlg = false;
  if (cTmr < sysMillis && cTmr >= sysOvf) {
    sysOvf = cTmr;
    return;
  }
  sysMillis += SYS_TIMER;
  // Check overflow
  if (sysMillis < cTmr && sysMillis > 2000000) {
    sysOvfFlg = true;
  }
  sysOvf = cTmr;
  /////////////////////////////////////////////
  // One second passed
  ++aliveCounter;
  /////////////////////////////////////////////
  // Count seconds without network connection
  ++resetDelay;
  /////////////////////////////////////////////
  // Keep alive message to broker
  ++keepAlive;
  if (keepAlive >= dconfig.keepAlive) {
    keepAlive = 0;
    mqttPublish(0, 0, MQTT_MSG_ALIVE);
  }
  /////////////////////////////////////////////
  // Re-subscribe at MQTT_RESUBSCRIBE intervals (6 min. arox.)
  ++reSubscribe;
  if (reSubscribe >= MQTT_RESUBSCRIBE) {
    #ifdef DEBUG_MODE
      writeDebug("Start periodic re-subscription", 1);
    #endif
    mqttSubscribe();
  }
  /////////////////////////////////////////////
  // Go Live sequence go off after GO_LIVE_DURATION seconds (1 minute default)
  if (goLive == 2 && aliveCounter > GO_LIVE_DURATION) {
    goLiveOff();
  }
  // Go Live unsubscriptions expire
  if (goLiveSwitch > 0 && aliveCounter > (GO_LIVE_DURATION + 30)) {
    goLiveSwitch = 0;
  }
  /////////////////////////////////////////////
  // Define indicators status
  LedBrgt = false;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    if (inst[i].tState) {
      LedBrgt = true;
      break;
    }
  }
  /////////////////////////////////////////////
  // Tic packets in transit
  mqttTicTransit();
  /////////////////////////////////////////////
  // Trigger Auto-discovery
  #ifdef MQTT_DISCOVERY
    mqttDiscovery();
  #endif
}

/** **********************************************************************************************
   Define the device ID
*********************************************************************************************** */
void setDeviceId () {
  byte mac[6];
  WiFi.macAddress(mac);
  sprintf(deviceId, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  #ifdef DEBUG_MODE
    sprintf(tmpMsg0, "Device setup: %s / %s", INST_TYPE, deviceId);
    writeDebug(tmpMsg0, 1);
  #endif
}

/** **********************************************************************************************
   Flash LED indicators
*********************************************************************************************** */
void flashLed () {
  ledsState = !ledsState;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    digitalWrite(inst[i].iPin, ((ledsState && inst[i].iStatus) ? HIGH : LOW) );
  }
}

/** **********************************************************************************************
   Handle LED indicators
*********************************************************************************************** */
void handleLed () {
  unsigned long lTmr = micros();
  if (lTmr > LedPwm || lTmr < LedOvf) {
    flashLed();
    // LEDs turned ON
    if (ledsState) {
      // If the light(s) are on, keep the LEDs on for LedBright
      // otherwise for LedDimm us
      LedPwm = lTmr + ((LedBrgt) ? LedBright : LedDimm);
    }
    // LED turned OFF
    else {
      // Keep the LEDs off for the rest of the PWM cycle
      LedPwm = lTmr + (LED_PWM - ((LedBrgt) ? LedBright : LedDimm));
    }
  }
  LedOvf = lTmr;
}

/** **********************************************************************************************
   Start WiFi
*********************************************************************************************** */
void wifiBegin () {
  if(!wifiSetup || (unsigned long)(millis() - wifiTimer) > WIFI_RECONNECT) {
    #ifdef OTA_UPDATES
      WiFi.mode(WIFI_STA);
    #endif
    WiFi.begin();
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    wifiTimer = millis();
    #ifdef DEBUG_MODE
      writeDebug("Connect to WiFi: ", 2, true);
      String dvSsid = WiFi.SSID();
      writeDebug(dvSsid.c_str());
    #endif
  }
  wifiStatus = false;
  wifiSetup = true;
}

/** **********************************************************************************************
   Check network connectivity
*********************************************************************************************** */
boolean netCheck () {
  if (!wifiSetup) { return false; };
  //////////////////////////////////////////
  // Check WiFi fail counters
  //////////////////////////////////////////
  if (wifiRstCtr > WIFI_RESTART_REBOOT || wifiConnCtr > WIFI_CONNECT_REBOOT) {
    // Reboot only if a network connection has been detected since boot and the lights are off
    if (canReboot) {
      bool rbt = true;
      for (int i = 0; i < INSTANCE_NUM; i++) {
        if (inst[i].tState) rbt = false; 
      }
      if (rbt) {
        #ifdef SAVE_ON_RESET
          // Save the parameters that triggered the reset process
          saveStatus();
        #endif
        // Reboot
        ESP.restart();
      }
    }
  }
  //////////////////////////////////////////
  // Check MQTT fail counters
  //////////////////////////////////////////
  if (mqttEventCtr > MQTT_MAX_EVENTS) {
    #ifdef DEBUG_MODE
      sprintf(tmpMsg0, "Broker is not responding. Reset network connections. %d / %d", wifiRstCtr, wifiRstTtl);
      writeDebug(tmpMsg0, 2);
    #endif
    // Disconnect MQTT
    mqttDisconnect();
    // Restart WiFi connection
    wifiStatus = false;
    WiFi.reconnect();
    ++wifiRstCtr;
    ++wifiRstTtl;
    return false;
  }
  //////////////////////////////////////////
  // Check WiFi Connection
  //////////////////////////////////////////
  // Not connected
  if (!WiFi.isConnected()) {
    // Status is Up, so connection just fell down
    if (wifiStatus) {
      // Change status
      wifiStatus = false;
      // Disconnect MQTT
      mqttDisconnect();
      #ifdef DEBUG_MODE
        writeDebug("WiFi is disconnected.", 2);
      #endif
    }
    // Connection is down
    return false;
  }
  // Connected, but status is Down
  if (!wifiStatus) {
    // Change status
    wifiStatus = true;
    // Force a MQTT re-connection
    mqttDisconnect();
    // Force an immediate visit to netConnect()
    netFirst = true;
  }
  // Reset consecutive connection counter
  wifiConnCtr = 0;
  //////////////////////////////////////////
  // Check MQTT connectivity
  //////////////////////////////////////////
  // Not connected
  if (!mqttClient.connected()) {
    // Status is Up, so connection just fell down
    if (mqttStatus) {
      // Change status
      mqttStatus = false;
      // Clear timers/counters
      mqttTmr0 = 0;
      mqttEventCtr = 0;
      #ifdef DEBUG_MODE
        writeDebug("MQTT is disconnected.", 4);
      #endif
    }
    // MQTT Connection is down
    return false;
  }
  // MQTT is connected, but status is Down
  if (!mqttStatus) {
    // Status is set on the Connect Callback function, so this staus is not expected
    ++mqttEventCtr;
    #ifdef DEBUG_MODE
      writeDebug("Ilegal MQTT Status.", 4);
    #endif
    return false;
  }
  // Check MQTT busy timeout
  if (mqttBusy0 > 0) {
    // If busy for long, clear counters
    if((unsigned long)(millis() - mqttTmr0) > MQTT_BUSY_TO) {
      mqttBusy0 = 0;
      ++mqttEventCtr;
    }
  }
  // All connections are OK
  wifiRstCtr = 0;
  // Once WiFi and MQTT connections are detected once, we allow a forced reboot
  canReboot = true;
  return true;
}

/** **********************************************************************************************
   Connect to the MQTT Broker
*********************************************************************************************** */
void netConnect () {
  if (!wifiSetup) { return; };
  if (!wifiStatus) {
    // Make sure connection is down before calling WiFi connector
    if (WiFi.status() != WL_CONNECTED) {
      #ifdef DEBUG_MODE
        sprintf(tmpMsg0, "Call WiFi Setup: %d, ERR: %d, RST: %d", WiFi.status(), wifiRstCtr, wifiRstTtl);
        writeDebug(tmpMsg0, 2);
      #endif
      wifiBegin();
      // Count the number of simultaneous connection attemps
      ++wifiConnCtr;
    }
    return;
  }
  if (mqttConnecting) {
    mqttConnecting = false;
    return;
  }
  mqttBegin();
  #ifdef DEBUG_MODE
    if (!mqttStatus) {
      writeDebug("Connecting to Broker: ", 4, true);
      writeDebug(dconfig.myId, 0, true);
      writeDebug(" / ", 0, true);
      writeDebug(dconfig.mqttuser, 0, true);
      writeDebug(" @ ", 0, true);
      writeDebug(dconfig.mqttbroker, 0, true);
      sprintf(tmpMsg0, " :: %d ", mqttEventCtr);
      writeDebug(tmpMsg0);
    } else {
      writeDebug("Call broker connection. ", 4);
    }
  #endif
  // Count the number of consecutive connection attemps
  ++mqttEventCtr;
  mqttConnecting = true;
  mqttClient.connect();
}

/** **********************************************************************************************
   Turn Off go live cycle
*********************************************************************************************** */
void goLiveOff () {
  if (goLive == 0) return;
  #ifdef DEBUG_MODE
    writeDebug("Go Live sequence off.", 3);
  #endif
  if (goLive == 2) {
    mqttUnSubscribeRetain();
  }
  goLive = 0;
}

/** **********************************************************************************************
   MQTT Topics

   Define MQTT topics
*********************************************************************************************** */
void mqttDefineTopics () {
  // Device STATUS topic
  sprintf(tmpMsg, "%s/%s/%s/state", dconfig.namespc, INST_TYPE, dconfig.myId);
  strlcpy(topicState, tmpMsg, sizeof(topicState));
  // Device SET topic
  sprintf(tmpMsg, "%s/%s/%s/set", dconfig.namespc, INST_TYPE, dconfig.myId);
  strlcpy(topicSet, tmpMsg, sizeof(topicSet));
  #ifdef DBL_CLICK
    // Device ACTION topic
    //sprintf(tmpMsg, "%s/%s/action", dconfig.namespc, deviceId);
    sprintf(tmpMsg, "%s/%s/action", dconfig.namespc, dconfig.myId);
    strlcpy(topicAction, tmpMsg, sizeof(topicAction));
  #endif
  // Instances
  for (int i = 0; i < INSTANCE_NUM; i++) {
    // Define the instance Unique ID
    sprintf(tmpMsg0, "U%s%d", deviceId, (i + 1));
    strlcpy(mqttTopics[i].uid, tmpMsg0, sizeof(mqttTopics[i].uid));
    // Define the instance Name
    if (strlen(dconfig.iName[i]) > 0) {
      strlcpy(mqttTopics[i].uname, dconfig.iName[i], sizeof(mqttTopics[i].uname));
    } else {
      sprintf(tmpMsg0, "N%s%d", deviceId, (i + 1));
      strlcpy(mqttTopics[i].uname, tmpMsg0, sizeof(mqttTopics[i].uname));
    }
    // Instance STATE topic
    if (snprintf(tmpMsg, sizeof(tmpMsg), "%s/%s/%s/state", dconfig.namespc, INST_TYPE, mqttTopics[i].uname) >= 0) {
      strlcpy(mqttTopics[i].state, tmpMsg, sizeof(mqttTopics[i].state));
    }
    // Instance SET topic
    if (snprintf(tmpMsg, sizeof(tmpMsg), "%s/%s/%s/set", dconfig.namespc, INST_TYPE, mqttTopics[i].uname) >= 0) {
      strlcpy(mqttTopics[i].set, tmpMsg, sizeof(mqttTopics[i].set));
    }
  }
}

/** **********************************************************************************************
   MQTT Setup
*********************************************************************************************** */
void mqttBegin () {
  if (mqttSetup) { return; }
  for (int i = 0; i < IN_TRANSIT; i++) {
    transit[i] = 0;
    transitTic[i] = 0;
  }
  #ifdef MQTT_DISCOVERY
    mqttReDiscover();
  #endif
  mqttClient.onConnect(mqttConnectCallback);
  mqttClient.onDisconnect(mqttDisconnectCallback);
  mqttClient.onSubscribe(mqttSubscribeCallback);
  mqttClient.onUnsubscribe(mqttUnsubscribeCallback);
  mqttClient.onMessage(mqttMessageCallback);
  mqttClient.onPublish(mqttPublishCallback);
  mqttClient.setServer(dconfig.mqttbroker, dconfig.mqttport);
  mqttClient.setCredentials(dconfig.mqttuser, dconfig.mqttpass);
  mqttClient.setClientId(dconfig.myId);
  mqttSetup = true;
  #ifdef DEBUG_MODE
    writeDebug("Setup Broker: ", 4, true);
    writeDebug(dconfig.mqttbroker, 0, true);
    writeDebug(" / ", 0, true);
    char prtnm[6];
    sprintf(prtnm, "%d", dconfig.mqttport);
    writeDebug(prtnm);
  #endif
}

/** **********************************************************************************************
   MQTT connection completed
   
   @param sessionPresent Flag indicating a session is present.
*********************************************************************************************** */
void mqttConnectCallback(bool sessionPresent) {
  if (mqttConnecting) {
    mqttStatus = true;
  }
  mqttConnecting = false;
  mqttEventCtr = 0;
  #ifdef DEBUG_MODE
    if (sessionPresent) {
      writeDebug("Connected to MQTT, with session.", 4);
    } else {
      writeDebug("Connected to MQTT.", 4);
    }
    if (goLive == 2) {
      writeDebug("Go Live sequence start, with retain turned ON.", 3);
    } else if (goLive == 1) {
      writeDebug("Go Live sequence start, with retain turned OFF.", 3);
    }
  #endif
  if (goLive == 2) {
    mqttSubscribeRetain();
  } else {
    mqttSubscribe();
  }
}

/** **********************************************************************************************
   MQTT disconection detected
   
   @param reason Disconnect reason
*********************************************************************************************** */
void mqttDisconnectCallback (AsyncMqttClientDisconnectReason reason) {
  mqttConnecting = false;
  mqttStatus = false;
  #ifdef DEBUG_MODE
    writeDebug("MQTT Disconnected. Reason:", 4);
    Serial.println((uint8_t)reason);
  #endif
}

/** **********************************************************************************************
   MQTT Subscription completed
   
   @param packetId Id number of the message
   @param qos QOS obtained from broker
*********************************************************************************************** */
void mqttSubscribeCallback (uint16_t packetId, uint8_t qos) {
  if (mqttBusy0 > 0) {
    --mqttBusy0;
  }
  if (mqttBusy0 == 0) {
    // Request last known state on first go-live
    if (goLive == 1) {
      goLive = 0;
      mqttPublish(0, 0, MQTT_MSG_GOLIVE);
    } else {
      mqttPublish(0, 0, MQTT_MSG_ALIVE);
      if (goLive == 0) {
        mqttPublishStatus(-1);
      }
    }
  }
  #ifdef DEBUG_MODE
    writeDebug("Subscribed.", 4);
  #endif
}

/** **********************************************************************************************
   MQTT Un-Subscription detected
   
   @param packetId Id number of the published message
*********************************************************************************************** */
void mqttUnsubscribeCallback (uint16_t packetId) {
  if (goLiveSwitch > 0) {
    --goLiveSwitch;
  }
  if (goLiveSwitch > 0) return;
  mqttSubscribe();
}

/** **********************************************************************************************
   MQTT Publish completed
   
   @param packetId Id number of the published message
*********************************************************************************************** */
void mqttPublishCallback(uint16_t packetId) {
  #ifdef DEBUG_MODE
    writeDebug("Published. ", 4, true);
    Serial.println(packetId);
  #endif
  mqttUnTransit(packetId);
  mqttEventCtr = 0;
}

/** **********************************************************************************************
   MQTT Register a packet in transit
   
   @param pId Packet ID.
*********************************************************************************************** */
void mqttTransit (uint16_t pId) {
  int cnt = 0;
  for (int i = 0; i < IN_TRANSIT; i++) {
    if (transit[i] > 0) {
      ++cnt;
      continue;
    }
    transit[i] = pId;
    transitTic[i] = 0;
    ++cnt;
    break;
  }
  if (cnt >= IN_TRANSIT) {
    transitBussy = true;
  }
}

/** **********************************************************************************************
   MQTT Un-register a packet in transit
   
   @param pId Packet ID.
*********************************************************************************************** */
void mqttUnTransit (uint16_t pId) {
  for (int i = 0; i < IN_TRANSIT; i++) {
    if (transit[i] != pId) continue;
    transit[i] = 0;
    transitTic[i] = 0;
    transitBussy = false;
  }
}

/** **********************************************************************************************
   MQTT Tic packets in transit
*********************************************************************************************** */
void mqttTicTransit () {
  for (int i = 0; i < IN_TRANSIT; i++) {
    if (transit[i] == 0) continue;
    ++transitTic[i];
    if (transitTic[i] > 5) {
      transit[i] = 0;
      transitTic[i] = 0;
      transitBussy = false;
    }
  }
}

/** **********************************************************************************************
   Force a disconnect from MQTT services
*********************************************************************************************** */
void mqttDisconnect () {
  if (mqttClient.connected()) {
    mqttClient.disconnect(true);
  }
  mqttStatus = false;
  mqttConnecting = false;
  mqttTmr0 = 0;
  mqttEventCtr = 0;
}

/** **********************************************************************************************
   MQTT subscribe to topics
*********************************************************************************************** */
void mqttSubscribe () {
  if (!mqttStatus) return;
  reSubscribe = 0;
  unsigned long cTmr = millis();
  #ifdef MQTT_CONFIG
    mqttClient.subscribe(topicSet, MQTT_SUB_QOS);
    ++mqttBusy0;
    mqttTmr0 = cTmr;
    #ifdef DEBUG_MODE
      writeDebug("Subscribe to Device SET topic: ", 4, true);
      writeDebug(topicSet);
    #endif
  #endif
  for (int i = 0; i < INSTANCE_NUM; i++) {
    mqttClient.subscribe(mqttTopics[i].set, MQTT_SUB_QOS);
    ++mqttBusy0;
    mqttTmr0 = cTmr;
    #ifdef DEBUG_MODE
      writeDebug("Subscribe to Light SET topic: ", 4, true);
      writeDebug(mqttTopics[i].set);
    #endif
  }
}

/** **********************************************************************************************
   MQTT subscribe to retained topics
*********************************************************************************************** */
void mqttSubscribeRetain () {
  if (!mqttStatus) return;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    mqttClient.subscribe(mqttTopics[i].state, MQTT_SUB_QOS);
    ++mqttBusy0;
    mqttTmr0 = millis();
    #ifdef DEBUG_MODE
      writeDebug("Subscribe to Light STATE topic: ", 4, true);
      writeDebug(mqttTopics[i].state);
    #endif
  }
}

/** **********************************************************************************************
   MQTT Un-subscribe from retained topics
*********************************************************************************************** */
void mqttUnSubscribeRetain () {
  if (!mqttStatus) return;
  goLiveSwitch = 0;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    ++goLiveSwitch;
    mqttClient.unsubscribe(mqttTopics[i].state);
    #ifdef DEBUG_MODE
      writeDebug("Un-subscribe to Light STATE topic: ", 4, true);
      writeDebug(mqttTopics[i].state);
    #endif
  }
}

/** **********************************************************************************************
   Publish a message to MQTT Broker

   @param tIndex Topic index (0 = device state; 1 = instance state; 2 = doubleClick)
   @param instance Instance index
   @param payload MQTT Payload
   @param useBuffer Use Buffer, if MQTT is ofline in non-blocking mode
*********************************************************************************************** */
void mqttPublish (int tIndex, int instance, const char* payload, bool useBuffer) {
  char topic[81];
  bool ret = dconfig.retain;
  if (tIndex == 0) {
    strlcpy(topic, topicState, sizeof(topic));
    ret = false;
  } 
  else if (tIndex == 1) {
    strlcpy(topic, mqttTopics[instance].state, sizeof(topic));
  }
  #ifdef DBL_CLICK
    else if (tIndex == 2) {
      strlcpy(topic, topicAction, sizeof(topic));
      ret = false;
    }
  #endif
  if (mqttStatus && !transitBussy && mqttBusy0 == 0) {
    mqttTransit(mqttClient.publish(topic, MQTT_PUB_QOS, ret, payload));
    ++mqttEventCtr;
    #ifdef DEBUG_MODE
      writeDebug("Publish on topic: ", 4, true);
      writeDebug(topic, 0, true);
      sprintf(tmpMsg0, " :: %d ", mqttEventCtr);
      writeDebug(tmpMsg0);
      writeDebug(payload);
    #endif
  } 
  else if (useBuffer) {
    // Save message in Buffer for later processing
    strlcpy(buffTop[buffStat], topic, sizeof(buffTop[buffStat]));
    strlcpy(buffMsg[buffStat], payload, sizeof(buffMsg[buffStat]));
    buffRet[buffStat] = ret;
    #ifdef DEBUG_MODE
      sprintf(tmpMsg0, "Buffer message %d for topic ", buffPub);
      writeDebug(tmpMsg0, 4, true);
      writeDebug(topic);
      writeDebug(payload);
    #endif
    if ((buffPub == (buffStat + 1)) || (buffStat == buffLim && buffPub == 0)) {
      buffPub++;
      if (buffPub > buffLim) {
        buffPub = 0;
      }
    }
    buffStat++;
    if (buffStat > buffLim) {
      buffStat = 0;
    }
  }
}

/** **********************************************************************************************
   Publish pending messages in the buffer
*********************************************************************************************** */
void mqttPublishBuffer () {
  if (buffStat == buffPub) return;
  if (!mqttStatus || transitBussy) return;
  mqttTransit(mqttClient.publish(buffTop[buffPub], MQTT_PUB_QOS, buffRet[buffPub], buffMsg[buffPub]));
  ++mqttEventCtr;
  #ifdef DEBUG_MODE
    sprintf(tmpMsg0, "Publish buffer %d for topic", buffPub);
    writeDebug(tmpMsg0, 4, true);
    writeDebug(buffTop[buffPub], 0, true);
    sprintf(tmpMsg0, " :: %d ", mqttEventCtr);
    writeDebug(tmpMsg0);
    writeDebug(buffMsg[buffPub]);
  #endif
  buffPub++;
  if (buffPub > buffLim) {
    buffPub = 0;
  }
}

/** **********************************************************************************************
   Publish light status

   @param index Light index [-1|0|1]. -1 indicates all instances.
*********************************************************************************************** */
void mqttPublishStatus (int index) {
  for (int i = 0; i < INSTANCE_NUM; i++) {
    if (index != i && index != -1) continue;
    sprintf(tmpMsg, "{\"state\":\"%s\",\"brightness\":%d,\"power\":%d,\"indicator\":\"%s\"}", ((inst[i].tState) ? "ON" : "OFF"), inst[i].tBright, inst[i].tPower, ((inst[i].iStatus) ? "ON" : "OFF"));
    mqttPublish(1, i, tmpMsg, true);
  }
}

#ifdef MQTT_DISCOVERY
/** **********************************************************************************************
   Reset Discovery
*********************************************************************************************** */
void mqttReDiscover () {
  discovered = false;
  discInst = 0;
  #ifdef DBL_CLICK
    discActn = 0;
  #endif
}

/** **********************************************************************************************
   Publish auto discovery message
   https://www.home-assistant.io/integrations/device_trigger.mqtt/#configuration-variables
*********************************************************************************************** */
void mqttDiscovery () {
  if (discovered) return;
  // Check network bussy status
  if (!mqttStatus || transitBussy || mqttBusy0 > 0) return;
  // Finished!
  if (discInst > INSTANCE_NUM) {
    discovered = true;
    return;
  }
  // Discover device
  char tmpMsgN[471];
  char ltr[2] = "@";
  int lgt = discInst + 1;
  bool br = dconfig.iDimm[discInst];
  ltr[0] += INSTANCE_NUM;
  #ifdef DBL_CLICK
    char typ[20];
    char py[3];
    if (discActn == 0) {
  #endif
  sprintf(tmpMsg0, "homeassistant/light/%s/config", mqttTopics[discInst].uname);
  sprintf(tmpMsgN, "{\"name\":\"%s\",\"uniq_id\":\"%s\",\"schema\":\"json\",\"brightness\":%s,\"bri_scl\":100,\"opt\":false,\"stat_t\":\"%s\",\"cmd_t\":\"%s\",\"ret\":false,\"dev\":{\"ids\":[\"D%s\"],\"name\":\"%s\",\"mf\":\"JAE\",\"mdl\":\"RDm%s%s\",\"sw\":\"%s\"}}", mqttTopics[discInst].uname, mqttTopics[discInst].uid, ((br) ? "true" : "false"), mqttTopics[discInst].state, mqttTopics[discInst].set, deviceId, dconfig.myId, ltr, HVERSION, SVERSION);
  #ifdef DBL_CLICK
    } else {
      if(discActn == 1) {
        strcpy(typ, "button_double_press");
        strcpy(py, "dc");
      } else {
        strcpy(typ, "button_triple_press");
        strcpy(py, "tc");
      }
      sprintf(tmpMsg0, "homeassistant/device_automation/%s/%s%d/config", deviceId, py, lgt);
      sprintf(tmpMsgN, "{\"atype\":\"trigger\",\"type\":\"%s\",\"stype\":\"button_%d\",\"payload\":\"%s%d\",\"topic\":\"%s\",\"dev\":{\"ids\":[\"D%s\"],\"name\":\"%s\",\"mdl\":\"RDm%s%s\",\"sw\":\"%s\",\"mf\":\"JAE\"}}", typ, lgt, py, lgt, topicAction, deviceId, dconfig.myId, ltr, HVERSION, SVERSION);
    }
    ++discActn;
    if (discActn > 2) {
      discActn = 0;
      ++discInst;
    }
  #else
    ++discInst;
  #endif
  mqttTransit(mqttClient.publish(tmpMsg0, MQTT_PUB_QOS, true, tmpMsgN));
  #ifdef DEBUG_MODE
    writeDebug("Discover on topic: ", 4, true);
    writeDebug(tmpMsg0, 0, true);
    writeDebug(tmpMsgN);
  #endif
  if (discInst >= INSTANCE_NUM) {
    discovered = true;
  }
}
#endif

/** **********************************************************************************************
   MQTT Message received

   Decode MQTT messages obtained from the subscription channels
   @param topic Topic the message was received on
   @param payload Message contents
   @param properties Message properties
   @param len Size of the message segment
   @param index Index of the segment
   @param total Total message size
*********************************************************************************************** */
void mqttMessageCallback (char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  #ifdef DEBUG_MODE
    sprintf(tmpMsg, "Incoming MQTT message: : %s, %d, %d, %d \n %s", topic, len, index, total, payload);
    writeDebug(tmpMsg, 4);
  #endif
  if (len != total) {
    #ifdef DEBUG_MODE
      writeDebug("Ignore MQTT segmented message.", 4);
    #endif
    return;
  }
  if (payload[0] != '{') {
    #ifdef DEBUG_MODE
      writeDebug("Ignore non JSON message.", 4);
    #endif
    return;
  }
  #ifdef JSON6
    // JSON V6.0
    DynamicJsonDocument json(1024);
    auto error = deserializeJson(json, payload);
    if (error) {
      #ifdef DEBUG_MODE
        writeDebug("Invalid JSON Structure", 4);
      #endif
      return;
    }
  #else
    // JSON V5.0
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(payload);
    if (!json.success()) {
      #ifdef DEBUG_MODE
        writeDebug("Invalid JSON Structure", 4);
      #endif
      return;
    }
  #endif
  #ifdef DEBUG_MODE
    writeDebug("JSON payload parsed", 4);
  #endif
  if(strcmp(topic, topicSet) == 0) {
    // Discard retained messages
    if (properties.retain) {
      #ifdef DEBUG_MODE
        writeDebug("Device SET topics do not accept retained messages.", 4);
      #endif
      return;
    }
    // No ACTION requested...
    if (!json.containsKey("action") || !json["action"].is<char*>()) {
      #ifdef DEBUG_MODE
        writeDebug("Invalid configuration message. No action defined.", 4);
      #endif
      return;
    }
    // Get the state of a single instance or all
    if (strcmp(json["action"], "getState") == 0) {
      if (json.containsKey("instance") && (json["instance"].is<signed int>() || json["instance"].is<unsigned int>())) {
        if (json["instance"] > 0 && json["instance"] <= INSTANCE_NUM) {
          mqttPublishStatus(((int)json["instance"] - 1));
        }
      } else {
        mqttPublishStatus(-1);
      }
      return;
    } 
    // Change state of LED indicators
    if (strcmp(json["action"], "getInfo") == 0) {
      const char* ipadd1 = WiFi.localIP().toString().c_str();
      #ifdef SAVE_ON_RESET
        sprintf(tmpMsg, "{\"dev\":\"%s\",\"ip\":\"%s\",\"bt\":%d,\"ac\":%ld,\"rc\":%d,\"rt\":%d,\"rst\":%d,\"rd\":%d,\"al\":%ld,\"rc\":%d,\"rt\":%d,\"rd1\":%d,\"al1\":%ld,\"rc1\":%d,\"rt1\":%d,\"rd2\":%d,\"al2\":%ld,\"rc2\":%d,\"rt2\":%d}", TITLE, ipadd1, debugBoot, aliveCounter, wifiRstCtr, wifiRstTtl, debugRst, resetDelay1, aliveCounter1, wifiRstCtr1, wifiRstTtl1, resetDelay2, aliveCounter2, wifiRstCtr2, wifiRstTtl2, resetDelay3, aliveCounter3, wifiRstCtr3, wifiRstTtl3);
        mqttPublish(0, 0, tmpMsg);
      #else
        sprintf(tmpMsg0, "{\"dev\":\"%s\",\"ip\":\"%s\",\"al\":%ld,\"rc\":%d,\"rt\":%d}", TITLE, ipadd1, aliveCounter, wifiRstCtr, wifiRstTtl);
        mqttPublish(0, 0, tmpMsg0);
      #endif
      return;
    }
    // Change state of LED indicators
    if (strcmp(json["action"], "indicators") == 0) {
      // Set duty cycle 
      if (json.containsKey("bright") && (json["bright"].is<signed int>() || json["bright"].is<unsigned int>())) {
        if (json["bright"] > 100) {
           json["bright"] = 100;
        } else if (json["bright"] < -1) {
           json["bright"] = -1;
        }
        LedBright = calcLed((int)json["bright"]);
      }
      if (json.containsKey("dimm") && (json["dimm"].is<signed int>() || json["dimm"].is<unsigned int>())) {
        if (json["dimm"] > 100) {
           json["dimm"] = 100;
        } else if (json["dimm"] < -1) {
           json["dimm"] = -1;
        }
        LedDimm = calcLed((int)json["dimm"]);
      }
      return;
    }
    if (strcmp(json["action"], "getConfig") == 0) {
      #ifndef MQTT_CONFIG
        #ifdef DEBUG_MODE
          writeDebug("Remote configuration disabled.'.", 1);
        #endif
      #else
        // Get the device configuration
        File configFile = LittleFS.open("/config.json", "r");
        if (configFile) {
          size_t size = configFile.size();
          // Allocate a buffer to store contents of the file.
          std::unique_ptr<char[]> buf(new char[size]);
          configFile.readBytes(buf.get(), size);
          // Publish config string
          mqttPublish(0, 0, buf.get());
          configFile.close();
        }
      #endif
      return;
    }
    if (strcmp(json["action"], "config") == 0) {
      #ifndef MQTT_CONFIG
        #ifdef DEBUG_MODE
          writeDebug("Remote configuration disabled.'.", 1);
        #endif
      #else
        // Configure
        if (!json.containsKey("secret") || !json["secret"].is<char*>()) {
          #ifdef DEBUG_MODE
            writeDebug("Missing key 'secret'.", 4);
          #endif
          return;
        }
        if (strcmp(json["secret"], "secret") != 0) {
          #ifdef DEBUG_MODE
            writeDebug("Invalid 'secret'.", 4);
          #endif
          return;
        }
        bool configChanged = false;
        if (json.containsKey("mqttport") && (json["mqttport"].is<signed int>() || json["mqttport"].is<unsigned int>())) {
          dconfig.mqttport = json["mqttport"];
          configChanged = true;
        }
        if (json.containsKey("mqttbroker") && json["mqttbroker"].is<char*>()) {
          strlcpy(dconfig.mqttbroker, json["mqttbroker"], sizeof(dconfig.mqttbroker));
          configChanged = true;
        }
        if (json.containsKey("mqttuser") && json["mqttuser"].is<char*>()) {
          strlcpy(dconfig.mqttuser, json["mqttuser"], sizeof(dconfig.mqttuser));
          configChanged = true;
        }
        if (json.containsKey("mqttpass") && json["mqttpass"].is<char*>()) {
          strlcpy(dconfig.mqttpass, json["mqttpass"], sizeof(dconfig.mqttpass));
          configChanged = true;
        }
        if (json.containsKey("namespc") && json["namespc"].is<char*>()) {
          strlcpy(dconfig.namespc, json["namespc"], sizeof(dconfig.namespc));
          configChanged = true;
          #ifdef MQTT_DISCOVERY
            mqttReDiscover();
          #endif
        }
        if (json.containsKey("mqttkey") && json["mqttkey"].is<char*>()) {
          strlcpy(dconfig.mqttkey, json["mqttkey"], sizeof(dconfig.mqttkey));
          configChanged = true;
        }
        if (json.containsKey("retain") && json["retain"].is<bool>()) {
          dconfig.retain = json["retain"];
          configChanged = true;
          #ifdef MQTT_DISCOVERY
            mqttReDiscover();
          #endif
        }
        if (json.containsKey("keepAlive") && (json["keepAlive"].is<signed int>() || json["keepAlive"].is<unsigned int>())) {
          dconfig.keepAlive = json["keepAlive"];
          configChanged = true;
        }
        if (json.containsKey("myId") && json["myId"].is<char*>()) {
          strlcpy(dconfig.myId, json["myId"], sizeof(dconfig.myId));
          configChanged = true;
          #ifdef MQTT_DISCOVERY
            mqttReDiscover();
          #endif
        }
        /* --------------------------------------------------------------------------------------------  */
        if (json.containsKey("LgtMode") && json["LgtMode"].is<bool>()) {
          dconfig.LgtMode = json["LgtMode"];
          setLightMode();
          configChanged = true;
        }
        if (json.containsKey("LedBright") && (json["LedBright"].is<signed int>() || json["LedBright"].is<unsigned int>())) {
          if (json["LedBright"] > 100) {
             json["LedBright"] = 100;
          } else if (json["LedBright"] < 0) {
             json["LedBright"] = 0;
          }
          dconfig.LedBright = json["LedBright"];
          LedBright = calcLed(dconfig.LedBright);
          configChanged = true;
        }
        if (json.containsKey("LedDimm") && (json["LedDimm"].is<signed int>() || json["LedDimm"].is<unsigned int>())) {
          if (json["LedDimm"] > 100) {
             json["LedDimm"] = 100;
          } else if (json["LedDimm"] < 0) {
             json["LedDimm"] = 0;
          }
          dconfig.LedDimm = json["LedDimm"];
          LedDimm = calcLed(dconfig.LedDimm);
          configChanged = true;
        }
        if (json.containsKey("LedDefault") && json["LedDefault"].is<bool>()) {
          dconfig.LedDefault = json["LedDefault"];
          configChanged = true;
        }
        if (json.containsKey("dimmTime") && (json["dimmTime"].is<signed int>() || json["dimmTime"].is<unsigned int>())) {
          if (json["dimmTime"] > MAX_DIMM_TIME) {
             json["dimmTime"] = MAX_DIMM_TIME;
          } else if (json["dimmTime"] < MIN_DIMM_TIME) {
             json["dimmTime"] = MIN_DIMM_TIME;
          }
          dconfig.dimmTime = json["dimmTime"];
          defineEdge();
          // Change repetition rate for buttons
          for (int i = 0; i < INSTANCE_NUM; i++) {
            myButtons[i].repInterval = calcRep();
          }
          configChanged = true;
        }
        if (json.containsKey("edgeTime") && (json["edgeTime"].is<signed int>() || json["edgeTime"].is<unsigned int>())) {
          if (json["edgeTime"] > MAX_EDGE_TIME) {
             json["edgeTime"] = MAX_EDGE_TIME;
          } else if (json["edgeTime"] < MIN_EDGE_TIME) {
             json["edgeTime"] = MIN_EDGE_TIME;
          }
          dconfig.edgeTime = json["edgeTime"];
          defineEdge();
          configChanged = true;
        }
        if (json.containsKey("transitionOn") && (json["transitionOn"].is<signed int>() || json["transitionOn"].is<unsigned int>())) {
          if (json["transitionOn"] > 10) {
             json["transitionOn"] = 10;
          } else if (json["transitionOn"] < 0) {
             json["transitionOn"] = 0;
          }
          dconfig.transitionOn = json["transitionOn"];
          configChanged = true;
        }
        if (json.containsKey("transitionOff") && (json["transitionOff"].is<signed int>() || json["transitionOff"].is<unsigned int>())) {
          if (json["transitionOff"] > 10) {
             json["transitionOff"] = 10;
          } else if (json["transitionOff"] < 0) {
             json["transitionOff"] = 0;
          }
          dconfig.transitionOff = json["transitionOff"];
          configChanged = true;
        }
        /* --------------------------------------------------------------------------------------------  */
        int lgt;
        for (int i = 0; i < INSTANCE_NUM; i++) {
          lgt = i + 1;
          sprintf(tmpMsg0, "name%d", lgt);
          if (json.containsKey(tmpMsg0) && json[tmpMsg0].is<char*>()) {
            strlcpy(dconfig.iName[i], json[tmpMsg0], sizeof(dconfig.iName[i]));
            configChanged = true;
            #ifdef MQTT_DISCOVERY
              mqttReDiscover();
            #endif
          }
          sprintf(tmpMsg0, "LgtDimm%d", lgt);
          if (json.containsKey(tmpMsg0) && json[tmpMsg0].is<bool>()) {
            dconfig.iDimm[i] = (dconfig.LgtMode) ? json[tmpMsg0] : false;
            configChanged = true;
          }
          sprintf(tmpMsg0, "LgtTrns%d", lgt);
          if (json.containsKey(tmpMsg0) && json[tmpMsg0].is<bool>()) {
            dconfig.iTrns[i] = (dconfig.LgtMode) ? json[tmpMsg0] : false;
            configChanged = true;
          }
          sprintf(tmpMsg0, "minPow%d", lgt);
          if (json.containsKey(tmpMsg0) && (json[tmpMsg0].is<signed int>() || json[tmpMsg0].is<unsigned int>())) {
            if (json[tmpMsg0] > MAX_MIN_POW) {
               json[tmpMsg0] = MAX_MIN_POW;
            } else if (json[tmpMsg0] < MIN_POW) {
               json[tmpMsg0] = MIN_POW;
            }
            dconfig.iMinPow[i] = json[tmpMsg0];
            configChanged = true;
          }
          sprintf(tmpMsg0, "brightness%d", lgt);
          if (json.containsKey(tmpMsg0) && (json[tmpMsg0].is<signed int>() || json[tmpMsg0].is<unsigned int>())) {
            if (json[tmpMsg0] > 100) {
               json[tmpMsg0] = 100;
            } else if (json[tmpMsg0] < 1) {
               json[tmpMsg0] = 1;
            }
            dconfig.iBrightness[i] = json[tmpMsg0];
            configChanged = true;
          }
        }
        /* --------------------------------------------------------------------------------------------  */
        // Save configuration changes
        if (configChanged) {
          saveConfig();
        }
        #ifdef DEBUG_MODE
          else {
            writeDebug("No changes in the configuration.", 1);
          }
        #endif
      #endif
      return;
    }
    #ifdef MQTT_RESET
      // Reset the Device
      if (strcmp(json["action"], "reset") == 0) {
        mqttPublish(0, 0, MQTT_MSG_RESET);
        #ifdef DEBUG_MODE
          writeDebug("Reset requested", 1);
        #endif
        ESP.restart();
        return;
      }
    #endif
    #if defined(SAVE_ON_RESET) && defined(SAVE_ON_RESET_CLEAR)
      // Clear status from EEPROM
      if (strcmp(json["action"], "clearmem") == 0) {
        clearStatus();
        return;
      }
    #endif
    #ifdef DEBUG_MODE
      writeDebug("Invalid configuration action.", 4);
    #endif
    return;
  }
  bool fLive = false;
  bool fPub = false;
  bool fTop = false;
  bool fDim = false;
  bool fTrn = false;
  int stt = -1;
  int brght = -1;
  int trns = -1;
  // Loop instance topics
  for (int i = 0; i < INSTANCE_NUM; i++) {
    // Validate SET or STATE (GoLive) topics
    if (strcmp(topic, mqttTopics[i].set) == 0) {
      if (properties.retain) {
        #ifdef DEBUG_MODE
          writeDebug("Instances do not accept retained messages on SET topics.", 4);
        #endif
        return;
      }
      fLive = false;
    } else if (goLive == 2 && strcmp(topic, mqttTopics[i].state) == 0) {
      fLive = true;
      #ifdef DEBUG_MODE
        writeDebug("Retained Message", 4);
      #endif
    } else {
      continue;
    }
    fTop = true;
    if (json.containsKey("action") && json["action"].is<char*>()) {
      if (properties.retain) {
        #ifdef DEBUG_MODE
          writeDebug("Action Topics do not accept retained messages.", 4);
        #endif
        return;
      }
      if (strcmp(json["action"], "getState") == 0) {
        mqttPublishStatus(i);
        return;
      }
      if (strcmp(json["action"], "getConfig") == 0) {
        fDim = dconfig.iDimm[i];
        fTrn = dconfig.iTrns[i];
        if (snprintf(tmpMsg, sizeof(tmpMsg), "{\"name\":\"%s\",\"uniq_id\":\"%s\",\"stat_t\":\"%s\",\"cmd_t\":\"%s\",\"dimmTime\":%d,\"edgeTime\":%d,\"dimmer\":%s,\"transition\":%s,\"tr_on\":%d,\"tr_off\":%d}", mqttTopics[i].uname, mqttTopics[i].uid, mqttTopics[i].state, mqttTopics[i].set, dconfig.dimmTime, dconfig.edgeTime, ((fDim) ? "true" : "false"), ((fTrn) ? "true" : "false"), dconfig.transitionOn, dconfig.transitionOff) >= 0) {
          mqttPublish(1, i, tmpMsg);
        }
        return;
      }
      #ifdef DEBUG_MODE
        writeDebug("Invalid action", 4);
      #endif
      return;
    }
    /* --------------------------------------------------------------------------------------------  */
    fPub = false;
    stt = -1;
    brght = -1;
    trns = -1;
    if (json.containsKey("brightness") && (json["brightness"].is<signed int>() || json["brightness"].is<unsigned int>())) {
      if (json["brightness"] > 100) {
        json["brightness"] = 100;
      } else if (json["brightness"] <= 0) {
        json["brightness"] = 0;
      }
      brght = json["brightness"];
      fPub = true;
    }
    if (json.containsKey("state") && json["state"].is<char*>()) {
      if (strcmp(json["state"], "ON") == 0) stt = 1;
      else if (strcmp(json["state"], "OFF") == 0) stt = 0;
      else if (strcmp(json["state"], "TOGGLE") == 0) stt = 2;
      fPub = true;
    }
    if (json.containsKey("transition") && (json["transition"].is<signed int>() || json["transition"].is<unsigned int>() || json["transition"].is<float>())) {
      if (json["transition"] > 10) {
        json["transition"] = 10;
      } else if (json["transition"] <= 0) {
        json["transition"] = 0;
      }
      trns = (int)json["transition"];
    }
    if (json.containsKey("indicator") && json["indicator"].is<char*>()) {
      inst[i].iStatus = (strcmp(json["indicator"], "OFF") == 0) ? false : true; 
    }
    if (!fPub) return;
    if (fLive) {
      setLightState(i, stt, brght, CONFIG_TRANSITION_WAKE, false, false);
      ++retainCnt;
      if (retainCnt >= INSTANCE_NUM) {
        goLiveOff();
      }
    } else {
      setLightState(i, stt, brght, trns, true, false);
    }
    break;
  }
  #ifdef DEBUG_MODE
    if (!fTop) {
      writeDebug("Invalid instance topic. ", 4, true);
      writeDebug(topic);
    }
  #endif
}

/** **********************************************************************************************
   Callback notifying that we need to save WiFi Manager config
*********************************************************************************************** */
void saveConfigCallback () {
  shouldSaveConfig = true;
  #ifdef DEBUG_MODE
    writeDebug("Should save config", 1);
  #endif
}

/** **********************************************************************************************
   Set the light mode based on configuration
*********************************************************************************************** */
void setLightMode () {
  for (int i = 0; i < INSTANCE_NUM; i++) {
    dconfig.iDimm[i] = dconfig.LgtMode;
    dconfig.iTrns[i] = dconfig.LgtMode;
  }
}

/** **********************************************************************************************
   Set light state
   
   @param i Light instance [0|1]
   @param state Light state. 
      - 0 = OFF 
      - 1 = ON 
      - 2 = Toggle
      - 3 = no action
   @param brightness Brightness [0 - 100]; -1 = No change; -2 = Next dimm value 
   @param transition Transition speed [0 - 10]
   @param pub Publish / Save new status
   @param remote Values set remotely
*********************************************************************************************** */
void setLightState (int i, int state, int brightness, int transition, bool pub, bool remote) {
  //i, 3, -1, 0, true, false
  bool newState = false;
  // Set new state
  if (state == 0) {
    newState = false;
  } else if (state == 1) {
    newState = true;
  } else if (state == 2) {
    newState = !inst[i].tState;
  } else if (state == 3) {
    newState = inst[i].tState;
  }
  // Set transition speed
  if (transition >= 0) {
    inst[i].tTransition = transition;
  } else {
    inst[i].tTransition = (newState) ? dconfig.transitionOn : dconfig.transitionOff;
  }
  if (state == 3) {
    inst[i].eCntr = 0;
  }
  else {
    if (dconfig.iDimm[i]) {
      if (brightness >= 0) {
        inst[i].tBright = brightness;
        inst[i].eCntr = 0;
      } else if (brightness == -2) {
        if (!inst[i].tState) {
          inst[i].tBright = 0;
          inst[i].tDirection = true;
        }
        if (inst[i].eCntr > 0) {
          --inst[i].eCntr;
        } else {
          if (inst[i].tDirection) {
            ++inst[i].tBright;  // Dimm up
          } else {
            --inst[i].tBright;  // Dimm down
          }
          if(inst[i].tBright > 100) {
            inst[i].tDirection = false;
            inst[i].tBright = 100;
            inst[i].eCntr = edgeSpeed;
          } else if(inst[i].tBright < 0) {
            inst[i].tDirection = true;
            inst[i].tBright = 0;
            inst[i].eCntr = edgeSpeed;
          }
        }
      } else {
        inst[i].eCntr = 0;
      }
      #ifdef DEBUG_MODE
        sprintf(tmpMsg0, "Light %d brightness = %d%s", (i + 1), inst[i].tBright, ((remote) ? ", set remotely" : " "));
        writeDebug(tmpMsg0, 3);
      #endif
    } else {
      inst[i].tTransition = 0;
      inst[i].tBright = (newState) ? 100 : 0;
    }
    inst[i].tState = newState;
  }
  // If publish flag is off, end here.
  if (!pub) return;
  // Save state
  #ifdef PERSISTENT_STATE
    if (!dconfig.retain) {
      saveLightState(i);
    }
  #endif
  // Publish new state
  mqttPublishStatus(i);
  // Inform state in debug
  #ifdef DEBUG_MODE
    if (state == 3) return;
    // Show new status
    sprintf(tmpMsg0, "Light %d turned %s %s", (i + 1), ((inst[i].tState) ? "ON" : "OFF"), ((remote) ? "remotely" : ""));
    writeDebug(tmpMsg0, 3);
  #endif
}

#ifdef PERSISTENT_STATE
/** **********************************************************************************************
   Save light state to EEPROM
   
   @param index Light index [0|1]
*********************************************************************************************** */
void saveLightState (int index) {
  byte val;
  int idx = index * 5;
  val = (inst[index].tState) ? 1 : 0;
  EEPROM.write(idx++, val);
  EEPROM.write(idx++, inst[index].tBright);
  val = (inst[index].iStatus) ? 1 : 0;
  EEPROM.write(idx, val);
  EEPROM.commit();
}

/** **********************************************************************************************
   Get lights state from EEPROM
*********************************************************************************************** */
void getLightState() {
  bool stt;
  byte val;
  int idx;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    idx = i * 5;
    val = EEPROM.read(idx++);
    stt = (val == 1) ? true : false;
    val = EEPROM.read(idx++);
    setLightState(i, stt, (int)val, CONFIG_TRANSITION_WAKE, true, false);
    val = EEPROM.read(idx);
    inst[i].iStatus = (val == 1) ? true : false;
  }
}
#endif

/** **********************************************************************************************
   Set default values for configuration parameters

   If you set additional parameters in the WiFiManager, define initial values here
*********************************************************************************************** */
void configDefault() {
  dconfig.keepAlive = CONFIG_KEEPALIVE;            // Publish status every 30 sec
  dconfig.mqttport = CONFIG_MQTT_PORT;
  dconfig.retain = CONFIG_MQTT_RETAIN;
  strcpy(dconfig.mqttbroker, CONFIG_MQTT_BROKER);
  strcpy(dconfig.mqttuser, CONFIG_MQTT_USER);
  strcpy(dconfig.mqttpass, CONFIG_MQTT_PASS);
  strcpy(dconfig.namespc, CONFIG_MQTT_NAMESPACE);
  strcpy(dconfig.mqttkey, CONFIG_MQTT_KEY);
  sprintf(tmpMsg0, "RDimmer_%s", deviceId);
  strcpy(dconfig.myId, tmpMsg0);
  dconfig.LgtMode = CONFIG_LGT_MODE;
  dconfig.LedBright = CONFIG_LED_BRIGHT;
  dconfig.LedDimm = CONFIG_LED_DIMM;
  dconfig.LedDefault = CONFIG_LED_DEFAULT;
  dconfig.dimmTime = CONFIG_DIMMING_TIME;
  dconfig.edgeTime = CONFIG_EDGE_TIME;
  dconfig.transitionOn = CONFIG_TRANSITION_ON;     // Transition speed to turn ON
  dconfig.transitionOff = CONFIG_TRANSITION_OFF;   // Transition speed to turn OFF
  for (int i = 0; i < INSTANCE_NUM; i++) {
    strcpy(dconfig.iName[i], "");
    dconfig.iDimm[i] = CONFIG_LGT_DIMM;
    dconfig.iTrns[i] = CONFIG_LGT_TRNS;
    dconfig.iMinPow[i] = CONFIG_MIN_POW;
    dconfig.iBrightness[i] = CONFIG_BRIGHTNESS;
  }
  // Define Edge Speed for dimmable range
  defineEdge();
}

/** **********************************************************************************************
   Load the configuration
*********************************************************************************************** */
void loadConfig() {
  // Read configuration from FS json
  #ifdef DEBUG_MODE
    writeDebug("Mounting FS...", 1);
  #endif
  mounted = LittleFS.begin();
  if (!mounted) {
    #ifdef DEBUG_MODE
      writeDebug("Failed to mount FS", 1);
    #endif
    return;
  }
  #ifdef DEBUG_MODE
    writeDebug("Mounted file system", 1);
  #endif
  if (!LittleFS.exists("/config.json")) {
    #ifdef DEBUG_MODE
      writeDebug("Config file does not exist, create a new one...", 1);
    #endif
    saveConfig();
    return;
  }
  // File exists, read and load configuration
  #ifdef DEBUG_MODE
    writeDebug("Reading config file", 1);
  #endif
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    #ifdef DEBUG_MODE
    writeDebug("Failed to open configuration file", 1);
    #endif
    return;
  }
  // Allocate a buffer to store contents of the file.
  size_t size = configFile.size();
  std::unique_ptr<char[]> buf(new char[size]);
  // Parse the JSON config string
  configFile.readBytes(buf.get(), size);
  #ifdef JSON6
    // JSON V6.0
    DynamicJsonDocument json(800);
    auto error = deserializeJson(json, buf.get());
    if (error) {
      #ifdef DEBUG_MODE
        writeDebug("Failed to parse configuration file", 1);
      #endif
      return;
    }
  #else
    // JSON V5.0
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());
    if (!json.success()) {
      #ifdef DEBUG_MODE
        writeDebug("Failed to parse configuration file", 1);
      #endif
      return;
    }
  #endif
  //
  #ifdef DEBUG_MODE
    writeDebug("Parsed config file", 1);
  #endif
  // Set the config values, preserve defaults if a parameter is missing
  dconfig.mqttport = json["mqttport"] | dconfig.mqttport;
  strlcpy(dconfig.mqttbroker, json["mqttbroker"] | dconfig.mqttbroker, sizeof(dconfig.mqttbroker));
  strlcpy(dconfig.mqttuser, json["mqttuser"] | dconfig.mqttuser, sizeof(dconfig.mqttuser));
  strlcpy(dconfig.mqttpass, json["mqttpass"] | dconfig.mqttpass, sizeof(dconfig.mqttpass));
  strlcpy(dconfig.namespc, json["namespc"] | dconfig.namespc, sizeof(dconfig.namespc));
  strlcpy(dconfig.mqttkey, json["mqttkey"] | dconfig.mqttkey, sizeof(dconfig.mqttkey));
  dconfig.retain = json["retain"] | dconfig.retain;
  dconfig.keepAlive = json["keepAlive"] | dconfig.keepAlive;
  strlcpy(dconfig.myId, json["myId"] | dconfig.myId, sizeof(dconfig.myId));
  if (strlen(dconfig.myId) == 0) {
    strlcpy(dconfig.myId, deviceId, sizeof(dconfig.myId));
  }
  dconfig.LgtMode = json["LgtMode"] | dconfig.LgtMode;
  dconfig.LedBright = json["LedBright"] | dconfig.LedBright;
  dconfig.LedDimm = json["LedDimm"] | dconfig.LedDimm;
  dconfig.LedDefault = json["LedDefault"] | dconfig.LedDefault;
  dconfig.dimmTime = json["dimmTime"] | dconfig.dimmTime;
  dconfig.edgeTime = json["edgeTime"] | dconfig.edgeTime;
  dconfig.transitionOn = json["transitionOn"] | dconfig.transitionOn;
  dconfig.transitionOff = json["transitionOff"] | dconfig.transitionOff;
  int lgt;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    lgt = i + 1;
    sprintf(tmpMsg0, "name%d", lgt);
    strlcpy(dconfig.iName[i], json[tmpMsg0] | dconfig.iName[i], sizeof(dconfig.iName[i]));
    if (dconfig.LgtMode) {
      sprintf(tmpMsg0, "LgtDimm%d", lgt);
      dconfig.iDimm[i] = json[tmpMsg0] | dconfig.iDimm[i];
      sprintf(tmpMsg0, "LgtTrns%d", lgt);
      dconfig.iTrns[i] = json[tmpMsg0] | dconfig.iTrns[i];
    } else {
      dconfig.iDimm[i] = false;
      dconfig.iTrns[i] = false;
    }
    sprintf(tmpMsg0, "minPow%d", lgt);
    dconfig.iMinPow[i] = json[tmpMsg0] | dconfig.iMinPow[i];
    dconfig.iMinPow[i] = (dconfig.iMinPow[i] < MIN_POW) ? MIN_POW : dconfig.iMinPow[i];
    sprintf(tmpMsg0, "brightness%d", lgt);
    dconfig.iBrightness[i] = json[tmpMsg0] | dconfig.iBrightness[i];
  }
  // Validate (backwards compatibility)
  if (dconfig.LedBright > 100 || dconfig.LedBright < 0) {
    dconfig.LedBright = CONFIG_LED_BRIGHT;
  }
  if (dconfig.LedDimm > 100 || dconfig.LedDimm < 0) {
    dconfig.LedDimm = CONFIG_LED_DIMM;
  }
  if (dconfig.dimmTime > MAX_DIMM_TIME || dconfig.dimmTime < MIN_DIMM_TIME) {
    dconfig.dimmTime = CONFIG_DIMMING_TIME;
  }
  if (dconfig.edgeTime > MAX_EDGE_TIME || dconfig.edgeTime < MIN_EDGE_TIME) {
    dconfig.edgeTime = CONFIG_EDGE_TIME;
  }
  // Set variables
  defineEdge();
  LedBright = calcLed(dconfig.LedBright);
  LedDimm = calcLed(dconfig.LedDimm);
  #ifdef DEBUG_MODE
    #ifdef JSON6
      // JSON V6.0
      serializeJson(json, Serial);
      writeDebug(" ");
    #else
      // JSON V5.0
      json.printTo(Serial);
    #endif
  #endif
  configFile.close();
}

/** **********************************************************************************************
   Save the configuration
*********************************************************************************************** */
void saveConfig() {
  if (!mounted) {
    #ifdef DEBUG_MODE
      writeDebug("FS not mounted", 1);
    #endif
    return;
  }
  #ifdef DEBUG_MODE
    writeDebug("Saving config", 1);
  #endif
  #ifdef JSON6
    // JSON V6.0
    DynamicJsonDocument json(800);
  #else
    // JSON V5.0
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
  #endif
  //  
  json["mqttport"] = dconfig.mqttport;
  json["mqttbroker"] = dconfig.mqttbroker;
  json["mqttuser"] = dconfig.mqttuser;
  json["mqttpass"] = dconfig.mqttpass;
  json["namespc"] = dconfig.namespc;
  json["mqttkey"] = dconfig.mqttkey;
  json["retain"] = dconfig.retain;
  json["keepAlive"] = dconfig.keepAlive;
  json["myId"] = dconfig.myId;
  json["LgtMode"] = dconfig.LgtMode;
  json["LedBright"] = dconfig.LedBright;
  json["LedDimm"] = dconfig.LedDimm;
  json["LedDefault"] = dconfig.LedDefault;
  json["dimmTime"] = dconfig.dimmTime;
  json["edgeTime"] = dconfig.edgeTime;
  json["transitionOn"] = dconfig.transitionOn;
  json["transitionOff"] = dconfig.transitionOff;
  int lgt;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    lgt = i + 1;
    sprintf(tmpMsg0, "name%d", lgt);
    json[tmpMsg0] = dconfig.iName[i];
    sprintf(tmpMsg0, "LgtDimm%d", lgt);
    json[tmpMsg0] = dconfig.iDimm[i];
    sprintf(tmpMsg0, "LgtTrns%d", lgt);
    json[tmpMsg0] = dconfig.iTrns[i];
    sprintf(tmpMsg0, "minPow%d", lgt);
    json[tmpMsg0] = dconfig.iMinPow[i];
    sprintf(tmpMsg0, "brightness%d", lgt);
    json[tmpMsg0] = dconfig.iBrightness[i];
  }
  //
  // Open file and print into it
  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile) {
    #ifdef DEBUG_MODE
      writeDebug("Failed to open config file for writing", 1);
    #endif
    mqttPublish(0, 0, MQTT_MSG_SAVE_FAIL);
    return;
  }
  #ifdef JSON6
    // JSON V6.0
    serializeJson(json, configFile);
  #else
    // JSON V5.0
    json.printTo(configFile);
  #endif
  //
  configFile.close();
  mqttPublish(0, 0, MQTT_MSG_SAVE);
  #ifdef DEBUG_MODE
    #ifdef JSON6
      // JSON V6.0
      serializeJson(json, Serial);
    #else
      // JSON V5.0
      json.printTo(Serial);
    #endif
    //
    writeDebug("");
  #endif
}

#ifdef SAVE_ON_RESET
/** **********************************************************************************************
   Load status from before reset
*********************************************************************************************** */
void loadLastStatus() {
  //int n = 0;
  int addr = SAVE_ON_RESET_AT;
  #ifdef DEBUG_MODE
    writeDebug("Reading debug information", 1);
  #endif
  addr += EEPROM_readAnything(addr, debugBoot);
  addr += EEPROM_readAnything(addr, debugRst);
  addr += EEPROM_readAnything(addr, resetDelay1);
  addr += EEPROM_readAnything(addr, wifiRstCtr1);
  addr += EEPROM_readAnything(addr, wifiRstTtl1);
  addr += EEPROM_readAnything(addr, aliveCounter1);
  addr += EEPROM_readAnything(addr, resetDelay2);
  addr += EEPROM_readAnything(addr, wifiRstCtr2);
  addr += EEPROM_readAnything(addr, wifiRstTtl2);
  addr += EEPROM_readAnything(addr, aliveCounter2);
  addr += EEPROM_readAnything(addr, resetDelay3);
  addr += EEPROM_readAnything(addr, wifiRstCtr3);
  addr += EEPROM_readAnything(addr, wifiRstTtl3);
  addr += EEPROM_readAnything(addr, aliveCounter3);
  #ifdef DEBUG_MODE
    sprintf(tmpMsg, "Got Info: (%d) %d, %d,  %d,  %d,  %d,  %ld,  %d,  %d,  %d,  %ld", addr, debugBoot, debugRst, resetDelay1, wifiRstCtr1, wifiRstTtl1, aliveCounter1, resetDelay2, wifiRstCtr2, wifiRstTtl2, aliveCounter2);
    writeDebug(tmpMsg, 1);
  #endif
  // Increase Boot Count
  if (debugBoot < 0) {
    debugBoot = 0;
  }
  debugBoot += 1;
  // Save Boot Count
  #ifdef DEBUG_MODE
    sprintf(tmpMsg0, "Save Boot counter: %d", debugBoot);
    writeDebug(tmpMsg0, 1);
  #endif
  addr = SAVE_ON_RESET_AT;
  //n = EEPROM_writeAnything(addr, debugBoot);
  EEPROM_writeAnything(addr, debugBoot);
  #ifdef DEBUG_MODE
    if (EEPROM.commit()) {
      writeDebug("Boot counter saved!", 1);
    } else {
      writeDebug("Failed saving boot counter.", 1);
    }
  #else
    EEPROM.commit();
  #endif
}

/** **********************************************************************************************
   Save status before booting
*********************************************************************************************** */
void saveStatus() {
  int addr = SAVE_ON_RESET_AT;
  // Increase the reboot counter
  debugRst += 1;
  // Save values 
  addr += EEPROM_writeAnything(addr, debugBoot);
  addr += EEPROM_writeAnything(addr, debugRst);
  addr += EEPROM_writeAnything(addr, resetDelay);
  addr += EEPROM_writeAnything(addr, wifiRstCtr);
  addr += EEPROM_writeAnything(addr, wifiRstTtl);
  addr += EEPROM_writeAnything(addr, aliveCounter);
  addr += EEPROM_writeAnything(addr, resetDelay1);
  addr += EEPROM_writeAnything(addr, wifiRstCtr1);
  addr += EEPROM_writeAnything(addr, wifiRstTtl1);
  addr += EEPROM_writeAnything(addr, aliveCounter1);
  addr += EEPROM_writeAnything(addr, resetDelay2);
  addr += EEPROM_writeAnything(addr, wifiRstCtr2);
  addr += EEPROM_writeAnything(addr, wifiRstTtl2);
  addr += EEPROM_writeAnything(addr, aliveCounter2);
  #ifdef DEBUG_MODE
    if (EEPROM.commit()) {
      writeDebug("Current status saved.", 1);
    } else {
      writeDebug("Failed saving current status.", 1);
    }
  #else
    EEPROM.commit();
  #endif
}

#ifdef SAVE_ON_RESET_CLEAR
/** **********************************************************************************************
   Clear saved status
*********************************************************************************************** */
void clearStatus() {
  int addr = SAVE_ON_RESET_AT;
  #ifdef DEBUG_MODE
    writeDebug("Clear EEPROM", 1);
  #endif
  // Clear values
  debugBoot = 0;
  debugRst = 0;
  resetDelay1 = 0;
  wifiRstCtr1 = 0;
  wifiRstTtl1 = 0;
  aliveCounter1 = 0;
  resetDelay2 = 0;
  wifiRstCtr2 = 0;
  wifiRstTtl2 = 0;
  aliveCounter2 = 0;
  resetDelay3 = 0;
  wifiRstCtr3 = 0;
  wifiRstTtl3 = 0;
  aliveCounter3 = 0;
  // Save values 
  addr += EEPROM_writeAnything(addr, debugBoot);
  addr += EEPROM_writeAnything(addr, debugRst);
  addr += EEPROM_writeAnything(addr, resetDelay1);
  addr += EEPROM_writeAnything(addr, wifiRstCtr1);
  addr += EEPROM_writeAnything(addr, wifiRstTtl1);
  addr += EEPROM_writeAnything(addr, aliveCounter1);
  addr += EEPROM_writeAnything(addr, resetDelay2);
  addr += EEPROM_writeAnything(addr, wifiRstCtr2);
  addr += EEPROM_writeAnything(addr, wifiRstTtl2);
  addr += EEPROM_writeAnything(addr, aliveCounter2);
  addr += EEPROM_writeAnything(addr, resetDelay3);
  addr += EEPROM_writeAnything(addr, wifiRstCtr3);
  addr += EEPROM_writeAnything(addr, wifiRstTtl3);
  addr += EEPROM_writeAnything(addr, aliveCounter3);
  if (EEPROM.commit()) {
    #ifdef DEBUG_MODE
      writeDebug("Cleared status values", 1);
    #endif
    mqttPublish(0, 0, MQTT_MSG_SC);
    return;
  }
  #ifdef DEBUG_MODE
    writeDebug("Failed deleting values", 1);
  #endif
  mqttPublish(0, 0, MQTT_MSG_SNC);
}
#endif

/** **********************************************************************************************
  Write a value to EEPROM

  @param ee EEPROM position [1 - 512]
  @param value Value variable
  @return Returns the number of bytes written to EEPROM
*********************************************************************************************** */
template <class T> int EEPROM_writeAnything(int ee, const T& value) {
  const byte* p = (const byte*)(const void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++) {
    EEPROM.write(ee++, *p++);
  }
  return i;
}

/** **********************************************************************************************
  Read a value from EEPROM

  @param ee MySensors EEPROM position [1 - 255]
  @param value Value variable
  @return Returns the number of bytes read from EEPROM
*********************************************************************************************** */
template <class T> int EEPROM_readAnything(int ee, T& value) {
  byte* p = (byte*)(void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++) {
    *p++ = EEPROM.read(ee++);
  }
  return i;
}
#endif

/** **********************************************************************************************
   Initialize button variables
   
  @param button Button Index
  @param pinId Hardware Pin
  @param eventState Active state
  @param features Button features
    - 1 = Long Press
    - 2 = Repeat (Requires "Long Press")
    - 4 = Multiple click
  @param repInterval Repeat interval (ms)
*********************************************************************************************** */
void initButton(int button, int pinId, int eventState, int features, int repInterval) {
  myButtons[button].pinId = pinId;
  myButtons[button].eventState = eventState;
  myButtons[button].features = features;
  myButtons[button].repInterval = repInterval;
  myButtons[button].state = !eventState;
  myButtons[button].lastState = !eventState;
}

/** **********************************************************************************************
   Get the status of a button

  @param btnIndex Button Index
  @return Returns:
    - 0 = No action
    - 1 = Button Down
    - 2 = Button Up
    - 3 = Button Up after long press
    - 4 = Button Up after repeat
    - 5 = Repeat
    - 6 = Double click
    - 7 = Triple click
    - 8 = Multi click
*********************************************************************************************** */
int getButtonStatus(int btnIndex) {
  // Init timer
  unsigned long cTmr = millis();
  // Read the state of the button
  int reading = digitalRead(myButtons[btnIndex].pinId);
  // State changes reset the debounce timer
  if (reading != myButtons[btnIndex].lastState) {
    myButtons[btnIndex].btnTimer = cTmr;
    myButtons[btnIndex].lastState = reading;
  }
  // Button sate changed
  if (reading != myButtons[btnIndex].state) {
    // Skip the rest of checking during debouncing
    if ((unsigned long)(cTmr - myButtons[btnIndex].btnTimer) < BTN_DEBOUNCE) return 0;
    // Whatever the reading is at, it's been there for longer than the debounce
    //  delay, so change the actual state
    myButtons[btnIndex].state = reading;
    // Button DOWN...
    if (myButtons[btnIndex].state == myButtons[btnIndex].eventState) {
      // ... Reset the repetition flag and timer
      myButtons[btnIndex].lngFlag = false;
      myButtons[btnIndex].repTimer = cTmr;
      return 1;
    }
    // Button UP...
    //  Long press/repeat enabled
    if(myButtons[btnIndex].lngFlag) {
      myButtons[btnIndex].clkCtr = 0;
    // ... Repeat or Long press
      if ((myButtons[btnIndex].features & 2) > 0) return 4;
      return 3;
    }
    // If double/multi click is enabled...
    if ((myButtons[btnIndex].features & 12) > 0) {
      // ... count the number of "clicks"
      myButtons[btnIndex].clkCtr++;
      myButtons[btnIndex].dblTimer = cTmr;
      return 0;
    }
    // Reset count and return "button up" value
    myButtons[btnIndex].clkCtr = 0;
    return 2;
  }
  // Button is down
  if (myButtons[btnIndex].state == myButtons[btnIndex].eventState) {
    if (myButtons[btnIndex].lngFlag) {
      if ((myButtons[btnIndex].features & 2) > 0 && (unsigned long)(cTmr - myButtons[btnIndex].repTimer) > myButtons[btnIndex].repInterval) {
        myButtons[btnIndex].repTimer = cTmr;
        return 5;
      }
    }
    else if ((myButtons[btnIndex].features & 1) > 0) {
      if ((unsigned long)(cTmr - myButtons[btnIndex].repTimer) > BTN_LONG_PRESS) {
        myButtons[btnIndex].repTimer = cTmr;
        // Set the repetition flag...
        myButtons[btnIndex].lngFlag = true;
        // If repetition is on, start repeat cycle
        if ((myButtons[btnIndex].features & 2) > 0) return 5;
      }
    }
    return 0;
  }
  // Button is up
  if ((myButtons[btnIndex].features & 4) > 0 && myButtons[btnIndex].clkCtr > 0) {
    // Check if the double click timer has expired
    if ((unsigned long)(cTmr - myButtons[btnIndex].dblTimer) < DBL_CLICK_INTERVAL) return 0;
    int cntr = myButtons[btnIndex].clkCtr;
    myButtons[btnIndex].clkCtr = 0;
    // Multi-click
    if (cntr >= MULT_CLICK_COUNT) {
      return 8;
    }
    // Triple-click
    else if (cntr >= 3) {
      return 7;
    }
    // Double-click
    else if (cntr >= 2) {
      return 6;
    }
    // Button UP
    return 2;
  }
  return 0;
}

/** **********************************************************************************************
   Calcuate the edge timers
*********************************************************************************************** */
void defineEdge () {
  edgeSpeed = dconfig.edgeTime / calcRep();
}

/** **********************************************************************************************
   Calcuate the LED timers
  @param level Brigthness level (0-100). -1 sets to default.
  @return Returns the PWM dutty cycle for the LEDs
*********************************************************************************************** */
int calcLed (int bright) {
  return ((bright < 0) ? dconfig.LedBright : bright) * LED_PWM / 100;
}

/** **********************************************************************************************
   Calcuate the repetition for buttons
   
  @return Returns the button repetition in ms
*********************************************************************************************** */
int calcRep () {
  return dconfig.dimmTime / 100;
}

/** **********************************************************************************************
   Go to setup mode by simulating a double reset
*********************************************************************************************** */
void goToSetupMode() {
  #ifdef DEBUG_MODE
    writeDebug("Multi-click detected. Boot in setup mode...", 1);
  #endif
  // Boot in MULT_CLICK_COUNT
  uint32_t data = 0xAAAAAAAA;
  ESP.rtcUserMemoryWrite(DRD_ADDRESS, &data, sizeof(data));
  ESP.reset();
}

#ifdef OTA_HTTP
/** **********************************************************************************************
   HTTP Update started
*********************************************************************************************** */
void http_update_started() {
  #ifdef DEBUG_MODE
    writeDebug("HTTP update process started", 1);
  #endif
}

/** **********************************************************************************************
   HTTP Update finished
*********************************************************************************************** */
void http_update_finished() {
  #ifdef DEBUG_MODE
    writeDebug("HTTP update process finished", 1);
  #endif
}

/** **********************************************************************************************
   HTTP Update progress
*********************************************************************************************** */
void http_update_progress(int cur, int total) {
  yield();
  #ifdef DEBUG_MODE
    sprintf(tmpMsg, "HTTP update process at %d of %d bytes...", cur, total);
    writeDebug(tmpMsg, 1);
  #endif
}

/** **********************************************************************************************
   HTTP Update error
*********************************************************************************************** */
void http_update_error(int err) {
  #ifdef DEBUG_MODE
    sprintf(tmpMsg, "HTTP update fatal error code %d", err);
    writeDebug(tmpMsg, 1);
  #endif
}
#endif


/** **********************************************************************************************
   Setup function
*********************************************************************************************** */
void setup() {
  /* ------------------------------------------------------------------------------
     Hardware configuration
     ------------------------------------------------------------------------------ */
  // Define pins
  pinMode(PIN_ZCROSS , INPUT_PULLUP);
  pinMode(PIN_TM1, OUTPUT);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  #ifdef DOUBLE_DIMMER
    pinMode(PIN_TM2, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_BUTTON2, INPUT_PULLUP);
  #endif
  // Initial pin state
  digitalWrite(PIN_TM1, LOW);
  #ifdef DOUBLE_DIMMER
    digitalWrite(PIN_TM2, LOW);
  #endif
  delay(10);

  /* ------------------------------------------------------------------------------
     Service variables
     ------------------------------------------------------------------------------ */
  int i;
  inst[0].tPin = PIN_TM1;
  inst[0].bPin = PIN_BUTTON1;
  inst[0].iPin = PIN_LED1;
  #if INSTANCE_NUM > 1
    inst[1].tPin = PIN_TM2;
    inst[1].bPin = PIN_BUTTON2;
    inst[1].iPin = PIN_LED2;
  #endif
  
  /* ------------------------------------------------------------------------------
     Enable debugging
     ------------------------------------------------------------------------------ */
  #ifdef DEBUG_MODE
    // Serial port configuration
    Serial.begin(SERIAL_SPEED);
    delay(200);
    writeDebug("");
    writeDebug(TITLE);
  #endif
  
  /* ------------------------------------------------------------------------------
     Define a Unique ID for the device (MAC Address)
     ------------------------------------------------------------------------------ */
  setDeviceId();

  /* ------------------------------------------------------------------------------
     Read device configuration
     ------------------------------------------------------------------------------ */
  // Clean FS - Uncomment to re-format during testing
  //LittleFS.format();

  // Set default values for configuration
  configDefault();

  // Load configuration
  loadConfig();

  // Buttons and Default values for instances
  for (i = 0; i < INSTANCE_NUM; i++) {
    initButton(i, inst[i].bPin, LOW, 7, calcRep());
    inst[i].tBright = dconfig.iBrightness[i];
    inst[i].tState = inst[i].tDirection = false;
    inst[i].tValue = inst[i].tPower = 0;
    inst[i].tCntr = inst[i].tTransition = 0;
    inst[i].iStatus = true;
  }

  /* ------------------------------------------------------------------------------
     Enter Setup Portal on double reset
     ------------------------------------------------------------------------------ */
  if (drd.detect()) {
    #ifdef DEBUG_MODE
      writeDebug("Double reset detected. Starting Setup Portal through WiFi...", 1);
    #endif
    // Show config mode by flashing the LEDs at a 1 sec. interval
    ticker.attach(0.5, flashLed);
    //
    // WiFiManager
    //
    // Configure MQTT Parameters for the WiFi Manager
    WiFiManagerParameter custom_device_id("deviceId", "Device Id", dconfig.myId, 40);
    char prtnm[6];
    sprintf(prtnm, "%d", dconfig.mqttport);
    WiFiManagerParameter custom_mqtt_port("mqtt_port", "MQTT port", prtnm, 6);
    WiFiManagerParameter custom_mqtt_broker("mqtt_broker", "MQTT broker", dconfig.mqttbroker, 64);
    WiFiManagerParameter custom_mqtt_user("mqtt_user", "MQTT user", dconfig.mqttuser, 40);
    WiFiManagerParameter custom_mqtt_pass("mqtt_pass", "MQTT password", dconfig.mqttpass, 40);
    WiFiManagerParameter custom_mqtt_ns("mqtt_ns", "MQTT namespace", dconfig.namespc, 40);
    #ifdef MQTT_SIGN
      WiFiManagerParameter custom_mqtt_key("mqtt_key", "MQTT shared key", dconfig.mqttkey, 40);
    #endif
    WiFiManagerParameter custom_mqtt_name0("name1", "Light 1 name", dconfig.iName[0], 40);
    #if INSTANCE_NUM > 1
      WiFiManagerParameter custom_mqtt_name1("name2", "Light 2 name", dconfig.iName[1], 40);    
    #endif
    #if INSTANCE_NUM > 2
      WiFiManagerParameter custom_mqtt_name2("name3", "Light 3 name", dconfig.iName[2], 40);    
    #endif
    #if INSTANCE_NUM > 3
      WiFiManagerParameter custom_mqtt_name3("name4", "Light 4 name", dconfig.iName[4], 40);    
    #endif
    // Create the instance of WiFiManager
    // Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //
    // Set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    //
    // Timeout
    #ifdef CONFIG_TIMEOUT
      wifiManager.setConfigPortalTimeout(CONFIG_TIMEOUT);
    #endif
    //
    // Set static ip;
    //wifiManager.setAPStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));
    //
    // Add the MQTT parameters to the WiFi manager
    wifiManager.addParameter(&custom_device_id);
    wifiManager.addParameter(&custom_mqtt_broker);
    wifiManager.addParameter(&custom_mqtt_port);
    wifiManager.addParameter(&custom_mqtt_user);
    wifiManager.addParameter(&custom_mqtt_pass);
    wifiManager.addParameter(&custom_mqtt_ns);
    #ifdef MQTT_SIGN
      wifiManager.addParameter(&custom_mqtt_key);
    #endif
    wifiManager.addParameter(&custom_mqtt_name0);
    #if INSTANCE_NUM > 1
      wifiManager.addParameter(&custom_mqtt_name1);
    #endif
    #if INSTANCE_NUM > 2
      wifiManager.addParameter(&custom_mqtt_name2);
    #endif
    #if INSTANCE_NUM > 3
      wifiManager.addParameter(&custom_mqtt_name3);
    #endif
    //
    // Reset settings - Uncomment to clean parameters during testing
    wifiManager.resetSettings();
    //
    // Fetches SSID and PASS and tries to connect...
    // If it does not connect it starts an access point with the specified name
    // here TITLE and goes into a blocking loop awaiting configuration
    strcpy(tmpMsg0, PORTAL_SSID);
    strcat(tmpMsg0, deviceId);
    if (!wifiManager.startConfigPortal(tmpMsg0)) {
      #ifdef DEBUG_MODE
        writeDebug("Failed to connect and hit timeout.", 1);
      #endif
      delay(3000);
      // Reset and try again, or maybe put it to deep sleep
      #ifdef DEBUG_MODE
        writeDebug("WiFi configurator timeout. Reset!", 1);
      #endif
      ESP.reset();
      delay(3000);
    }
    //
    // Save User Configuration to FS
    //
    if (shouldSaveConfig) {
      // Read Additional parameters
      strcpy(dconfig.myId, custom_device_id.getValue());
      strcpy(prtnm, custom_mqtt_port.getValue());
      dconfig.mqttport = atoi(prtnm);
      strcpy(dconfig.mqttbroker, custom_mqtt_broker.getValue());
      strcpy(dconfig.mqttuser, custom_mqtt_user.getValue());
      strcpy(dconfig.mqttpass, custom_mqtt_pass.getValue());
      strcpy(dconfig.namespc, custom_mqtt_ns.getValue());
      #ifdef MQTT_SIGN
        strcpy(dconfig.mqttkey, custom_mqtt_key.getValue());
      #endif
      strcpy(dconfig.iName[0], custom_mqtt_name0.getValue());
      #if INSTANCE_NUM > 1
        strcpy(dconfig.iName[1], custom_mqtt_name1.getValue());
      #endif
      #if INSTANCE_NUM > 2
        strcpy(dconfig.iName[2], custom_mqtt_name2.getValue());
      #endif
      #if INSTANCE_NUM > 3
        strcpy(dconfig.iName[3], custom_mqtt_name3.getValue());
      #endif
      // Save the configuration
      saveConfig();
    }
    delay(2000);
    ticker.detach();
    #ifdef DEBUG_MODE
      writeDebug("WiFi configurator done. Restart!", 1);
    #endif
    ESP.restart();
  }
  
  /* ------------------------------------------------------------------------------
     Initialization
     ------------------------------------------------------------------------------ */
  
  #if defined(SAVE_ON_RESET) || defined(PERSISTENT_STATE)
    // Start EEPROM
    #ifdef DEBUG_MODE
      writeDebug("Init EEPROM", 1);
    #endif
    EEPROM.begin(512);
  #endif
  
  #ifdef SAVE_ON_RESET
    // Load last known status
    loadLastStatus();
  #endif
  
  // Start Network
  if (WiFi.SSID()) {
    wifiBegin();
  }
  #ifdef DEBUG_MODE
    else {
      writeDebug("Starting without WiFi", 1);
    }
  #endif
    
  /* ------------------------------------------------------------------------------
     Start OTA Updates
     ------------------------------------------------------------------------------ */
  #ifdef OTA_UPDATES
    otaEnabled = false;
    if (wifiSetup) {
      int wt = 0;
      // Wait up to 8 seconds for WiFi... otherwise start without OTA
      do {
        #ifdef DEBUG_MODE
          writeDebug("Wait for WiFi for OTA...", 2);
        #endif
        delay(500);
        ++wt;
        otaEnabled = (WiFi.waitForConnectResult() == WL_CONNECTED) ? true : false;
      } while (wt < 16 && !otaEnabled);
    }
    // If WiFi is available, configure OTA updates
    if (otaEnabled) {
      #ifdef OTA_HTTP
        // Web server update
        #ifdef DEBUG_MODE
          writeDebug("Request update to remote HTTP server", 1);
        #endif
        // Request for the latest image
        WiFiClient client;
        ESPhttpUpdate.setLedPin(PIN_LED1, HIGH);
        ESPhttpUpdate.onStart(http_update_started);
        ESPhttpUpdate.onEnd(http_update_finished);
        ESPhttpUpdate.onProgress(http_update_progress);
        ESPhttpUpdate.onError(http_update_error);
        #ifdef OTA_HTTP_URL
          t_httpUpdate_return ret = ESPhttpUpdate.update(client, OTA_HTTP_URL, OTA_IMAGE_VERSION);
        #else
          t_httpUpdate_return ret = ESPhttpUpdate.update(client, OTA_HTTP_SERVER, OTA_HTTP_PORT, OTA_SCRIPT_NAME, OTA_IMAGE_VERSION);
        #endif
        #ifdef DEBUG_MODE
          // Report results
          switch(ret) {
            case HTTP_UPDATE_FAILED:
              sprintf(tmpMsg, "OTA HTTP Update failed (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
              writeDebug(tmpMsg, 1);
              break;
            case HTTP_UPDATE_NO_UPDATES:
              writeDebug("OTA HTTP up to date.", 1);
              break;
            case HTTP_UPDATE_OK:
              writeDebug("OTA HTTP update ok.", 1); // may not be called since we reboot the ESP
              break;
          }
        #endif
        otaEnabled = false;
      #else
        // Setup Arduino updates
        #ifdef DEBUG_MODE
          writeDebug("Configure OTA port...", 2);
        #endif
        ArduinoOTA.setHostname("RDIMMER");
        #ifdef OTA_AR_PORT
          ArduinoOTA.setPort(OTA_AR_PORT);
        #endif
        #if defined(OTA_AR_HASH)
          ArduinoOTA.setPasswordHash(OTA_AR_HASH);
        #elif defined(OTA_AR_PASS)
          ArduinoOTA.setPassword(OTA_AR_PASS);
        #endif
        ArduinoOTA.onStart([]() {
          #ifdef DEBUG_MODE
            if (ArduinoOTA.getCommand() == U_FLASH) {
              writeDebug("Start updating sketch", 1);
            } else { // U_FS
              writeDebug("Start updating filesystem", 1);
            }
          #endif
          // Turn off lights
          for (int i = 0; i < INSTANCE_NUM; i++) {
            setLightState(i, 0, 0, 0, false, false);
          }
          delay(300);
          // End dimmer interruptions
          detachInterrupt(digitalPinToInterrupt(PIN_ZCROSS));
          timer1_detachInterrupt();
          // Disconnect MQTT
          mqttDisconnect();
          // Un-mount File System
          if (mounted) {
            LittleFS.end();
          }
          // Set the flag to start 
          otaUpdating = true;
        });
        ArduinoOTA.onEnd([]() {
          #ifdef DEBUG_MODE
            writeDebug("End OTA", 1);
          #endif
          otaUpdating = false;
        });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
          #ifdef DEBUG_MODE
            sprintf(tmpMsg0, "Progress: %u%%", (progress / (total / 100)));
            writeDebug(tmpMsg0, 1);
          #endif
        });
        ArduinoOTA.onError([](ota_error_t error) {
          #ifdef DEBUG_MODE
            sprintf(tmpMsg0, "Error[%u]: ", error);
            writeDebug(tmpMsg0, 1);
            if (error == OTA_AUTH_ERROR) {
              writeDebug("Auth Failed", 1);
            } else if (error == OTA_BEGIN_ERROR) {
              writeDebug("Begin Failed", 1);
            } else if (error == OTA_CONNECT_ERROR) {
              writeDebug("Connect Failed", 1);
            } else if (error == OTA_RECEIVE_ERROR) {
              writeDebug("Receive Failed", 1);
            } else if (error == OTA_END_ERROR) {
              writeDebug("End Failed", 1);
            }
          #endif
          otaUpdating = false;
        });
        ArduinoOTA.begin();
      #endif
    }
  #endif

  /* ------------------------------------------------------------------------------
     Dimmer setup
     ------------------------------------------------------------------------------ */
  #ifdef DEBUG_MODE
    writeDebug("Normal Boot", 1);
  #endif

  // Define MQTT Topics
  mqttDefineTopics();
  
  if (wifiSetup) {
    mqttBegin();
  }
  
  // Detect AC frequency and Zero Pulse duration
  #ifdef DEBUG_MODE
    writeDebug("Entering Dimmer auto configuration mode.", 3);
  #endif
  inSetup = true;
  netFirst = true;
  attachInterrupt(digitalPinToInterrupt(PIN_ZCROSS ), zeroCrossSetup, CHANGE);
  while (inSetup) {
    // Flash LEDs during setup (5 per second)
    tmrOff = millis();
    if(tmrOff > tmrOvf) {
      tmrOvf = tmrOff + 100;
      flashLed();
    }
    yield();
  }
  detachInterrupt(digitalPinToInterrupt(PIN_ZCROSS ));
  
  /* ------------------------------------------------------------------------------
     Start Dimmer
     ------------------------------------------------------------------------------ */
  // Start Dimmer interrupt cycle
  delay(200);
  #ifdef DEBUG_MODE
    writeDebug("Start dimmer.", 3);
  #endif
  // Set default state for indicators
  // Set the initial state for indicators
  for (i = 0; i < INSTANCE_NUM; i++) {
    inst[i].iStatus = dconfig.LedDefault;
  }
  // Interrups for ZC and setup for TRIAC timers
  tmr[0] = pulseH + frecTic;
  tmrOvf = offTic + tmr[0];
  timer1_isr_init();
  timer1_attachInterrupt(triggerTriacs);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  attachInterrupt(digitalPinToInterrupt(PIN_ZCROSS ), zeroCross, RISING);
  if (!dconfig.retain) {
    goLive = 1;
    #ifdef PERSISTENT_STATE
      getLightState();
    #endif
  }
  sysMillis = millis();
}

/** **********************************************************************************************
   Loop function
*********************************************************************************************** */
void loop() {
  /* ------------------------------------------------------------------------------
     Handle OTA updates
     ------------------------------------------------------------------------------ */
  #if defined(OTA_UPDATES) && !defined(OTA_HTTP)
    if (otaEnabled) {
      ArduinoOTA.handle();
    }
  #endif

  /* ------------------------------------------------------------------------------
     Validate network connections
     ------------------------------------------------------------------------------ */
  unsigned long cTmr = millis();
  if (netCheck()) {
    // Process pending messages
    if ((unsigned long)(cTmr - pubMillis) > MQTT_BUFFER_TMR) {
      // Process pending messages
      mqttPublishBuffer();
      pubMillis = cTmr;
    }
    // Refresh net check timer
    netMillis = cTmr;
    // Reset net fail second couter
    resetDelay = 0;
  } else {
    // Reconnect, but not every cycle, peventing conection hang times...
    //  Attemp to reconnect every NET_RECONNECT milliseconds (2.5 sec. aprox)
    if (netFirst || (unsigned long)(cTmr - netMillis) > NET_RECONNECT) {
      netFirst = false;
      netConnect();
      netMillis = cTmr;
    }
    // Refresh the  publish timer
    pubMillis = cTmr;
  }
  
  /* ------------------------------------------------------------------------------
     Keep track of running time
     ------------------------------------------------------------------------------ */
  systemTimer();
  
  /* ------------------------------------------------------------------------------
     Set action for buttons depending on status
     ------------------------------------------------------------------------------ */
  int btn;
  #ifdef DBL_CLICK
    char pyl[4];
  #endif
  for (int i = 0; i < INSTANCE_NUM; i++) {
    btn = getButtonStatus(i);
    // Button down
    if (btn == 1) {
      // Any user activity deactivates the GoLive sequence
      goLiveOff(); 
    }
    // Button UP = Toggle light status
    else if (btn == 2) {
      setLightState(i, 2, -1, -1, true, false);
    }
    // Button UP after long press = toggle switch or report light status
    else if (btn == 3 || btn == 4) {
      if (dconfig.iDimm[i]) {
        setLightState(i, 3, -1, 0, true, false);
      } else {
        setLightState(i, 2, -1, -1, true, false);
      }
    }
    // Button Repeat with dimmer enabled
    else if (dconfig.iDimm[i] && btn == 5) {
      setLightState(i, 1, -2, 0, false, false);
    }
    #ifdef DBL_CLICK
      else if(btn == 6) {
        #ifdef DEBUG_MODE
          writeDebug("Double-click button", 1);
        #endif
        sprintf(pyl, "dc%d", (i + 1));
        mqttPublish(2, i, pyl, false);
      }
      else if(btn == 7) {
        #ifdef DEBUG_MODE
          writeDebug("Triple-click button", 1);
        #endif
        sprintf(pyl, "tc%d", (i + 1));
        mqttPublish(2, i, pyl, false);
      }
    #endif
    #ifdef MULT_CLICK
      else if(btn == 8) {
        #ifdef DEBUG_MODE
          writeDebug("Multi-click button", 1);
        #endif
        goToSetupMode();
      }
    #endif
  }
  ////////////////////////////////////////////////////////////////////////////////
  // Set the LED indicator brightness
  ////////////////////////////////////////////////////////////////////////////////
  handleLed();
}
