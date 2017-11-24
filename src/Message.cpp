#include "Message.hpp"

Message::Message(float *data, int size)
{   
    j["mag"] = {};
    for (auto i = 0; i < size; i++)
    {
        j["mag"].push_back(data[i]);
    }

}

Message::Message(float *data, int size, time_t result)
{
    j["mag"] = {};
    for (auto i = 0; i < size; i++)
    {
        j["mag"].push_back(data[i]);
    }

    auto t = asctime(localtime(&result));
    j["time"] = t;
}

Message::Message(float *data, int size, time_t result, int temperature)
{
    j["mag"] = {};
    for (auto i = 0; i < size; i++)
    {
        j["mag"].push_back(data[i]);
    }

    auto t = asctime(localtime(&result));
    j["time"] = t;

    j["temp"] = temperature;
}

const char* Message::get_message()
{
    std::string msg = j.dump();
    return msg.data();
}

std::string Message::get_string()
{
    return j.dump();
}

int Message::get_length()
{
    std::string msg = j.dump();
    return msg.length();
}