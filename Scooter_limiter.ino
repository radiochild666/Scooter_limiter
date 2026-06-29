#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void show() { // Often used sequence - Function to simplify code
  display.display(); delay(2000); display.fillScreen(SSD1306_BLACK);
}
#define RPM_PIN 2
// number of ticks per revolution
//#define RPM_DIVISOR 15
#define RPM_DIVISOR 30
#define RPM_LEVEL LOW
#define RPM_TIME_DIV 60.f
int LED_PIN = 13;
int rpm_count;
static float rpm = 0.0;
static int rpm_state;
static uint32_t rpm_time_ts;
static uint32_t disp_time_ts;
static unsigned int rpm_ticks = 0;
static void rpm_tick_handler(void) {
      rpm_ticks=rpm_ticks+1;
}
void setup() {
     if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Old Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    Serial.begin(9600);
    pinMode(RPM_PIN, INPUT_PULLUP);
    rpm_state = digitalRead(RPM_PIN);
    rpm_time_ts = millis();
    disp_time_ts = millis();;
    Serial.println("Started test");
//    if(rpm_state==HIGH) {
//        attachInterrupt(digitalPinToInterrupt(RPM_PIN),rpm_tick_handler,FALLING);
//            } else {
//        attachInterrupt(digitalPinToInterrupt(RPM_PIN),rpm_tick_handler,RISING);
//    }
attachInterrupt(digitalPinToInterrupt(RPM_PIN),rpm_tick_handler,CHANGE);
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);
  display.setCursor(25,10);
  display.println("Scooter");
  display.setCursor(25,35);
  display.println("Limiter");
  display.display(); 
  delay(1000);
  display.fillScreen(BLACK);          // Fill screen BLACK
  display.display();
}
void loop() {
    uint32_t ms = millis();
     if(rpm_time_ts + (1000/RPM_TIME_DIV) > ms) {
         rpm_time_ts = ms;
         //rpm = rpm_ticks/(float)RPM_DIVISOR * (60.f / RPM_TIME_DIV);
         rpm = ((float)rpm_ticks/(float)RPM_DIVISOR) * (60.f / RPM_TIME_DIV);
         rpm_ticks = 0;
     }

    if(disp_time_ts + 1000 > ms) {
        disp_time_ts = ms;
/*        Serial.print("Sensor: ");
        Serial.print(rpm_ticks);
        Serial.print(". RPM: ");
        Serial.print(rpm);
        Serial.println(".");
*/    
            }
            display.clearDisplay();
  // Finish
    display.setTextSize(1);             // Normal 1:1 pixel scale
    //display.setTextColor(SSD1306_WHITE); 
    display.setCursor(1,15);             
    display.print("Sensor: ");
    display.print(rpm_ticks);
    display.setCursor(1,40);
    display.print("RPM: ");
    display.print(rpm);
    display.display();
}
