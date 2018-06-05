#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "json.hpp"

#include <ctime>
#include <iostream>
#include <string>
#include <utility>
#include <fftw3.h>

using json = nlohmann::json;

class Message
{
    public:
        Message(std::string loc, float *data, int size);
        Message(std::string loc, float *data, int size, int ctime);
        Message(std::string loc, float *data, int size, int ctime, int temperature);
        Message(std::string loc, fftw_complex *out, int size, int ctime, float lat);
        // For sending just a time value
        Message(std::string loc, int ctime);

        char*       get_message();
        std::string get_string();
        int         get_length();
        
        // This overload is mostly for convenience
        friend std::ostream& operator<<(std::ostream& os, const Message& m);

    private:
        json j;
        int length;
        std::string msg; // String formatted version for convenience
};

#endif