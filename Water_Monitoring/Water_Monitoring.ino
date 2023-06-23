#include <LiquidCrystal.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8);
TinyGPS gps;
SoftwareSerial mySerial(17, 16);
SoftwareSerial receivingSerial(19, 18);
SoftwareSerial gpsserial(15, 14);

int pressurePin = 0;
int temperaturePin = 1;
int waterPin = 2;
int pHPin = 3;

float lat = 27.2046, lon = 77.4977;

char PRESSURESHOW[10];
char TEMPERATURESHOW[4];
char WATERFLOWSHOW[10];
char PHSHOW[10];
int TEMPERATURE = 27;
float PRESSURE = 101;
float WATERFLOW = 100;
float PH = 8;
const char location_statement[50];

int number_of_iter = 0;

void calculate_pressure() {
  PRESSURE = analogRead(pressurePin);
  String strintPRESSUREVALUE = String(PRESSURE);
  strintPRESSUREVALUE.toCharArray(PRESSURESHOW, 10);
}

void calculate_temperature() {
  TEMPERATURE = analogRead(temperaturePin);
  TEMPERATURE = TEMPERATURE / 2;
  String strTEMPERATUREVALUE = String(TEMPERATURE);
  strTEMPERATUREVALUE.toCharArray(TEMPERATURESHOW, 10);
}

void calculate_waterflow() {
  WATERFLOW = analogRead(waterPin);
  WATERFLOW = WATERFLOW * 0.029;
  String strWATERVALUE = String(WATERFLOW);
  strWATERVALUE.toCharArray(WATERFLOWSHOW, 10);
}

void calculate_ph() {
  PH = analogRead(pHPin);
  PH = PH * 0.01367;
  String strPH = String(PH);
  strPH.toCharArray(PHSHOW, 10);
}

void conditionforsms(){
  if (TEMPERATURE < 20 || TEMPERATURE > 30 || PRESSURE > 850 || PRESSURE < 800 || PH < 5 || PH > 10 || WATERFLOW < 20){
    if (number_of_iter == 0){
      mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
      delay(1000);  // Delay of 1 second
      mySerial.println("AT+CMGS=\"+919625268535\"\r"); //mobile number to send a text to
      delay(1000);
      receivingSerial.println();
      receivingSerial.println("       SOS! EMERGENCY!");
      receivingSerial.println("......................");
      receivingSerial.println("Issue in Water Pipe!");
      receivingSerial.println("......................");
      receivingSerial.println("Location....");
      receivingSerial.println(gps_connect());
      receivingSerial.println("......................");
      receivingSerial.println("Needed Immediate Attention");
      receivingSerial.println("......................");
      receivingSerial.println("Current Status...");
      receivingSerial.println("Water Flow Rate - " + String(WATERFLOWSHOW) + " l/min");
      receivingSerial.println("Temperature - " + String(TEMPERATURESHOW)+ "°C");
      receivingSerial.println("Pressure - " + String(PRESSURESHOW) + "Pa");
      receivingSerial.println("pH Value - " + String(PHSHOW));
      receivingSerial.println();
      delay(100);
      mySerial.println((char)26);// ASCII code of CTRL+Z for saying the end of sms to  the module 
      delay(1000);
      if (mySerial.available()>0){
        receivingSerial.write(mySerial.read());
      }
      number_of_iter = 1;
    }
  }
}

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  gpsserial.begin(9600);
  receivingSerial.begin(9600);

  lcd.begin(20, 4);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water Flow - ");
  lcd.setCursor(0, 1);
  lcd.print("pH value - ");
  lcd.setCursor(0, 2);
  lcd.print("Pressure - ");
  lcd.setCursor(0, 3);
  lcd.print("Temperature - ");

}

void loop() {
  // put your main code here, to run repeatedly:
  calculate_pressure();
  calculate_temperature();
  calculate_waterflow();
  calculate_ph();
  lcd.setCursor(0,0);
  lcd.print("Flow Rate- ");
  lcd.print(WATERFLOWSHOW);
  lcd.print(" l/min");
  lcd.setCursor(0,1);
  lcd.print("pH value- ");
  lcd.print(PHSHOW);
  lcd.setCursor(0,2);
  lcd.print("Pressure- ");
  for(int i=0; i<7; i++){
    lcd.write(PRESSURESHOW[i]);
  }
  lcd.print("Pa");
  lcd.setCursor(0, 3);
  lcd.print("Temperature- ");// print name
  lcd.print(TEMPERATURESHOW);
  lcd.print((char)223);
  lcd.print("C ");
  delay(500);  
  if(PH !=0 && WATERFLOW !=0){
    conditionforsms();  
  }
}


float gps_connect() {
  while (receivingSerial.available()) { // check for gps data
    if (gps.encode(gpsserial.read())) // encode gps data
    {
      gps.f_get_position(&lat, &lon); // get latitude and longitude
    }
  }

  String latitude = String(lat, 6);
  String longitude = String(lon, 6);
  String location = latitude + "°N , " + longitude + "°S";
  location.toCharArray(location_statement, 50);
  receivingSerial.println("Latitude: " + latitude + "," "Longitude: " + longitude);
  delay(1000);
}
