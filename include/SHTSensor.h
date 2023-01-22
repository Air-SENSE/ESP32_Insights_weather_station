/**
 * @file PMS7003Sensor.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SHTSENSOR_H
#define SHTSENSOR_H

#include "SHT85.h"
#include "esp_log.h"
#include "configs.h"


#define SHT85_ADDRESS         0x44

SHT85 sht;

#define ERROR_SHT_INIT_FAILED       0xe0
#define ERROR_SHT_GET_DATA_FAILED   0xe1

/**
 * @brief Initialize SHTSENSOR 
 *  
 * @return ERROR_CODE 
 * @retval ERROR_NONE: if success
 * @retval ERROR_PMS_INIT_FAILED: if failed
 */
ERROR_CODE sht_init();

/**
 * @brief Get new data from SHTSENSOR
 * 
 * @param[out] _co_2: PM CO2
 * 
 * @return ERROR_CODE 
 * @retval ERROR_NONE: if success
 * @retval ERROR_PMS_GET_DATA_FAILED: if failed
 */
ERROR_CODE sht_getdata(float &_temperature, float &_humidity);


ERROR_CODE sht_init()
{
    if(sht.begin(SHT85_ADDRESS))
    {
        log_e("SHT Sensor init success!");
        return ERROR_NONE;
    }
    log_e("SHT Sensor init failed!");
    return ERROR_SHT_INIT_FAILED;
}

ERROR_CODE sht_getdata(float &_temperature, float &_humidity)
{
    if (sht.read())
    {
        _temperature = sht.getTemperature();
        _humidity = sht.getHumidity();
        log_e("SHT Sensor get data successsfully! %f %f",_temperature, _humidity);
        return ERROR_NONE;
    } else 
    {
        log_e("SHT Sensor get data failed!");
        return ERROR_SHT_GET_DATA_FAILED;
    }
}

#endif