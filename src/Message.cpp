#include "Message.hpp"

Message::Message(std::string loc, float *data, int size)
{   
    j["loc"] = loc;
    j["mag"] = {};
    for (auto i = 0; i < size; i++)
    {
        j["mag"].push_back(data[i]);
    }

    msg = j.dump();
    length = msg.length();
}

Message::Message(std::string loc, float *data, int size, int ctime)
{
    j["loc"] = loc;
    j["mag"] = {};
    for (auto i = 0; i < size; i++)
    {
        j["mag"].push_back(data[i]);
    }

    j["time"] = ctime;

    msg = j.dump();
    length = msg.length();
}

Message::Message(std::string loc, float *data, int size, int ctime, int temperature)
{
    j["loc"] = loc;
    j["mag"] = {};
    for (auto i = 0; i < size; i++)
    {
        j["mag"].push_back(data[i]);
    }

    j["time"] = ctime;

    j["temp"] = temperature;

    msg = j.dump();
    length = msg.length();
}

Message::Message(std::string loc, fftwf_complex *out, int size, int ctime, int lat)
{
    j["loc"] = loc;
    j["complex"]["real"] = {};
    j["complex"]["imag"] = {};
      
    for (auto i = 0; i < size; i++)
    {
        // 0 is real, 1 is imaginary in fftw's API
        j["complex"]["real"].push_back(out[i][0]);
        j["complex"]["imag"].push_back(out[i][1]);
    }
      
    j["time"] = ctime;
    j["latency"] = lat;
    msg = j.dump();
    length = msg.length();
}

Message::Message(std::string loc, int ctime)
{
    j["loc"] = loc;
    j["Latency"] = ctime;

    msg = j.dump();
    length = msg.length();
}

char* Message::get_message()
{
    return const_cast<char*>(msg.data());
}

std::string Message::get_string()
{
    return msg;
}

std::ostream& operator<<(std::ostream& os, const Message& m)
{
    os << m.j;
    return os;
}


int Message::get_length()
{
    return length;
}
