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
  RRoble Duble Dimmer
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
  Flash Size: 4M (1M SPIFFS)
  Flash Mode: QIO
  Flash Frequency: 40 MHz
  Reset Method: ck
  Debug Port: Disabled
  Debug Level: None
  IwIP Varaint: v2 Lower Memory
  
  
  ------------------------------------------------------------------------------------------------
  Todo
  ------------------------------------------------------------------------------------------------

  - Default turn on value at boot / override persistent status
  - 
  
  ------------------------------------------------------------------------------------------------
  ------------------------------------------------------------------------------------------------
  @author Jesus Amozurrutia Elizalde <jamozu@gmail.com>
  @version 0.5.9a
  @date 2020/12/14
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
// Software version
#define SVERSION "0.5.9a"

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
#define OTA_UPDATES

// Enable debug messages to serial port
#define DEBUG_MODE

// Save staus information on reset (useful for debugging network issues)
#define SAVE_ON_RESET

// Allow clearing saved status via MQTT message
#define SAVE_ON_RESET_CLEAR

/* -------------------------------------------------------------------------------------------- 
    Debug
   -------------------------------------------------------------------------------------------- */

// Serial port speed
#define SERIAL_SPEED 74880


/* -------------------------------------------------------------------------------------------- 
    OTA Updates
   -------------------------------------------------------------------------------------------- */

// OTA Port
#define OTA_AR_PORT 8266

// Password for secure OTA Arduino Updates (not recommended)
//#define OTA_AR_PASS "myOtaPass"

// MD5 Hash for secure OTA Arduino Updates
//#define OTA_AR_HASH "266ba2d990fc2ff1ef34c571e4ea49de"
                            

// Define the next 4 parameters to update from an HTTP Server
//#define OTA_HTTP_SERVER "myDomain.com"
//#define OTA_HTTP_PORT 80
//#define OTA_SCRIPT_NAME "dimmerUpdater.php"
//#define OTA_IMAGE_VERSION SVERSION


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
#define BUFFER_SIZE 20

// MQTT Subscription Busy Time Out (ms)
#define MQTT_BUSY_TO 5000

// MQTT Buffer message timer (ms)
#define MQTT_BUFFER_TMR 350

// Re-subscribe to MQTT Topics (sec)
#define MQTT_RESUBSCRIBE 600


/* -------------------------------------------------------------------------------------------- 
   Dimmer definitions
   -------------------------------------------------------------------------------------------- */
// Go live sequence max duration (sec)
#define GO_LIVE_DURATION 60

// Mimimum power applied to lamps (10% is a safe value, but some  LED lamps require between 
//  15% and 20% to be clearly vissible
#define MIN_POW  10

// Maximum value for Minimum power
#define MAX_MIN_POW  50

// Button long press in ms
#define BTN_LONG_PRESS  800

// Button double click timer in ms
#define DBL_CLICK_INTERVAL  400

// Number of double clicks to trigger the Setup Portal
#define MULT_CLICK_COUNT 3

// Interval to get the double clicks, after the first double is detected in ms
#define MULT_CLICK_INTERVAL 800

// Transition speed on wakeup
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
#define CONFIG_MQTT_NAME01 ""             // "name01" configuration parameter default
#define CONFIG_MQTT_NAME02 ""             // "name02" configuration parameter default
#define CONFIG_LGT_MODE true              // "LgtMode" configuration parameter default
#define CONFIG_LGT1_DIMM true             // "Lgt1Dimm" configuration parameter default
#define CONFIG_LGT1_TRNS true             // "Lgt1Trns" configuration parameter default
#define CONFIG_LGT2_DIMM true             // "Lgt2Dimm" configuration parameter default
#define CONFIG_LGT2_TRNS true             // "Lgt2Trns" configuration parameter default
#define CONFIG_LED_BRIGHT 1000            // "LedBright" configuration parameter default
#define CONFIG_LED_DIMM 250               // "LedDimm" configuration parameter default
#define CONFIG_LED_DEFAULT true           // "LedDefault" configuration parameter default
#define CONFIG_DIMMING_TIME 25            // "dimmTime" configuration parameter default
#define CONFIG_EDGE_TIME 8                // "edgeTime" configuration parameter default
#define CONFIG_TRANSITION_ON 0            // "transitionOn" configuration parameter default
#define CONFIG_TRANSITION_OFF 0           // "transitionOff" configuration parameter default
#define CONFIG_MIN_POW1 10                // "minPow1" configuration parameter default
#define CONFIG_MIN_POW2 10                // "minPow2" configuration parameter default
#define CONFIG_DBL_TOPIC ""               // "topicDouble" configuration parameter default
#define CONFIG_DBL_PAYLOAD "dobleclick"   // "payloadDouble" configuration parameter default


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
   -------------------------------------------------------------------------------------------- */
#include <FS.h>                 // This needs to be first, or it all crashes and burns...
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
   Conditional libraries
   -------------------------------------------------------------------------------------------- */
#ifdef OTA_UPDATES
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif

#if defined(SAVE_ON_RESET) || defined(PERSISTENT_STATE)
#include <EEPROM.h>
#endif

/* -------------------------------------------------------------------------------------------- 
   General definitions
   -------------------------------------------------------------------------------------------- */

// System timer (one second)
#define SYS_TIMER 1000

// Number of MQTT instances
#ifdef DOUBLE_DIMMER
  #define INSTANCE_NUM 2
#else
  #define INSTANCE_NUM 1
#endif

#if defined(OTA_HTTP_SERVER) && defined(OTA_HTTP_PORT) && defined(OTA_SCRIPT_NAME) && defined(OTA_IMAGE_VERSION)
  #define OTA_HTTP
#endif

#ifdef SAVE_ON_RESET
  #define SAVE_ON_RESET_AT 10
#endif

/* -------------------------------------------------------------------------------------------- 
   Hardware
   -------------------------------------------------------------------------------------------- */

// Pins
#define PIN_ZCROSS 13        // Zero crossing signal pin
#ifdef DOUBLE_DIMMER
  #define PIN_TM1 4          // Triac 1 trigger signal
  #define PIN_TM2 5          // Triac 2 trigger signal
  #define PIN_BUTTON1 12     // Button 1
  #define PIN_BUTTON2 14     // Button 2
  #define PIN_LED1 2         // LED indicator 1
  #define PIN_LED2 0         // LED indicator 2
#else
  #define PIN_TM1 5          // Triac 1 trigger signal
  #define PIN_BUTTON1 12     // Button 1
  #define PIN_LED1 2         // LED indicator 1
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
  int mqttport;                 // MQTT port
  char mqttbroker[65];          // Broker domain or IP address
  char mqttuser[41];            // MQTT User
  char mqttpass[41];            // MQTT Password
  char namespc[41];             // MQTT Namespace
  char mqttkey[41];             // MQTT signature key (not implemented yet)
  bool retain;                  // MQTT Retain enable/disable 
  int keepAlive;                // MQTT Keepalive interval
  char myId[21];                // MQTT namespace Unique ID
  bool LgtMode;                 // General mode (Dimmer or Switch)
  int LedBright;                // LED Indicator level in bright mode (lights on)
  int LedDimm;                  // LED Indicator level in dimm mode level (lights off)
  boolean LedDefault;           // Default status for LED Indicators
  int dimmTime;                 // Dimming time (tenths of seconds)
  int edgeTime;                 // Time on the edge of the dimmable range (tenths of seconds)
  int transitionOn;             // Transition speed to turn ON
  int transitionOff;            // Transition speed to turn OFF
  char name01[41];              // Name for light bulb 1
  bool Lgt1Dimm;                // Light 1 mode (Dimmer or Switch)
  bool Lgt1Trns;                // Light 1 transitions enabled
  int minPow1;                  // Min. Power % for Light 1
  #ifdef DOUBLE_DIMMER
    char name02[41];              // Name for light bulb 2
    bool Lgt2Dimm;                // Light 2 mode (Dimmer or Switch)
    bool Lgt2Trns;                // Light 2 transitions enabled
    int minPow2;                  // Min. Power % for Light 2
  #endif
  #ifdef DBL_CLICK
    char topicDouble[81];         // Topic used to publish when double click is detected
    char payloadDouble[81];       // Payload for double click
  #endif
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

// MQTT Instance Topics
struct topicStruct {
  char uname[41];     // Light name
  char uid[41];       // Light unique ID
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
  int longInterval;
  int repInterval;
  int dblInterval;
  int state;
  int lastState;
  boolean repFlag;
  boolean dblFlag;
  unsigned long dbncInterval;
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
// Keep alive counter
int keepAlive = 0;
// Second counter (low precision)
unsigned long aliveCounter = 0;
// Temp variables to create messages and compound strings
char tmpMsg[341];
char tmpMsg0[101];
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
  boolean discovered = false;
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
// MQTT Retain Flag
boolean retain = true;
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

/* -------------------------------------------------------------------------------------------- 
   Light Configuration
   -------------------------------------------------------------------------------------------- */
// Transition speed to turn ON
int transitionOn = CONFIG_TRANSITION_ON;
// Transition speed to turn OFF
int transitionOff = CONFIG_TRANSITION_OFF;
// Dimm edges speed
int edgeSpeed = 10;
// Device mode (true = Dimmer; false = switch)
boolean LgtMode = CONFIG_LGT_MODE;
// Light 1 mode (true = Dimmer; false = switch)
boolean Lgt1Dimm = CONFIG_LGT1_DIMM;
// Light 1 transitions
boolean Lgt1Trns = CONFIG_LGT1_TRNS;
// Min. Power % for Light 1
int minPow1 = CONFIG_MIN_POW1;
#ifdef DOUBLE_DIMMER
  // Light 2 mode (true = Dimmer; false = switch)
  boolean Lgt2Dimm = CONFIG_LGT2_DIMM;
  // Light 2 transitions
  boolean Lgt2Trns = CONFIG_LGT2_TRNS;
  // Min. Power % for Light 2
  int minPow2 = CONFIG_MIN_POW2;
#endif

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
// Trigger Timer 1; Trigger first set of TRIACs
unsigned long tmr1 = 0;
// Trigger Timer 2; Trigger second set of TRIACs
unsigned long tmr2 = 0;
// Trigger Timer 3; Turn off triacs
unsigned long tmr3 = 0;
// Trigger Timer 4; Overflow Timer till next cycle
unsigned long tmr4 = 0;
// Trigger indicator (Indicates which triacs will be triggered and the sequence)
int trigger = 0;
// Trigger sequence counter
int trigSeq = 0;
// Count events at the edge of the brightness range for light 1
int edgeCntr1 = 0;
// Count events at the edge of the brightness range for light 2
int edgeCntr2 = 0;
// Zero Cross Pulse debounce timer
static unsigned long zcLast = 0;
// Missed Zero Cross Pulse counter
int zcSkip = 0;

/* -------------------------------------------------------------------------------------------- 
   Triac definitions and control
   -------------------------------------------------------------------------------------------- */
// Status for Light 1
boolean T1State = false;
// Current brightness
int T1Bright = 100;
// Light 1 Dimming direction
boolean T1Direction = false;
// Target power value (10 - 100)%
int T1Power = 100;
// Current light 1 power value
int T1Value = 0;
// Light 1 transition speed control
int T1Counter = 0;
// Duty Cycle value for light 1 in segments
int T1DuCy = 0;
#ifdef DOUBLE_DIMMER
  // Status for Light 2
  boolean T2State = false;
  // Current brightness
  int T2Bright = 100;
  // Light 2 Dimming direction
  boolean T2Direction = false;
  // Target power value (5 - 100)%
  int T2Power = 100;
  // Current light 2 power value
  int T2Value = 0;
  // Light 2 transition speed control
  int T2Counter = 0;
  // Duty Cycle value for light 2 in segments
  int T2DuCy = 0;
#endif

/* -------------------------------------------------------------------------------------------- 
   LED Configuration
   -------------------------------------------------------------------------------------------- */
// LED bright level (PWM)
int LedBright = CONFIG_LED_BRIGHT;
// LED dimm level (PWM)
int LedDimm = CONFIG_LED_DIMM;
// LED default status
boolean LedDefault = true;
// LEDs status
boolean ledsState = false;
// LED PWM brightness control
unsigned long LedPwm = 0;
// LED PWM counter overflow control
unsigned long LedOvf = 0;
// LED indicator status for Light 1
boolean Led1Status = true;
#ifdef DOUBLE_DIMMER
  // LED indicator status for Light 2
  boolean Led2Status = true;
#endif

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
   Doble click and multi click
   -------------------------------------------------------------------------------------------- */

#ifdef MULT_CLICK
  // Double click counter
  int multiClickCnt = 0;
  
  // Double click timer
  unsigned long multiClickTmr = 0;
  
  #ifdef DOUBLE_DIMMER
    // Double click counter for button 2
    int multiClickCnt2 = 0;
    
    // Double click timer for button 2
    unsigned long multiClickTmr2 = 0;
  #endif
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
void ICACHE_RAM_ATTR zeroCrossSetup () {
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
  // Detetc the frequency
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
        #ifdef DEBUG_MODE
          sprintf(tmpMsg0, "Bad frequency tics = %d", cycleP);
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
    // variations and may not be detected as it looks like debounce noise.
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
      ++pulseCnt2;
    } else if(pulseNum < 4 || (zcW > pulseS4 && zcW < pulseE4)) {
      if (pulseNum < 4) {
        pulseS4 = zcW * 0.95;
        pulseE4 = zcW * 1.05;
        pulseNum = 4;
      }
      ++pulseCnt2;
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
      #ifdef DEBUG_MODE
        sprintf(tmpMsg0, "Out of range ZC pulse = %d ", zcW);
        writeDebug(tmpMsg0, 3);
      #endif
    } 
    // Check if enough pulses have been detected
    else if (pulseCnt1 > SETUP_EVENTS || pulseCnt2 > SETUP_EVENTS || pulseCnt3 > SETUP_EVENTS || pulseCnt4 > SETUP_EVENTS || pulseCnt5 > SETUP_EVENTS) {
      // Get the mid of the ZC pulse with the mean of the width
      pulseH = (zcW / 2 + (zcW * .1));
      #ifdef DEBUG_MODE
        sprintf(tmpMsg0, "ZC pulse width detected = %d ", zcW);
        writeDebug(tmpMsg0, 3);
      #endif
    }
  }
}

/** **********************************************************************************************
   Zero Cross signal detector

   Detects the Zero Cross signal and resets the cycle timers to trigger the TRIACs.
*********************************************************************************************** */
void ICACHE_RAM_ATTR zeroCross () {
  ////////////////////////////////////////////////////////////////////////////////
  // Debounce
  ////////////////////////////////////////////////////////////////////////////////
  unsigned long now = micros();
  if((unsigned long)(now - zcLast) < ZC_DEBOUNCE) return;
  zcLast = now;
  ////////////////////////////////////////////////////////////////////////////////
  // Calculate TRIAC triggers
  ////////////////////////////////////////////////////////////////////////////////
  // Stop other interruptions during ZC calculations
  noInterrupts();
  // Reset the missed ZC pulse counter
  zcSkip = 0;
  // Reset timers
  tmr1 = 0;
  tmr2 = 10;
  tmr3 = 10;
  // Reset triggers
  unsigned long trig1 = 0;
  #ifdef DOUBLE_DIMMER
    unsigned long trig2 = 0;
  #endif
  ////////////////////////////////////////////////////////////////////////////////
  // Calc Triac 1
  ////////////////////////////////////////////////////////////////////////////////
  if(Lgt1Dimm) {
    // Dimmer 1 is enabled
    if (T1State) {
      // Light 1 is ON
      if (T1Value < T1Power) {
        if (transitionOn == 0) {
          T1Value = T1Power;
          T1DuCy = powerToSegment(T1Value);
        } else {
          // Fade in
          --T1Counter;
          if (T1Counter <= 0) {
            T1Counter = transitionOn;
            ++T1Value;
            T1DuCy = powerToSegment(T1Value);
          }
        }
      } else if (T1Value > T1Power) {
        if (transitionOn == 0) {
          T1Value = T1Power;
          T1DuCy = powerToSegment(T1Value);
        } else {
          // Fade out
          --T1Counter;
          if (T1Counter <= 0) {
            T1Counter = transitionOn;
            --T1Value;
            T1DuCy = powerToSegment(T1Value);
          }
        }
      }
    } else {
      // Light 1 is OFF
      if (T1Value > minPow1) {
        if (transitionOff == 0) {
          T1Value = 0;
          T1DuCy = powerToSegment(T1Value);
        } else {
          // Fade out
          --T1Counter;
          if (T1Counter <= 0) {
            T1Counter = transitionOff;
            --T1Value;
            T1DuCy = powerToSegment(T1Value);
          }
        }
      } else {
        T1Value = 0;
        T1DuCy = SEGMENTS;
      }
    }
    // Calculate trigger timer
    if (T1Value >= minPow1) {
      trig1 = T1DuCy * frecTic + pulseH;
    }
  }
  // If light is ON and Dimming is disabled, just turn TRIAC ON after zeroCross
  else if (T1State) {
    T1Value = 100;
    trig1 = pulseH;
  }
  // Light is off
  else {
    T1Value = 0;
  }
  #ifdef DOUBLE_DIMMER
    ////////////////////////////////////////////////////////////////////////////////
    // Calc Triac 2
    ////////////////////////////////////////////////////////////////////////////////
    if(Lgt2Dimm) {
      // Dimmer 2 is enabled
      if (T2State) {
        // Light 2 is ON
        if (T2Value < T2Power) {
          if (transitionOn == 0) {
            T2Value = T2Power;
            T2DuCy = powerToSegment(T2Value);
          } else {
            // Fade in
            --T2Counter;
            if (T2Counter <= 0) {
              T2Counter = transitionOn;
              ++T2Value;
              T2DuCy = powerToSegment(T2Value);
            }
          }
        } else if (T2Value > T2Power) {
          if (transitionOn == 0) {
            T2Value = T2Power;
            T2DuCy = powerToSegment(T2Value);
          } else {
            // Fade out
            --T2Counter;
            if (T2Counter <= 0) {
              T2Counter = transitionOn;
              --T2Value;
              T2DuCy = powerToSegment(T2Value);
            }
          }
        }
      } else {
        // Light 2 is OFF
        if (T2Value > minPow2) {
          if (transitionOff == 0) {
            T2Value = 0;
            T2DuCy = powerToSegment(T2Value);
          } else {
            // Fade out
            --T2Counter;
            if (T2Counter <= 0) {
              T2Counter = transitionOff;
              --T2Value;
              T2DuCy = powerToSegment(T2Value);
            }
          }
        } else {
          T2Value = 0;
          T2DuCy = SEGMENTS;
        }
      }
      // Calculate trigger timer
      if (T2Value >= minPow2) {
        trig2 = T2DuCy * frecTic + pulseH;
      }
    } 
    // If light is ON and Dimming is disabled, just turn TRIAC ON after zeroCross
    else if (T2State) {
      T2Value = 100;
      trig2 = pulseH;
    }
    // Light is off
    else {
      T2Value = 0;
    }
  #endif
  ////////////////////////////////////////////////////////////////////////////////
  // Calc timers
  ////////////////////////////////////////////////////////////////////////////////
  trigSeq = 0;
  #ifdef DOUBLE_DIMMER
    if (trig1 > trig2) {
      // Trigger T2 then T1
      if (trig2 > 0) {
        trigger = 5;
        tmr1 = trig2;
        tmr2 = trig1 - trig2;
        tmr3 = cycleTic - trig1 - offTic;
      }
      // Trigger T1 only
      else {
        trigger = 1;
        tmr1 = trig1;
        tmr2 = cycleTic - trig1 - offTic;
      }
    } else if (trig2 > trig1) {
      // Trigger T1 then T2
      if (trig1 > 0) {
        trigger = 4;
        tmr1 = trig1;
        tmr2 = trig2 - trig1;
        tmr3 = cycleTic - trig2 - offTic;
      } 
      // Trigger T2 only
      else {
        trigger = 2;
        tmr1 = trig2;
        tmr2 = cycleTic - trig2 - offTic;
      }
    }
    // Trigger T1 and T2
    else if(trig1 > 0) {
      trigger = 3;
      tmr1 = trig1;
      tmr2 = cycleTic - trig1 - offTic;
    } 
    // Both lights are off
    else {
      trigger = 0;
      tmr1 = pulseH + frecTic;
      tmr2 = cycleTic - tmr1 - offTic;
    }
  #else
    // Trigger T1
    if(trig1 > 0) {
      trigger = 1;
      tmr1 = trig1;
      tmr2 = cycleTic - trig1 - offTic;
    }
    // Light is off
    else {
      trigger = 0;
      tmr1 = pulseH + frecTic;
      tmr2 = cycleTic - tmr1 - offTic;
    }
  #endif
  // Timer 4 is set to trigger TRIACS in the next cycle in case the ZC pulse is missed to 
  // prevent flickering
  tmr4 = offTic + tmr1;
  trigSeq = 0;
  // Restart interruptions and set the timer for the first step
  interrupts();
  timer1_write(tmr1);
}

#ifdef DOUBLE_DIMMER
/** **********************************************************************************************
   Trigger TRIACs

   Timer interrupt used to trigger TRIACs.
   Each half cycle of the AC signal is divided in 4 steps:
   1 - Time between the middle of the Zero Cross pulse and the first set of TRIACs. If both 
       TRIACs have the same power level, the will be triggered.
   2 - Trigger the second set of TRIACs. If both TRIAcs are triggered in step 1, this step is 
       skipped.
   3 - Turn off trigger signal for all TRIACs / Sets the timer for the next half cycle.
   4 - Time between the Off time and the first set of TRIACs in the next half cycle (Step 1).
       This allows to continue even if a Zero Cross pulse is not detected, preventing flickering.
       Timer 4 depends on the AC frequency.
*********************************************************************************************** */
void ICACHE_RAM_ATTR triggerTriacs () {
  // If lights are off, turn off TRIAC signals
  if (T1Value == 0) {
    digitalWrite(PIN_TM1, LOW);
  }
  if (T2Value == 0) {
    digitalWrite(PIN_TM2, LOW);
  }
  // If both lights are off, we are done here.
  if(trigger == 0) return;
  // Increase the trigger sequence
  ++trigSeq;
  // Step 1
  if (trigSeq == 1) {
    // Set timer for step 2/3
    timer1_write(tmr2);
    // Trigger TRIAC 1
    if (trigger == 1) {
      digitalWrite(PIN_TM1, HIGH);
      // Jump to step 3
      ++trigSeq;
    }
    // Trigger TRIAC 2
    else if (trigger == 2 ) {
      digitalWrite(PIN_TM2, HIGH);
      // Jump to step 3
      ++trigSeq;
    } 
    // Trigger TRIAC 1 and TRIAC 2
    else if (trigger == 3 ) {
      digitalWrite(PIN_TM1, HIGH);
      digitalWrite(PIN_TM2, HIGH);
      // Jump to step 3
      ++trigSeq;
    }
    // Trigger TRIAC 1
    else if (trigger == 4 ) {
      digitalWrite(PIN_TM1, HIGH);
    } 
    // Trigger TRIAC 2
    else if (trigger == 5 ) {
      digitalWrite(PIN_TM2, HIGH);
    }
  }
  // Step 2
  else if(trigSeq == 2) {
    // Set timer for step 3
    timer1_write(tmr3);
    // Trigger TRIAC 2
    if (trigger == 4) {
      digitalWrite(PIN_TM2, HIGH);
    }
    // Trigger TRIAC 1
    else if (trigger == 5) {
      digitalWrite(PIN_TM1, HIGH);
    }
  }
  // Step 3
  else if(trigSeq == 3) {
    // Set timer for step 1 in the next cycle
    timer1_write(tmr4);
    trigSeq = 0;
    // Turn off TRIAC 1
    if (Lgt1Dimm || !T1State) {
      digitalWrite(PIN_TM1, LOW);
    }
    // Turn off TRIAC 2
    if (Lgt2Dimm || !T2State) {
      digitalWrite(PIN_TM2, LOW);
    }
  }
  // Count the number of times this function is executed
  ++zcSkip;
  // If the counter is not cleared for a few cycles (the ZC pulse has not been detected)
  // turn off TRIACs
  if (zcSkip > 9) {
    trigger = 0;
  }
}

#else
/** **********************************************************************************************
   Trigger TRIAC

   Timer interrupt used to trigger the TRIAC.
   Each half cycle of the AC signal is divided in 3 
   1 - Time between Zero Cross signal and the TRIAC
       Adds the half length of the Zero Cross Pulse
   2 - Turn off trigger signal for the TRIAC / Sets the timer for the next
       half cycle. (if the ZC pulse is not detected the triacs are triggered
       anyway, which prevents flickering)
   3 - Time between the Off time and the first set of TRIACs in the next cycle
       This allows to continue even if a ZC is missed, and prevents flickering
******************************************************************* **************************** */
void ICACHE_RAM_ATTR triggerTriacs () {
  // If light is off, turn off TRIAC signal
  if (T1Value == 0) {
    digitalWrite(PIN_TM1, LOW);
  }
  // If light is off, we are done here.
  if(trigger == 0) return;
  // Increase the trigger sequence
  ++trigSeq;
  if (trigSeq == 1) {
    timer1_write(tmr2);
    if (trigger == 1) {
      digitalWrite(PIN_TM1, HIGH);
    }
  } else if(trigSeq == 2) {
    timer1_write(tmr4);
    if (Lgt1Dimm || !T1State) {
      digitalWrite(PIN_TM1, LOW);
    }
    trigSeq = 0;
  }
  ++zcSkip;
  if (zcSkip > 5) {
    trigger = 0;
  }
}
#endif

/** **********************************************************************************************
   Converts power % to trigger segment
   
   @param v Power value (0-100).
*********************************************************************************************** */
int powerToSegment (int v) {
  if (v < 53) {
    if (v < 24) {
      if (v <= 10) {
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
   @param light Light bulb number [1|2].
*********************************************************************************************** */
int getPower (int bright, int light) {
  float sg = MIN_POW ;
  if (light == 1) {
    sg = minPow1;
  }
  #ifdef DOUBLE_DIMMER
    else if (light == 2) {
      sg = minPow2;
    }
  #endif
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
  unsigned long cTmr = millis();
  if ((unsigned long)(cTmr - sysMillis) < SYS_TIMER) {
    return;
  }
  // One second passed
  sysMillis = cTmr;
  ++aliveCounter;
  // Count seconds without network connection
  ++resetDelay;
  // Keep alive message to broker
  ++keepAlive;
  if (keepAlive >= dconfig.keepAlive) {
    keepAlive = 0;
    mqttPublish(0, 0, "online");
  }
  // Re-subscribe at 6 min intervals
  ++reSubscribe;
  if (reSubscribe >= MQTT_RESUBSCRIBE) {
    #ifdef DEBUG_MODE
      writeDebug("Start periodic re-subscription", 1);
    #endif
    mqttSubscribe();
  }
  // Go Live sequence go off after 1 minute
  if (goLive == 2 && aliveCounter > GO_LIVE_DURATION) {
    goLiveOff();
  }
  // Go Live unsubscriptions expire
  if (goLiveSwitch > 0 && aliveCounter > (GO_LIVE_DURATION + 30)) {
    goLiveSwitch = 0;
  }
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
  if (ledsState && Led1Status) {
    digitalWrite(PIN_LED1, HIGH);
  } else {
    digitalWrite(PIN_LED1, LOW);
  }
  #ifdef DOUBLE_DIMMER
    if (ledsState && Led2Status) {
      digitalWrite(PIN_LED2, HIGH);
    } else {
      digitalWrite(PIN_LED2, LOW);
    }
  #endif
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
    // Reboot only if a network connectio has been detected since boot and the lights are off
    while (canReboot) {
      #ifdef DOUBLE_DIMMER
        if (T1State || T2State) break;
      #else
        if (T1State) break;
      #endif
      #ifdef SAVE_ON_RESET
        // Save the parameters that triggered the reset process
        saveStatus();
      #endif
      // Reboot
      ESP.restart();
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
  goLive = 0;
  if (mqttStatus) {
    goLiveSwitch = 0;
    for (int i = 0; i < INSTANCE_NUM; i++) {
      ++goLiveSwitch;
      mqttClient.unsubscribe(mqttTopics[i].state);
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
   MQTT Topics

   Define MQTT topics
*********************************************************************************************** */
void mqttDefineTopics () {
  // Device STATUS topic
  sprintf(tmpMsg, "%s/%s/%s/state", dconfig.namespc, INST_TYPE, dconfig.myId);
  strlcpy(topicState, tmpMsg, sizeof(topicState));
  // Device SET topic
  #ifdef MQTT_CONFIG
    sprintf(tmpMsg, "%s/%s/%s/set", dconfig.namespc, INST_TYPE, dconfig.myId);
    strlcpy(topicSet, tmpMsg, sizeof(topicSet));
  #endif
  // Instances
  for (int i = 0; i < INSTANCE_NUM; i++) {
    // Define the instance Unique ID
    sprintf(tmpMsg0, "M%s%0d", deviceId, (i + 1));
    strlcpy(mqttTopics[i].uid, tmpMsg0, sizeof(mqttTopics[i].uid));
    // Define the instance Name
    if (strlen(mqttTopics[i].uname) > 0) {
      sprintf(tmpMsg0, "%s", mqttTopics[i].uname);
    }
    // Instance STATE topic
    sprintf(tmpMsg, "%s/%s/%s/state", dconfig.namespc, INST_TYPE, tmpMsg0);
    strlcpy(mqttTopics[i].state, tmpMsg, sizeof(mqttTopics[i].state));
    // Instance SET topic
    sprintf(tmpMsg, "%s/%s/%s/set", dconfig.namespc, INST_TYPE, tmpMsg0);
    strlcpy(mqttTopics[i].set, tmpMsg, sizeof(mqttTopics[i].set));
  }
}

/** **********************************************************************************************
   MQTT Setup
*********************************************************************************************** */
void mqttBegin () {
  if (mqttSetup) { return; }
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
      mqttPublish(0, 0, "golive");
    } else {
      mqttPublish(0, 0, "online");
      if (goLive == 0) {
        mqttPublishStatus(0);
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
  if (goLiveSwitch == 0) {
    mqttSubscribe();
  }
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
  mqttEventCtr = 0;
}

/** **********************************************************************************************
   MQTT subscribe to topics
*********************************************************************************************** */
void mqttSubscribe () {
  if (!mqttStatus) return;
  #ifdef MQTT_DISCOVERY
    mqttDiscovery();
  #endif
  reSubscribe = 0;
  unsigned long cTmr = millis();
  #ifdef MQTT_CONFIG
    mqttClient.subscribe(topicSet, 0);
    ++mqttBusy0;
    mqttTmr0 = cTmr;
    #ifdef DEBUG_MODE
      writeDebug("Subscribe to Device SET topic: ", 4, true);
      writeDebug(topicSet);
    #endif
  #endif
  for (int i = 0; i < INSTANCE_NUM; i++) {
    mqttClient.subscribe(mqttTopics[i].set, 0);
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
   Publish a message to MQTT Broker

   @param tIndex Topic index (0 = device state; 1 = instance state; 2 = doubleClick)
   @param instance Instance index
   @param payload MQTT Payload
   @param useBuffer Use Buffer, if MQTT is ofline in non-blocking mode
*********************************************************************************************** */
void mqttPublish (int tIndex, int instance, const char* payload, bool useBuffer) {
  char topic[81];
  bool ret = retain;
  if (tIndex == 0) {
    strlcpy(topic, topicState, sizeof(topic));
    ret = false;
  } 
  else if (tIndex == 1) {
    strlcpy(topic, mqttTopics[instance].state, sizeof(topic));
  }
  #ifdef DBL_CLICK
    else if (tIndex == 2) {
      strlcpy(topic, dconfig.topicDouble, sizeof(topic));
      ret = false;
    }
  #endif
  if (mqttStatus && mqttBusy0 == 0) {
    mqttClient.publish(topic, MQTT_PUB_QOS, ret, payload);
    ++mqttEventCtr;
    #ifdef DEBUG_MODE
      writeDebug("Publish on topic: ", 4, true);
      writeDebug(topic, 0, true);
      sprintf(tmpMsg0, " :: %d ", mqttEventCtr);
      writeDebug(tmpMsg0);
      writeDebug(payload);
    #endif
  } else if (useBuffer) {
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
  if (buffStat == buffPub) { return; }
  if (!mqttStatus) { return; }
  mqttClient.publish(buffTop[buffPub], MQTT_PUB_QOS, buffRet[buffPub], buffMsg[buffPub]);
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

   @param light Light number [0|1|2]. 0 indicates both lights.
*********************************************************************************************** */
void mqttPublishStatus (int light) {
  /* ------------------------------------------------------------------------------------------ 
     Publish the status for each MQTT Instance
    ------------------------------------------------------------------------------------------- */
  if (light == 0 || light == 1) {
    sprintf(tmpMsg, "{\"state\":\"%s\",\"brightness\":%d,\"power\":%d,\"indicator\":\"ON\"}", ((T1State) ? "ON" : "OFF"), T1Bright, T1Power, ((Led1Status) ? "ON" : "OFF"));
    mqttPublish(1, 0, tmpMsg, true);
  }
  #ifdef DOUBLE_DIMMER
    if(light == 0 || light == 2) {
      sprintf(tmpMsg, "{\"state\":\"%s\",\"brightness\":%d,\"power\":%d,\"indicator\":\"ON\"}", ((T2State) ? "ON" : "OFF"), T2Bright, T2Power, ((Led2Status) ? "ON" : "OFF"));
      mqttPublish(1, 1, tmpMsg, true);
    }
  #endif
}

#ifdef MQTT_DISCOVERY
/** **********************************************************************************************
   Publish auto discovery message
*********************************************************************************************** */
void mqttDiscovery() {
  if (discovered) return;
  char tmpMsgN[451];
  discovered = true;
  for (int i = 0; i < INSTANCE_NUM; i++) {
    sprintf(tmpMsg0, "homeassistant/light/%s/config", mqttTopics[i].uname);
    sprintf(tmpMsgN, "{\"name\":\"%s\",\"uniq_id\":\"%s\",\"schema\":\"json\",\"brightness\":true,\"bri_scl\":100,\"opt\":false,\"stat_t\":\"%s\",\"cmd_t\":\"%s\",\"ret\":%s,\"dev\":{\"ids\":[\"D%s\"],\"name\":\"%s\",\"mf\":\"JAE\",\"mdl\":\"RDm05\",\"sw\":\"%s\"}}", mqttTopics[i].uname, mqttTopics[i].uid, mqttTopics[i].state, mqttTopics[i].set, ((retain) ? "true" : "false"), deviceId, dconfig.myId, SVERSION);
    mqttClient.publish(tmpMsg0, MQTT_PUB_QOS, true, tmpMsgN);
    #ifdef DEBUG_MODE
      writeDebug("Discovery on topic: ", 4, true);
      writeDebug(tmpMsg0, 0, true);
      writeDebug(" ", 0, true);
      writeDebug(tmpMsgN);
    #endif
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
  // JSON V5.0
  //DynamicJsonBuffer jsonBuffer;
  //JsonObject& json = jsonBuffer.parseObject(payload);
  //if (!json.success()) {
  //  #ifdef DEBUG_MODE
  //    writeDebug("Invalid JSON Structure", 4);
  //  #endif
  //  return;
  //}
  // JSON V6.0
  DynamicJsonDocument json(1024);
  auto error = deserializeJson(json, payload);
  if (error) {
    #ifdef DEBUG_MODE
      writeDebug("Invalid JSON Structure", 4);
    #endif
    return;
  }
  //
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
    // An ACTION is requested...
    else if (json.containsKey("action") && json["action"].is<char*>()) {
      // Get the device STATUS
      if (strcmp(json["action"], "getStatus") == 0) {
        if (json.containsKey("instance") && (json["instance"].is<signed int>() || json["instance"].is<unsigned int>())) {
          if (json["instance"] > 0 && json["instance"] <= INSTANCE_NUM) {
            mqttPublishStatus(json["instance"]);
          }
        } else {
          mqttPublishStatus(0);
        }
      } 
      // Get Device information
      else if (strcmp(json["action"], "getInfo") == 0) {
        #ifdef SAVE_ON_RESET
          sprintf(tmpMsg, "%s / BT: %d / AL: %d / RC: %d / RT: %d / RST: %d | RD: %d / AL: %d / RC: %d / RT: %d | RD: %d / AL: %d / RC: %d / RT: %d | RD: %d / AL: %d / RC: %d / RT: %d ", TITLE, debugBoot, aliveCounter, wifiRstCtr, wifiRstTtl, debugRst, resetDelay1, aliveCounter1, wifiRstCtr1, wifiRstTtl1, resetDelay2, aliveCounter2, wifiRstCtr2, wifiRstTtl2, resetDelay3, aliveCounter3, wifiRstCtr3, wifiRstTtl3);
          mqttPublish(0, 0, tmpMsg);
        #else
          sprintf(tmpMsg0, "%s / ALIVE: %d / RstCtr: %d / RstTtl: %d", TITLE, aliveCounter, wifiRstCtr, wifiRstTtl);
          mqttPublish(0, 0, tmpMsg0);
        #endif
      }
      // Change state of LED indicators
      else if (strcmp(json["action"], "indicators") == 0) {
        // Set duty cycle 
        if (json.containsKey("bright") && (json["bright"].is<signed int>() || json["bright"].is<unsigned int>())) {
          if (json["bright"] > 100) {
             json["bright"] = 100;
          } else if (json["bright"] < 0) {
             json["bright"] = 0;
          }
          if (json["bright"] == 0) {
            LedBright = dconfig.LedBright;
          } else {
            pulseNum = json["bright"];
            LedBright = pulseNum * 10;
          }
        }
        if (json.containsKey("dimm") && (json["dimm"].is<signed int>() || json["dimm"].is<unsigned int>())) {
          if (json["dimm"] > 100) {
             json["dimm"] = 100;
          } else if (json["dimm"] < 0) {
             json["dimm"] = 0;
          }
          if (json["dimm"] == 0) {
            LedDimm = dconfig.LedDimm;
          } else {
            pulseNum = json["dimm"];
            LedDimm = pulseNum * 10;
          }
        }
      }
      #ifdef MQTT_CONFIG
        // Get the device configuration
        else if (strcmp(json["action"], "getConfig") == 0) {
          File configFile = SPIFFS.open("/config.json", "r");
          if (configFile) {
            size_t size = configFile.size();
            // Allocate a buffer to store contents of the file.
            std::unique_ptr<char[]> buf(new char[size]);
            configFile.readBytes(buf.get(), size);
            // Publish config string
            mqttPublish(0, 0, buf.get());
            configFile.close();
          }
        } 
        // Configure
        else if (strcmp(json["action"], "config") == 0) {
          if (json.containsKey("secret") && json["secret"].is<char*>()) {
            if (strcmp(json["secret"], "secret") == 0) {
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
                discovered = false;
              }
              if (json.containsKey("mqttkey") && json["mqttkey"].is<char*>()) {
                strlcpy(dconfig.mqttkey, json["mqttkey"], sizeof(dconfig.mqttkey));
                configChanged = true;
              }
              if (json.containsKey("retain") && json["retain"].is<bool>()) {
                dconfig.retain = json["retain"];
                retain = dconfig.retain;
                configChanged = true;
                discovered = false;
              }
              if (json.containsKey("keepAlive") && (json["keepAlive"].is<signed int>() || json["keepAlive"].is<unsigned int>())) {
                dconfig.keepAlive = json["keepAlive"];
                configChanged = true;
              }
              if (json.containsKey("myId") && json["myId"].is<char*>()) {
                strlcpy(dconfig.myId, json["myId"], sizeof(dconfig.myId));
                configChanged = true;
                discovered = false;
              }
              /* --------------------------------------------------------------------------------------------  */
              if (json.containsKey("name01") && json["name01"].is<char*>()) {
                strlcpy(dconfig.name01, json["name01"], sizeof(dconfig.name01));
                configChanged = true;
                discovered = false;
              }
              #ifdef DOUBLE_DIMMER
              if (json.containsKey("name02") && json["name02"].is<char*>()) {
                strlcpy(dconfig.name02, json["name02"], sizeof(dconfig.name02));
                configChanged = true;
                discovered = false;
              }
              #endif
              if (json.containsKey("LgtMode") && json["LgtMode"].is<bool>()) {
                dconfig.LgtMode = json["LgtMode"];
                LgtMode = dconfig.LgtMode;
                setLightMode();
                configChanged = true;
              }
              if (json.containsKey("Lgt1Dimm") && json["Lgt1Dimm"].is<bool>()) {
                dconfig.Lgt1Dimm = json["Lgt1Dimm"];
                if (LgtMode) {
                  Lgt1Dimm = dconfig.Lgt1Dimm;
                }
                configChanged = true;
              }
              if (json.containsKey("Lgt1Trns") && json["Lgt1Trns"].is<bool>()) {
                dconfig.Lgt1Trns = json["Lgt1Trns"];
                if (LgtMode) {
                  Lgt1Trns = dconfig.Lgt1Trns;
                }
                configChanged = true;
              }
              #ifdef DOUBLE_DIMMER
              if (json.containsKey("Lgt2Dimm") && json["Lgt2Dimm"].is<bool>()) {
                dconfig.Lgt2Dimm = json["Lgt2Dimm"];
                if (LgtMode) {
                  Lgt2Dimm = dconfig.Lgt2Dimm;
                }
                configChanged = true;
              }
              if (json.containsKey("Lgt2Trns") && json["Lgt2Trns"].is<bool>()) {
                dconfig.Lgt2Trns = json["Lgt2Trns"];
                if (LgtMode) {
                  Lgt2Trns = dconfig.Lgt2Trns;
                }
                configChanged = true;
              }
              #endif
              if (json.containsKey("LedBright") && (json["LedBright"].is<signed int>() || json["LedBright"].is<unsigned int>())) {
                if (json["LedBright"] > CONFIG_LED_BRIGHT) {
                   json["LedBright"] = CONFIG_LED_BRIGHT;
                } else if (json["LedBright"] < 0) {
                   json["LedBright"] = 0;
                }
                dconfig.LedBright = json["LedBright"];
                LedBright = dconfig.LedBright;
                configChanged = true;
              }
              if (json.containsKey("LedDimm") && (json["LedDimm"].is<signed int>() || json["LedDimm"].is<unsigned int>())) {
                if (json["LedDimm"] > CONFIG_LED_BRIGHT) {
                   json["LedDimm"] = CONFIG_LED_BRIGHT;
                } else if (json["LedDimm"] < 0) {
                   json["LedDimm"] = 0;
                }
                dconfig.LedDimm = json["LedDimm"];
                LedDimm = dconfig.LedDimm;
                configChanged = true;
              }
              if (json.containsKey("LedDefault") && json["LedDefault"].is<bool>()) {
                dconfig.LedDefault = json["LedDefault"];
                configChanged = true;
              }
              if (json.containsKey("dimmTime") && (json["dimmTime"].is<signed int>() || json["dimmTime"].is<unsigned int>())) {
                if (json["dimmTime"] > 100) {
                   json["dimmTime"] = 100;
                } else if (json["dimmTime"] < 0) {
                   json["dimmTime"] = 0;
                }
                dconfig.dimmTime = json["dimmTime"];
                defineEdge();
                defineButtons();
                configChanged = true;
              }
              if (json.containsKey("edgeTime") && (json["edgeTime"].is<signed int>() || json["edgeTime"].is<unsigned int>())) {
                if (json["edgeTime"] > 30) {
                   json["edgeTime"] = 30;
                } else if (json["edgeTime"] < 0) {
                   json["edgeTime"] = 0;
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
                transitionOn = dconfig.transitionOn;
                configChanged = true;
              }
              if (json.containsKey("transitionOff") && (json["transitionOff"].is<signed int>() || json["transitionOff"].is<unsigned int>())) {
                if (json["transitionOff"] > 100) {
                   json["transitionOff"] = 100;
                } else if (json["transitionOff"] < 0) {
                   json["transitionOff"] = 0;
                }
                dconfig.transitionOff = json["transitionOff"];
                transitionOff = dconfig.transitionOff;
                configChanged = true;
              }
              if (json.containsKey("minPow1") && (json["minPow1"].is<signed int>() || json["minPow1"].is<unsigned int>())) {
                if (json["minPow1"] > MAX_MIN_POW) {
                   json["minPow1"] = MAX_MIN_POW;
                } else if (json["minPow1"] < MIN_POW) {
                   json["minPow1"] = MIN_POW;
                }
                dconfig.minPow1 = json["minPow1"];
                minPow1 = dconfig.minPow1;
                configChanged = true;
              }
              #ifdef DOUBLE_DIMMER
              if (json.containsKey("minPow2") && (json["minPow2"].is<signed int>() || json["minPow2"].is<unsigned int>())) {
                if (json["minPow2"] > MAX_MIN_POW) {
                   json["minPow2"] = MAX_MIN_POW;
                } else if (json["minPow2"] < MIN_POW) {
                   json["minPow2"] = MIN_POW;
                }
                dconfig.minPow2 = json["minPow2"];
                minPow2 = dconfig.minPow2;
                configChanged = true;
              }
              #endif
              // Save configuration changes
              if (configChanged) {
                saveConfig();
              }
              #ifdef DEBUG_MODE
                else {
                    writeDebug("No changes in the configuration.", 1);
                }
              #endif
            }
            #ifdef DEBUG_MODE
              else {
                writeDebug("Invalid 'secret'.", 4);
              }
            #endif
          }
          #ifdef DEBUG_MODE
            else {
              writeDebug("Missing key 'secret'.", 4);
            }
          #endif
        }
      #else
        #ifdef DEBUG_MODE
          else if (strcmp(json["action"], "getConfig") == 0) {
            writeDebug("Remote configuration disabled.'.", 1);
          } 
          // Configure
          else if (strcmp(json["action"], "config") == 0) {
            writeDebug("Remote configuration disabled.'.", 1);
          }
        #endif
      #endif
      #ifdef MQTT_RESET
        // Reset the Device
        else if (strcmp(json["action"], "reset") == 0) {
          mqttPublish(0, 0, "reset");
          #ifdef DEBUG_MODE
            writeDebug("Reset requested", 1);
          #endif
          ESP.restart();
        }
      #endif
      #if defined(SAVE_ON_RESET) && defined(SAVE_ON_RESET_CLEAR)
        // Clear status from EEPROM
        else if (strcmp(json["action"], "clearmem") == 0) {
          clearStatus();
        }
      #endif
      #ifdef DEBUG_MODE
        // Invalid action 
        else {
          writeDebug("Invalid configuration action.", 4);
        }
      #endif
    }
    #ifdef DEBUG_MODE
      else {
        writeDebug("Invalid configuration message. No action defined.", 4);
      }
    #endif
    return;
  }
  bool fGo = false;
  bool fLive = false;
  bool fPub = false;
  bool fTop = false;
  int stt = -1;
  int brght = -1;
  int trns = -1;
  // Loop instance topics
  for (int i = 0; i < INSTANCE_NUM; i++) {
    fGo = false;
    // Validate SET or STATE (GoLive) topics
    if (strcmp(topic, mqttTopics[i].set) == 0) {
      if (properties.retain) {
        #ifdef DEBUG_MODE
          writeDebug("Instances do not accept retained messages on SET topics.", 4);
        #endif
        fTop = true;
      } else {
        fGo = true;
        fLive = false;
      }
    } else if (goLive == 2 && strcmp(topic, mqttTopics[i].state) == 0) {
      fGo = true;
      fLive = true;
      #ifdef DEBUG_MODE
        writeDebug("Retained Message", 4);
      #endif
    }
    if (fGo) {
      fTop = true;
      if (json.containsKey("action") && json["action"].is<char*>()) {
        if (properties.retain) {
          #ifdef DEBUG_MODE
            writeDebug("Topic actions do not accept retained messages.", 4);
          #endif
        } else if (strcmp(json["action"], "getStatus") == 0) {
          mqttPublishStatus(i + 1);
        } else if (strcmp(json["action"], "getConfig") == 0) {
          mqttPublish(1, i, deviceId);
        } 
        #ifdef DEBUG_MODE
          else {
            writeDebug("Invalid action", 4);
          }
        #endif
      } else {
        fPub = false;
        fGo = false;
        stt = -1;
        brght = -1;
        trns = -1;
        /* --------------------------------------------------------------------------------------------  */
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
          fPub = true;
        }
        if (json.containsKey("transition") && (json["transition"].is<signed int>() || json["transition"].is<unsigned int>())) {
          if (json["transition"] > 10) {
            json["transition"] = 10;
          } else if (json["transition"] <= 0) {
            json["transition"] = 0;
          }
          trns = json["transition"];
        }
        if (json.containsKey("indicator") && json["indicator"].is<char*>()) {
          if (strcmp(json["indicator"], "OFF") == 0) {
            if (i == 0) {
              Led1Status = false;
            } 
            #ifdef DOUBLE_DIMMER
              else {
                Led2Status = false;
              }
            #endif
          } else {
            if (i == 0) {
              Led1Status = true;
            } 
            #ifdef DOUBLE_DIMMER
              else {
                Led2Status = true;
              }
            #endif
          }
          fPub = true;
        }
        if (fPub) {
          if (fLive) {
            setLightState(i + 1, stt, brght, CONFIG_TRANSITION_WAKE, false, false);
            ++retainCnt;
            if (retainCnt >= INSTANCE_NUM) {
              goLiveOff();
            }
          } else {
            setLightState(i + 1, stt, brght, trns, true, false);
          }
        }
      }
    }
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
  if (LgtMode) {
    Lgt1Dimm = dconfig.Lgt1Dimm;
    Lgt1Trns = dconfig.Lgt1Trns;
    #ifdef DOUBLE_DIMMER
      Lgt2Dimm = dconfig.Lgt2Dimm;
      Lgt2Trns = dconfig.Lgt2Trns;
    #endif
  } else {
    Lgt1Dimm = false;
    Lgt1Trns = false;
    #ifdef DOUBLE_DIMMER
      Lgt2Dimm = false;
      Lgt2Trns = false;
    #endif
  }
}

/** **********************************************************************************************
   Set light state
   
   @param light Light number [1|2]
   @param state Light state. 
      - 0 = OFF 
      - 1 = ON 
      - 2 = Toogle
      - 3 = no action
   @param brightness Brightness [0 - 100]; -1 = No change
   @param transition Brightness [0 - 100]; -1 = default
   @param pub Publish / Save new status
   @param remote Values set remotely
*********************************************************************************************** */
void setLightState(int light, int state, int brightness, int transition, bool pub, bool remote) {
  bool newState = false;
  // Set new state
  if (state == 0) {
    //chg = true;
    newState = false;
  } else if (state == 1) {
    //chg = true;
    newState = true;
  } else if (state == 2) {
    //chg = true;
    if (light == 1) newState = !T1State;
    #ifdef DOUBLE_DIMMER
      else if (light == 2) newState = !T2State;
    #endif
  } else if (state == 3) {
    if (light == 1) newState = T1State;
    #ifdef DOUBLE_DIMMER
      else if (light == 2) newState = T2State;
    #endif
  }
  // Set transition speed
  if (transition >= 0) {
    if (newState) {
      transitionOn = transition;
    } else {
      transitionOff = transition;
    }
  } else {
    transitionOn = dconfig.transitionOn;
    transitionOff = dconfig.transitionOff;
  }
  // Calculate light 1
  if (light == 1) {
    if (state != 3) {
      if (Lgt1Dimm) {
        if (brightness >= 0) {
          T1Bright = brightness;
        }
        if (newState) T1Power = getPower(T1Bright, 1);
        else T1Power = 0;
        if (!Lgt1Trns || transition == 0) {
          if(newState) {
            T1Value = T1Power;
          } else {
            T1Value = 0;
            T1DuCy = SEGMENTS;
          }
        }
        #ifdef DEBUG_MODE
          if (remote) {
            writeDebug("Light 1 brightness set remotely", 3);
          } else {
            sprintf(tmpMsg0, "Light 1 brightness = %d", T1Bright);
            writeDebug(tmpMsg0, 3);
          }
        #endif
        if (Lgt1Trns && newState && T1Value < minPow1) {
          T1Value = minPow1;
        }
        T1DuCy = powerToSegment(T1Value);
      } else {
        T1Value = (newState) ? 100 : 0;
        T1DuCy = (newState) ? 0 : SEGMENTS;
      }
      T1State = newState;
    }
    // If publish flag is off, end here.
    if (!pub) return;
    // Save state
    #ifdef PERSISTENT_STATE
      if (!retain) {
        saveLightState(1);
      }
    #endif
    // Publish new state
    mqttPublishStatus(1);
    #ifdef DEBUG_MODE
      // Show new status
      if (state != 3) {
        if (T1State) {
          if (remote) {
            writeDebug("Light 1 turned ON remotely", 3);
          } else {
            writeDebug("Light 1 On", 3);
          }
        } else {
          if (remote) {
            writeDebug("Light 1 turned OFF remotely", 3);
          } else {
            writeDebug("Light 1 Off", 3);
          }
        }
      }
    #endif
    return;
  }
  #ifdef DOUBLE_DIMMER
    if (light != 2) return;
    if (state != 3) {
      if (Lgt2Dimm) {
        if (brightness >= 0) {
          T2Bright = brightness;
        }
        if (newState) T2Power = getPower(T2Bright, 1);
        else T2Power = 0;
        if (!Lgt2Trns || transition == 0) {
          if(newState) {
            T2Value = T2Power;
          } else {
            T2Value = 0;
            T2DuCy = SEGMENTS;
          }
        }
        #ifdef DEBUG_MODE
          if (remote) {
            writeDebug("Light 2 brightness set remotely", 3);
          } else {
            sprintf(tmpMsg0, "Light 2 brightness = %d", T2Bright);
            writeDebug(tmpMsg0, 3);
          }
        #endif
        if (Lgt2Trns && newState && T2Value < minPow2) {
          T2Value = minPow2;
        }
        T2DuCy = powerToSegment(T2Value);
      } else {
        T2Value = (newState) ? 100 : 0;
        T2DuCy = (newState) ? 0 : SEGMENTS;
      }
      T2State = newState;
    }
    // If publish flag is off, end here.
    if (!pub) return;
    // Save state
    #ifdef PERSISTENT_STATE
      if (!retain) {
        saveLightState(2);
      }
    #endif
    // Publish new state
    mqttPublishStatus(2);
    #ifdef DEBUG_MODE
      // Show new status
      if (state != 3) {
        if (T2State) {
          if (remote) {
            writeDebug("Light 2 turned ON remotely", 3);
          } else {
            writeDebug("Light 2 On", 3);
          }
        } else {
          if (remote) {
            writeDebug("Light 2 turned OFF remotely", 3);
          } else {
            writeDebug("Light 2 Off", 3);
          }
        }
      }
    #endif
  #endif
}

#ifdef PERSISTENT_STATE
/** **********************************************************************************************
   Save light state to EEPROM
   
   @param light Light number [1|2]
*********************************************************************************************** */
void saveLightState (int light) {
  byte val;
  if (light == 1) {
    val = (T1State) ? 1 : 0;
    EEPROM.write(0, val);
    EEPROM.write(1, T1Bright);
    val = (Led1Status) ? 1 : 0;
    EEPROM.write(2, val);
  } 
  #ifdef DOUBLE_DIMMER
    else if (light == 2) {
      val = (T2State) ? 1 : 0;
      EEPROM.write(5, val);
      EEPROM.write(6, T2Bright);
      val = (Led2Status) ? 1 : 0;
      EEPROM.write(7, val);
    }
  #endif
  EEPROM.commit();
}

/** **********************************************************************************************
   Get lights state from EEPROM
*********************************************************************************************** */
void getLightState() {
  byte val;
  transitionOn = 5;
  val = EEPROM.read(0);
  T1State = (val == 1) ? true : false;
  T1Bright = EEPROM.read(1);
  val = EEPROM.read(2);
  Led1Status = (val == 1) ? true : false;
  setLightState(1, T1State, T1Bright, CONFIG_TRANSITION_WAKE, true, false);
  #ifdef DOUBLE_DIMMER
    val = EEPROM.read(5);
    T2State = (val == 1) ? true : false;
    T2Bright = EEPROM.read(6);
    val = EEPROM.read(7);
    Led2Status = (val == 1) ? true : false;
    setLightState(2, T2Bright, T1Bright, CONFIG_TRANSITION_WAKE, true, false);
  #endif
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
  strcpy(dconfig.name01, CONFIG_MQTT_NAME01);
  dconfig.Lgt1Dimm = CONFIG_LGT1_DIMM;
  dconfig.Lgt1Trns = CONFIG_LGT1_TRNS;
  dconfig.minPow1 = CONFIG_MIN_POW1;
  #ifdef DOUBLE_DIMMER
    strcpy(dconfig.name02, CONFIG_MQTT_NAME02);
    dconfig.Lgt2Dimm = CONFIG_LGT2_DIMM;
    dconfig.Lgt2Trns = CONFIG_LGT2_TRNS;
    dconfig.minPow2 = CONFIG_MIN_POW2;
  #endif
  #ifdef DBL_CLICK
    strcpy(dconfig.topicDouble, CONFIG_DBL_TOPIC);
    strcpy(dconfig.payloadDouble, CONFIG_DBL_PAYLOAD);
  #endif
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
  mounted = SPIFFS.begin();
  if (!mounted) {
    #ifdef DEBUG_MODE
      writeDebug("Failed to mount FS", 1);
    #endif
    return;
  }
  #ifdef DEBUG_MODE
    writeDebug("Mounted file system", 1);
  #endif
  if (!SPIFFS.exists("/config.json")) {
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
  File configFile = SPIFFS.open("/config.json", "r");
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
  // JSON V5.0
  //DynamicJsonBuffer jsonBuffer;
  //JsonObject& json = jsonBuffer.parseObject(buf.get());
  //if (!json.success()) {
  //  #ifdef DEBUG_MODE
  //    writeDebug("Failed to parse configuration file", 1);
  //  #endif
  //  return;
  //}
  // JSON V6.0
  DynamicJsonDocument json(800);
  auto error = deserializeJson(json, buf.get());
  if (error) {
    #ifdef DEBUG_MODE
      writeDebug("Failed to parse configuration file", 1);
    #endif
    return;
  }
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
  strlcpy(dconfig.name01, json["name01"] | dconfig.name01, sizeof(dconfig.name01));
  dconfig.Lgt1Dimm = json["Lgt1Dimm"] | dconfig.Lgt1Dimm;
  dconfig.Lgt1Trns = json["Lgt1Trns"] | dconfig.Lgt1Trns;
  dconfig.minPow1 = json["minPow1"] | dconfig.minPow1;
  #ifdef DOUBLE_DIMMER
    strlcpy(dconfig.name02, json["name02"] | dconfig.name02, sizeof(dconfig.name02));
    dconfig.Lgt2Dimm = json["Lgt2Dimm"] | dconfig.Lgt2Dimm;
    dconfig.Lgt2Trns = json["Lgt2Trns"] | dconfig.Lgt2Trns;
    dconfig.minPow2 = json["minPow2"] | dconfig.minPow2;
  #endif
  #ifdef DBL_CLICK
    strlcpy(dconfig.topicDouble, json["topicDouble"] | dconfig.topicDouble, sizeof(dconfig.topicDouble));
    strlcpy(dconfig.payloadDouble, json["payloadDouble"] | dconfig.payloadDouble, sizeof(dconfig.payloadDouble));
  #endif
  // Set variables
  defineEdge();
  retain = dconfig.retain;
  transitionOn = dconfig.transitionOn;
  transitionOff = dconfig.transitionOff;
  LgtMode = dconfig.LgtMode;
  LedBright = dconfig.LedBright;
  LedDimm = dconfig.LedDimm;
  LedDefault = dconfig.LedDefault;
  minPow1 = (dconfig.minPow1 < MIN_POW) ? MIN_POW : dconfig.minPow1;
  strlcpy(mqttTopics[0].uname, dconfig.name01, sizeof(mqttTopics[0].uname));
  #ifdef DOUBLE_DIMMER
    minPow2 = (dconfig.minPow2 < MIN_POW) ? MIN_POW : dconfig.minPow2;
    strlcpy(mqttTopics[1].uname, dconfig.name02, sizeof(mqttTopics[1].uname));
  #endif
  setLightMode();
  #ifdef DEBUG_MODE
    // JSON V5.0
    //json.printTo(Serial);
    // JSON V6.0
    serializeJson(json, Serial);
    writeDebug(" ");
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
  // JSON V5.0
  //DynamicJsonBuffer jsonBuffer;
  //JsonObject& json = jsonBuffer.createObject();
  // JSON V6.0
  DynamicJsonDocument json(800);
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
  json["name01"] = dconfig.name01;
  json["Lgt1Dimm"] = dconfig.Lgt1Dimm;
  json["Lgt1Trns"] = dconfig.Lgt1Trns;
  json["minPow1"] = dconfig.minPow1;
  #ifdef DOUBLE_DIMMER
    json["name02"] = dconfig.name02;
    json["Lgt2Dimm"] = dconfig.Lgt2Dimm;
    json["Lgt2Trns"] = dconfig.Lgt2Trns;
    json["minPow2"] = dconfig.minPow2;
  #endif
  #ifdef DBL_CLICK
    json["topicDouble"] = dconfig.topicDouble;
    json["payloadDouble"] = dconfig.payloadDouble;
  #endif
  //
  // Open file and print into it
  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    #ifdef DEBUG_MODE
      writeDebug("Failed to open config file for writing", 1);
    #endif
    mqttPublish(0, 0, "configuration save failed");
    return;
  }
  // JSON V5.0
  //json.printTo(configFile);
  // JSON V6.0
  serializeJson(json, configFile);
  //
  configFile.close();
  mqttPublish(0, 0, "configuration saved");
  #ifdef DEBUG_MODE
    // JSON V5.0
    //json.printTo(Serial);
    // JSON V6.0
    serializeJson(json, Serial);
    //
    writeDebug("");
  #endif
  // End save
}

#ifdef SAVE_ON_RESET
/** **********************************************************************************************
   Load status from before reset
*********************************************************************************************** */
void loadLastStatus() {
  int n = 0;
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
    sprintf(tmpMsg, "Got Info: (%d) %d, %d,  %d,  %d,  %d,  %d,  %d,  %d,  %d,  %d", addr, debugBoot, debugRst, resetDelay1, wifiRstCtr1, wifiRstTtl1, aliveCounter1, resetDelay2, wifiRstCtr2, wifiRstTtl2, aliveCounter2);
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
  n = EEPROM_writeAnything(addr, debugBoot);
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
    mqttPublish(0, 0, "status cleared");
    return;
  }
  writeDebug("Failed deleting values", 1);
  mqttPublish(0, 0, "status not cleared");
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
  int i;
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
  int i;
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
  @param dbncInterval Debounce interval (ms)
  @param longInterval Long press interval or repeat start interval (ms)
  @param repInterval Repeat interval (ms)
  @param dblInterval Double click interval (ms)
*********************************************************************************************** */
int initButton(int button, int pinId, int eventState, int dbncInterval, int longInterval, int repInterval, int dblInterval) {
  myButtons[button].pinId = pinId;
  myButtons[button].eventState = eventState;
  myButtons[button].dbncInterval = dbncInterval;
  myButtons[button].longInterval = longInterval;
  myButtons[button].repInterval = repInterval;
  myButtons[button].dblInterval = dblInterval;
  myButtons[button].state = !eventState;
  myButtons[button].lastState = !eventState;
}

/** **********************************************************************************************
   Get the status of a button

  @param btnIndex Button Index
  @return Returns:
    - 1 = Button Down
    - 2 = Button Up
    - 3 = Button Up after long press
    - 4 = Button Up after repeat
    - 5 = Repeat
    - 6 = Double click
*********************************************************************************************** */
int getButtonStatus(int btnIndex) {
  // Init return value and timer
  int retVal = 0;
  unsigned long cTmr = millis();
  // Read the state of the button
  int reading = digitalRead(myButtons[btnIndex].pinId);
  // State changes reset the debounce timer
  if (reading != myButtons[btnIndex].lastState) {
    myButtons[btnIndex].btnTimer = cTmr;
  }
  // Different state detected
  if (reading != myButtons[btnIndex].state) {
    // Compare the debounce timer
    if ((unsigned long)(cTmr - myButtons[btnIndex].btnTimer) > myButtons[btnIndex].dbncInterval) {
      // Whatever the reading is at, it's been there for longer than the debounce
      //  delay, so change the actual state
      myButtons[btnIndex].state = reading;
      // Button Down 
      if (myButtons[btnIndex].state == myButtons[btnIndex].eventState) {
        retVal = 1;
        // Reset the repetition flag and timer
        myButtons[btnIndex].repFlag = false;
        myButtons[btnIndex].repTimer = cTmr;
      }
      // Button Up with double click
      else if (myButtons[btnIndex].dblFlag) {
        myButtons[btnIndex].dblFlag = false;
        retVal = 6;
      } 
      // Button Up after...
      else if(myButtons[btnIndex].repFlag) {
        // ... long press
        if (myButtons[btnIndex].repInterval == 0) {
          retVal = 3;
        }
        // ... repeat
        else {
          retVal = 4;
        }
      }
      // Button Up, start double click detection
      else if (myButtons[btnIndex].dblInterval > 0) {
        myButtons[btnIndex].dblFlag = true;
        myButtons[btnIndex].dblTimer = cTmr;
      } 
      // Button Up
      else {
        retVal = 2;
      }
    }
  } 
  // Button is down
  else if (myButtons[btnIndex].state == myButtons[btnIndex].eventState) {
    if (myButtons[btnIndex].repFlag) {
      if (myButtons[btnIndex].repInterval > 0) {
        if ((unsigned long)(cTmr - myButtons[btnIndex].repTimer) > myButtons[btnIndex].repInterval) {
          retVal = 5;
          myButtons[btnIndex].repTimer = cTmr;
        }
      }
    } else if (myButtons[btnIndex].longInterval > 0) {
      if ((unsigned long)(cTmr - myButtons[btnIndex].repTimer) > myButtons[btnIndex].longInterval) {
        myButtons[btnIndex].repTimer = cTmr;
        // Set the repetition flag... if the event caused a repetition, it will not create a Button Up!
        myButtons[btnIndex].repFlag = true;
      }
    }
  } 
  // A Button UP was detected previously, check double click timer
  else if (myButtons[btnIndex].dblFlag) {
    // Check if the double click timer has expired
    if ((unsigned long)(cTmr - myButtons[btnIndex].dblTimer) > myButtons[btnIndex].dblInterval) {
      myButtons[btnIndex].dblFlag = false;
      retVal = 2;
    }
  }
  myButtons[btnIndex].lastState = reading;
  return retVal;
}

/** **********************************************************************************************
   Publish the double click
*********************************************************************************************** */
void defineButtons() {
  // Initialize Buttons
  #ifdef DBL_CLICK
    initButton(0, PIN_BUTTON1, LOW, 70, BTN_LONG_PRESS, dconfig.dimmTime, DBL_CLICK_INTERVAL);
  #else
    initButton(0, PIN_BUTTON1, LOW, 70, BTN_LONG_PRESS, dconfig.dimmTime, 0);
  #endif
  #ifdef DOUBLE_DIMMER
    #ifdef DBL_CLICK
      initButton(1, PIN_BUTTON2, LOW, 70, BTN_LONG_PRESS, dconfig.dimmTime, DBL_CLICK_INTERVAL);
    #else
      initButton(1, PIN_BUTTON2, LOW, 70, BTN_LONG_PRESS, dconfig.dimmTime, 0);
    #endif
  #endif
}

/** **********************************************************************************************
   Calcuate the edge timers
*********************************************************************************************** */
void defineEdge() {
  edgeSpeed = (dconfig.edgeTime * 100) / dconfig.dimmTime;
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
  //SPIFFS.format();

  // Set default values for configuration
  configDefault();

  // Load configuration
  loadConfig();

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
    WiFiManagerParameter custom_mqtt_name01("name01", "Light 1 name", dconfig.name01, 40);
    #ifdef DOUBLE_DIMMER
      WiFiManagerParameter custom_mqtt_name02("name02", "Light 2 name", dconfig.name02, 40);
    #endif
    #ifdef DBL_CLICK
      WiFiManagerParameter custom_mqtt_dblTpc("topicDouble", "Double click topic", dconfig.topicDouble, 40);
      WiFiManagerParameter custom_mqtt_dblPay("payloadDouble", "Double click payload", dconfig.payloadDouble, 40);
    #endif

    // Create the instance of WiFiManager
    // Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // Set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    // Timeout
    #ifdef CONFIG_TIMEOUT
      wifiManager.setConfigPortalTimeout(CONFIG_TIMEOUT);
    #endif

    // Set static ip;
    //wifiManager.setAPStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));

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
    wifiManager.addParameter(&custom_mqtt_name01);
    #ifdef DOUBLE_DIMMER
      wifiManager.addParameter(&custom_mqtt_name02);
    #endif
    #ifdef DBL_CLICK
      wifiManager.addParameter(&custom_mqtt_dblTpc);
      wifiManager.addParameter(&custom_mqtt_dblPay);
    #endif
    
    // Reset settings - Uncomment to clean parameters during testing
    wifiManager.resetSettings();

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
      strcpy(dconfig.name01, custom_mqtt_name01.getValue());
      #ifdef DOUBLE_DIMMER
        strcpy(dconfig.name02, custom_mqtt_name02.getValue());
      #endif
      #ifdef DBL_CLICK
        strcpy(dconfig.topicDouble, custom_mqtt_dblTpc.getValue());
        strcpy(dconfig.payloadDouble, custom_mqtt_dblPay.getValue());
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
     
  #ifdef DEBUG_MODE
    writeDebug("Normal Boot", 1);
  #endif

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
  
  // Define MQTT Topics
  mqttDefineTopics();
  
  // Start Network
  if (WiFi.SSID()) {
    wifiBegin();
    mqttBegin();
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
      // Wait up to 5 seconds for WiFi... otherwise start without OTA
      do {
        #ifdef DEBUG_MODE
          writeDebug("Wait for WiFi for OTA...", 2);
        #endif
        delay(500);
        ++wt;
        otaEnabled = (WiFi.waitForConnectResult() == WL_CONNECTED) ? true : false;
      } while (wt < 10 && !otaEnabled);
    }
    // If WiFi is available, configure OTA updates
    if (otaEnabled) {
      #ifdef OTA_HTTP
        // Web server update
        #ifdef DEBUG_MODE
          writeDebug("Request update to remote HTTP server", 1);
        #endif
        // Request for the latest image
        t_httpUpdate_return ret = ESPhttpUpdate.update(OTA_HTTP_SERVER, OTA_HTTP_PORT, OTA_SCRIPT_NAME, OTA_IMAGE_VERSION);
        #ifdef DEBUG_MODE
          // Report results
          switch(ret) {
            case HTTP_UPDATE_FAILED:
              sprintf(tmpMsg, "OTA HTTP Update failed (%d): %s", , ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
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
          T1State = false;
          T1Value = T1Power = 0;
          T1DuCy = powerToSegment(T1Value);
          #ifdef DOUBLE_DIMMER
            T2State = false;
            T2Value = T2Power = 0;
            T2DuCy = powerToSegment(T2Value);
          #endif
          delay(200);
          // End dimmer interruptions
          detachInterrupt(digitalPinToInterrupt(PIN_ZCROSS ));
          timer1_detachInterrupt();
          // Disconnect MQTT
          mqttDisconnect();
          // Un-mount File System
          if (mounted) {
            SPIFFS.end();
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
  defineButtons();
  
  // Detect AC frequency and Zero Pulse duration
  #ifdef DEBUG_MODE
    writeDebug("Entering Dimmer auto configuration mode.", 3);
  #endif
  inSetup = true;
  netFirst = true;
  attachInterrupt(digitalPinToInterrupt(PIN_ZCROSS ), zeroCrossSetup, CHANGE);
  while (inSetup) {
    // Flash LEDs during setup (5 per second)
    tmr1 = millis();
    if(tmr1 > tmr2) {
      tmr2 = tmr1 + 100;
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
  timer1_isr_init();
  timer1_attachInterrupt(triggerTriacs);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  attachInterrupt(digitalPinToInterrupt(PIN_ZCROSS ), zeroCross, RISING);
  // Set the initial status for indicators
  #ifdef DOUBLE_DIMMER
    Led1Status = Led2Status = LedDefault;
  #else
    Led1Status = LedDefault;
  #endif
  if (!retain) {
    goLive = 1;
    #ifdef PERSISTENT_STATE
      getLightState();
    #endif
  }
}

/** **********************************************************************************************
   Loop function
*********************************************************************************************** */
void loop() {
  /* ------------------------------------------------------------------------------
     Handle OTA updates
     ------------------------------------------------------------------------------ */
  #ifdef OTA_UPDATES
    #if !defined(OTA_HTTP)
      if (otaEnabled) {
        ArduinoOTA.handle();
      }
    #endif
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
    // Reset debug fail couter
    resetDelay = 0;
  } else {
    // Reconnect, but not every cycle, peventing conection hang times...
    //  Attemp to reconnect every 2.5 seconds
    if (netFirst || (unsigned long)(cTmr - netMillis) > NET_RECONNECT) {
      netFirst = false;
      netConnect();
      netMillis = cTmr;
    }
    // Refresh the  publish timer
    pubMillis = cTmr;
  }
  
  /* ------------------------------------------------------------------------------
     Keep track of running time (not acurate)
     ------------------------------------------------------------------------------ */
  systemTimer();
  
  /* ------------------------------------------------------------------------------
     Set action for buttons depending on status
     ------------------------------------------------------------------------------ */
  // Check button status
  int btn1 = getButtonStatus(0);
  // Button 1 down
  if (btn1 == 1) {
    // Any user activity deactivates the GoLive sequence
    goLiveOff(); 
  }
  // Button 1 UP = Toogle light status
  else if (btn1 == 2) {
    edgeCntr1 = 0;
    setLightState(1, 2, T1Bright, -1, true, false);
  }
  // Button 1 UP after long press = toogle switch or report light status
  else if (btn1 == 3 || btn1 == 4) {
    if (Lgt1Dimm) {
      edgeCntr1 = 0;
      setLightState(1, 3, -1, 0, true, false);
    } else {
      setLightState(1, 2, T1Bright, -1, true, false);
    }
  }
  // Button 1 Repeat with dimmer enabled
  else if (Lgt1Dimm && btn1 == 5) {
    if (!T1State) {
      T1Bright = 0;
      T1Direction = true;
    }
    if (edgeCntr1 > 0) {
      --edgeCntr1;
    } else {
      if (T1Direction) {
        ++T1Bright;  // Dimm up
      } else {
        --T1Bright;  // Dimm down
      }
      if(T1Bright > 100) {
        T1Direction = false;
        T1Bright = 100;
        edgeCntr1 = edgeSpeed;
      } else if(T1Bright < 0) {
        T1Direction = true;
        T1Bright = 0;
        edgeCntr1 = edgeSpeed;
      }
      setLightState(1, 1, T1Bright, 0, false, false);
    }
  }
  #if defined(DBL_CLICK) || defined(MULT_CLICK)
    else if(btn1 == 6) {
      #ifdef DBL_CLICK
        mqttPublish(2, 0, dconfig.payloadDouble, false);
      #endif
      #ifdef MULT_CLICK
        #ifdef DEBUG_MODE
          writeDebug("Multi-click count ", 1);
        #endif
        if (multiClickCnt == 0) {
          multiClickTmr = cTmr;
        }
        if ((unsigned long)(cTmr - multiClickTmr) > MULT_CLICK_INTERVAL) {
          multiClickCnt = 0;
        }
        ++multiClickCnt;
        if (multiClickCnt >= MULT_CLICK_COUNT) {
          goToSetupMode();
        }
      #endif
    }
  #endif
  #ifdef MULT_CLICK
    if (multiClickCnt > 0) {
      if ((unsigned long)(cTmr - multiClickTmr) > MULT_CLICK_INTERVAL) {
        multiClickCnt = 0;
      }
    }
  #endif

  #ifdef DOUBLE_DIMMER
    int btn2 = getButtonStatus(1);
    ////////////////////////////////////////////////////////////////////////////////
    // Set action for button 2 depending on status
    ////////////////////////////////////////////////////////////////////////////////
    // Button 2 UP
    if (btn2 == 1) {
      // Any user activity deactivates the GoLive sequence
      goLiveOff();
    }
    // Button 2 UP
    else if (btn2 == 2) {
      edgeCntr1 = 0;
      setLightState(2, 2, T2Bright, -1, true, false);
    }
    // Button 2 UP after long press
    else if (btn2 == 3 || btn2 == 4) {
      if (Lgt2Dimm) {
        edgeCntr1 = 0;
        setLightState(2, 3, -1, 0, true, false);
      } else {
        setLightState(2, 2, T2Bright, -1, true, false);
      }
    }
    // Button 2 Repeat with dimmer enabled
    else if (Lgt2Dimm && btn2 == 5) {
      if (!T2State) {
        T2Bright = 0;
        T2Direction = true;
      }
      if (edgeCntr2 > 0) {
        --edgeCntr2;
      } else {
        if (T2Direction) {
          ++T2Bright;  // Dimm up
        } else {
          --T2Bright;  // Dimm down
        }
        if(T2Bright > 100) {
          T2Direction = false;
          T2Bright = 100;
          edgeCntr2 = edgeSpeed;
        } else if(T2Bright < 0) {
          T2Direction = true;
          T2Bright = 0;
          edgeCntr2 = edgeSpeed;
        }
        setLightState(2, 1, T2Bright, 0, false, false);
      }
    }
    #if defined(DBL_CLICK) || defined(MULT_CLICK)
      else if(btn2 == 6) {
        #ifdef DBL_CLICK
          mqttPublish(2, 0, dconfig.payloadDouble, false);
        #endif
        #ifdef MULT_CLICK
          #ifdef DEBUG_MODE
            writeDebug("Multi-click count " ,1);
          #endif
          if (multiClickCnt2 == 0) {
            multiClickTmr2 = cTmr;
          }
          if ((unsigned long)(cTmr - multiClickTmr2) > MULT_CLICK_INTERVAL) {
            multiClickCnt2 = 0;
          }
          ++multiClickCnt2;
          if (multiClickCnt2 >= MULT_CLICK_COUNT) {
            goToSetupMode();
          }
        #endif
      }
    #endif
    #ifdef MULT_CLICK
      if (multiClickCnt2 > 0) {
        if ((unsigned long)(cTmr - multiClickTmr2) > MULT_CLICK_INTERVAL) {
          multiClickCnt2 = 0;
        }
      }
    #endif
  #endif
  ////////////////////////////////////////////////////////////////////////////////
  // Set the LED indicator brightness
  ////////////////////////////////////////////////////////////////////////////////
  unsigned long lTmr = micros();
  if (lTmr > LedPwm || lTmr < LedOvf) {
    flashLed();
    if (ledsState) {
      #ifdef DOUBLE_DIMMER
        if (T1State || T2State) {
          LedPwm = lTmr + LedBright;
        }
      #else
        if (T1State) {
          LedPwm = lTmr + LedDimm;
        }
      #endif
      else {
        LedPwm = lTmr + LedDimm;
      }
    } else {
      #ifdef DOUBLE_DIMMER
        if (T1State || T2State) {
          LedPwm = lTmr + (CONFIG_LED_BRIGHT - LedBright);
        }
      #else
        if (T1State) {
          LedPwm = lTmr + (CONFIG_LED_BRIGHT - LedBright);
        }
      #endif
      else {
        LedPwm = lTmr + (CONFIG_LED_BRIGHT - LedDimm);
      }
    }
  }
  LedOvf = lTmr;
}
