
#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define LED_PIN 10
#define NUM_LEDS 300
int BRIGHTNESS = 64;

#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

int mode_count = 0;
int debounce = 0;

bool r_done = 0;
bool g_done = 0;
bool b_done = 0;

CRGBPalette16 currentPalette;
TBlendType currentBlending;

#define UPDATES_PER_SECOND 100
#define FRAMES_PER_SECOND 120

const uint8_t kMatrixWidth = 16;
const uint8_t kMatrixHeight = 16;

const bool kMatrixSerpentineLayout = true;
#define MAX_DIMENSION ((kMatrixWidth > kMatrixHeight) ? kMatrixWidth : kMatrixHeight)

static uint16_t x;
static uint16_t y;
static uint16_t z;

uint16_t speed = 20; // speed is set dynamically once we've started up

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.
uint16_t scale = 30; // scale is set dynamically once we've started up

// This is the array that we keep our computed noise values in
uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];

CRGBPalette16 currentPalette1(PartyColors_p);
uint8_t colorLoop = 1;

void mapNoiseToLEDsUsingPalette();
void SetupRandomPalette();
void fillnoise8();
void ChangePaletteAndSettingsPeriodically();
void noise_plus_palette();

// uint16_t XY(uint8_t x, uint8_t y);

// put function declarations here:
void mode_changer();
void increase_brightness();
void decrease_brightness();

void SetupTotallyRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void SetupPurpleAndGreenPalette();

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
    {
        CRGB::Red,
        CRGB::Gray, // 'white' is too bright compared to red and blue
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Red,
        CRGB::Gray,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Blue,
        CRGB::Black,
        CRGB::Black};

void ChangePalettePeriodically();
void FillLEDsFromPaletteColors(uint8_t colorIndex);

void mode_count_0();
void show_red();
void show_green();
void show_blue();

void addGlitter(fract8 chanceOfGlitter);
void rainbow();
void rainbowWithGlitter();
void confetti();
void sinelon();
void juggle();
void bpm();

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void demo_reel();

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  delay(3000); // power-up safety delay
  Serial.println("in setup");
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  x = random16();
  y = random16();
  z = random16();
  Serial.println("finished setup");
}

void loop()
{
  // put your main code here, to run repeatedly:
  bool dimmer = digitalRead(13);

  if (dimmer == HIGH)
  {
    decrease_brightness();
  }

  bool brighter = digitalRead(12);

  if (brighter == HIGH)
  {
    increase_brightness();
  }

  if (BRIGHTNESS > 255)
  {
    BRIGHTNESS = 255;
  }

  if (BRIGHTNESS < 5)
  {
    BRIGHTNESS = 5;
  }

  bool mode = digitalRead(11);

  if (mode == HIGH)
  {
    mode_changer();
  }

  Serial.println(mode_count);

  FastLED.setBrightness(BRIGHTNESS);

  if (mode_count == 0)
  {
    mode_count_0();
  }

  else if (mode_count == 1)
  {
    show_red();
  }
  else if (mode_count == 2)
  {
    show_green();
  }
  else if (mode_count == 3)
  {
    show_blue();
  }
  else if (mode_count == 4)
  {
    demo_reel();
  }

}

// put function definitions here:
void mode_changer()
{

  if (debounce++ > 2)
  
  {
          for (int h = 0; h < NUM_LEDS; h++)
      {
        leds[h] = CRGB(0, 0, 0);
        FastLED.show();}
    debounce = 0;
    r_done = 0;
    g_done = 0;
    b_done = 0;
    if (mode_count++ > 4)
    {
      mode_count = 0;
      for (int h = 0; h < NUM_LEDS; h++)
      {
        leds[h] = CRGB(0, 0, 0);
        FastLED.show();
      }
      delay(500);
    }
  }
}

void increase_brightness()
{
  BRIGHTNESS = BRIGHTNESS + 1;
}

void decrease_brightness()
{
  BRIGHTNESS = BRIGHTNESS - 1;
}

void SetupTotallyRandomPalette()
{
  for (int i = 0; i < 16; ++i)
  {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid(currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green = CHSV(HUE_GREEN, 255, 255);
  CRGB black = CRGB::Black;

  currentPalette = CRGBPalette16(
      green, green, black, black,
      purple, purple, black, black,
      green, green, black, black,
      purple, purple, black, black);
}

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand)
  {
    lastSecond = secondHand;
    if (secondHand == 0)
    {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 10)
    {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 15)
    {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 20)
    {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 25)
    {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 30)
    {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if (secondHand == 35)
    {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 40)
    {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 45)
    {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 50)
    {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 55)
    {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }
  }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void mode_count_0()
{
  ChangePalettePeriodically();

  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */

  FillLEDsFromPaletteColors(startIndex);
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void show_red()
{
  if (!r_done)
  {
    r_done = 1;
    for (int g = 0; g < NUM_LEDS; g++)
    {
      leds[g] = CRGB(255, 0, 0);
      FastLED.show();
    }
  }
}

void show_green()
{
  if (!g_done)
  {
    g_done = 1;
    for (int h = 0; h < NUM_LEDS; h++)
    {
      leds[h] = CRGB(0, 255, 0);
      FastLED.show();
    }
  }
}

void show_blue()
{
  if (!b_done)
  {
    b_done = 1;
    // demo_reel();

    for (int j = 0; j < NUM_LEDS; j++)
    {
      leds[j] = CRGB(0, 0, 255);
      FastLED.show();
    }
  }
}

void addGlitter(fract8 chanceOfGlitter)
{
  if (random8() < chanceOfGlitter)
  {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for (int i = 0; i < NUM_LEDS; i++)
  { // 9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++)
  {
    leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void demo_reel()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // do some periodic updates
  EVERY_N_MILLISECONDS(20) { gHue++; }   // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS(10) { nextPattern(); } // change patterns periodically
}
/*
void noise_plus_palette()
{
  ChangePaletteAndSettingsPeriodically();

  // generate noise data
  fillnoise8();

  // convert the noise data to colors in the LED array
  // using the current palette
  mapNoiseToLEDsUsingPalette();

  FastLED.show();
}
/*
void fillnoise8()
{
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  if (speed < 50)
  {
    dataSmoothing = 200 - (speed * 4);
  }

  for (int i = 0; i < MAX_DIMENSION; i++)
  {
    int ioffset = scale * i;
    for (int j = 0; j < MAX_DIMENSION; j++)
    {
      int joffset = scale * j;

      uint8_t data = inoise8(x + ioffset, y + joffset, z);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing)
      {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
        data = newdata;
      }

      noise[i][j] = data;
    }
  }

  z += speed;

  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;
}

void mapNoiseToLEDsUsingPalette()
{
  static uint8_t ihue = 0;

  for (int i = 0; i < kMatrixWidth; i++)
  {
    for (int j = 0; j < kMatrixHeight; j++)
    {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri = noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if (colorLoop)
      {
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the
      // light/dark dynamic range desired
      if (bri > 127)
      {
        bri = 255;
      }
      else
      {
        bri = dim8_raw(bri * 2);
      }

      CRGB color = ColorFromPalette(currentPalette1, index, bri);
      leds[XY(i, j)] = color;
    }
  }

  ihue += 1;
}

// This function generates a random palette that's a gradient
// between four different colors.  The first is a dim hue, the second is
// a bright hue, the third is a bright pastel, and the last is
// another bright hue.  This gives some visual bright/dark variation
// which is more interesting than just a gradient of different hues.
void SetupRandomPalette()
{
  currentPalette1 = CRGBPalette16(
      CHSV(random8(), 255, 32),
      CHSV(random8(), 255, 255),
      CHSV(random8(), 128, 255),
      CHSV(random8(), 255, 255));
}

uint16_t XY(uint8_t x, uint8_t y)
{
  uint16_t i;
  if (kMatrixSerpentineLayout == false)
  {
    i = (y * kMatrixWidth) + x;
  }
  if (kMatrixSerpentineLayout == true)
  {
    if (y & 0x01)
    {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    }
    else
    {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}
/*
#define HOLD_PALETTES_X_TIMES_AS_LONG 1
void ChangePaletteAndSettingsPeriodically()
{
  uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand)
  {
    lastSecond = secondHand;
    if (secondHand == 0)
    {
      currentPalette1 = RainbowColors_p;
      speed = 20;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 5)
    {
      SetupPurpleAndGreenPalette();
      speed = 10;
      scale = 50;
      colorLoop = 1;
    }
    if (secondHand == 10)
    {
      SetupBlackAndWhiteStripedPalette();
      speed = 20;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 15)
    {
      currentPalette1 = ForestColors_p;
      speed = 8;
      scale = 120;
      colorLoop = 0;
    }
    if (secondHand == 20)
    {
      currentPalette1 = CloudColors_p;
      speed = 4;
      scale = 30;
      colorLoop = 0;
    }
    if (secondHand == 25)
    {
      currentPalette1 = LavaColors_p;
      speed = 8;
      scale = 50;
      colorLoop = 0;
    }
    if (secondHand == 30)
    {
      currentPalette1 = OceanColors_p;
      speed = 20;
      scale = 90;
      colorLoop = 0;
    }
    if (secondHand == 35)
    {
      currentPalette1 = PartyColors_p;
      speed = 20;
      scale = 30;
      colorLoop = 1;
    }
    if (secondHand == 40)
    {
      SetupRandomPalette();
      speed = 20;
      scale = 20;
      colorLoop = 1;
    }
    if (secondHand == 45)
    {
      SetupRandomPalette();
      speed = 50;
      scale = 50;
      colorLoop = 1;
    }
    if (secondHand == 50)
    {
      SetupRandomPalette();
      speed = 90;
      scale = 90;
      colorLoop = 1;
    }
    if (secondHand == 55)
    {
      currentPalette1 = RainbowStripeColors_p;
      speed = 30;
      scale = 20;
      colorLoop = 1;
    }
  }
}*/