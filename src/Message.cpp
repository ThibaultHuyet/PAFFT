#include "Message.hpp"

Message::Message(float *data, int size)
{   
    j["mag"] = {};
    for (auto i = 0; i < size; i++)
    {
        j["mag"].push_back(data[i]);
    }

    msg = j.dump();
    length = msg.length();
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

    msg = j.dump();
    length = msg.length();
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

    msg = j.dump();
    length = msg.length();
}

const char* Message::get_message()
{
    return msg.data();
}

std::string Message::get_string()
{
    return msg;
}

int Message::get_length()
{
    return length;
}