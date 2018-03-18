#!/bin/sh
sudo apt-key adv --keyserver hkp://keyserver.ubuntu.com:80 --recv 2930ADAE8CAF5059EE73BB4B58712A2291FA4AD5 -y
echo "deb [ arch=amd64,arm64 ] https://repo.mongodb.org/apt/ubuntu xenial/mongodb-org/3.6 multiverse" | sudo tee /etc/apt/sources.list.d/mongodb-org-3.6.list

sudo add-apt-repository ppa:deadsnakes/ppa -y

sudo apt-get update
sudo apt-get install -y python3.6 mongodb-org python3-pip mosquitto mosquitto-clients

sudo service mongod start