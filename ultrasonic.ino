#include <WiFi.h>
#include <PubSubClient.h>

// Define your WiFi credentials and MQTT broker information
const char* ssid = "Elly";
const char* password = "";
const char* mqttServer = "16.171.34.170";
const int mqttPort = 1883; // Default MQTT port
const char* mqttUser = "your_username";
const char* mqttPassword = "your_password";

// Define your MQTT topic
const char* mqttTopic = "parking";

WiFiClient espClient;
PubSubClient client(espClient);

// Define the pins for the ultrasonic sensors
const int trigPin1 = 35;  // Trigger pin for sensor 1
const int echoPin1 = 38;  // Echo pin for sensor 1
const int trigPin2 = 39;  // Trigger pin for sensor 2
const int echoPin2 = 40;  // Echo pin for sensor 2
// const int trigPin3 = 6;  // Trigger pin for sensor 3
// const int echoPin3 = 7;  // Echo pin for sensor 3

// Variables to store the duration and distance for each sensor
long duration1, duration2, duration3;
int distance1, distance2, distance3;

void setup() {
  // Initialize Serial communication
  Serial.begin(9600);
  client.setServer(mqttServer, mqttPort);
  setup_wifi();
  // Define the trigPins as OUTPUT and echoPins as INPUT
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  // pinMode(trigPin3, OUTPUT);
  // pinMode(echoPin3, INPUT);
}

void setup_wifi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Connected to MQTT broker");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }  

  // Measure distance for sensor 1
  measureDistance(trigPin1, echoPin1, distance1);

  // Measure distance for sensor 2
  measureDistance(trigPin2, echoPin2, distance2);

  // Measure distance for sensor 3
  // measureDistance(trigPin3, echoPin3, distance3);

  // Print the distances to the Serial Monitor
  Serial.print("Distance 1: ");
  Serial.print(distance1);
  Serial.println(" cm"); //id 1 //parked

  Serial.print("Distance 2: ");
  Serial.print(distance2);
  Serial.println(" cm"); //id 2 //vacant


  // Serial.print("Distance 3: ");
  // Serial.print(distance3); //id 3
  // Serial.println(" cm");
  int parking_slots=3;
  if (distance1<10 && distance2 <10){
    parking_slots -=2;
  }else  if (distance1<10){
    parking_slots -=1;
  }else if(distance2<10){
    parking_slots -=1;
  }
  Serial.println(parking_slots);

 
  String data= String(parking_slots);
  
  if (client.publish(mqttTopic, data.c_str())) {// we can send the data we want 
    Serial.println("Data sent to MQTT broker: " + data);
  } else {
    Serial.println("Data send failed");
  }

  client.loop();

  // Add a delay before the next set of measurements
  delay(1000); // You can adjust the delay time as needed
}

void measureDistance(int trigPin, int echoPin, int &distance) {
  // Clear the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Set the trigPin to HIGH state for 10 microseconds to trigger the sensor
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Read the duration of the pulse from the echoPin
  int duration;
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance in centimeters
  distance = duration * 0.034 / 2;
}