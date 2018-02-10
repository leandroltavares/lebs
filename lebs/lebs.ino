
/*
 * lcd_nav.ino - Example code using the menu system library
 *
 * This example shows using the menu system with a 16x2 LCD display
 * (controled over serial).
 *
 * Copyright (c) 2015 arduino-menusystem
 * Licensed under the MIT license (see LICENSE)
 */

#include <LiquidCrystal.h>
#include <avr/pgmspace.h>
#include <EEPROM.h>

#define LCD_COL 20
#define MENU_LEVEL1_COUNT 4

#define BTN_OK 
#define BTN_NEXT;
#define BTN_PREV;
#define BTN_BACK;
#define BTN_UP;
#define BTN_DOWN;

#define TEMPERATURE_SENSOR_PIN A15

const char string_0[] PROGMEM = "Create Recipe";  //Mode 0
const char string_1[] PROGMEM = "Load Recipe"; //Mode 1
const char string_2[] PROGMEM = "Start Mashing"; //Mode 2
const char string_3[] PROGMEM = "Check Status" ; //Mode 3

const char string_4[] PROGMEM = "Add Ramp"; //SubMode 0
const char string_5[] PROGMEM = "Remove Ramp"; //SubMode 1

const char recipe_num[] PROGMEM = "Recipe #";
const char ramp_num[] PROGMEM = "Ramp #";

const char* const main_menu_text[] PROGMEM = {string_0, string_1, string_2, string_3};

const char* const create_recipe_menu_text[] PROGMEM = {string_4, string_5}; 

typedef struct ramp{
  float temperatureSetPoint;
  int duration;
  bool pump;
}ramp; // 7 bytes


typedef struct recipe{
  ramp ramps[10];
  int rampsCount;
}recipe; //74 bytes


char buffer [LCD_COL];
// renderer

// The LCD circuit:
//    * LCD RS pin to digital pin 8
//    * LCD Enable pin to digital pin 9
//    * LCD D4 pin to digital pin 4
//    * LCD D5 pin to digital pin 5
//    * LCD D6 pin to digital pin 6
//    * LCD D7 pin to digital pin 7
//    * LCD R/W pin to ground
LiquidCrystal lcd = LiquidCrystal(31, 33, 35, 37, 39, 41);

int lcdRow = 0;
int currentMode = 0;
int currentSubMode = 0;
int menuLevel = 0;
float temperature = 0.0;

void serial_print_help() {
    Serial.println("***************");
    Serial.println("w: go to previus item (up)");
    Serial.println("s: go to next item (down)");
    Serial.println("a: go back (right)");
    Serial.println("d: select \"selected\" item");
    Serial.println("?: print this help");
    Serial.println("h: print this help");
    Serial.println("***************");
}

void serial_handler() {
    char inChar;
    if ((inChar = Serial.read()) > 0) {
        switch (inChar) {
            case '1': // Previus item
                buttonPrev();
                break;
            case '2': // Next item
                buttonNext();
                break;
            case '3': // Back presed
                buttonOK();
                break;
            case '4':
               buttonBack();
               break;
            case '?':
            case 'h': // Display help
                serial_print_help();
                break;
            case 'c':
              lcd.clear();
              break;
            case 't':
              readTemperature();
              Serial.println(temperature);
            default:
                break;
        }
    }
}

// Standard arduino functions

void setup() {
    Serial.begin(9600);
    lcd.begin(20, 4);

    memset(buffer, '\0', 20);

    //serial_print_help();

    initMenu();
}

void loop() {
    serial_handler();
}

void lcdHome(){
  setCursor(0,0);
}

void setCursor(int col, int row){
  lcdRow = row;
  lcd.setCursor(col, row);
}

void initMenu(){
  lcdHome();
  printCenter("La Extraditable");
  setCursor(0, 1);
  printCenter("Brewing System v1");
  setCursor(0, 3);
  printCenter("Press OK to Start");
}

void printCenter(const char* str){
  clearRow();
  int leadingSpaces = (LCD_COL - strlen(str)) / 2;
  lcd.setCursor(leadingSpaces, lcdRow);
  lcd.print(str);
}

void clearRow(){
  setCursor(0, lcdRow);
  for(int i =0; i < LCD_COL; i++){
    lcd.print(" ");
  } 
  setCursor(0, lcdRow);
}

void changeMode(){
  strcpy_P(buffer, (char*)pgm_read_word(&(main_menu_text[currentMode])));
  Serial.println(buffer);
  printMenuOption();
}

void changeSubMode(){
  
}

void selectCurrentMode(){
  setCursor(0, 2);
  strcpy_P(buffer, (char*)pgm_read_word(&(main_menu_text[currentMode])));
  printCenter(buffer);
  setCursor(0, 3);
  clearRow();
  currentSubMode = 0;  
}

void selectCurrentSubMode(){
  if(currentMode == 0){//Create reciepe
    strcpy_P(buffer, (char*)pgm_read_word(&(create_recipe_menu_text[currentMode])));
    printCenter(buffer);
    setCursor(0, 3);
  }
}

void printMenuOption(){
  setCursor(0, 3);
  clearRow();
  lcd.print("<");
  int leadingSpaces = (LCD_COL - strlen(buffer)) / 2;                      
  setCursor(leadingSpaces, lcdRow);
  lcd.print(buffer);
  setCursor(LCD_COL - 1, lcdRow);
  lcd.print(">");
}

void saveReciepe(recipe r, int recipeId){
  EEPROM.put(recipeId * sizeof(recipe), r);
}

recipe loadReciepe(int recipeId){
  recipe result;
  EEPROM.get(recipeId * sizeof(recipe), result);
  return result; 
}

void addRamp(){

}

void printReciepe(){

}

void buttonOK(){
  
  if(menuLevel == 1){
    selectCurrentMode();
  }
  
  if(menuLevel < 2){
    menuLevel++;
  }
}

void buttonBack(){
  if(menuLevel > 0){
    menuLevel--;
  }
}

void buttonNext(){
  if(menuLevel == 1){
    currentMode = ++currentMode % MENU_LEVEL1_COUNT;
    changeMode();
  }
  else if(menuLevel == 2){
    currentSubMode = ++currentSubMode % 0;
    changeSubMode();
  }
}

void buttonPrev(){
  if(menuLevel == 1){
    currentMode = --currentMode;
    if(currentMode < 0){
      currentMode = MENU_LEVEL1_COUNT - 1;
    }
    changeMode();
  }
}

void readTemperature(){
  temperature = ( 5.0 * analogRead(TEMPERATURE_SENSOR_PIN) * 100.0) / 1024.0;  
}

void buttonUp(){
}

void buttonDown(){
}

