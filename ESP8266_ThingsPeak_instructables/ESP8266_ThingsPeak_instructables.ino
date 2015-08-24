#include "DHT.h"
#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

#include <Wire.h>
#include <Adafruit_BMP085.h> // BMP085 pressure sensor
Adafruit_BMP085 bmp;

// Variables used on the "delay" function
unsigned long time;
unsigned long previousTime;
boolean enterFunction= true;
//-----------------------

int ntc5k = A0;
float value_ntc5k;
int UVsensor = A1;
float UVlight;
int ldr = A2;
float value_ldr;
int lm35 = A3;
float value_lm35;
float value_temp_BMP;
float value_altitude_BMP;
float value_pressure_BMP;
float UVvalue;

//*-- IoT Information
#define SSID "FRITZEN"
#define PASS "3649-9334Js"
#define IP "184.106.153.149" // ThingSpeak IP Address: 184.106.153.149

// GET /update?key=[THINGSPEAK_KEY]&field1=[data 1]&field2=[data 2]...;
String GET = "GET /update?key=VS3CGGU88I4F3VX4";

void setup() {
  Serial2.begin(9600);
  Serial.begin(115200);
  dht.begin();

  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }

  sendDebug("AT");
  delay(2000); // changed from 5000 to 2000
  if(Serial2.find("OK"))
  {
    Serial.println("RECEIVED: OK\nData ready to sent!");
    connectWiFi();
  }

}

void loop() {
  time = micros();
  if (enterFunction == true){
    previousTime= time;
    Serial.println(previousTime); // for debugging

    // Start your code below 
    //-----------------------


  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  
 
  value_ntc5k = analogRead(ntc5k); // curve fitting for the NTC
  value_ntc5k = ((value_ntc5k*0.1002)-23.859)*1.1201; // curve fitting for the NTC
  value_ldr = (analogRead(ldr)*1/0.097752)/100; // making LDR value a percentual (0-100%)
  value_lm35 = analogRead(lm35)/3.081664;
  value_temp_BMP = bmp.readTemperature(); // BMP085 temperature
  value_pressure_BMP = bmp.readPressure(); // BMP085 pressure
  value_altitude_BMP = bmp.readAltitude(); // BMP085 altitude 
  UVlight=analogRead(UVsensor)*0.00495161; //UV index
  String temp =String(value_ntc5k);// turn integer to string
  String temp2 =String(value_lm35);
  String UVunit= String(UVlight);// turn integer to string
  String ldrlight=String(value_ldr);// turn integer to string
  String humid=String(h); //DHT11 humidity
  String temp3=String(t);
  String temp4=String(value_temp_BMP);
  String pressure=String(value_pressure_BMP);
  //String altitude1=String(value_altitude_BMP); 
  updateTS(temp,temp2,temp3,temp4,ldrlight,humid,pressure,UVunit);
  Serial.println(value_ntc5k);
  Serial.println(UVlight);
  Serial.println(value_ldr);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  
  //-----------------------
  // End of your code
  }
  
  // The DELAY time is adjusted in the constant below >> 
  if (time - previousTime < 19999990){ // 1 million microsencods= 1 second delay
    /* I have actually used 0.999990 seconds, in a trial to compensate the time that
       this IF function takes to be executed. this is really a point that
       need improvement in my code */   
    enterFunction= false;
  } 
  else {
    enterFunction= true;
  }
  
}
//----- update the  Thingspeak string with 3 values
void updateTS( String A, String B, String C , String D, String E, String F, String G, String H)
{
  // ESP8266 Client
  String cmd = "AT+CIPSTART=\"TCP\",\"";// Setup TCP connection
  cmd += IP;
  cmd += "\",80";
  sendDebug(cmd);
  delay(800);
  if( Serial2.find( "Error" ) )
  {
    Serial.print( "RECEIVED: Error\nExit1" );
    return;
  }

  cmd = GET + "&field1=" + A +"&field2="+ B + "&field3=" + C +"&field4=" + D +"&field5=" + E +"&field6=" + F +"&field7=" + G +"&field8=" + H +"\r\n";
  Serial2.print( "AT+CIPSEND=" );
  Serial2.println( cmd.length() );
  if(Serial2.find( ">" ) )
  {
    Serial.print(">");
    Serial.print(cmd);
    Serial2.print(cmd);
  }
  else
  {
    sendDebug( "AT+CIPCLOSE" );//close TCP connection
  }
  if( Serial2.find("OK") )
  {
    Serial.println( "RECEIVED: OK" );
  }
  else
  {
    Serial.println( "RECEIVED: Error\nExit2" );
  }
}

void sendDebug(String cmd)
{
  Serial.print("SEND: ");
  Serial.println(cmd);
  Serial2.println(cmd);
}

boolean connectWiFi()
{
  Serial2.println("AT+CWMODE=1");//WiFi STA mode - if '3' it is both client and AP
  delay(2000);
  //Connect to Router with AT+CWJAP="SSID","Password";
  // Check if connected with AT+CWJAP?
  String cmd="AT+CWJAP=\""; // Join accespoint
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  sendDebug(cmd);
  delay(5000);
  if(Serial2.find("OK"))
  {
    Serial.println("RECEIVED: OK");
    return true;
  }
  else
  {
    Serial.println("RECEIVED: Error");
    return false;
  }

  cmd = "AT+CIPMUX=0";// Set Single connection
  sendDebug( cmd );
  if( Serial2.find( "Error") )
  {
    Serial.print( "RECEIVED: Error" );
    return false;
  }
}
