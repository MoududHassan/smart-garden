#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <avr/wdt.h>

#define ARDUINO_CLIENT_ID "arduino_1"  
#define LED_4_STATUS "led_4_status"   //Topic
#define SUBJECT_LED_4_CMD "led_4_cmd" //Topic
#define LED_ON "ON"
#define LED_OFF "OFF"
#define RESET "RESET"
#define PUBLISH_DELAY 5000
#define MESSAGE_RECEIVED_DELAY 20000 

int led = 4;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };   //physical mac address
byte ip[] = { 192, 168, 0, 190 };                      // ip in lan (that's what you need to use in your browser. ("192.168.1.178")
byte gateway[] = { 192, 168, 0, 1 };                   // internet access via router
byte subnet[] = { 255, 255, 255, 0 };
IPAddress mqttServer(192, 168, 0, 101); 

EthernetClient ethClient;
PubSubClient client(ethClient);

long previousMillis;
long lastmessageReceived;

void setup() {
   Serial.begin(9600);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  } 
     // MTTQ parameters
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);

  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  // Ethernet shield configuration
  Ethernet.begin(mac, ip);

  delay(1500); // Allow hardware to stabilize

  previousMillis = millis();

}

void loop() {
  rebootIfNeeded();
  
  if (!client.connected())
        reconnect();
  String led4Status = LED_OFF;
  //This block is not needed but here it shows how to loop through certain interval    
  if (millis() - previousMillis > PUBLISH_DELAY){
    previousMillis = millis();

    if (digitalRead(led) == HIGH) {
      led4Status = LED_ON;
    } else {
      led4Status = LED_OFF;
    }
    Serial.println("going to publish status: " + led4Status);
    client.publish(LED_4_STATUS, (char*)led4Status.c_str());
  }

  client.loop();
}

void reconnect() {
  
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection ... ");
    // Attempt to connect first guest is user and second is pass
    if (client.connect(ARDUINO_CLIENT_ID, "guest","guest")) {
      Serial.println("connected");
      // (re)subscribe
      client.subscribe(SUBJECT_LED_4_CMD);
    } else {
      Serial.print("Connection failed, state: ");
      Serial.print(client.state());
      Serial.println(", retrying in 5 seconds");
      digitalWrite(led, LOW); 
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

// sub callback function
void callback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("[sub: ");
  Serial.print(topic);
  Serial.print("] ");
  char message[length + 1] = "";
  for (int i = 0; i < length; i++)
    message[i] = (char)payload[i];
  message[length] = '\0';
  Serial.println(message);

  // SUB_LED topic section
  if (strcmp(topic, SUBJECT_LED_4_CMD) == 0)
  {
    if (strcmp(message, LED_ON) == 0){
      if (digitalRead(led) == LOW) {
        digitalWrite(led, HIGH);  
      }
    }
      
    if (strcmp(message, LED_OFF) == 0){
      if (digitalRead(led) == HIGH) {
        digitalWrite(led, LOW);  
      }
    }

    lastmessageReceived = millis();
 
  //Instantely let the client know about the status
  client.publish(LED_4_STATUS, message);
  }
}

void rebootIfNeeded() {
  int value = (millis() - lastmessageReceived);
  if(value > MESSAGE_RECEIVED_DELAY){
    lastmessageReceived = millis();
    Serial.print("value of:");
    Serial.println(value);
    delay(2000);
    wdt_disable();
    wdt_enable(WDTO_15MS);
    while (1) {}
  }  
}