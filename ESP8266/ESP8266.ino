#include <SoftwareSerial.h>
//#define esp8266 Serial2
#define CH_PD 4 
#define speed8266 9600 // This is the speed that worked with my ESP8266
SoftwareSerial esp8266(2,3);
#define DEBUG true
 
void setup()
{
  esp8266.begin (speed8266); 
  Serial.begin(9600);
  pinMode(11,OUTPUT);
  digitalWrite(11,HIGH);
  
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);
  
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  reset8266(); // Pin CH_PD needs a reset before start communication
  InitWifiModule(); // Inciate module as WebServer 
}




void loop()
{
  if(esp8266.available()) // check if the esp is sending a message 
  {

    
    if(esp8266.find("+IPD,"))
    {
     delay(1000); // wait for the serial buffer to fill up (read all the serial data)
     // get the connection id so that we can then disconnect
     int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
          
     esp8266.find("pin="); // advance cursor to "pin="
     
     int pinNumber = (esp8266.read()-48)*10; // get first number i.e. if the pin 13 then the 1st number is 1, then multiply to get 10
     pinNumber += (esp8266.read()-48); // get second number, i.e. if the pin number is 13 then the 2nd number is 3, then add to the first number
     
     digitalWrite(pinNumber, !digitalRead(pinNumber)); // toggle pin    
     
     // make close command
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendData(closeCommand,1000,DEBUG); // close connection
    }
  }
}
void InitWifiModule()
{
  sendData("AT+RST\r\n", 2000, DEBUG); // reset
  sendData("AT+CWJAP=\"SPARSH\",\"sparsh123\"\r\n", 2000, DEBUG); //Connect network
  delay(3000);
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  sendData("AT+CIFSR\r\n", 1000, DEBUG); // Show IP Adress
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); // Multiple conexions
  sendData("AT+CIPSERVER=1,333\r\n", 1000, DEBUG); // start comm port 80
}
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
/*************************************************/ 
 // Send AT commands to module
String sendData(String command, const int timeout, boolean debug)
{
  String response = "";
  esp8266.print(command);
  long int time = millis();
  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      // The esp has data so display its output to the serial window
      char c = esp8266.read(); // read the next character.
      response += c;
    }
  }
  if (debug)
  {
    Serial.print(response);
  }
  return response;
}


void reset8266 ()
{
  pinMode(CH_PD, OUTPUT);
  digitalWrite(CH_PD, LOW);
  delay(300);
  digitalWrite(CH_PD, HIGH);
}
