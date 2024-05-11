#define _SSID "Vu" 
#define _PASSWORD "phat@@@@???!!"
#define REFERENCE_URL "he-thong-lora-default-rtdb.firebaseio.com"

#define ledRun   21
#define ledAlarm 5
#define buzz     12


#include "iconSuDung.h"
#include <TFT_eSPI.h> 
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  

#include "LoRa_E32.h"
LoRa_E32 e32ttl(&Serial2); 
#include <ESP32Firebase.h>

#include <WiFi.h>

TaskHandle_t TaskFirebase;
void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);


float temptValue1 = 0, temptValue2 = 0;
float humiValue1 = 0, humiValue2 = 0;
float dustValue1 = 0, dustValue2 = 0;
float gasValue1 = 0, gasValue2 = 0;
int temptThres = 40, humiThres = 70, dustThres = 30, gasThres = 50;
int temptThres2 = 40, humiThres2 = 70, dustThres2 = 30, gasThres2 = 50;

unsigned long previousMillis = 0, currentMillis = 0;
int ledState   = 0;
unsigned long previousMillis1 = 0, currentMillis1 = 0;
Firebase firebase(REFERENCE_URL);
void setup()
{
  Serial.begin(115200);

  WiFi.begin (_SSID,_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  pinMode(ledRun, OUTPUT);
  pinMode(ledAlarm, OUTPUT);
  pinMode(buzz, OUTPUT);

  digitalWrite(ledRun, LOW);
  digitalWrite(ledAlarm, LOW);

  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  e32ttl.begin();

  xTaskCreatePinnedToCore(
    FirebaseTask,   /* Task function. */
    "TaskFirebase",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &TaskFirebase,      /* Task handle to keep track of created task */
    1); 
}

void loop()
{
  getReceiveData();
  CanhBao();
} 

void FirebaseTask(void *pvParameters) {
  while (1) 
  {
    firebase.setFloat("Trạm 1/Nhiệt độ", temptValue1);
    firebase.setFloat("Trạm 1/Độ ẩm không khí", humiValue1);
    firebase.setFloat("Trạm 1/Nồng độ CO", gasValue1);
    firebase.setFloat("Trạm 1/Bụi", dustValue1);
    
    firebase.setFloat("Trạm 2/Nhiệt độ", temptValue2);
    firebase.setFloat("Trạm 2/Độ ẩm không khí", humiValue2);
    firebase.setFloat("Trạm 2/Nồng độ CO", gasValue2);
    firebase.setFloat("Trạm 2/Bụi", dustValue2); 

    temptThres = firebase.getFloat("Ngưỡng 1/Nhiệt độ");
    humiThres = firebase.getFloat("Ngưỡng 1/Độ ẩm không khí");
    dustThres = firebase.getFloat("Ngưỡng 1/Bụi");
    gasThres = firebase.getFloat("Ngưỡng 1/Nồng độ CO");
    Serial.print(temptThres); 
    Serial.print(humiThres); 
    Serial.print(dustThres); 
    Serial.print(gasThres); 

    temptThres2 = firebase.getFloat("Ngưỡng 2/Nhiệt độ");
    humiThres2 = firebase.getFloat("Ngưỡng 2/Độ ẩm không khí");
    dustThres2 = firebase.getFloat("Ngưỡng 2/Bụi");
    gasThres2 = firebase.getFloat("Ngưỡng 2/Nồng độ CO");
    Serial.print(temptThres); 
    Serial.print(humiThres); 
    Serial.print(dustThres); 
    Serial.print(gasThres);

    vTaskDelay(pdMS_TO_TICKS(3000));
  }
}

void getReceiveData() {
  if (e32ttl.available()  > 1) 
  {
    ResponseContainer rs = e32ttl.receiveMessage();
    String message = rs.data;
    slitReceiveData(message);
    TFT_Print();
  }
}

void slitReceiveData(String rx_string)
{
    String str1 = getValue(rx_string, ',', 0);
    String str2 = getValue(rx_string, ',', 1);
    String str3 = getValue(rx_string, ',', 2);
    String str4 = getValue(rx_string, ',', 3);
    String str5 = getValue(rx_string, ',', 4);

    if(str1 == "1")
    {
      temptValue1 = str2.toInt();
      humiValue1 = str3.toInt();
      dustValue1 = str4.toInt();
      gasValue1 = str5.toInt();
    } else if(str1 == "2")
    {
      temptValue2 = str2.toInt();
      humiValue2 = str3.toInt();
      dustValue2 = str4.toInt();
      gasValue2 = str5.toInt();
    }
}
void TFT_Print() {

  tft.drawRect(1, 2, 158, 126, TFT_WHITE); // Màu nền đen
  tft.drawLine(40, 0, 40, 128, TFT_WHITE);
  tft.drawLine(100, 0, 100, 128, TFT_WHITE);

  tft.drawLine(0, 29, 158, 29, TFT_WHITE);  // Kẻ ngang
  tft.drawLine(0, 54, 158, 54, TFT_WHITE);
  tft.drawLine(0, 79, 158, 79, TFT_WHITE);
  tft.drawLine(0, 103, 158, 103, TFT_WHITE);
  tft.setSwapBytes(true);
  tft.pushImage(9, 30, 24, 24, iconNhietdo);
  tft.pushImage(9, 55, 24, 24, iconDoam);
  tft.pushImage(10, 80, 23, 23, iconDat);
  tft.pushImage(9, 104, 23, 23, iconBui);

  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Màu chữ trắng trên nền đen
  tft.setTextFont(1);

  tft.pushImage(8, 4, 23, 23, Logo);

  tft.setTextColor(TFT_WHITE, TFT_BLACK); // Màu chữ trắng trên nền đen
  tft.setTextFont(2);
  tft.drawString("Area 1", 50, 7); tft.drawString("Area 2", 110, 7);
}


//**************************************************************************************************************
void CanhBao() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= 500) {
    previousMillis = currentMillis;
    if (ledState == HIGH) {
      ledState = LOW;
    } else {
      ledState = HIGH;
    }
  }


if (gasValue1  > gasThres)
  {
    digitalWrite(ledRun, LOW);        
    digitalWrite(ledAlarm, ledState);

    digitalWrite(buzz, !ledState); 
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(44, 84); tft.print(gasValue1, 2); 
  } else
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(44, 84); tft.print(gasValue1, 2); 
  }

  if (gasValue2  > gasThres2)
  {
    digitalWrite(ledRun, LOW);        
    digitalWrite(ledAlarm, ledState);  
    digitalWrite(buzz, !ledState); 
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(104, 84); tft.print(gasValue2, 2);
  } else
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(104, 84); tft.print(gasValue2, 2);
  }









  if (dustValue1  > dustThres)
  {
    digitalWrite(ledRun, LOW);        
    digitalWrite(ledAlarm, ledState);

    digitalWrite(buzz, !ledState); 
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(44, 107); tft.print(dustValue1, 2); 
  } else
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(44, 107); tft.print(dustValue1, 2); tft.print(" ");
  }

  if (dustValue2  > dustThres2)
  {
    digitalWrite(ledRun, LOW);        
    digitalWrite(ledAlarm, ledState);  
    digitalWrite(buzz, !ledState); 
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(104, 107); tft.print(dustValue2, 2); 
  } else
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(104, 107); tft.print(dustValue2, 2); tft.print(" ");
  }

if (humiValue1  < humiThres)
  {
    digitalWrite(ledRun, LOW);        
    digitalWrite(ledAlarm, ledState);  
    digitalWrite(buzz, !ledState); 
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(44, 57); tft.print(humiValue1, 2);
  } else
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(44, 57); tft.print(humiValue1, 2);
  }

if (humiValue2  < humiThres2)
  {
    digitalWrite(ledRun, LOW);        
    digitalWrite(ledAlarm, ledState);  
    digitalWrite(buzz, !ledState); 
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(2);tft.setCursor(104, 57); tft.print(humiValue2, 2);
  } else
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(104, 57); tft.print(humiValue2, 2);
  }

if (temptValue1  > temptThres)
  {
    digitalWrite(ledRun, LOW);        
    digitalWrite(ledAlarm, ledState); 
    digitalWrite(buzz, !ledState);  
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(44, 31); tft.print(temptValue1, 2); 
  } else
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(44, 31); tft.print(temptValue1, 2); 
  }

if (temptValue2  > temptThres2)
  {
    digitalWrite(ledRun, LOW);        
    digitalWrite(ledAlarm, ledState);  
    digitalWrite(buzz, !ledState); 
    
    tft.setTextColor(TFT_RED, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(104, 31); tft.print(temptValue2, 2);
  } else
  {
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextFont(2);
    tft.setCursor(104, 31); tft.print(temptValue2, 2);
  }

    if((dustValue1 < dustThres) && (dustValue2 < dustThres2) && (temptValue1 < temptThres) &&
      (temptValue2 < temptThres2) && (humiValue1 > humiThres) && (humiValue2 > humiThres2)
      && (gasValue1 < gasThres) && (gasValue2 < gasThres2))
    {
      digitalWrite(ledRun, HIGH);       
      digitalWrite(ledAlarm, LOW);
      digitalWrite(buzz, LOW);
    }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
 
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}