#define MAC      1  
 

#include "LoRa_E32.h"
LoRa_E32 e32ttl(3, 2);

#include "DHT.h"
#define DHTPIN 6     
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

int ledPin = 13;
int dustPin = A0;
int dustVal = 0;
int voltage = 0;
int dustRaw = 0;

int gasPin = A1; 

int gasValue = 0;
float temptValue = 0, humiValue = 0;
float dustValue = 0;

String packet;
/***************************************************/
void setup()
{
  Serial.begin(9600);
  dht.begin();
  e32ttl.begin();
  pinMode(ledPin, OUTPUT);
}

void loop()
{
  readValueDHT11();
  Serial.println();
  readValueDust();
  Serial.println();
  readValueGas();
  Serial.println();
 
  if (runEvery(6000)) 
  {
    packet = packet + String(MAC) + "," + String(temptValue) + "," + String(humiValue) 
    + "," + String(dustValue) + "," + String(gasValue);
    ResponseStatus rs = e32ttl.sendFixedMessage(0, 6, 0x09, packet);
    packet = "";
  }
  delay(500);
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

void readValueDHT11()
{
   humiValue = dht.readHumidity();
   temptValue = dht.readTemperature();
   Serial.print("humiValue:    ");
   Serial.print(humiValue);
   Serial.print("temptValue:    ");
   Serial.print(temptValue);
}
void readValueGas()
{
    gasValue = analogRead(gasPin);
    Serial.print("gasValue:    ");
    Serial.print(gasValue);
}
void readValueDust()
{
  digitalWrite(ledPin, LOW); 
  delayMicroseconds(280);
  dustVal = analogRead(dustPin); 
  delayMicroseconds(40);
  digitalWrite(ledPin, HIGH); 
  delayMicroseconds(9680);

 voltage = (dustVal * 5 / 1024.0); 

 dustValue = 170 * voltage - 0.1;

  if (dustValue < 0 )
    dustValue = 0;
  if (dustValue > 500)
    dustValue = 500;

    Serial.print("dustvalue:      ");
    Serial.print(dustValue);
}

