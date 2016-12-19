#include "FastLED.h"

#define NUM_LEDS 50

#define DATA_PIN 13

// Define the array of leds
CRGB leds[NUM_LEDS];
String command = "";
String commandTmp = "";
int inChar;
int rArr[NUM_LEDS];
int gArr[NUM_LEDS];
int bArr[NUM_LEDS];
int brightnessArr[NUM_LEDS];
boolean commandFinished = false;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  Serial.println("INIT");
  resetLED();
  showLED();
}

void loop() {
  command = "";
  while (Serial.available() > 0) {
    inChar = Serial.read();
    //command finished
    if (inChar == '\n' || commandTmp.length() > 100) {
      commandFinished = true;
      command = commandTmp;
      command.toLowerCase();
      commandTmp = "";
    } else {
      commandTmp += (char) inChar;
    }
  }

  if (commandFinished) {
    if (command.length() > 0) {
      if (command.equals("show")) {
        showLED();
      } else if (command.equals("status")) {
        statusLED();
      }  else if (command.equals("reset")) {
        resetLED();
        showLED();
      } else {
        updateLED(command);
      }
    }

    commandFinished = false;
  }
}

int getBrightness(int brightness) {
  return (255 - (2.55 * brightness));
}

boolean isLEDValid(String string) {
  return string.length() > 0 && string.toInt() >= 1 && string.toInt() <= NUM_LEDS;
}

boolean isRGBValid(String string) {
  return string.length() > 0 && string.toInt() >= 0 && string.toInt() <= 255;
}

boolean isBrightnessValid(String string) {
  return string.length() > 0 && string.toInt() >= 0 && string.toInt() <= 100;
}

void showLED() {
  Serial.println("SHOW");
  FastLED.show();
}

void resetLED() {
  Serial.println("RESET");
  for (int i = 0; i < NUM_LEDS; i++) {
    rArr[i] = 0;
    gArr[i] = 0;
    bArr[i] = 0;
    brightnessArr[i] = 0;
    leds[i] = CRGB::Black;
  }
}

void statusLED() {
  Serial.println("STATUS");
  for (int i = 0; i < NUM_LEDS; i++) {
    Serial.println(String(i + 1) + "," + String(rArr[i]) + "," + String(gArr[i]) + "," + String(bArr[i]) + "," + String(brightnessArr[i]));
  }
}

void updateLED(String command) {
  String ledNumString = getValue(command, ',', 0);
  String rString = getValue(command, ',', 1);
  String gString = getValue(command, ',', 2);
  String bString = getValue(command, ',', 3);
  String brightnessString = getValue(command, ',', 4);

  boolean ledValid = isLEDValid(ledNumString);
  boolean rBool = isRGBValid(rString);
  boolean gBool = isRGBValid(gString);
  boolean bBool = isRGBValid(bString);
  boolean brightnessBool = isBrightnessValid(brightnessString);

  if (!ledValid || !rBool  || !gBool  || !gBool || !brightnessBool) {
    Serial.println("INVALID " + command);
  } else {
    int ledI = ledNumString.toInt() - 1;
    rArr[ledI] = rString.toInt();
    gArr[ledI] = gString.toInt();
    bArr[ledI] = bString.toInt();
    brightnessArr[ledI] =  brightnessString.toInt();
    int brightnessAlpha = getBrightness(brightnessArr[ledI]);

    Serial.println("VALID " + command);
    Serial.println("LED " + ledNumString);
    Serial.println("R " + rString);
    Serial.println("G " + gString);
    Serial.println("B " + bString);
    Serial.println("BRIGHTNESS " + brightnessString);

    leds[ledI] = CRGB::Black;

    leds[ledI].setRGB(rArr[ledI], gArr[ledI], bArr[ledI]);

    if (brightnessArr[ledI] == 0) {
      leds[ledI] = CRGB::Black;
    } else if (brightnessArr[ledI] == 100) {
      leds[ledI].maximizeBrightness();
    } else {
      leds[ledI].fadeLightBy(brightnessAlpha);
    }
  }
}


String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  String returnString = found > index ? data.substring(strIndex[0], strIndex[1]) : "";
  returnString.replace(String(separator), "");
  return returnString;
}

