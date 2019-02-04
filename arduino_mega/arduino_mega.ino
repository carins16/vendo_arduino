#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

SoftwareSerial Middleware(13,12); //(RX, TX) serial communication between arduino & nodemcu
// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

// Arduino mega pins
// byte rowPins[ROWS] = {22, 24, 26, 28};
// byte colPins[COLS] = {30, 32, 34, 36};
// Arduino uno pins
byte rowPins[ROWS] = {11, 10, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = { 7, 6, 5, 4}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

struct {
  int id;
  String name;
  int price;
  int qty;
  bool selected = false;
} Items;

void setup(){
  //Initialize serial port 115200
  Serial.begin(9600);
  Middleware.begin(9600);

  lcd.init();      //initialize the lcd
  lcd.backlight(); //open the backlight
}

void loop(){
  Keypad_Function();
  get_items(); //listen to items received from nodemcu (must not be removed in the loop function)
}

void Keypad_Function() {
  char key = keypad.getKey();
  if (key) {
    int itemId = key - 48;
    if (itemId >= 1 && itemId <= 9) {
      request_items(itemId);
    } else if (key == 'A') {
      if (Items.selected) {
        if (Items.qty > 0) {
          purchase_items();
        } else {
          Serial.println("Item No. " + (String)Items.id + " is out of stock!");
        }
      } else {
        Serial.println("Please select an item!");
      }
    }
  }
}

void display_items() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ITEM: " + Items.name);
  lcd.setCursor(0, 1);
  lcd.print("PRICE: " + (String)Items.price); // convert int into (String)
}