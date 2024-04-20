#include <string>

#define ID 1029384756

const std::string id = "ID : " + std::to_string(ID);

const std::string accept = "{" + id + " user_answer : yes }"; 

const std::string decline = "{" + id + " user_answer : no }"; 