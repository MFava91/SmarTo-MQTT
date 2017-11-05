#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

    //WiFi Config
    #define CONFIG_WIFI_SSID "WIFI_SSID"
    #define CONFIG_WIFI_PASS "WIFI_PASSWORD"

    //Mqtt config
    #define CONFIG_MQTT_HOST "MQTT_IP"
    #define CONFIG_MQTT_USER "MQTT_USER"
    #define CONFIG_MQTT_PASS "MQTT_PASSWORD"
    #define CONFIG_MQTT_CLIENT_ID "MQTT_CLIENT_ID"
    #define CONFIG_MQTT_TOPIC_LIGHT "MQTT_TOPIC_LIGHT"
    #define CONFIG_MQTT_TOPIC_MOTION "MQTT_TOPIC_MOTION"

//General config
#define CONFIG_LIGHT_SENSOR_PIN 33
#define CONFIG_MOTION_SENSOR_PIN 5
#define CONFIG_SENSOR_DELAY 3000 // Milliseconds between readings

//Deep sleep config
#define FPM_SLEEP_MAX_TIME 0xFFFFFFF
#define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex

boolean firstTimeLightOn = true;    
int lastMotionSensorValue = -1;
int counter = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(CONFIG_WIFI_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

}

void connecttMQTT() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(CONFIG_MQTT_CLIENT_ID, CONFIG_MQTT_USER, CONFIG_MQTT_PASS)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setEspSleepWakeUp() {
    if(digitalRead(CONFIG_LIGHT_SENSOR_PIN) == 0) {
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 1);
    } else {
        esp_sleep_enable_ext0_wakeup(GPIO_NUM_33, 0);
    }
}

void readAndSleep() {
    int lightStatus = digitalRead(CONFIG_LIGHT_SENSOR_PIN);
    Serial.println("------------------------------------");
    Serial.println("LightSensor: "); Serial.println(digitalRead(CONFIG_LIGHT_SENSOR_PIN));
    Serial.println("MotionSensor: "); Serial.println(digitalRead(CONFIG_MOTION_SENSOR_PIN));
    Serial.println("------------------------------------");
    client.publish(CONFIG_MQTT_TOPIC_LIGHT, String(digitalRead(CONFIG_LIGHT_SENSOR_PIN)).c_str(), true);
    client.publish(CONFIG_MQTT_TOPIC_MOTION, String(digitalRead(CONFIG_MOTION_SENSOR_PIN)).c_str(), true);
    Serial.println("Sleep!!!");
    esp_deep_sleep_start();
}

void setup() {
    pinMode(CONFIG_LIGHT_SENSOR_PIN, INPUT);
    pinMode(CONFIG_MOTION_SENSOR_PIN, INPUT);
    Serial.begin(115200);
    setEspSleepWakeUp();
    client.setServer(CONFIG_MQTT_HOST, 1883);
    setupWifi();
    connecttMQTT();
}

void loop() {
    if(digitalRead(CONFIG_LIGHT_SENSOR_PIN) == 0) {
        if(firstTimeLightOn) {
            client.publish(CONFIG_MQTT_TOPIC_LIGHT, String(digitalRead(CONFIG_LIGHT_SENSOR_PIN)).c_str(), true);
            firstTimeLightOn = false;
        }
        int motionStatus = digitalRead(CONFIG_MOTION_SENSOR_PIN);
        if(motionStatus != lastMotionSensorValue) {
            lastMotionSensorValue = motionStatus;
            Serial.println("MotionSensor: "); Serial.println(digitalRead(CONFIG_MOTION_SENSOR_PIN));
            client.publish(CONFIG_MQTT_TOPIC_MOTION, String(digitalRead(CONFIG_MOTION_SENSOR_PIN)).c_str(), true);
        }
    } else {
        firstTimeLightOn = true;
        readAndSleep();
    }
    delay(CONFIG_SENSOR_DELAY);
}

