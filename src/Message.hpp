#pragma once

#include "json.hpp"

#include <ctime>
#include <iostream>
#include <string>
#include <utility>

using json = nlohmann::json;

class Message
{
    public:
        Message(std::string loc, float *data, int size);
        Message(std::string loc, float *data, int size, bool ctime);
        Message(std::string loc, float *data, int size, bool ctime, int temperature);

        const char* get_message();
        std::string get_string();
        int         get_length();
        
        // This overload is mostly for convenience
        friend std::ostream& operator<<(std::ostream& os, const Message& m);

    private:
        json j;
        int length;
        std::string msg; // String formatted version for convenience
};