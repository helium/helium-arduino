/*
 * Copyright 2017, Helium Systems, Inc.
 * All Rights Reserved. See LICENCE.txt for license information
 *
 *
 * Install the following libraries through Sketch->Manage Libraries:
 *     - ArduinoJson
 *     - Helium
 * 
 * This is a basic template for operating multiple sensors on one board
 * in a procedural fashion. It includes one configuration variable which
 * I will leave to you to define for your application. See the Config 
 * example if you are not familiar with configuration variables.  
 */

#include "Arduino.h"
#include "Board.h"
#include "Helium.h"
#include "HeliumUtil.h"
#include "ArduinoJson.h"
/* Please see https://arduinojson.org to determine the byte overhead for each json
   structured message. This will change based on the kind of sensor data you are 
   sending. As it is set now, this is two objects in a 100 byte size structure.
   This will probably be way more than enough for two sensor readings per send.
   Please check with their documentation on sizing the object!
   */

#define CHANNEL_NAME "Helium MQTT"
#define CONFIG_VAR "channel.interval_ms"

Helium  helium(&atom_serial);
Channel channel(&helium);
Config  config(&channel);

int32_t interval_ms; //CONFIGURATION VARIABLE
bool cycle = false; 
int32_t reading_1;
int32_t reading_2;

enum States
{
    first,
    second,
    third,
    fourth,
    fifth
};

States state;

void
update_config(bool stale)
{
    if (stale)
    {
        DBG_PRINT(F("Fetching Config - "));
        int status = config.get(CONFIG_VAR, &interval_ms, 0);
        report_status(status);

        if (status == helium_status_OK)
        {
            DBG_PRINT(F("Updating Config - "));
            status = config.set(CONFIG_VAR, interval_ms);
            report_status(status);
        }
    }
}

void
setup()
{
    Serial.begin(9600);
    DBG_PRINTLN(F("Starting"));

    // Begin communication with the Helium Atom The baud rate differs
    // per supported board and is configured in Board.h
    helium.begin(HELIUM_BAUD_RATE);

    // Connect the Atom to the Helium Network
    helium_connect(&helium);
    // and do a channel connect
    channel_create(&channel, CHANNEL_NAME);

    // Get the initial interval
    update_config(true);
}



void
loop()
{
    DynamicJsonDocument doc(100);
    while(cycle = false){
      switch(state) {
        case first:
/* 
          put sensing code for fist two sensors here and save the returned values to reading_1
          and reading_2
*/
          doc["1"] = reading_1;
          doc["2"] = reading_2;

          state = States::second;
          break;
        case second:
/*
          next sensor code
*/
          doc["1"] = reading_1;
          doc["2"] = reading_2;

          state = States::third;
          break;
        case third:
/*
          next sensor code
*/
          doc["1"] = reading_1;
          doc["2"] = reading_2;

          state = States::fourth;
          break;
        case fourth:
/*
          next sensor code
*/
          doc["1"] = reading_1;
          doc["2"] = reading_2;

          state = States::fifth;
          break;
        case fifth:
 /*
          next sensor code
 */         
          doc["1"] = reading_1;
          doc["2"] = reading_2;

          state = States::first;
          cycle = true;
          break;
/* 
          you can add cases incrementally to add more sensors
          i.e.
        case sixth: 
          state = States::first;
          cycle = true;
          break;
*/       
      }
      char buffer[HELIUM_MAX_DATA_SIZE];
      size_t used = serializeJson(doc, buffer);

      channel_send(&channel, CHANNEL_NAME, buffer, used);
      delay(1000);
    }
    // Check and update the send interval
    update_config(config.is_stale());
    cycle = false;
    // Wait till the next time
    delay(interval_ms);
}
