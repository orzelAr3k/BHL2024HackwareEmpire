#include <ESP8266WiFi.h>  
#include <LiquidCrystal.h> //Dołączenie bilbioteki
#include <SocketIoClient.h>
#include <WebSocketClient.h>
#include "messages.h"
#include <regex>

#define PAIR_END_1 "no"
#define PAIR_END_2 "more"

LiquidCrystal lcd(12, 14, 0, 4, 5, 16); //Informacja o podłączeniu nowego wyświetlacza

typedef std::pair<std::string, std::string> key_val_pair;
typedef std::map<std::string, std::string> json_map;

const char* ssid     ="LPAWiFi1"; // Tu wpisz nazwę swojego wifi
const char* password = "LPAWiFi1Password"; // Tu wpisz hasło do swojego wifi

void parse_json(json_map &map, std::string message){
  key_val_pair pair;
  while (true){
    pair = get_key_val(message);
    if (pair.first == PAIR_END_1 && pair.second == PAIR_END_2){
      break;
    }
    map[pair.first] = pair.second;  
  }
}

key_val_pair get_key_val(std::string &message){
  key_val_pair retVal;
  auto start_key = message.find("\"");
  if (start_key == std::string::npos){
    retVal.first = PAIR_END_1;
    retVal.second = PAIR_END_2;
    return retVal;
  }
  
  auto end_key = message.find("\"", start_key + 1);
  auto key = message.substr(start_key+1, end_key-start_key-1);
  message = message.erase(0, end_key+1);

  auto start_val = message.find("\"");
  
  auto end_val = message.find("\"", start_val + 1);
  auto val = message.substr(start_val+1, end_val-start_val-1);
  retVal.first = key;
  retVal.second = val;
  return retVal;
}

void response_to_server(std::string message){
  const char* msg = message.c_str();
  Serial.println("Msg original");
  Serial.println((String)(msg));
  message.erase(0, 1);
  message.erase(message.size() - 1);
  json_map map;
  parse_json(map, message);

  Serial.println("Map below");
  for (const auto& c : map){
    const char* key = c.first.c_str();
    const char* val = c.second.c_str();
    Serial.println((String)(key));
    Serial.println((String)(val));
  }

}


WiFiClient client;

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
    client.print("dupa");
    client.print("{\"test\" : \"1234\"}");
    String response = client.readStringUntil('}');
    std::string std_response = std::string(response.c_str());
    Serial.println(response);
    std_response = "{\"test\" : \"123\", \"test2\" : \"elo\"}";
    response_to_server(std_response);
  } else {
    Serial.println("Disconnected...");
    Serial.println("Reconnecting");
    client.connect("192.168.241.195", 9000);
  }
  delay(1000);
  
}
