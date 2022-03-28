#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#define IN_PIN 12
// Общие настройки
// WiFi-settings
const char* ssid = "L\itr_skotcha";
const char* password = "kusok_moloka";

// MQTT-settings
const char* mqtt_server = "188.0.19.63";
const char* mqtt_login = "someonef";
const char* mqtt_password = "someonef";
const char* mqtt_clientId = "messegecatcher";

char msg[50]; // Буфер сообщениия MQTT

// Создаем объект-киента WiFi и MQTT
WiFiClient espClient;
PubSubClient client(espClient);

LiquidCrystal_I2C lcd(0x27, 16, 2);

bool flag = false;
int letters_count = 0;
bool message_sent = false;
unsigned long timing;
char s1[] = "New message";
char s2[] = "Empty";
 
void setup(){
  pinMode(BUILTIN_LED, OUTPUT); 
  pinMode(12, INPUT);    
  Serial.begin(115200);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);   
  lcd.print("Empty");
  delay(5000);
  lcd.clear();
  setup_wifi(); //  Настройка WiFi подключения
 
  client.setServer(mqtt_server, 1883); // Задаем адрес сервера MQTT, но пока не подключаемся
  if (!client.connected()) {
    mqtt_reconnnect();
  }
  String pred_value = "Empty";
  client.publish("home/mailbox/status", String(pred_value).c_str());
  client.publish("home/mailbox/value", String(0).c_str());
}

bool tflag = false;

void loop()
{
  if (tflag == false)
  {
    lcd.setCursor(0, 0);   
    lcd.print("Count messages:");
    lcd.setCursor(0, 1);
    lcd.print(String(letters_count));
    tflag = true;
  }
  if (!client.connected()) {
    mqtt_reconnnect();
  }
  if(digitalRead(12) == 1 && message_sent == false)
  {
    String pred_value = "New message";
    letters_count += 1;
    message_sent = true;
    client.publish("home/mailbox/status", String(pred_value).c_str());
    client.publish("home/mailbox/value", String(letters_count).c_str());
    Serial.println("Hi");
    lcd.clear();
    lcd.setCursor(1, 0);   
    lcd.print("New message!");
    delay(5000);
    lcd.clear();
    tflag = false;
  }
  if (millis() - timing > 3000 && digitalRead(12) == 0)
  {
    timing = millis();
    message_sent = false;
  } 
}

void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); // Установка типа шифрования (если есть пароль - оставляем так, если нет, то надо на что-то поменять (на что, хз))
  WiFi.begin(ssid, password);
  //WiFi.begin(ssid); // Меняем строчку выше на эту, если пароля нет

  // Ждем пока не подключится
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros()); // Что это - без понятия, но наверное лучше оставить)))

  // Выводим инфу о подключении
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqtt_reconnnect() {
  // Пока не подулючится, будем пытаться это сделть
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(mqtt_clientId, mqtt_login, mqtt_password)) {
      Serial.println("Mqtt connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
