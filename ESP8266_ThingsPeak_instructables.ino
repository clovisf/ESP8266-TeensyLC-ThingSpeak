int sensor_temp = A0;
int value_temp;
int sensor_light = A1;
int value_light;
int sensor_humid = A2;
int value_humid;

//*-- IoT Information
#define SSID "Fritzen"
#define PASS "familiafritzen"
#define IP "184.106.153.149" // ThingSpeak IP Address: 184.106.153.149

// GET /update?key=[THINGSPEAK_KEY]&field1=[data 1]&field2=[data 2]...;
String GET = "GET /update?key=VS3CGGU88I4F3VX4";

void setup() {
   Serial2.begin(9600);
  Serial.begin(115200);

  sendDebug("AT");
  delay(5000);
  if(Serial2.find("OK"))
  {
    Serial.println("RECEIVED: OK\nData ready to sent!");
    connectWiFi();
  }

}

void loop() {
  value_temp = analogRead(sensor_temp);
  value_light = analogRead(sensor_light);
  value_humid = analogRead(sensor_humid);
  String temp =String(value_temp);// turn integer to string
  String light= String(value_light);// turn integer to string
  String humid=String(value_humid);// turn integer to string
  updateTS(temp,light, humid);
  delay(3000); //
}
//----- update the  Thingspeak string with 3 values
void updateTS( String T, String L , String H)
{
  // ESP8266 Client
  String cmd = "AT+CIPSTART=\"TCP\",\"";// Setup TCP connection
  cmd += IP;
  cmd += "\",80";
  sendDebug(cmd);
  delay(2000);
  if( Serial2.find( "Error" ) )
  {
    Serial.print( "RECEIVED: Error\nExit1" );
    return;
  }

  cmd = GET + "&field1=" + T +"&field2="+ L + "&field3=" + H +"\r\n";
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
