extern unsigned long OTAUntilMillis;
extern unsigned long now;

extern char msg[50];

void callback(char* topic, byte* payload, unsigned int length) {
  String payloadStr = "";
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  
  for (unsigned int i = 0; i < length; i++) {
    payloadStr += ((char)payload[i]);
  }

  Serial.println(payloadStr);

  // powerOff, powerOn, getStatus, startOTA, powerReset, resetESP

  // powerOff
  if (payloadStr.equals(F("powerOff"))) {
    motiionControlStop();
  // powerOn
  } else if (payloadStr.equals(F("powerOn"))) {
  motionControlStart();
  // getStatus
  } else if (payloadStr.equals(F("getStatus"))) {
      // snprintf (msg, 20, "PowerState: %i", curState);
      // client.publish(TOPIC_T, msg);
    exhibitStatusMsg();
    snprintf (msg, 30, "Up for: %i seconds", now / SECONDS);
    client.publish(TOPIC_T, msg);
    
    #if DHT_PRESENT == 1
      snprintf (msg, 50, "Current Temp: %i deg F", farenRead);
      client.publish(TOPIC_T, msg);
    
      snprintf (msg, 50, "Current Humidity: %i Percent", humidiRead);
      client.publish(TOPIC_T, msg);
    #endif
  // startOTA
  } else if (payloadStr.equals(F("startOTA"))) {
    client.publish(TOPIC_T, "OTA for 5m");
    OTAUntilMillis = now + 300 * SECONDS; // 5 minutes
  
  // powerReset
  } else if (payloadStr.equals(F("powerReset"))) {
    motiionControlStop();
    delay(10000);
    motionControlStart();
    loop();
  
  // resetESP
  } else if (payloadStr.equals(F("resetESP"))) {
    client.publish(TOPIC_T, "ESP8266 resetting");
    delay(500);
    ESP.restart();
  } else if (payloadStr.equals(F("custom_action1"))) {         
    client.publish(TOPIC_T, "Custom action 1 run");
  } else if (payloadStr.equals(F("custom_action2"))) {
    client.publish(TOPIC_T, "Custom action 2 run");
  } else if (payloadStr.equals(F("custom_action3"))) {
    client.publish(TOPIC_T, "Custom action 3 run");
  } else if (payloadStr.equals(F("custom_action4"))) {
    client.publish(TOPIC_T, "Custom action 4 run");
  } else if (payloadStr.equals(F("custom_action5"))) {
    client.publish(TOPIC_T, "Custom action 5 run");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    // Create a random client ID
    String clientId = F("ESPCli-");
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println(F("connected"));
      // And resubscribe
      client.subscribe(TOPIC_T);
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client.state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}