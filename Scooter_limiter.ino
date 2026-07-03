#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Wheel: 125 mm RADIUS (including tire) → 250 mm diameter
const float WHEEL_DIAMETER_M = 0.250f;
const float WHEEL_CIRCUMFERENCE_M = 3.14159265f * WHEEL_DIAMETER_M;
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
int throttleIn;
int throttleOut;
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
#define RPM_TIMEOUT_MS 250
int LED_PIN = 13;
int rpm_count;
// static float rpm = 0.0;
static float speed_kmh = 0;
static uint32_t last_sample_ms = 0;
static int rpm_state;
static uint32_t rpm_time_ts;
static uint32_t disp_time_ts;
static unsigned int rpm_ticks = 0;
static volatile uint32_t rpm_start_ts;
static volatile uint32_t rpm_end_ts;
static volatile float rpm;
volatile uint32_t rpm_interval_us = 0;
volatile bool     rpm_new_sample  = false;

static void rpm_tick_handler(void) {
    rpm_ticks=rpm_time_ts + 1;
    uint32_t now = micros();
    rpm_interval_us = now - rpm_end_ts;   // unsigned: rollover-safe
    rpm_end_ts = now;
    rpm_new_sample = true;
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
 //   render_stats_init(&stats,stats_interval_buffer,NULL,100);
    rpm_end_ts = micros();
//    micros_ts = micros();
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
    noInterrupts();
    uint32_t interval = rpm_interval_us;
    bool     fresh    = rpm_new_sample;
    rpm_new_sample    = false;
    interrupts();
    
if (fresh && interval > 0) {
       // one tick = 1/RPM_DIVISOR rev; 60e6 us per minute
rpm = 60000000.0f / ((float)interval * RPM_DIVISOR);
// km/h = rev/min × (m/rev) × (60 min/h) ÷ (1000 m/km)
speed_kmh = rpm * WHEEL_CIRCUMFERENCE_M * 60.0f / 1000.0f;
        last_sample_ms = millis();
    }
    else if (millis() - last_sample_ms > RPM_TIMEOUT_MS) {
        speed_kmh = 0;
        rpm = 0.0f;
    }
/*    if (fresh && interval > 0) {
        // one tick = 1/RPM_DIVISOR rev; 60e6 us per minute
        rpm = 60000000.0f / ((float)interval * RPM_DIVISOR);
    }
    if(disp_time_ts + 1000 > ms) {
        disp_time_ts = ms;
        Serial.print("Sensor: ");
        Serial.print(rpm_ticks);
        Serial.print(". RPM: ");
        Serial.print(rpm);
        Serial.println(".");
  
            }
*/
            display.clearDisplay();
  // Finish
    display.setTextSize(1);             // Normal 1:1 pixel scale
    //display.setTextColor(SSD1306_WHITE); 
    display.setCursor(1,15);             
    display.print("Speed: ");
    display.print(speed_kmh);
    display.setCursor(1,40);
    display.print("RPM: ");
    display.print(rpm);
    display.display();
    int throttleIn = analogRead(A0);
    throttleOut = map(throttleIn, 0, 1023, 0, 255);
    analogWrite(5, throttleOut);
}
