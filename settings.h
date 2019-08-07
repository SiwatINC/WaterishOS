/* ------------------------------------------
   |           Node Identification          |
   ------------------------------------------ */
const String nodename = "developmentkit"; //Device Name
const String osversion = "Alpha 4"; //Waterish OS Version
/* ------------------------------------------
   |              WiFi Setup                |
   ------------------------------------------ */
const boolean wifi = true;
const char* ssid = ".........."; //The name of the WiFi network
const char* password = ".........."; // WiFi Password
/* ------------------------------------------
   |           LCD Display Setup            |
   ------------------------------------------ */
const boolean enable_lcd = true; //Enable LCD Display
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; //setup pin for the lcd display
const boolean enable_button = true; //Enable Button Interupt
const int buttonpin = 0; //Set button pin
/* ------------------------------------------
   |          Sensor Configuration          |
   ------------------------------------------ */
const unsigned long period = 1000; //Period between sensor sweep
/* ------------------------------------------
   |        Telemetry Configuration         |
   ------------------------------------------ */
const boolean telemetry = true; //Enable telemetry
const String telemetryserver = "siwatsystem.com"; //Server to send data to
const String tememetryuser = "....."; //Server's username auth
const String telemetrypassword = "....."; //Server's Password
