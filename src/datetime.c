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
 * @brief Tracks number of overflow events, significantly increasing the lifetime of the RTC passed the 24-bit RTC COUNTER register.
 *        This is multiplied with the multiplier that corresponds to the 24-bit counter to determine how many clock ticks have passed.
 */
static uint32_t overflow_counter = 0U;
#define OVRFLW_MULTIPLIER (0xffffffU)

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
    rtc_config.prescaler = 32U; /* we want a tick with rate of 2 kHz, TODO: magic number */
    datetime_state = DATETIME_UNSET;
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
    ASSERT(datetime_in != NULL);

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

        datetime_state = DATETIME_SET;
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

typedef enum
{
    DT_MSEC = 0,
    DT_SEC,
    DT_MIN,
    DT_HR,
    DT_DAY,
    DT_MONTH,
    DT_YEAR,
    DT_PARAMS_MAX
} datetime_params_t;

static uint32_t clock_dividers[DT_PARAMS_MAX] = {
    1000U, 60U, 60U, 24U, 31U, 12U, 0xffffffffU
};

/**
 * @brief Get datetime values
 * 
 * @param datetime_out pointer to datetime struct to store datetime info
 * @return datetime_err_code_t datetime_err_code_t Error code, what went wrong?
 */
datetime_err_code_t datetime_get(datetime_t* datetime_out)
{
    ASSERT(datetime_out != NULL);

    datetime_err_code_t ret = DATETIME_ERR;

    if(datetime_state == DATETIME_SET)
    {
        /* get value of RTC COUNTER register */
        uint32_t rtc_counter = nrf_rtc_counter_get(rtc.p_reg);

        /* get number of milliseconds since last time set */
        uint32_t carryover = (OVRFLW_MULTIPLIER * overflow_counter) + rtc_counter;

        /* sdjlkhfd */
        uint8_t overflow = 0;

        for(datetime_params_t i = DT_MSEC ; i < DT_PARAMS_MAX ; i++) {
            uint32_t divider = clock_dividers[i];

            /* determine how much to add to this time unit */
            uint32_t time_addition = carryover % divider;

            /* determine the carryover to the next time unit */
            carryover = carryover / divider;

            /* add to time, determine if there are any overflows */
            uint32_t new_time = 0U;
            switch(i) {
                case DT_MSEC:
                    new_time = datetime.msec + time_addition + overflow;
                    datetime_out->msec = new_time % divider;
                    break;
                case DT_SEC:
                    new_time = datetime.sec + time_addition + overflow;
                    datetime_out->sec = new_time % divider;
                    break;
                case DT_MIN:
                    new_time = datetime.min + time_addition + overflow;
                    datetime_out->min = new_time % divider;
                    break;
                case DT_HR:
                    new_time = datetime.hr + time_addition + overflow;
                    datetime_out->hr = new_time % divider;
                    break;
                case DT_DAY:
                    new_time = datetime.day + time_addition + overflow;
                    datetime_out->day = new_time % divider;
                    break;
                case DT_MONTH:
                    new_time = datetime.month + time_addition + overflow;
                    datetime_out->month = new_time % divider;
                    break;
                case DT_YEAR:
                    new_time = datetime.year + time_addition + overflow;
                    datetime_out->year = new_time % divider;
                    break;
                default:
                    break;
            }

            overflow = (new_time >= divider) ? 1U : 0U;
        }

        ret = DATETIME_OK;
    }

    return ret;
}