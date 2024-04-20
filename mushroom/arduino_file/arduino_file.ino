#include  <ESP8266WiFi.h>  
#include <LiquidCrystal.h> //Dołączenie bilbioteki
#include <SocketIoClient.h>
#include <WebSocketClient.h>

LiquidCrystal lcd(12, 14, 0, 4, 5, 16); //Informacja o podłączeniu nowego wyświetlacza
 
const char* ssid     ="LPAWiFi1"; // Tu wpisz nazwę swojego wifi
const char* password = "LPAWiFi1Password"; // Tu wpisz hasło do swojego wifi

void event(const char * payload, size_t length) {
  Serial.println("Test");
}

//int ledPin = 2; // D4
//int ledPin2 = 12; // D6
WiFiClient client;
WebSocketClient ws(true);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  //Wyświetlacz LED
  lcd.begin(16, 2); //Deklaracja typu
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("Witam"); //Wyświetlenie tekstu
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print("Pozdrawiam!"); //Wyświetlenie tekstu
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  
  Serial.println("");
  Serial.println("WiFi connected");

 if (client.connect("192.168.241.195", 9000)){  // connect to server
      Serial.println("Connected to server");
  } 
}

void loop() {
  
if (client.connected()){
  client.print("dupa\n");
  String response = client.readStringUntil('\n');
  Serial.println(response);
} else {
  Serial.println("Disconnected");
  Serial.println("Reconnecting");
  client.connect("192.168.241.195", 9000);
}

  delay(1000);
  
}
