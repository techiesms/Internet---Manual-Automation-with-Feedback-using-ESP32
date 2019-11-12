/*
 * Code for Internet and Manual Home Automation with Realtime feedback
 * 
 * 
 * This code is written by and for techiesms YouTube Channel(http://www.youtube.com/techiesms)
 * 
 * This code is made with a lot of effort by our team hence
 * do give credits while using this code for any of your hobby project 
 * or college project
 * 
 * Rights of this code and logic is reserved by techiesms 
 * Using this for commercial purpose withuot permission is 
 * subject to violation of Rights 
 * 
 * 
*/

#include <PubSubClient.h> //https://github.com/knolleary/pubsubclient
#include <WiFi.h>
#include <stdio.h>


/****************************************
   Define Constants
 ****************************************/

#define WIFISSID "SSID" // Put your WifiSSID here
#define PASSWORD "PASS" // Put your wifi password here
#define TOKEN "Ubidots Token" // Put your Ubidots' TOKEN

#define VARIABLE_LABEL_SUB_1 "r1" // Assing the variable label to subscribe
#define VARIABLE_LABEL_SUB_2 "r2" // Assing the variable label to subscribe
#define VARIABLE_LABEL_SUB_3 "r3" // Assing the variable label to subscribe
#define VARIABLE_LABEL_SUB_4 "r4" // Assing the variable label to subscribe

#define VARIABLE_LABEL_PUB_1 "r1" // Assing the variable label to subscribe
#define VARIABLE_LABEL_PUB_2 "r2" // Assing the variable label to subscribe
#define VARIABLE_LABEL_PUB_3 "r3" // Assing the variable label to subscribe
#define VARIABLE_LABEL_PUB_4 "r4" // Assing the variable label to subscribe


#define DEVICE_LABEL "esp32" // Assig the device label to subscribe
#define MQTT_CLIENT_NAME "testing" // MQTT client Name, put a Random ASCII

#define S1 32
#define S2 35
#define S3 34
#define S4 39

#define R1 15
#define R2 2
#define R3 4
#define R4 22

#define LED1 26
#define LED2 25
#define LED3 27
#define Buzzer 21

#define DEBUG_SW 0


int switch_ON_Flag1_previous_II = 1;
int switch_ON_Flag1_previous_I = 0;

int switch_ON_Flag2_previous_II = 1;
int switch_ON_Flag2_previous_I = 0;

int switch_ON_Flag3_previous_II = 1;
int switch_ON_Flag3_previous_I = 0;

int switch_ON_Flag4_previous_II = 1;
int switch_ON_Flag4_previous_I = 0;

char mqttBroker[] = "industrial.api.ubidots.com";

const int ERROR_VALUE = 65535;  // Set here an error value

const uint8_t NUMBER_OF_VARIABLES = 4; // Number of variable to subscribe to
char * variable_labels[NUMBER_OF_VARIABLES] = {"r1", "r2", "r3", "r4"};

float CONTROL1; // Name of the variable to be used within the code.
float CONTROL2; // Name of the variable to be used within the code.
float CONTROL3; // Name of the variable to be used within the code.
float CONTROL4; // Name of the variable to be used within the code.

bool DEBUG = 0;
int i, j, k;

float value; // To store incoming value.
uint8_t variable; // To keep track of the state machine and be able to use the switch case.
char payload_publish[100];
char topicToPublish[200];

/****************************************
   Initializate constructors for objects
 ****************************************/

WiFiClient ubidots;
PubSubClient client(ubidots);

/****************************************
   Auxiliar Functions
 ****************************************/

void callback(char* topic, byte* payload, unsigned int length) {
  char* variable_label = (char *) malloc(sizeof(char) * 30);;
  get_variable_label_topic(topic, variable_label);
  value = btof(payload, length);
  set_state(variable_label);
  execute_cases();
  free(variable_label);
}

// Parse topic to extract the variable label which changed value
void get_variable_label_topic(char * topic, char * variable_label) {
  if (DEBUG)Serial.print("topic:");
  Serial.println(topic);
  sprintf(variable_label, "");
  for (int i = 0; i < NUMBER_OF_VARIABLES; i++) {
    char * result_lv = strstr(topic, variable_labels[i]);
    if (result_lv != NULL) {
      uint8_t len = strlen(result_lv);
      char result[100];
      uint8_t i = 0;
      for (i = 0; i < len - 3; i++) {
        result[i] = result_lv[i];
      }
      result[i] = '\0';
      if (DEBUG)Serial.print("Label is: ");
      Serial.println(result);
      sprintf(variable_label, "%s", result);
      break;
    }
  }
}

// cast from an array of chars to float value.
float btof(byte * payload, unsigned int length) {
  char * demo_ = (char *) malloc(sizeof(char) * 10);
  for (int i = 0; i < length; i++) {
    demo_[i] = payload[i];
  }
  return atof(demo_);
}

// State machine to use switch case
void set_state(char* variable_label) {
  variable = 0;
  for (uint8_t i = 0; i < NUMBER_OF_VARIABLES; i++) {
    if (strcmp(variable_label, variable_labels[i]) == 0) {
      break;
    }
    variable++;
  }
  if (variable >= NUMBER_OF_VARIABLES) variable = ERROR_VALUE; // Not valid
}

// Function with switch case to determine which variable changed and assigned the value accordingly to the code variable
void execute_cases() {
  switch (variable) {
    case 0:
      CONTROL1 = value;
      digitalWrite(R1, !value);
      if (DEBUG)Serial.print("CONTROL1: ");
      Serial.println(CONTROL1);
      Serial.println();
      break;
    case 1:
      CONTROL2 = value;
      digitalWrite(R2, !value);
      if (DEBUG)Serial.print("CONTROL2: ");
      Serial.println(CONTROL2);
      Serial.println();
      break;
    case 2:
      CONTROL3 = value;
      digitalWrite(R3, !value);
      if (DEBUG)Serial.print("CONTROL3: ");
      Serial.println(CONTROL3);
      Serial.println();
      break;
    case 3:
      CONTROL4 = value;
      digitalWrite(R4, !value);
      if (DEBUG)Serial.print("CONTROL4: ");
      Serial.println(CONTROL4);
      Serial.println();
      break;

    case ERROR_VALUE:
      Serial.println("error");
      Serial.println();
      break;
    default:
      Serial.println("default");
      Serial.println();
  }

}

void reconnect() {

  int interval = 12000;
  int previousMillis = 0;
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    Serial.print(".");
    int currentMillis = millis();

    if (currentMillis - previousMillis >= interval)
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      ESP.restart();
    }
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, ""))
    {
      Serial.println("connected");
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, HIGH);
    } else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, HIGH);
      digitalWrite(LED3, LOW);
      // Wait 2 seconds before retrying
      /**************************************************/
      // call all the required functions here
      Call_ManualAutomation();

      /**************************************************/
    }


    delay(250);
  }
}

void Call_WiFiSetup() {
  int interval = 1000;
  int previousMillis = 0;
  Serial.print("Wait for WiFi... ");
  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, HIGH);
  delay(200);
  digitalWrite(LED3, HIGH);
  delay(200);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  delay(500);

  digitalWrite(LED1, HIGH);
  delay(200);
  digitalWrite(LED2, HIGH);
  delay(200);
  digitalWrite(LED3, HIGH);
  delay(200);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED)
      break;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      Call_ManualAutomation();

      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      digitalWrite(LED3, LOW);
    }
    delay(250);
  }

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  delay(500);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  delay(500);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  delay(500);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  delay(500);
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  delay(500);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  delay(500);

}


/****************************************
   Main Functions
 ****************************************/

void setup() {
  Serial.begin(115200);

  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(Buzzer, OUTPUT);


  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);

  WiFi.begin(WIFISSID, PASSWORD);
  Serial.println();
  Serial.println();
  delay(3000);
  /*************/
  Call_WiFiSetup();
  /*************/
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);
}

void Call_SUB() {

  // Subscribes for getting the value of the control variable in the temperature-box device
  char topicToSubscribe_variable_1[200];
  sprintf(topicToSubscribe_variable_1, "%s", ""); // Cleans the content of the char
  sprintf(topicToSubscribe_variable_1, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(topicToSubscribe_variable_1, "%s/%s/lv", topicToSubscribe_variable_1, VARIABLE_LABEL_SUB_1);
  Serial.println("subscribing to topic:");
  Serial.println(topicToSubscribe_variable_1);
  client.subscribe(topicToSubscribe_variable_1);

  char topicToSubscribe_variable_2[200];
  sprintf(topicToSubscribe_variable_2, "%s", ""); // Cleans the content of the char
  sprintf(topicToSubscribe_variable_2, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(topicToSubscribe_variable_2, "%s/%s/lv", topicToSubscribe_variable_2, VARIABLE_LABEL_SUB_2);
  Serial.println("subscribing to topic:");
  Serial.println(topicToSubscribe_variable_2);
  client.subscribe(topicToSubscribe_variable_2);

  char topicToSubscribe_variable_3[200];
  sprintf(topicToSubscribe_variable_3, "%s", ""); // Cleans the content of the char
  sprintf(topicToSubscribe_variable_3, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(topicToSubscribe_variable_3, "%s/%s/lv", topicToSubscribe_variable_3, VARIABLE_LABEL_SUB_3);
  Serial.println("subscribing to topic:");
  Serial.println(topicToSubscribe_variable_3);
  client.subscribe(topicToSubscribe_variable_3);

  char topicToSubscribe_variable_4[200];
  sprintf(topicToSubscribe_variable_4, "%s", ""); // Cleans the content of the char
  sprintf(topicToSubscribe_variable_4, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
  sprintf(topicToSubscribe_variable_4, "%s/%s/lv", topicToSubscribe_variable_4, VARIABLE_LABEL_SUB_4);
  Serial.println("subscribing to topic:");
  Serial.println(topicToSubscribe_variable_4);
  client.subscribe(topicToSubscribe_variable_4);

}

void Call_SwitchControl() {
  if (digitalRead(S1) == LOW)
  {

    if (switch_ON_Flag1_previous_I == 0 )
    {
      digitalWrite(R1, LOW);
      Serial.println("Relay1- ON");

      sprintf(topicToPublish, "%s", ""); // Cleans the content of the char
      sprintf(topicToPublish, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payload_publish, "{\"%s\":%s}", VARIABLE_LABEL_PUB_1, "1"); // Adds the variable label
      // sprintf(payload_publish, "%s,\"%s\":%s}", payload_publish, VARIABLE_LABEL_PUB_1, "1"); // Adds the variable label
      Serial.println("publishing to topic:");
      Serial.println(topicToPublish);
      Serial.println("Payload published:");
      Serial.println(payload_publish);
      client.publish(topicToPublish, payload_publish);
      delay(100);
      switch_ON_Flag1_previous_I = 1;
      switch_ON_Flag1_previous_II = 1;
    }
    if (DEBUG_SW) Serial.println("Switch1 -ON");
    //delay(200);

  }
  if (digitalRead(S1) == HIGH )
  {

    if (switch_ON_Flag1_previous_II == 1)
    {
      digitalWrite(R1, HIGH);
      Serial.println("Relay OFF");

      sprintf(topicToPublish, "%s", ""); // Cleans the content of the char
      sprintf(topicToPublish, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payload_publish, "{\"%s\":%s}", VARIABLE_LABEL_PUB_1, "0"); // Adds the variable label
      //sprintf(payload_publish, "%s,\"%s\":%s}", payload_publish, VARIABLE_LABEL_PUB_1, "0"); // Adds the variable label
      Serial.println("publishing to topic:");
      Serial.println(topicToPublish);
      Serial.println("Payload published:");
      Serial.println(payload_publish);
      client.publish(topicToPublish, payload_publish);
      delay(100);
      switch_ON_Flag1_previous_II = 0;
      switch_ON_Flag1_previous_I = 0;
    }
    if (DEBUG_SW)Serial.println("Switch1 OFF");
    //delay(200);
  }
  /*********************************************************************/
  /*********************************************************************/
  if (digitalRead(S2) == LOW)
  {

    if (switch_ON_Flag2_previous_I == 0)
    {
      digitalWrite(R2, LOW);
      Serial.println("Relay2 ON");

      sprintf(topicToPublish, "%s", ""); // Cleans the content of the char
      sprintf(topicToPublish, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payload_publish, "{\"%s\":%s}", VARIABLE_LABEL_PUB_2, "1"); // Adds the variable label
      // sprintf(payload_publish, "%s,\"%s\":%s}", payload_publish, VARIABLE_LABEL_PUB_2, str_sensor2); // Adds the variable label
      Serial.println("publishing to topic:");
      Serial.println(topicToPublish);
      Serial.println("Payload published:");
      Serial.println(payload_publish);
      client.publish(topicToPublish, payload_publish);
      delay(100);
      switch_ON_Flag2_previous_II = 1;
      switch_ON_Flag2_previous_I = 1;
    }

    if (DEBUG_SW)Serial.println("Switch2 ON");
    //delay(200);
  }
  if (digitalRead(S2) == HIGH)
  {

    if (switch_ON_Flag2_previous_II  == 1)
    {
      digitalWrite(R2, HIGH);
      Serial.println("Relay2 OFF");

      sprintf(topicToPublish, "%s", ""); // Cleans the content of the char
      sprintf(topicToPublish, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payload_publish, "{\"%s\":%s}", VARIABLE_LABEL_PUB_2, "0"); // Adds the variable label
      // sprintf(payload_publish, "%s,\"%s\":%s}", payload_publish, VARIABLE_LABEL_PUB_2, str_sensor2); // Adds the variable label
      Serial.println("publishing to topic:");
      Serial.println(topicToPublish);
      Serial.println("Payload published:");
      Serial.println(payload_publish);
      client.publish(topicToPublish, payload_publish);
      delay(100);
      switch_ON_Flag2_previous_II = 0;
      switch_ON_Flag2_previous_I = 0;
    }

    if (DEBUG_SW)Serial.println("Switch2 OFF");
    //delay(200);
  }
  /*********************************************************************/
  /*********************************************************************/
  if (digitalRead(S3) == LOW)
  {

    if (switch_ON_Flag3_previous_I == 0)
    {
      digitalWrite(R3, LOW);
      Serial.println("Relay3 ON");

      sprintf(topicToPublish, "%s", ""); // Cleans the content of the char
      sprintf(topicToPublish, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payload_publish, "{\"%s\":%s}", VARIABLE_LABEL_PUB_3, "1"); // Adds the variable label
      // sprintf(payload_publish, "%s,\"%s\":%s}", payload_publish, VARIABLE_LABEL_PUB_2, str_sensor2); // Adds the variable label
      Serial.println("publishing to topic:");
      Serial.println(topicToPublish);
      Serial.println("Payload published:");
      Serial.println(payload_publish);
      client.publish(topicToPublish, payload_publish);
      delay(100);
      switch_ON_Flag3_previous_II = 1;
      switch_ON_Flag3_previous_I = 1;
    }

    if (DEBUG_SW)Serial.println("Switch3 ON");
    //delay(200);
  }
  if (digitalRead(S3) == HIGH)
  {

    if (switch_ON_Flag3_previous_II == 1)
    {
      digitalWrite(R3, HIGH);
      Serial.println("Relay3 OFF");

      sprintf(topicToPublish, "%s", ""); // Cleans the content of the char
      sprintf(topicToPublish, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payload_publish, "{\"%s\":%s}", VARIABLE_LABEL_PUB_3, "0"); // Adds the variable label
      // sprintf(payload_publish, "%s,\"%s\":%s}", payload_publish, VARIABLE_LABEL_PUB_2, str_sensor2); // Adds the variable label
      Serial.println("publishing to topic:");
      Serial.println(topicToPublish);
      Serial.println("Payload published:");
      Serial.println(payload_publish);
      client.publish(topicToPublish, payload_publish);
      delay(100);
      switch_ON_Flag3_previous_II = 0;
      switch_ON_Flag3_previous_I = 0;
    }

    if (DEBUG_SW)Serial.println("Switch3 OFF");
    //delay(200);
  }
  /*********************************************************************/
  /*********************************************************************/
  if (digitalRead(S4) == LOW)
  {

    if (switch_ON_Flag4_previous_I == 0)
    {
      digitalWrite(R4, LOW);
      Serial.println("Relay4 ON");

      sprintf(topicToPublish, "%s", ""); // Cleans the content of the char
      sprintf(topicToPublish, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payload_publish, "{\"%s\":%s}", VARIABLE_LABEL_PUB_4, "1"); // Adds the variable label
      // sprintf(payload_publish, "%s,\"%s\":%s}", payload_publish, VARIABLE_LABEL_PUB_2, str_sensor2); // Adds the variable label
      Serial.println("publishing to topic:");
      Serial.println(topicToPublish);
      Serial.println("Payload published:");
      Serial.println(payload_publish);
      client.publish(topicToPublish, payload_publish);
      delay(100);
      switch_ON_Flag4_previous_II = 1;
      switch_ON_Flag4_previous_I = 1;
    }

    if (DEBUG_SW)Serial.println("Switch4 ON");
    //delay(200);
  }
  if (digitalRead(S4) == HIGH)
  {

    if (switch_ON_Flag4_previous_II == 1)
    {

      digitalWrite(R4, HIGH);
      Serial.println("Relay4 OFF");

      sprintf(topicToPublish, "%s", ""); // Cleans the content of the char
      sprintf(topicToPublish, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
      sprintf(payload_publish, "{\"%s\":%s}", VARIABLE_LABEL_PUB_4, "0"); // Adds the variable label
      Serial.println("publishing to topic:");
      Serial.println(topicToPublish);
      Serial.println("Payload published:");
      Serial.println(payload_publish);
      client.publish(topicToPublish, payload_publish);
      delay(100);
      switch_ON_Flag4_previous_II = 0;
      switch_ON_Flag4_previous_I = 0;
    }
    if (DEBUG_SW)Serial.println("Switch4 OFF");
    //delay(200);
  }

}


void Call_ManualAutomation() {
  Serial.println("Manual Automation");
  digitalWrite(R1, digitalRead(S1));
  Serial.println("Relay-1: ");
  Serial.println(digitalRead(S1));
  delay(1);
  digitalWrite(R2, digitalRead(S2));
  Serial.println("Relay-2: ");
  Serial.println(digitalRead(S2));
  delay(1);
  digitalWrite(R3, digitalRead(S3));
  Serial.println("Relay-3: ");
  Serial.println(digitalRead(S3));
  delay(1);
  digitalWrite(R4, digitalRead(S4));
  Serial.println("Relay-4: ");
  Serial.println(digitalRead(S4));
  delay(1);
}


void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Call_WiFiSetup();
  }
  Call_SwitchControl();
  if (!client.connected()) {
    reconnect();
    Call_SUB();
  }



  client.loop();
  delay(1);

}
