/*****************************************************
 * name:Real-time Clock Module 
 * function:you can see the current date and time displayed on the I2C LCD1602.
 ******************************************************/
//Email:support@sunfounder.com
//Website:www.sunfounder.com

//include the libraries
#include <stdio.h>
#include <string.h>
#include <DS1302.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//YYYY/MM//DD 24:00:00
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address
uint8_t RST_PIN   = 5;  //RST pin attach to
uint8_t SDA_PIN   = 6;  //IO pin attach to
uint8_t SCL_PIN = 7;  //clk Pin attach to
/* Create buffers */
char buf[50];
char day[10];

String comdata = "";
int numdata[7] ={ 0}, j = 0, mark = 0;
/* Create a DS1302 object */
DS1302 rtc(RST_PIN, SDA_PIN, SCL_PIN);//create a variable type of DS1302


void print_time()
{
  /* Get the current time and date from the chip */
  Time t = rtc.time();
  /* Name the day of the week */
  memset(day, 0, sizeof(day));
  switch (t.day)
  {
  case 1: 
    strcpy(day, "Sun"); 
    break;
  case 2: 
    strcpy(day, "Mon"); 
    break;
  case 3: 
    strcpy(day, "Tue"); 
    break;
  case 4: 
    strcpy(day, "Wed"); 
    break;
  case 5: 
    strcpy(day, "Thu"); 
    break;
  case 6: 
    strcpy(day, "Fri"); 
    break;
  case 7: 
    strcpy(day, "Sat"); 
    break;
  }
  /* Format the time and date and insert into the temporary buffer */
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d", day, t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  //FORMAT YYYY/MM/DD 24:00:00
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d", day, t.yr, t.mon, t.date, t.hr, t.min, t.sec);
  /* Print the formatted string to serial so we can see the time */
  Serial.println(buf);
  lcd.setCursor(2,0);
  lcd.print(t.yr);
  lcd.print("-");
  lcd.print(t.mon/10);
  lcd.print(t.mon%10);
  lcd.print("-");
  lcd.print(t.date/10);
  lcd.print(t.date%10);
  lcd.print(" ");
  lcd.print(day);
  lcd.setCursor(4,1);
  lcd.print(t.hr);
  lcd.print(":");
  lcd.print(t.min/10);
  lcd.print(t.min%10);
  lcd.print(":");
  lcd.print(t.sec/10);
  lcd.print(t.sec%10);

  lcd.setCursor(0, 2);
  lcd.print("line 1");
  lcd.setCursor(1, 3);
  lcd.print("line 2");
}


void setup()
{
  Serial.begin(9600);
  lcd.begin(20, 4);
  rtc.write_protect(false);
  rtc.halt(false);
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight
  Time t(2019, 4, 2, 2, 6, 0, 0); //initialize the time
  // day, t.yr, t.mon, t.date, t.hr, t.min, t.sec)
  /* Set the time and date on the chip */
  rtc.time(t);
}

void loop()
{
    defaultRTC();
}

void defaultRTC(){
  /*add the data to comdata when the serial has data  */
  while (Serial.available() > 0)
  {
    comdata += char(Serial.read());
    delay(2);
    mark = 1;
  }
  /* Use a comma to separate the strings of comdata,
   and then convert the results into numbers to be saved in the array numdata[] */
  if (mark == 1)
  {
    Serial.print("You inputed : ");
    Serial.println(comdata);
    for (int i = 0; i < comdata.length(); i++)
    {
      if (comdata[i] == ',' || comdata[i] == 0x10 || comdata[i] == 0x13)
      {
        j++;
      }
      else
      {
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
  print_time();
  delay(1000);
  lcd.clear();
}

