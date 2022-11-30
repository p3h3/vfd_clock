#include "NTPClient.h"
#include "ESP8266WiFi.h"
#include "WiFiUdp.h"

#define sh_latch 4
#define sh_clock 5
#define sh_data 0

const int numbers[] = {
  0b1011000000110100,
  0b0011000000000000,
  0b1010000000110010,
  0b1011000000100010,
  0b0011000000000110,
  0b1001000000100110,
  0b1001000000110110,
  0b1011000000000000,
  0b1011000000110110,
  0b1011000000000110,
  0
};

const int punc[] = {
  0b0000010000000000,
  0b0000011000000000,
};

const int digits[] = {
  0b0100000000000000,
  0b0000100000000000,
  0b0000000100000000,
  0b0000000001000000,
  0b0000000000001000,
  0b0000000000000001
};

const char* ssid       = "clubpenguin_hotpot";
const char* password   = "plsBringItBack";

short hour = 0;
short minute = 0;
short second = 0;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  pinMode(sh_latch, OUTPUT);
  pinMode(sh_clock, OUTPUT);
  pinMode(sh_data, OUTPUT);

  shiftOutInt(numbers[0] | digits[0]);

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }

  timeClient.begin();
  timeClient.setTimeOffset(7200);

}

void shiftOutInt(int i){
  digitalWrite(sh_latch, HIGH);
  shiftOut(sh_data, sh_clock, LSBFIRST, i);
  shiftOut(sh_data, sh_clock, LSBFIRST, (i >> 8));
  digitalWrite(sh_latch, LOW);
}

void updateTime(){
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  struct tm *ptm = gmtime ((time_t *)&epochTime); 
  
  hour = timeClient.getHours();
  minute = timeClient.getMinutes();
  second = timeClient.getSeconds();
  
  short monthDay = ptm->tm_mday;
  short currentMonth = ptm->tm_mon+1;
  int currentYear = ptm->tm_year+1900;
}

void loop() {
  updateTime();

  for(int j = 0; j < 6*10; j++){
    switch (j % 6){
      case 0:
        {
        short n = floor(hour / 10);
        shiftOutInt(numbers[n] | digits[5]);
        }
        break;
      case 1:
        {
        short n = hour % 10;
        shiftOutInt(numbers[n] | digits[4] | punc[0]);
        }
        break;
      case 2:
        {
        short n = floor(minute / 10);
        shiftOutInt(numbers[n] | digits[3]);
        }
        break;
      case 3:
        {
        short n = minute % 10;
        shiftOutInt(numbers[n] | digits[2] | punc[0]);
        }
        break;
      case 4:
        {
        short n = floor(second / 10);
        shiftOutInt(numbers[n] | digits[1]);
        }
        break;
      case 5:
        {
        short n = second % 10;
        shiftOutInt(numbers[n] | digits[0] | punc[0]);
        }
        break;
      default:
        shiftOutInt(0);
    }
    delay(2);
  }
  
}
