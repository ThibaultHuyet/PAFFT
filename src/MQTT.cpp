#include "MQTT.hpp"

MQTT::MQTT(std::string address, std::string client_id)
{
    conn_opts = MQTTClient_connectOptions_initializer;
    pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;

    MQTTClient_create(&client,
                      address.data(),
                      client_id.data(),
                      MQTTCLIENT_PERSISTENCE_NONE,
                      NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    int rc = 0;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
        {
            throw std::runtime_error("Failed to connect to MQTT client");
        }
}

MQTT::~MQTT()
{
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}

void MQTT::publish_message(Message m, std::string topic, int qos)
{
    int rc = 0;
    pubmsg.payload = m.get_message();
    pubmsg.payloadlen = m.get_length();
    pubmsg.qos = qos;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, topic.data(), &pubmsg, &token);
    rc = MQTTClient_waitForCompletion(client, token, 200L);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        throw std::runtime_error("Failed to publish message");
    }
}