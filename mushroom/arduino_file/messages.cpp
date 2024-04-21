# include "messages.h"

std::string game_response(std::string &usr_rsp, std::string &game_id){
    std::string message = "{";
    message = message + "\"deviceId\" : \"" + std::string(DEVICE_ID) + "\",";
    message = message + "\"type\" : \"" + usr_rsp + "\",";
    message = message + "\"gameId\" : \"" + game_id + "\"";
    message = message + "}";
    return message;
} 
