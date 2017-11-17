/*
	For the moment, this file is temporary.
	My goal with this file was to explore the potential of sending json data through c++.
	I wanted to do this in the main program for a while as it would allow me to send
	other needed data. For example, in ultrasonics, it is important to know the temperature data
	that a value was taken at as temperature affects us quite a bit
	Sending json data also allows me to send time at the same time which is useful for my purposes
*/


#include <iostream>
#include <mosquitto.h>
#include <string>
#include <unistd.h>
#include <ctime>
#include "json.hpp"

#define MQTT_HOSTNAME "localhost"
#define MQTT_PORT 1883
#define MQTT_TOPIC "count"

using json = nlohmann::json;

int main()
	{
		time_t result = time(nullptr);

		struct mosquitto *mosq = nullptr;
	    mosquitto_lib_init();
	    mosq = mosquitto_new(nullptr, true, nullptr);
	    if (!mosq)
	    {
	    	exit(-1);
	    }

	    int ret = mosquitto_connect(mosq, MQTT_HOSTNAME, MQTT_PORT, 0);
	    if (ret)
	    {
	    	exit(-1);
	    }


	    int count = 0;
	    json j;
	    while (count < 100)
	    	{
	    		j["count"] = count;
	    		auto t = asctime(localtime(&result));
	    		j["time"] = t;

			    std::string msg = j.dump();
	    		auto payload = msg.data();
	    		int payLen = msg.length();

			    ret = mosquitto_publish(
		                                mosq,               // Initialized with mosquitto_lib_init
		                                nullptr,            // int *mid
		                                MQTT_TOPIC,         // Topic to publish to
		                                payLen,             // int payload length
		                                payload,              // Message being sent
		                                0,                  // Quality of Service
		                                false               // Retain message
		                                );
			    count++;
			    std::cout << msg << "\n";
			    if (ret)
			    {
			    	exit(-1);
			    }
			}

		sleep(500);
	}
