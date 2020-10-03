/**
 * @file datetime.c
 * @author UBC Capstone Team 2020/2021
 * @brief Datetime library for accurate recording of date and time
 */

#include "datetime.h"
#include "nrf.h"
#include "nrf_drv_rtc.h"

/**
 * @brief RTC isntance
 */
const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(0);

/**
 * @brief RTC configuration
 */
static nrf_drv_rtc_config_t rtc_config = NRF_DRV_RTC_DEFAULT_CONFIG;

/**
 * @brief Tracks number of overflow events, significantly increasing the lifetime of the RTC passed the 24-bit RTC COUNTER register
 */
static uint32_t overflow_counter = 0U;

/**
 * @brief Module state
 */
static datetime_state_t datetime_state = DATETIME_UNINIT;

/**
 * @brief Datetime info stored
 */
datetime_t datetime = {0, 0, 0, 0, 0, 0, 0};

/******************
 * Event handlers
 ******************/

static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
    if(int_type == NRF_DRV_RTC_INT_OVERFLOW)
    {
        overflow_counter++;
    }
}

/******************
 * Start of API
 ******************/

/**
 * @brief Initialize datetime library
 */
void datetime_init(void)
{
    ASSERT(datetime_state == DATETIME_UNINIT);

    ret_code_t ret;

    /* NOTE: frequency of RTC clock tick is determined by:
     *   f_rtc [kHz] = 32.768 / (PRESCALER + 1 )
     */
    rtc_config.prescaler = 33U;
    ret = nrf_drv_rtc_init(&rtc, &rtc_config, rtc_handler);
    APP_ERROR_CHECK(ret);
}

/**
 * @brief Set datetime and start RTC ticks
 * 
 * @param datetime_in datetime info to set
 * @return datetime_err_code_t datetime_err_code_t Error code, what went wrong?
 */
datetime_err_code_t datetime_set(datetime_t* datetime_in)
{
    datetime_err_code_t ret = DATETIME_ERR;

    if(datetime_state == DATETIME_UNSET) {
        datetime.year = datetime_in->year;
        datetime.month = datetime_in->month;
        datetime.day = datetime_in->day;
        datetime.hr = datetime_in->hr;
        datetime.min = datetime_in->min;
        datetime.sec = datetime_in->sec;
        datetime.msec =  datetime_in->msec;

        /* enable OVRFLW event and interrupt */
        nrf_drv_rtc_overflow_enable(&rtc, true);

        /* enable RTC instance */
        nrf_drv_rtc_enable(&rtc);

        ret = DATETIME_OK;
    }

    return ret;
}

/**
 * @brief Reset datetime and stop RTC ticks
 * 
 * @return datetime_err_code_t Error code, what went wrong?
 */
datetime_err_code_t datetime_reset(void)
{
    datetime_err_code_t ret = DATETIME_ERR;

    /* TODO */

    return ret;
}

/**
 * @brief Get datetime values
 * 
 * @param datetime_out pointer to datetime struct to store datetime info
 * @return datetime_err_code_t datetime_err_code_t Error code, what went wrong?
 */
datetime_err_code_t datetime_get(datetime_t* datetime_out)
{
    datetime_err_code_t ret = DATETIME_ERR;

    /* TODO */

    return ret;
}