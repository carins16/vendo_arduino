// Arduino Mega
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial Middleware(11,10); //(RX, TX) serial communication between arduino & nodemcu
// initialize the library with the numbers of the interface pins
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup(){
  //Initialize serial port 115200
  Serial.begin(9600);
  Middleware.begin(9600);

  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);

  lcd.init();      //initialize the lcd
  lcd.backlight(); //open the backlight
}

// items details that will be used
int itemId;
String itemName;
int itemPrice;
int itemQty;


void loop(){

  // change this code into numkeys
  if (digitalRead(2) == LOW) {
    lcd.clear();
    request_items(1);
    delay(200);
  } else if (digitalRead(3) == LOW) {
    lcd.clear();
    request_items(2);
    delay(200);
  } else if (digitalRead(4) == LOW) {
    lcd.clear();
    request_items(3);
    delay(200);
  } else if (digitalRead(5) == LOW) {
    purchase_items();
    delay(200);
  }


  get_items(); //listen to items received from nodemcu (must not be removed in the loop function)
}

void display_items() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ITEM: " + itemName);
  lcd.setCursor(0, 1);
  lcd.print("PRICE: " + (String)itemPrice); // convert int into (String)
}

void request_items(int itemId) {
  DynamicJsonBuffer jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();
  // data that will be send
  root["type"] = "REQUEST_ITEM";
  root["id"] = itemId;

  root.printTo(Middleware); // send data to nodemcu
  root.prettyPrintTo(Serial); // preview request in serial
}

void get_items() {
  if (Middleware.available() > 0) {
    DynamicJsonBuffer jsonBuffer;

    JsonObject &root = jsonBuffer.parseObject(Middleware);

    // store recieved data to global variables
    itemId    = root["id"];
    itemName  = root["name"].as<String>();
    itemPrice = root["price"];
    itemQty   = root["qty"];

    display_items();            // display item info in lcd
    root.prettyPrintTo(Serial); // preview received data in serial
  }
}

void purchase_items() {
  DynamicJsonBuffer jsonBuffer;

  JsonObject &root = jsonBuffer.createObject();
  // get item to be purchase
  root["type"] = "PURCHASE_ITEM";
  root["id"] = itemId;
  root["name"] = itemName;
  root["price"] = itemPrice;

  root.printTo(Middleware);   // send data to nodemcu
  root.prettyPrintTo(Serial); // preview data to be send in serial
}