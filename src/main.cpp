#include "main.h"


void device_getData()
{
  struct sensorData sensorDataTemp_st;

  if(sht_getdata(sensorDataTemp_st.temperature, sensorDataTemp_st.humidity) == ERROR_NONE)
    {
    sensorData_st.temperature   = sensorDataTemp_st.temperature;
    sensorData_st.humidity      = sensorDataTemp_st.humidity;
  }
}

void device_dataManagement()
{  
  struct sensorData sensorDataTemp_st;
  sensorDataTemp_st = sensorData_st;
  DS3231_getStringDateTime(realTime, DateTime::TIMESTAMP_FULL , dateTime_string);	
  createSensorDataString(sensorDataString, NAME_DEVICE, dateTime_string ,sensorDataTemp_st);
	DS3231_getStringDateTime(realTime, DateTime::TIMESTAMP_DATE, nameFileSaveData);
	SDcard_saveStringDataToFile(&connectionStatus_st, sensorDataString);
  createMessageMQTTString(messageData, (const char *)espID, timeClient, sensorDataTemp_st);
  MQTT_postData(messageData.c_str(), &connectionStatus_st, mqttClient);
}

void setup() {
    Serial.begin(SERIAL_DEBUG_BAUDRATE);
    log_e("Booting...");
    WIFI_init();
    if(WiFi.status() == WL_CONNECTED)
    {
      insights_init();
    }
    Wire.begin(PIN_SDA_GPIO, PIN_SCL_GPIO, I2C_CLOCK_SPEED);
	  DS3231_init(realTime, timeClient, Wire, connectionStatus_st);
    sht_init();
#ifdef USING_MQTT
	MQTT_initClient(topic, espID, mqttClient, &connectionStatus_st);
	timeClient.begin();
#endif
#ifdef USING_SDCARD
	SDcard_init(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, PIN_CS_SD_CARD, &connectionStatus_st);
#endif
    log_e("Init Done");
  unsigned long time_4_first_record = millis();
  while (millis() - time_4_first_record < DEVICE_PRE_GET_DATE_TIME)
  {
    device_getData();  
    DS3231_getStringDateTime(realTime, DateTime::TIMESTAMP_FULL , dateTime_string);	
  }
}

unsigned long device_previousDataControl = 0;
unsigned long device_previousWifiReconnect = 0;

void loop() {
  // get data from all sensor
  device_getData();
  if(millis() - device_previousDataControl > DEVICE_DATA_SAVE_INTERVAL_TIME) 
  {
    device_dataManagement();
    device_previousDataControl = millis();
  }
  // if WiFi is down, try reconnecting every CHECK_WIFI_TIME seconds
  if ((WiFi.status() != WL_CONNECTED) && (millis() - device_previousWifiReconnect >= WIFI_CONNECT_INTERVAL)) {
    log_e("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    device_previousWifiReconnect = millis();
  }
  insights_post_data();
}