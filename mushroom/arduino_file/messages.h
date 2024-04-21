#include <string>

#define DEVICE_ID "10293"
#define TEMP "temperature"
#define HUM "humidity"
#define NOISE "noise"  
const std::string welcome_msg = "{\"type\":\"id\", \"deviceId\":\"" + std::string(DEVICE_ID) + "\"}";

std::string game_response(std::string &usr_rsp, std::string &game_id); 

