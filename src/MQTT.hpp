#ifndef MQTT_HPP
#define MQTT_HPP

#include "Message.hpp"
#include "MQTTClient.h"

class MQTT
{
    public:
        MQTT(std::string address, std::string client_id);
        ~MQTT();
        void publish_message(Message m, std::string topic, int qos);

    private:
        MQTTClient                client;
        MQTTClient_connectOptions conn_opts;
        MQTTClient_message        pubmsg;
        MQTTClient_deliveryToken  token;
};

#endif