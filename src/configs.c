/**
 * @file configs.h
 * @author UBC Capstone Team 2020/2021
 * @brief API for getting and setting device configurations
 */

#include "configs.h"

configs_t GLOBAL_CONFIGS =
{
    .configs_bytes = {0}
};

/**
 * @brief Attempt to get configurations from persistent memory
 * 
 * @param configs Configurations will be saved here
 * @return sysret_t Driver status
 * @retval RET_ERR if persistent memory doesn't contain configurations
 * @retval RET_OK if configurations are present in persistent memory
 */
sysret_t configs_get(configs_t* configs)
{
    sysret_t ret;

    /* read configurations from flash */
    ret = mt25q_read(0, configs->configs_bytes, CONFIGS_FRAME_SIZE);
    SYSRET_CHECK(ret);

    if(configs->device_configs.header != CONFIGS_FRAME_HEADER)
        ret = RET_ERR;

    return ret;
}

/**
 * @brief Save configurations to persistent memory
 * 
 * @param configs Configurations to save
 * @return sysret_t Driver status
 */
sysret_t configs_save(configs_t* configs)
{
    sysret_t ret;

    /* erase first subsector in flash */
    ret = mt25q_4kB_subsector_erase(0);
    SYSRET_CHECK(ret);

    /* save configs to flash */
    ret = mt25q_page_program(0, GLOBAL_CONFIGS.configs_bytes, CONFIGS_FRAME_SIZE);
    return ret;
}