#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
#define LED_PIN 6     //light strip connect to Pin D6
#define NUMPIXELS 4  //number of Led on the light strip
#define BUTTON_PIN 5


Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
//create a new light strip object to define the data pattern

#define DELAYVAL 500  //time (in milliseconds) to pause between pixels

volatile int light_setting = 0;
int NUM_SETTINGS = 5;

void setup() {
  Serial.begin(9600);
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();  //intialize NeoPixel strip object
  pixels.setBrightness(20);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ISR_demo, FALLING);

}


void ISR_demo() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  // https://forum.arduino.cc/t/debouncing-an-interrupt-trigger/45110
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 300) 
  {
    light_setting = (light_setting + 1) % NUM_SETTINGS;
    Serial.println(light_setting);
  }
  last_interrupt_time = interrupt_time;
}

// void button_clicked(){
//   if(digitalRead(BUTTON_PIN) == LOW){
//     Serial.println("pressed");
//   }
// }
bool setting_changed_wait(int n, int idx, int t_step = 1) {
  for (int i = 0; i < n; i++) {
    if (light_setting != idx) {
      return true;
    }
    delay(t_step);
  }
  return false;
}

void redGreenBlue(int idx) {
  while (true) {
    pixels.fill(pixels.Color(255, 0, 0), 0, 4);
    pixels.show();

    if (setting_changed_wait(1000, idx)) return;

    pixels.fill(pixels.Color(0, 255, 0), 0, 4);
    pixels.show();
    if (setting_changed_wait(1000, idx)) return;

    pixels.fill(pixels.Color(0, 0, 255), 0, 4);
    pixels.show();
    if (setting_changed_wait(1000, idx)) return;
  }
}

void rainbow(int idx) {
  int wait = 10;
  while (true) {
    for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {

      for (int i = 0; i < pixels.numPixels(); i++) {
        int pixelHue = firstPixelHue + (i * 65536L / pixels.numPixels());
        pixels.setPixelColor(i, pixels.gamma32(pixels.ColorHSV(pixelHue)));
      }
      pixels.show();
      if (setting_changed_wait(10, idx)) return;
    }
  }
}

void off(int idx) {
  pixels.clear();
  pixels.show();

  setting_changed_wait(2, idx);
}

void solid(int r, int g, int b, int idx) {
  pixels.fill(pixels.Color(r, g, b), 0, 4);
  pixels.show();
  while (true) {
    if (setting_changed_wait(1000, idx)) return;
  }
}

void loop() {
  switch (light_setting) {
    case 0:
      redGreenBlue(0);
      break;
    case 1:
      off(1);
      break;
    case 2:
      solid(255, 0, 0, 2);
      break;
    case 3:
      solid(0, 100, 0, 3);
      break;
    case 4: 
      solid(0, 0, 100, 4);
      break;

    // case 2:
    //   rainbow(2);
    //   break;
  }
}

