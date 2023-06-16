#define BLYNK_TEMPLATE_ID "TMPL62rwp0aW3"
#define BLYNK_TEMPLATE_NAME "Deteksi objek dengan ultrasonik"
#define BLYNK_AUTH_TOKEN "zckKCBW8_go2Fse8Kku8CjL3_MTag5b2"

#include <CTBot.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

BlynkTimer timer;
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "";
char pass[] = "";

CTBot TeleBot;
String ssidbot = "";
String passbot = "";
String tokenbot = "6065894141:AAGD7UTAEA05StLdhHnI2__zIWUxjUSmTJI";
const int idbot =  1241137265;

#define TRIGGERPIN D1
#define ECHOPIN D0
#define ledPinMerah D7
#define ledPinKuning D6
#define ledPinHijau D5
#define PinPIR D2
#define buzzer D8

bool switchOnOff = 0;
bool notifikasiTelegram = 0;

bool deteksiGerakan(){
  int nilaiPIR = digitalRead(PinPIR);
  bool val = 0;

  if(nilaiPIR == 1){
    Serial.println("Pir sensor mendeteksi gerakan!");
    Blynk.virtualWrite(V4, "Pir sensor mendeteksi gerakan!");
    digitalWrite(ledPinKuning, HIGH);
    Blynk.virtualWrite(V6, 1);
    val = 1;
  }else{
    digitalWrite(ledPinKuning, LOW);
    Blynk.virtualWrite(V6, 0);
    Blynk.virtualWrite(V4, "None");
    val = 0;
  }
  return val;
}

int hitungJarak(){
  long durasi, jarak;
  digitalWrite(TRIGGERPIN, LOW);
  delay(10);

  digitalWrite(TRIGGERPIN, HIGH);
  delay(10);

  digitalWrite(TRIGGERPIN, LOW);
  durasi = pulseIn(ECHOPIN, HIGH);
  jarak = (durasi/2)/29.1;
  Serial.print("Jarak Terdeteksi : ");
  Serial.print(jarak);
  Serial.println(" cm");
  Blynk.virtualWrite(V3, jarak);
  delay(500);
  return jarak;
}

//Buat ngebaca nilai V2 untuk nyalain alat
BLYNK_WRITE(V2){
  int pinV2 = param.asInt();
  Serial.println(pinV2);
  if(pinV2 == 1){
    switchOnOff = 1;
  }else{
    switchOnOff = 0;
  }
}

//Buat ngasi notif telegram
BLYNK_WRITE(V1){
  int pinV1 = param.asInt();
  Serial.println(pinV1);
  if(pinV1 == 1){
    notifikasiTelegram = 1;
  }else{
    notifikasiTelegram = 0;
  }
}

//Inisiasi berbagai hal
void setup() {
  Serial.begin(9600);
  //inisiasi pin
  pinMode(ledPinMerah, OUTPUT);
  pinMode(ledPinKuning, OUTPUT);
  pinMode(ledPinHijau, OUTPUT);
  pinMode(TRIGGERPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(PinPIR, INPUT);
  pinMode(buzzer, OUTPUT);
  digitalWrite(ledPinMerah, LOW);
  digitalWrite(ledPinKuning, LOW);
  digitalWrite(ledPinHijau, LOW);
  digitalWrite(buzzer, LOW);
  //biar bisa nyambung ke blynk
  Blynk.begin(auth, ssid, pass);
  //biar bisa nyambung ke bot telegram
  TeleBot.wifiConnect(ssidbot, passbot);
  TeleBot.setTelegramToken(tokenbot);
}

//looping kode
void loop() {
  if (switchOnOff == 1){
    digitalWrite(ledPinHijau, HIGH);
    Blynk.virtualWrite(V7, 1);

    bool val = deteksiGerakan();
    int jarak = hitungJarak();

    if(jarak < 30 && val == 1){
      digitalWrite(ledPinMerah, HIGH);
      Blynk.virtualWrite(V5, 1);
      tone(buzzer, 250, 2000);
      delay(2000);
      if(notifikasiTelegram == 1){
        TeleBot.sendMessage(idbot, "Ada tamu bang!");
      }
    }else{
      digitalWrite(ledPinMerah, LOW);
      Blynk.virtualWrite(V5, 0);
    }
  }
  else{
    digitalWrite(ledPinHijau, LOW);
    digitalWrite(ledPinKuning, LOW);
    digitalWrite(ledPinMerah, LOW);
    Blynk.virtualWrite(V7, 0);
  }
  Blynk.run();
  timer.run();

  //ngirim data apakah device sedang online ato engga
  Blynk.virtualWrite(V0, 1);
}
