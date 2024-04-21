#include <ESP8266WiFi.h>  
#include <LiquidCrystal.h> //Dołączenie bilbioteki
#include "messages.h"
#include <map>

#define PAIR_END_1 "no"
#define PAIR_END_2 "more"
#define ID "_id"
#define GAME "name"
#define MAX_PLAYERS "members"
#define ACTUAL_PLAYERS "players"
#define GAME_ID "gameId"
#define MS_500 500

LiquidCrystal lcd(12, 14, 0, 4, 5, 16); //Informacja o podłączeniu nowego wyświetlacza

typedef std::pair<std::string, std::string> key_val_pair;
typedef std::map<std::string, std::string> json_map;

enum sensor_data{
  GOOD = 0,
  TOO_LOW = 1,
  TOO_HIGH = 2
}; 

const char* ssid     ="LPAWiFi1"; // Tu wpisz nazwę swojego wifi
const char* password = "LPAWiFi1Password"; // Tu wpisz hasło do swojego wifi
std::string game_id = "";
unsigned long button_time = 0;  
unsigned long last_button_time = 0;
bool busy = true;
WiFiClient client;

void ICACHE_RAM_ATTR button_interrupt(){
  auto accept = digitalRead(D7);
  auto decline = digitalRead(D4);
  auto busy = digitalRead(D8);

  button_time = millis();
  if (button_time - last_button_time > MS_500){
    if (accept == LOW){
      std::string usr_rsp = "accept";
      std::string rsp = game_response(usr_rsp, game_id);
      client.print(rsp.c_str());
    }
    else if (decline == LOW){
      std::string usr_rsp = "reject";
      std::string rsp = game_response(usr_rsp, game_id);
      client.print(rsp.c_str());
    }
    else if (busy == LOW){
      std::string bsy= "{\"type\":\"busy\"}";
      client.print(bsy.c_str());
      busy = !busy;
    }
    last_button_time = button_time;
  }
}

void print_game_ready(){
  lcd.clear(); 
  lcd.begin(16, 2); 
  lcd.setCursor(3, 0); 
  lcd.print("Game ready"); 
  lcd.setCursor(1, 1); 
  lcd.print("time for break"); 
}

void print_server_sck(){
  lcd.clear(); 
  lcd.begin(16, 2); //Deklaracja typu
  lcd.setCursor(5, 0); //Ustawienie kursora
  lcd.print("Answer"); 
  lcd.setCursor(2, 1); //Ustawienie kursora
  lcd.print("acknowledged"); 
}

void lcd_measurement(sensor_data &temp, sensor_data &hum, sensor_data &noise){
  lcd.clear(); 
  lcd.begin(16, 2); //Deklaracja typu
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("Bad environment"); 
  lcd.setCursor(0, 1); //Ustawienie kursora

  if (temp != GOOD){
    if (temp == TOO_HIGH){
      lcd.print("T-H"); //Wyświetlenie tekstu
    } else {
      lcd.print("T-L"); //Wyświetlenie tekstu
    }
  }
  lcd.setCursor(5, 1); //Ustawienie kursora
  if (hum != GOOD){
    if (hum == TOO_HIGH){
      lcd.print("H-H"); //Wyświetlenie tekstu
    } else {
      lcd.print("H-L"); //Wyświetlenie tekstu
    }
  }
  lcd.setCursor(10, 1); //Ustawienie kursora
  if (noise != GOOD){
    lcd.print("L-H"); //Wyświetlenie tekstu
  }    
}

void print_measurement(json_map &map){
  sensor_data tmp = GOOD;
  sensor_data hum = GOOD;
  sensor_data noise = GOOD;
    if (map.find((std::string)(TEMP)) != map.end()){
      std::string err = map[TEMP];
      if (err.find("low") != std::string::npos){
        tmp = TOO_LOW;
      } else {
        tmp = TOO_HIGH;
      }
    }
    if (map.find((std::string)(HUM)) != map.end()){
      std::string err = map[HUM];
      if (err.find("low") != std::string::npos){
        hum = TOO_LOW;
      } else {
        hum = TOO_HIGH;
      }
    }
    if (map.find((std::string)(NOISE)) != map.end()){
      noise = TOO_HIGH;
    }
  lcd_measurement(tmp, hum, noise);
}

void save_game_id(json_map &map){
  game_id = map[GAME_ID];
}

void print_game(json_map &map){
  std::string name = map[GAME];
  std::string max = map[MAX_PLAYERS];
  std::string act = map[ACTUAL_PLAYERS];
  std::string players = "Players: " + act + "/" + max;
  lcd.clear();
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print(name.c_str()); //Wyświetlenie tekstu
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print(players.c_str()); //Wyświetlenie tekstu
}

bool check_id(json_map &map){
  return map[ID] == std::string(DEVICE_ID);
}

int check_game(json_map){
  return 0;
}

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
  message = message.erase(0, end_val+1);
  // if (val.find("\"") != std::string::npos){
  //   message.erase(0, 1);
  //   message.erase(message.size() - 1);
  // }
  retVal.first = key;
  retVal.second = val;
  Serial.println("key_value_pair");
  Serial.println((String)(key.c_str()));
  Serial.println((String)(val.c_str()));
  return retVal;
}

void response_to_server(std::string message){
  const char* msg = message.c_str();
  Serial.println("Msg original");
  Serial.println((String)(msg));
  message.erase(0, 1);
  // message.erase(message.size() - 1);
  // message = message + ",";
  json_map map;
  parse_json(map, message);

  // Serial.println("Map below");
  // for (const auto& c : map){
  //   const char* key = c.first.c_str();
  //   const char* val = c.second.c_str();
  //   Serial.println((String)(key));
  //   Serial.println((String)(val));
  // }

  // bool status = true; 
  // status = check_id(map);
  save_game_id(map);
  std::string msg_type = map["type"];
  if (msg_type == "game"){
    print_game(map);
  } else if (msg_type == "measurements"){
    print_measurement(map);
  } else if (msg_type == "game_response"){
    print_server_sck();
  } else if (msg_type == "game_accept"){
    print_game_ready();
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(D4, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  pinMode(D8, INPUT);
  attachInterrupt(digitalPinToInterrupt(D4), button_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(D7), button_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(D8), button_interrupt, FALLING); 
  //Wyświetlacz LED
  lcd.begin(16, 2); //Deklaracja typu
  lcd.setCursor(0, 0); //Ustawienie kursora
  lcd.print("Good to see you"); //Wyświetlenie tekstu
  lcd.setCursor(0, 1); //Ustawienie kursora
  lcd.print("Have a nice work"); //Wyświetlenie tekstu
  
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
      client.print(welcome_msg.c_str());
  } 
}

void loop() {
  
  if (client.connected()){
    String response = client.readStringUntil('}');
    std::string std_response = std::string(response.c_str());
    // std_response = R"({"gameId":"12345","name":"Szachy"})";
    if (std_response.size()){
      Serial.println(response);
      response_to_server(std_response);
    }
  } else {
    Serial.println("Connection lost, trying to reconnect");
    client.connect("192.168.241.195", 9000);
    client.print(welcome_msg.c_str());

  }

  delay(1000);
  
}
