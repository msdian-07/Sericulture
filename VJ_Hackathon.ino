#include <FirebaseESP32.h>
#include <WiFi.h>
#include "DHT.h"
#define MQ3_PIN 34 
#define VOLTAGE_REF 5.0
#define CO_SENSOR_CALIBRATION 9.83 
#define SENSOR_SENSITIVITY_CO 0.2

#define WIFI_SSID "Abcdefg"  
#define WIFI_PASSWORD "12349876" 
#define FIREBASE_HOST "https://zhagaram-99dd9-default-rtdb.firebaseio.com/"
#define FIREBASE_Authorization_key "THD2CtDBpgCzkVkHGtFCz0babdtGxVCOnnOPdJBj"

#define DHTPIN 14    
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

FirebaseData firebaseData;
FirebaseJson json;
int motor =0;
int count =0;
int flag = 0;
int p_flag=0;
int d_flag=0;
int Relay=27;
int motor_button=0;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() {
  pinMode(5,OUTPUT);
  digitalWrite(5,HIGH);
 Serial.begin(115200);
  dht.begin();
   WiFi.begin (WIFI_SSID, WIFI_PASSWORD);
   Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
   pinMode(18,OUTPUT);
  digitalWrite(18,HIGH); 
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();


  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.api_key = FIREBASE_Authorization_key; 
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_Authorization_key; // Set legacy token


  Firebase.begin(&firebaseConfig, &firebaseAuth);

  pinMode(Relay,OUTPUT);
  pinMode(12,OUTPUT);
  Firebase.setFloat(firebaseData, "/Safety/counter", 0);
  pinMode(26,OUTPUT);
  pinMode(Relay,OUTPUT);
  digitalWrite(Relay, LOW);
}

void loop() {
  float hum = dht.readHumidity();
  float temp = dht.readTemperature(); 
  int sensorValue = analogRead(MQ3_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);
  float coConcentration = pow(10, (voltage - CO_SENSOR_CALIBRATION)); 
  if (Firebase.getString(firebaseData, "/Safety/motor_on")) {
    motor = firebaseData.stringData().toInt();
  }
   if (Firebase.getString(firebaseData, "/Safety/flag")) {
    p_flag = firebaseData.stringData().toInt();
  }
  if (Firebase.getString(firebaseData, "/Safety/d_flag")) {
    d_flag = firebaseData.stringData().toInt();
  }
  if (Firebase.getString(firebaseData, "/Safety/motor")) {
    motor_button = firebaseData.stringData().toInt();}
  
   if (isnan(hum) || isnan(temp)  ){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print("°C");
  Serial.print(" Humidity: ");
  Serial.print(hum);
  Serial.print("% ");
  Serial.println(coConcentration);
  Serial.println();
  //Serial.println(p_flag);
  if((temp > 33) || (motor == 1)){
    flag=1;
    digitalWrite(Relay, HIGH);
    Firebase.setFloat(firebaseData, "/Safety/motor_on", 1);
  }
  if (temp<=30){
    digitalWrite(Relay, LOW);
    Firebase.setFloat(firebaseData, "/Safety/motor_on", 0);
    if (flag == 1){
      count++;
      flag=0;
      Firebase.setFloat(firebaseData, "/Safety/counter", count);
    }

  }
  if (p_flag==1){
    digitalWrite(12,HIGH);
  }
  if (p_flag==0){
    digitalWrite(12,LOW);
  }
   if (d_flag==1){  
    digitalWrite(26,HIGH);
  }
  if (d_flag==0){
    digitalWrite(26,LOW);
  }
  

  Firebase.setFloat(firebaseData, "/Safety/temperature", temp);
  Firebase.setFloat(firebaseData, "/Safety/CO",  coConcentration);
   delay(2000);
   }