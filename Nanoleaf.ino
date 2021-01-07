#include <BluetoothSerial.h>
BluetoothSerial ESP_BT;
String InputCommand;
#define MAX_PARAMETER_COUNT 5
String Parameters[MAX_PARAMETER_COUNT];

#include <EEPROM.h>
#define EEPROM_SIZE 2048

#include <FastLED.h>
#define LED_PIN 23
CRGB* leds;

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  Serial.begin(9600);
  while(!ESP_BT.begin("Nanoleaf"))
  {
      
  }
  InitializeVariables();
}

bool getCommand() {
  if (ESP_BT.available())
  {
    InputCommand = ESP_BT.readString();
    return 1;
  } else
  {
    return 0;
  }
}

bool parseCommand() {
  uint8_t parameter_iterator = 0;
  if (InputCommand.length() == 0)
  {
    return 0;
  }

  for (int i = 0; i < MAX_PARAMETER_COUNT; i++)
  {
    Parameters[i] = "";
  }

  for (int i = 0; i < InputCommand.length(); i++)
  {
    if (InputCommand[i] == ',')
    {
      parameter_iterator ++;
      i++;
    }
    Parameters[parameter_iterator] += InputCommand[i];
  }
  return 1;
}

void executeCommand() {
  if (strstr(Parameters[0].c_str(), "getAllData") != NULL)
  {
    String all_data_string;
    all_data_string = EEPROM.read(0);
    all_data_string += ",";
    all_data_string += EEPROM.read(1);
    all_data_string += ",";
    all_data_string += EEPROM.read(2);
    ESP_BT.println(all_data_string);
  } else if (strstr(Parameters[0].c_str(), "setLEDcount") != NULL)
  {
    leds = (CRGB*)realloc(leds, EEPROM.read(0)*sizeof(CRGB));
    EEPROM.write(0,Parameters[1].toInt());
    EEPROM.commit();
  }else if (strstr(Parameters[0].c_str(), "changeInitializeState") != NULL)
  {
    EEPROM.write(1, Parameters[1].toInt());
    EEPROM.commit();    
  }else if (strstr(Parameters[0].c_str(), "setBrightness") != NULL){
    FastLED.setBrightness(Parameters[1].toInt());
    EEPROM.write(2, Parameters[1].toInt());
    EEPROM.commit();
  }
}

void InitializeVariables()
{
  //Anfrage an Smartphone. Wenn nicht gefunden aus EEPROM grundwert verwenden.
  leds = (CRGB*)malloc(EEPROM.read(0) * sizeof(CRGB));
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, EEPROM.read(0));
  FastLED.setBrightness(EEPROM.read(2));
}

void loop() {
  leds[0] = CRGB::Red;
  FastLED.show();
  if (getCommand())
  {
    if (parseCommand())
    {
      Serial.println(InputCommand);
      executeCommand();
    }
  }
}
