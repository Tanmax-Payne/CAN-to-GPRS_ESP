//  ESP32 WROOM 32U
//  ESP32 Dev Module
/////////////////////////////////////////////////////////////////
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include <Adafruit_SSD1306.h>       // legacy: #include "SSD1306.h"
#include "Button2.h"                                                  // Imported LIbrary from Hackster.io 
#include <WiFi.h>                                                     // Standard Lib for esp
#include <Board_Identify.h>                                           // For a9g  and other CSR based GSM boards             
/////////////////////////////////////////////////////////////////
#define ADC_PINS 35
#define BUTTON_A_PIN  36  //  down                          // Add later  a 5d rocker or  
#define BUTTON_B_PIN  39  //  OK
#define BUTTON_C_PIN  33  //  back
#define BUTTON_D_PIN  34  //  up
#define BUTTON_E_PIN  32  //  fwd
#define ENABLE_SSD1306                                                // Enable SDA and SCL Pins (define)
#define RXD2 14                                                             // GPS pin tx
#define TXD2 12                                                             // GPS pin Rx
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
/////////////////////////////////////////////////////////////////
// Declaration for an SSD1306 display connected to I2C (default SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Button2 buttonA = Button2(BUTTON_A_PIN);
Button2 buttonB = Button2(BUTTON_B_PIN);
Button2 buttonC = Button2(BUTTON_C_PIN);
Button2 buttonD = Button2(BUTTON_D_PIN);
Button2 buttonE = Button2(BUTTON_E_PIN);
/////////////////////////////////////////////////////////////////
int vref = 1100;
int menu_horiz_pos = 1;
int menu_vert_pos = 1;
bool ok_sel = false;
/////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();
  Serial.println("\nT18 Phone Test");
  Serial1.begin(115200, SERIAL_8N1, RXD2, TXD2);
  InitPhone();
  IdentBoard();
  InitDisplay();
  SetButtonHandlers();
}
/////////////////////////////////////////////////////////////////
void loop() {
  buttonA.loop();
  buttonB.loop();
  buttonC.loop();
  buttonD.loop();
  buttonE.loop();
  while (Serial1.available()) {
    Serial.print(char(Serial1.read()));
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(menu_horiz_pos); display.print(" - ");
  display.println(menu_vert_pos);

  display.setCursor(0, 20);
  // do something different depending on the range value:
  if (menu_vert_pos == 1) {
    switch (menu_horiz_pos) {
      case 1:    // info menu
        display.println("INFO");
        break;
      case 2:    // power and rssi
        display.println("POWER");
        break;
      case 3:    // sms
        display.println("SMS");
        break;
      case 4:    // phonebook AT+CPBR
        display.println("CURRENT PHONEBOOK");
        break;
      case 5:    // time
        display.println("TIME");
        break;
      case 6:    // gps
        display.println("GPS");
        break;
      case 7:    // expansion...
        display.println("end");
        break;
    }
  }
  if (menu_vert_pos == 2) {
switch (menu_horiz_pos) {
      case 1:    // ATI
        display.println("HANSET");
        break;
      case 2:    // AT+CSQ
        display.println("RSSI");
        break;
      case 3:    // AT+CMGL
        display.println("READ ALL SMS");
        break;
      case 4:    // change phonebook SIM or MEMORY
        display.println("CHANGE PHONEBOOK");
        break;
      case 5:    // set alarm
        display.println("SET ALARM");
        break;
      case 6:    // turn gps on
        display.println("GPS ON");
        break;
      case 7:    // add more stuff
        display.println("end");
        break;
    }
  }
  display.display();
}
/////////////////////////////////////////////////////////////////
void SetButtonHandlers() {
  buttonA.setClickHandler(click);
  buttonB.setClickHandler(click);
  buttonC.setClickHandler(click);
  buttonD.setClickHandler(click);
  buttonE.setClickHandler(click);
}
void InitDisplay() 
{
  // Initialising the UI will init the display too.
/*  
  display.clearDisplay();
  display.setTextSize(0);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.println("A9G Phone Test");
  display.println("V1.1");
  display.println("ESP32 WROOM 32U");
  // write the buffer to the display
  display.display();
  Serial.println("Display Init....");  */
  delay(3000);
}

void IdentBoard() {
  // Print the board information to the serial using the defined terms  // INTERNAL_Functs
  Serial.println("Detecting Board...");
  Serial.print(F("Board Type: "));
  Serial.println(BoardIdentify::type);
  Serial.print(F("Board Make: "));
  Serial.println(BoardIdentify::make);
  Serial.print(F("Board Model: "));
  Serial.println(BoardIdentify::model);
  Serial.print(F("Board MCU: "));
  Serial.println(BoardIdentify::mcu);
}

void showVoltage() {                      // Sample DAta in case of  no GPS 
  static uint64_t timeStamp = 0;
  if (millis() - timeStamp > 1000) {
    timeStamp = millis();
    uint16_t v = analogRead(ADC_PINS);
    float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);
    String voltage = "Voltage :" + String(battery_voltage) + " Volts";
    display.clearDisplay();
    display.println(voltage);
    display.display();
    Serial.println("DEBUG: Value: " + String(v) + " ,T18 Battery Voltage is" + String(battery_voltage) + "V");
  }
}
void InitPhone() {                              // AT COmm-SEt for  a9 gprs n SMS 
  Serial1.println("AT");
  delay(250);
  Serial1.println("ATE1V1");
  delay(250);
  Serial1.println("AT+CMEE=2");
  delay(250);
  Serial1.println("ATS0=0");
  delay(250);
  Serial1.println("AT+SNFS=1");
  delay(250);
  Serial1.println("AT+CMGF=1");                         // Longer packet willl need higher Delay and WAit
  delay(330);
  Serial1.println("AT+CSMP=17,167,0,4");
  delay(450);
  Serial1.println("ATI");
  delay(250);
}
void SendCommand(String command) {
  Serial1.println(command);
}

void click(Button2 & btn) {                         // Add DEpendencies 
  if (btn == buttonA) {
    Serial.println("down");
    display.clearDisplay();
    display.display();
    menu_horiz_pos = menu_horiz_pos - 1;
    if ( menu_horiz_pos < 1) {
      menu_horiz_pos = 1;
    }
  } else if (btn == buttonB) {
    Serial.println("OK clicked");
    ok_sel = true;
  } else if (btn == buttonC) {
    Serial.println("back clicked");
    menu_vert_pos = menu_vert_pos - 1;
    if ( menu_vert_pos < 1) {
      menu_vert_pos = 1;
    }
  } else if (btn == buttonD) {
    Serial.println("up clicked");
    menu_horiz_pos = menu_horiz_pos + 1;
  } else if (btn == buttonE) {
    Serial.println("fwd clicked");
    menu_vert_pos = menu_vert_pos + 1;
  }
}

/////////////////////////////////////////////////////////////////
