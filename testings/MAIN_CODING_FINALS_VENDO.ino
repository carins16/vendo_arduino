#include <Keypad.h>
#include <Servo.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
//#include <virtuabotixRTC.h> //Library used
////////////////////////////////////////////////
int temp = 0;
int wait;
int tim = 500;
int servoPin = 2;
const byte ROWS = 4;
const byte COLS = 4;
char key;
char outputKey;
char keyInput;
int keyOutput;
int coin;
int bill;
float balance;
Servo Servo1, Servo2, Servo3, Servo4, Servo5, Servo6, Servo7, Servo8, Servo9; 
////////////////////////////////////////////////
LiquidCrystal_I2C lcd(0x27,16,2);
//LiquidCrystal_I2C lcd(0x3F, 16, 2);
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34, 36};
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
///////////////////////////////////////////////

/////////////////////////ITEM///////////
float itemPrice[] = {2.00,//0
                      4.00,//1
                      3.00,//2
                      1.00,//3
                      2.00,//4
                      4.00,//5
                      3.00,//6
                      1.00,//7
                      2.00};//8
                      
bool itemAvail[] = {true,//0
                     true,//1
                     true,//2
                     true,//3
                     true,//4
                     true,//5
                     true,//6
                     true,//7
                     true};//8
                     
char itemCode[] = { '1',//0
                     '2',//1
                     '3',//2
                     '4',//3
                     '5',//4
                     '6',//5
                     '7',//6
                     '8',//7
                     '9'};//8

String itemName[] = {"Slurpee",//0
                     "Gulp",//1
                     "noodles",//2
                     "tshirt",//3
                     "peanuts",//4
                     "crackers",//5
                     "chippy",//6
                     "chilly",//7
                     "asin Suka"//8
                     };

//virtuabotixRTC myRTC(6, 7, 8); //If you change the wiring change the pins here also

void setup() {
    //Servo1.attach(servoPin); 
    lcd.init();  //initialize the lcd
    lcd.backlight();  //open the backlight


    //    myRTC.setDS1302Time(15, 22, 21, 7, 14, 1, 2018); //Here you write your actual time/date as shown above 
    Serial.begin(9600);
}

void loop() {

   //RTC_time();
   RunCodeInMillis();
   //Serial.println("test");
}

void RunCodeInMillis(){
coin = 2.00;
bill = 100.00;
balance = bill + coin;
    static unsigned long timer = millis();
    static int deciSeconds = 0;
   
      if (millis() - timer >= 100) {
          timer += 50;
          deciSeconds++; // 100 milliSeconds is equal to 1 deciSecond
          //Serial.print("deci : ");
          
          if (deciSeconds == 30000) { // Reset to 0 after counting for 1000 seconds.
              deciSeconds=0;//RESET CONDITIONS
            //lcd.clear();
        }   //RUN FUNCTIONS HERE
              Keypad_Function();
            
            
           //INITIALIZING FUNCTION HERE
            LCD_Display(key);
     
      } 
}

void Keypad_Function(){
    key = keypad.getKey();
    if (key)
    { 
      Serial.print("KEY : ");
      Serial.println(key);
      lcd.clear(); 
    }else{
      
    }
}

void ControlSelection(int set){

  if(set == 1){
    
  }else if(set == 2){
    
  }
   
}

void purchaseItem(char code, int balance, Servo servoCode){

    if(code == '1'){
       if(true == itemAvail[0]){
            if(balance == itemPrice[0]){
                  delay(300);
                  // Rotate the helical coil, discharge the selected item
                  servoCode.writeMicroseconds(2000); // rotate
                  delay(950);
                  servoCode.writeMicroseconds(1500);  // stop
                  delay(500);
            }else{//not enough Balance
              
            }
       }else{//item not available
        
       }
    }else{// item code error
      
    }
}

void LCD_Display(char displayItem){
     switch(displayItem) {
      case '1' :
         itemDetails(itemAvail[0],itemPrice[0],itemCode[0],itemName[0]);
         break;
      case '2' :
          itemDetails(itemAvail[1],itemPrice[1],itemCode[1],itemName[1]);
         break;
      case '3' :
          itemDetails(itemAvail[2],itemPrice[2],itemCode[2],itemName[2]);
         break;
      case '4' :
          itemDetails(itemAvail[3],itemPrice[3],itemCode[3],itemName[3]);
         break;
      case '5' :
          itemDetails(itemAvail[4],itemPrice[4],itemCode[4],itemName[4]);
         break;
      case '6' :
          itemDetails(itemAvail[5],itemPrice[5],itemCode[5],itemName[5]);
         break;
      case '7' :
          itemDetails(itemAvail[6],itemPrice[6],itemCode[6],itemName[6]);
         break;
      case '8' :
          itemDetails(itemAvail[7],itemPrice[7],itemCode[7],itemName[7]);
         break;
      case '9' :
         itemDetails(itemAvail[8],itemPrice[8],itemCode[8],itemName[8]);
         break;
      case 'A' ://check Balance
         LCD_prompt(3);          
         break;         
       default :
         displayItem = displayItem;
     }
}

void itemDetails(bool avail, float price,char itemCode, String itemName){
     //OUTPUT
     lcd.setCursor(0,0);
     lcd.print("Item:");
     lcd.setCursor(5,0);
     lcd.print(itemCode);
     lcd.setCursor(7,0);
     lcd.print("P");
     lcd.setCursor(8,0);
     lcd.print(price);
     lcd.setCursor(0,1);
     lcd.print(itemName);
}

void doneCommand(){
    coin = 0;
    balance = 0;
}

void LCD_prompt(int displayNumber){
     switch(displayNumber) {
      case 1 :
         lcd.setCursor(0,0);
         lcd.print("INSERT COIN");
         break;
      case 2 :
         lcd.setCursor(0,0);
         lcd.print("NOT ENOUGH");
         lcd.setCursor(0,1);
         lcd.print("BALANCE");
         break;
      case 3 :
         lcd.setCursor(0,0);
         lcd.print("YOUR BALANCE:");
         lcd.setCursor(1,1);
         lcd.print("P");
         lcd.setCursor(2,1);
         lcd.print(balance);
         break;
      case 4 :
         lcd.setCursor(7,0);
         lcd.print("ERROR :O");
         break;
      default :
         displayNumber = displayNumber;
     }
}

void RTC_time(){
  // This allows for the update of variables for time or accessing the individual elements.
// myRTC.updateTime();

// Start printing elements as individuals
 Serial.print("Current Date / Time: ");
// Serial.print(myRTC.dayofmonth); //You can switch between day and month if you're using American system
 Serial.print("/");
// Serial.print(myRTC.month);
 Serial.print("/");
// Serial.print(myRTC.year);
 Serial.print(" ");
// Serial.print(myRTC.hours);
 Serial.print(":");
// Serial.print(myRTC.minutes);
 Serial.print(":");
// Serial.println(myRTC.seconds);

// Delay so the program doesn't print non-stop
 delay(1000);
}

