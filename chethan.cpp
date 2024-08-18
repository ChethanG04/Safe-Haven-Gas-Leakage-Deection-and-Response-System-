#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = " ";   //Your wifi name 
const char* password = "12345678";

int ledPin = 4;      // D2 for LED
int relayPin = 5;    // D1 for Relay (controlling the fan)
int buzzerPin = 0;   // D3 for Buzzer

// Initialize Telegram BOT
#define BOTtoken " " // your Bot Token from Botfather within the quotes
#define CHAT_ID " " // Chat ID

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

void setup() {
  Serial.begin(9600);

  pinMode(ledPin, OUTPUT);   // initialize LED pin as OUTPUT
  pinMode(relayPin, OUTPUT); // initialize Relay pin as OUTPUT
  pinMode(buzzerPin, OUTPUT); // initialize Buzzer pin as OUTPUT

  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
  bot.sendMessage(CHAT_ID, "Gas level monitoring", "");
}

void loop() {
  Serial.println("Gas level monitoring is on");
  int gasLevel = analogRead(A0); // Read gas level from MQ-2 sensor
  Serial.print("Gas Level: ");
  Serial.println(gasLevel);

  if (gasLevel > 725) { // Gas level exceeds threshold
    Serial.print("Gas Is Leaking, Plz take Care ");
    digitalWrite(ledPin, LOW);     // LED Off
    digitalWrite(relayPin, HIGH);  // Relay On (Fan On)

    // Beeping the buzzer
    for (int i = 0; i < 10; i++) {
      digitalWrite(buzzerPin, HIGH); // Buzzer On
      delay(1000);                    // Wait 200 milliseconds
      digitalWrite(buzzerPin, LOW);  // Buzzer Off
      delay(1000);                    // Wait 200 milliseconds
    }

    bot.sendMessage(CHAT_ID, "Gas Is Leaking, please take care", "");
    delay(5000);
  } else { // Gas level is below threshold
    digitalWrite(ledPin, HIGH);    // LED On
    digitalWrite(relayPin, LOW);   // Relay Off (Fan Off)
    digitalWrite(buzzerPin, LOW);  // Buzzer Off
  }
  delay(1000);
}
