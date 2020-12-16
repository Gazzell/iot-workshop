#include <arduino-timer.h>

#include <DHT.h>
#include <Adafruit_NeoPixel.h>

#define DEMO_MODE false

#define DHT_PIN 6
#define DHTTYPE DHT11

#define NEO_DATA_PIN 2

#define SENSOR_UPDATE_TIME 2000

#define NUM_PIXELS 16
#define TEMP_PIXELS 11
#define HUM_PIXELS 3
#define ZERO_PIXEL 2

#define MIN_TEMP -10
#define MAX_TEMP 40

#define TEMPERATURE_RANGE_COUNT 7

Adafruit_NeoPixel pixels(NUM_PIXELS, NEO_DATA_PIN, NEO_GRB + NEO_KHZ800);
DHT dht(DHT_PIN, DHTTYPE);
auto timer = timer_create_default();

int separatorPixels[2] = { 11, 15 };

int dryColor[3] = { 230, 191, 0 };
int okColor[3] = { 0, 200, 0 };
int wetColor[3] = { 0, 0, 255 };

float temperatureRanges[7] = { -20.0, 0.0, 10.0, 18.0, 25.0, 30.0, 50.0 };
int temperatureColorsByIndex[7][3] = {
    { 195, 159, 249 },
    { 82, 113, 249 },
    { 73, 170, 167 },
    { 66, 244, 122 },
    { 244, 231, 83 },
    { 249, 173, 42 },
    { 245, 10, 14 }
};

float currentHumidity = 0.0;
float currentTemperature = -15.0;

float lerp(int val1, int val2, float grad) {
    return (val2 - val1) * grad + val1;
}

float normalize(float min, float max, float val) {
    return (val - min) / (max - min);
}

void setSeparatorPixels() {
    for (int i = 0; i < 2; i++) {
        pixels.setPixelColor(separatorPixels[i], pixels.Color(27, 11, 0));
    }
}

int getTemperatureUpperIndex(float temperature) {
    int index = 1;
    while (index < TEMPERATURE_RANGE_COUNT) {
        if (temperature < temperatureRanges[index]) {
            break;
        }
        index++;
    }
    return index;
}

void fillTemperature(float temperature) {
    int temperaturePixel = map(int(temperature), MIN_TEMP, MAX_TEMP, 0, TEMP_PIXELS);
    temperaturePixel = constrain(temperaturePixel, 0, TEMP_PIXELS - 1);

    int temperatureUpperIndex = getTemperatureUpperIndex(temperature);
    int* color1 = temperatureColorsByIndex[temperatureUpperIndex - 1];
    int* color2 = temperatureColorsByIndex[temperatureUpperIndex];
    float gradient = normalize(temperatureRanges[temperatureUpperIndex - 1], temperatureRanges[temperatureUpperIndex], temperature);
    float r = lerp(color1[0], color2[0], gradient);
    float g = lerp(color1[1], color2[1], gradient);
    float b = lerp(color1[2], color2[2], gradient);

    if (temperaturePixel < ZERO_PIXEL) {
        pixels.fill(pixels.Color(r, g, b), temperaturePixel, ZERO_PIXEL - temperaturePixel + 1);
    }
    else {
        pixels.fill(pixels.Color(r, g, b), ZERO_PIXEL, temperaturePixel - ZERO_PIXEL + 1);
    }
}

void fillHumidity() {
    int* color1 = okColor;
    int* color2 = currentHumidity < 50.0 ? dryColor : wetColor;
    float gradient = abs(currentHumidity - 50.0) * 2.0 * 0.01;
    float r = lerp(color1[0], color2[0], gradient);
    float g = lerp(color1[1], color2[1], gradient);
    float b = lerp(color1[2], color2[2], gradient);

    pixels.fill(pixels.Color(r, g, b), TEMP_PIXELS + 1, 3);
}

bool readDHT(void*) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return true;
    }

    currentHumidity = h;
    currentTemperature = t;

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.println(F("°C "));

    return true;
}

void updateDemoValues() {
    currentHumidity += 0.01;
    currentTemperature += 0.002;

    if (currentHumidity > 100.0) {
        currentHumidity = 0.0;
    }

    if (currentTemperature > MAX_TEMP + 10.0) {
        currentTemperature = MIN_TEMP - 10.0;
    }
}


void setup() {
    Serial.begin(9600);
    Serial.println(F("Thermometer!"));

    pixels.begin();
    pixels.setBrightness(30);

    dht.begin();

    if (!DEMO_MODE) {
        timer.every(SENSOR_UPDATE_TIME, readDHT);
    }
}

void loop() {
    timer.tick();
    pixels.clear();

    setSeparatorPixels();
    fillTemperature(currentTemperature);
    fillHumidity();

    if (DEMO_MODE) {
        updateDemoValues();
    }

    pixels.show();
}
