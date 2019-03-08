///----------------Libraries----------------//
#include <stdio.h>
#include <Servo.h>
#include <string.h>
#include <DS1302.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Adafruit_PWMServoDriver.h>

///----------------Pin initialize Library type----------------//
SoftwareSerial Middleware(13, 12);  //(RX, TX) serial communication between arduino & nodemcu
LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display
Servo Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9;

///----------------data variables----------------//
int servoPin = 9;
int coin = 0;
int balance =0;
char key;
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

struct{//data from NodeMCU
  String id;
  String name;
  int price;
  int qty;
  String date;
  bool selected = false;

} Items;

char buf[50];
char day[10];
String comdata = "";
int numdata[7] = {0}, j = 0, mark = 0;

#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

///----------------Pin initialize Native type----------------//
int coin_acceptor = 2;
byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34, 36};// keypad pins

uint8_t RST_PIN = 5; //RST pin attach to
uint8_t SDA_PIN = 6; //IO pin attach to
uint8_t SCL_PIN = 7; //clk Pin attach to

uint8_t slot = 0; //optional if servo is fixed on call function

///----------------Initialize instants of class-----------//
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
DS1302 rtc(RST_PIN, SDA_PIN, SCL_PIN); //create a variable type of DS1302

void setup() {///////////////SETUP
  Serial.begin(9600);
  Middleware.begin(9600);
  rtc.write_protect(true);
  rtc.halt(false);
  Time t(2019, 3, 7, 19, 37, 0, 0); //initialize the time
  rtc.time(t); /* Set the time and date on the chip */
  pinMode(coin_acceptor, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(coin_acceptor),CoinAcceptor, RISING);
  lcd.begin(20, 4);
  lcd.init();
  lcd.backlight();
  pwm.begin();
  pwm.setPWMFreq(60);
  Servo1.attach(9);
  Servo2.attach(8);
  delay(10);

  display_default();
}

void loop() { /// RUN ///

  RunCodeInMillis();
}

void spinServo(Servo SlotNumber){

  SlotNumber.write(0); // rotate
  delay(2000);
  SlotNumber.write(90); // stop
}

void servoControl(){

  if (Items.selected) {
    if (Items.qty > 0)
    {
      if (balance >= Items.price)
      {
        balance = balance - Items.price;

          switch (slot)
          {
            case 0:
              spinServo(Servo1);
              break;
            case 1:
              spinServo(Servo2);
              break;
            default:
              break;
          }
        
        // for (int degree = 0; degree <= 180; degree += 1) //adjust rotation here
        // {
        //   pwm.setPWM(slot, 0, map(180, 0, 180, SERVOMIN, SERVOMAX));
        //   delayMicroseconds(500);
        // }
        //pwm.setPWM(slot, 0, 0); //stop
        print_time();
        purchase_items();
      }
      else
      {
        balance = balance;
        display_items("Not Enough Balance");
      }
    }
    else
    {
      display_items("Item Unavailable");
      Serial.println("Item No. " + Items.id + " is out of stock!");
    }
  }else{
    Serial.println("Please select an item!");
  }
    
  
}

void Keypad_Function_simple(){
  key = keypad.getKey();
    if (key){ 
        Serial.print("KEY : ");
        Serial.println(key);
        remoteControl(key);
    }else{
        key = key;
    }
}

void remoteControl(char keypress){

     switch(keypress) {
      case '1' : request_items(1); slot = 0; break;
      case '2' : request_items(2); slot = 1; break;
      case '3' : request_items(3); slot = 2; break;
      case '4' : request_items(4); slot = 3; break;
      case '5' : request_items(5); slot = 4; break;
      case '6' : request_items(6); slot = 5; break;
      case '7' : display_items("Nothing in Slot 7"); break;
      case '8' : display_items("Nothing in Slot 8"); break;
      case '9' : display_items("Nothing in Slot 9"); break;
      case 'A' : display_items("A"); break;
      case 'B' : display_items("Buying"); servoControl(); break;
      case 'C' : display_items("C"); break;
      case 'D' : display_items("D"); break;  
      default : keypress = keypress; Serial.println(slot);
     }
}
void display_items(String msg){

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BAL: " + (String)balance);
  //lcd.setCursor(12, 0);
  //lcd.print("LEFT: " + (String)Items.qty);
  lcd.setCursor(2, 1);
  lcd.print("ITEM: " + Items.name);
  //lcd.print("ITEM: SLURPEE");
  lcd.setCursor(2, 2);
  lcd.print("PRICE: " + (String)Items.price); // convert int into (String)
  //lcd.print("PRICE: P10");
  lcd.setCursor(0, 3);
  lcd.print(msg);

}

void display_default(){

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELCOME TO SARISARI");
  lcd.setCursor(0, 1);
  lcd.print("VENDING MACHINE");
  lcd.setCursor(0, 2);
  lcd.print("SELECT AN ITEM(1-6)");
  lcd.setCursor(0, 3);
  lcd.print("PRESS 'B' TO BUY");
}

void request_items(int itemId) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    // data that will be send
    root["type"]    = "REQUEST_ITEM";
    root["id"]      = "D" + (String)itemId;

    root.printTo(Middleware);   // send data to nodemcu
    root.prettyPrintTo(Serial); // preview request in serial
}

void get_items() {
    if (Middleware.available() > 0) {
        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(Middleware);

        if (root.success()) {
            // store recieved data to global variables
            Items.id = root["id"].as<String>();
            Items.name  = root["name"].as<String>();
            Items.price = root["price"];
            Items.qty   = root["qty"];
            Items.selected = true;

            display_items("Selected Item: " + Items.id); // display item info in lcd
            root.prettyPrintTo(Serial); // preview received data in serial
        }
    }
}

void purchase_items() {

    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    // get item to be purchase
    root["type"]    = "PURCHASE_ITEM";
    root["id"]      = Items.id;
    root["name"]    = Items.name;
    root["price"]   = Items.price;
    root["date"]  = Items.date;

    clear_items();
    //display_items("Buying");
    root.printTo(Middleware);   // send data to nodemcu
    root.prettyPrintTo(Serial); // preview data to be send in serial
}

void clear_items() {
    Items.id        = "";
    Items.name      = "";
    Items.price     = 0;
    Items.qty       = 0;
    Items.selected = false;
}

void print_time()
{
  /* Get the current time and date from the chip */
  Time t = rtc.time();
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02u:%02u:%02u", t.yr, t.mon, t.date, t.hr, t.min, t.sec);
    
    if(buf != ""){
      Items.date = (String)buf;
    }else{
      Items.date = Items.date;
    }
    
  Serial.println(Items.date);
}

void realTimeClock(){

  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    mark = 1;
  }
  /* Use a comma to separate the strings of comdata,
   and then convert the results into numbers to be saved in the array numdata[] */
  if (mark == 1){
    Serial.print("You inputed : ");
    Serial.println(comdata);
    for (int i = 0; i < comdata.length(); i++){
      if (comdata[i] == ',' || comdata[i] == 0x10 || comdata[i] == 0x13){
        j++;
      }else{
        numdata[j] = numdata[j] * 10 + (comdata[i] - '0');
      }
    }
    /* The converted numdata add up to the time format, then write to DS1302*/
    Time t(numdata[0], numdata[1], numdata[2], numdata[3], numdata[4], numdata[5], numdata[6]);
    rtc.time(t);
    mark = 0;
    j = 0;
    /* clear comdata ，in order to wait for the next input  */
    comdata = String("");
    /* clear numdata */
    for (int i = 0; i < 7; i++)
      numdata[i] = 0;
  }

  /* print the current time */
  //print_time();
}

void CoinAcceptor(){

  if (digitalRead(coin_acceptor) == LOW){
    balance++;
    display_items("Coin inserted beep");
  }

}

void RunCodeInMillis(){//// Run your codes here :)
  static unsigned long timer = millis();
  static int deciSeconds1 = 0, deciSeconds2 = 0;
  if (millis() - timer >= 100){
    timer += 50;
    deciSeconds1++; // 100 milliSeconds is equal to 1 deciSecond
    deciSeconds2++;

    if (deciSeconds1 == 24){// Reset to 0 after counting for 1000 seconds.
      realTimeClock();
      //Serial.println(slot);
      deciSeconds1 = 0; //RESET CONDITIONS                 //lcd.clear();
    }                  //RUN FUNCTIONS HERE
  
    //INITIALIZING FUNCTION HERE

    pwm.setPWM(0, 0, map(180, 0, 180, SERVOMIN, SERVOMAX));
    get_items();
    CoinAcceptor();
    Keypad_Function_simple();


      
    
  }

}