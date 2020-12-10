#include <Adafruit_NeoPixel.h>

#define PIN        2

#define NUM_PIXELS 16
#define TEMP_PIXELS 11
#define HUM_PIXELS 3
#define ZERO_PIXEL 3

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int separatorPixels[2] = { 11, 15 };

int dryColor[3] = { 230, 191, 0 };
int okColor[3] = { 0, 200, 0 };
int wetColor[3] = { 0, 0, 255 };

float currentHumidity = 0.0;

float lerp(int val1, int val2, float grad) {
    return (val2 - val1) * grad + val1;
}

void setSeparatorPixels() {
    for (int i = 0; i < 2; i++) {
        pixels.setPixelColor(separatorPixels[i], pixels.Color(27, 11, 0));
    }
}

void fillTempGradient() {
    for (int i = 0; i < TEMP_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(30, 30, 30));
    }
}

void fillHumidity() {
    int* color1 = okColor;
    int* color2 = currentHumidity < 50.0 ? dryColor : wetColor;
    float gradient = abs(currentHumidity - 50.0) * 2.0;
    float r = lerp(color1[0], color2[0], gradient * 0.01);
    float g = lerp(color1[1], color2[1], gradient * 0.01);
    float b = lerp(color1[2], color2[2], gradient * 0.01);

    for (int i = TEMP_PIXELS + 1; i < TEMP_PIXELS + 4; i++) {
        pixels.setPixelColor(i, pixels.Color(r, g, b));
    }
}


void setup() {
    pixels.begin();
}

void loop() {
    pixels.clear();

    setSeparatorPixels();
    fillTempGradient();
    fillHumidity();

    currentHumidity += 0.1;
    delay(10);

    if (currentHumidity > 100.0) {
        currentHumidity = 0.0;
    }

    pixels.show();
}
