#include "mqtt_fastwrapper.h"

#include <string>
#include <map>
#include <list>

#include <iostream>

using namespace std;

TMQTTFastObserver::~TMQTTFastObserver()
{

}

void TMQTTFastObserver::OnConnect(int rc)
{
    // call this for all observers; required for subscription
    for (auto &map_item : Observers)
        for (auto &item : map_item.second)
            item->OnConnect(rc);
}

void TMQTTFastObserver::OnMessage(const struct mosquitto_message *message)
{
    // call it only for observers who subscribed to this topic
    for (auto &item : Observers[message->topic])
        item->OnMessage(message);
}

void TMQTTFastObserver::OnSubscribe(int mid, int qos_count, const int *granted_qos)
{
}
