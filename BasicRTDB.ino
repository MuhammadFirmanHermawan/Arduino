
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>



#define WIFI_SSID "Hotspot Firman"
#define WIFI_PASSWORD "085523575949"
#define API_KEY "AIzaSyBvBycIR6Je8SLC4pERxCASDklUjROqT3c"
#define DATABASE_URL "https://gigihfedm7-default-rtdb.asia-southeast1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
#define USER_EMAIL "fadhil@gmail.com"
#define USER_PASSWORD "naruto"

FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;


const int trigPin = 12;
const int echoPin = 14;


#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm = 0.0;
float sumdistanceCm = 0.0;
float ratarataKetinggianAir = 0.0;



const int ph_Pin = A0;
float PH_step;
int nilai_analog_PH;
double TeganganPh;
float PH4 = 3.3;
float PH7 = 2.6;

float Po = 0.0;
float sumPo = 0.0;
float ratarataPH = 0.0;
int counter;

void setup()
{
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT); 
  pinMode (ph_Pin, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

#if defined(ESP8266)
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif
  fbdo.setResponseSize(2048);
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;

}

void loop()
{


  Serial.print("Counter : ");
  Serial.println(counter+1);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_VELOCITY / 2;
  sumdistanceCm += distanceCm;
  Serial.print("Ketinggian Air (cm): ");
  Serial.println(distanceCm);

  if (
    nilai_analog_PH = analogRead(ph_Pin)) {
    Serial.print("Nilai ADC Ph : ");
    Serial.println(nilai_analog_PH);
    TeganganPh = 3.3 / 1024.0 * nilai_analog_PH;
    Serial.print("TeganganPh: ");
    Serial.println(TeganganPh, 3);

    PH_step =  (PH4 - PH7) / 3;
    Po = 7.00 + ((PH7 - TeganganPh) / PH_step);
    sumPo += Po;

    Serial.print("Nilai PH cairan : ");
    Serial.println(Po, 2);

  }

  counter++;
  Serial.println();
  
  if (Firebase.ready() && counter == 5)
  {
    delay(600);
    
    sendDataPrevMillis = millis();
    ratarataKetinggianAir = sumdistanceCm / 5;
    ratarataPH = sumPo / 5;
    counter = 0;

    char mydata1[30];
    snprintf(mydata1, sizeof(mydata1), "%0.2f", ratarataKetinggianAir );
    double val1 = atof(mydata1);
    Serial.printf("Set float... %s\n", Firebase.RTDB.setFloat(&fbdo, F("/sensor/ketinggianAir"), val1) ? "ok" : fbdo.errorReason().c_str());
    delay(600);
    char mydata2[30];
    snprintf(mydata2, sizeof(mydata2), "%0.2f", ratarataPH );
    double val2 = atof(mydata2);
    Serial.printf("Set float... %s\n", Firebase.RTDB.setFloat(&fbdo, F("/sensor/phAir"), val2) ? "ok" : fbdo.errorReason().c_str());

    sumdistanceCm = 0.0;
    sumPo = 0;

    Serial.println();

  }
  
  delay(3000);
}
