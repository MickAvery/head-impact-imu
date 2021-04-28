/**
 * @file configs.h
 * @author UBC Capstone Team 2020/2021
 * @brief API for getting and setting device configurations
 */

#include "configs.h"
#include "nrf_assert.h"

metadata_t GLOBAL_CONFIGS =
{
    .configs_bytes = {0}
};

char* configs_datalog_mode_strings[CONFIGS_DATALOG_MODE_MAX] =
{
    "CONTINUOUS", "TRIGGER"
};

char* configs_trigger_on_strings[CONFIGS_TRIGGER_ON_MAX] =
{
    "LINEAR ACCELERATION", "ANGULAR VELOCITY"
};

char* configs_trigger_axis_strings[CONFIGS_TRIGGER_AXIS_MAX] =
{
    "RESULTANT", "PER AXIS"
};

char* configs_gyro_sample_rate_strings[CONFIGS_GYRO_SAMPLE_RATE_MAX] =
{
    "4500 Hz", "2000 Hz", "1000 Hz", "500 Hz", "250 Hz", "125 Hz"
};

char* configs_low_g_accel_sample_rate_strings[CONFIGS_LOW_G_ACCEL_SAMPLE_RATE_MAX] =
{
    "4500 Hz", "2000 Hz", "1000 Hz", "500 Hz", "250 Hz", "125 Hz"
};

char* configs_high_g_accel_sample_rate_strings[CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_MAX] =
{
    "6400 Hz", "3200 Hz", "1600 Hz", "800 Hz", "400 Hz"
};

/**
 * @brief Timer ticks corresponding to High G accelerometer sampling rates.
 * 
 * Tick is calculated with (1 / rate) / (1 / 32768Hz),
 * where 32768Hz is the RTC clock frequency
 */
size_t configs_sample_rate_ticks[CONFIGS_HIGH_G_ACCEL_SAMPLE_RATE_MAX] =
{
    5, 10, 20, 40, 80
};

/**
 * @brief Attempt to get configurations from persistent memory
 * 
 * @param configs Configurations will be saved here
 * @return sysret_t Driver status
 * @retval RET_ERR if persistent memory doesn't contain configurations
 * @retval RET_OK if configurations are present in persistent memory
 */
sysret_t configs_get(metadata_t* configs)
{
    sysret_t ret;

    /* read configurations from flash */
    ret = mt25q_read(0, configs->configs_bytes, CONFIGS_FRAME_SIZE);
    SYSRET_CHECK(ret);

    if(configs->device_metadata.current_dev_configs.header != CONFIGS_FRAME_HEADER)
        ret = RET_ERR;

    return ret;
}

/**
 * @brief Save configurations to persistent memory
 * 
 * @param configs Configurations to save
 * @return sysret_t Driver status
 */
sysret_t configs_save(metadata_t* configs)
{
    ASSERT(configs);
    sysret_t ret;

    /* There's no point in saving datalog_en=TRUE in memory */
    bool tmp = configs->device_metadata.current_dev_configs.datalog_en;
    configs->device_metadata.current_dev_configs.datalog_en = false;

    /* erase first subsector in flash */
    ret = mt25q_4kB_subsector_erase(0);
    SYSRET_CHECK(ret);

    /* save configs to flash */
    ret = mt25q_page_program(0, configs->configs_bytes, CONFIGS_FRAME_SIZE);

    /* restore prior config */
    configs->device_metadata.current_dev_configs.datalog_en = tmp;
    return ret;
}