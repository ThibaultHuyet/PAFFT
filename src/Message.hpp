#include "json.hpp"
#include <string>
#include <utility>

using json = nlohmann::json;

class Message
{
    public:
        Message(float *data, int size);
        Message(float *data, int size, time_t result);
        Message(float *data, int size, time_t result, int temperature);

        const char* get_message();
        std::string get_string();
        int         get_length();
        
    private:
        json j;
};