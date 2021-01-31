/**
 * @file datetime.c
 * @author UBC Capstone Team 2020/2021
 * @brief Datetime library for accurate recording of date and time
 */

#include "datetime.h"
#include "nrf.h"
#include "nrf_drv_rtc.h"
#include "arm_math.h"

/**
 * @brief RTC isntance
 */
const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2);

/**
 * @brief RTC configuration
 */
static nrf_drv_rtc_config_t rtc_config = NRF_DRV_RTC_DEFAULT_CONFIG;

/**
 * @brief Precise RTC period in ms
 */
static float32_t rtc_period = 0.0f;

/**
 * @brief Tracks number of overflow events, significantly increasing the lifetime of the RTC passed the 24-bit RTC COUNTER register.
 *        This is multiplied with the multiplier that corresponds to the 24-bit counter to determine how many clock ticks have passed.
 */
static volatile uint32_t overflow_counter = 0U;
#define OVRFLW_MULTIPLIER (0xffffffU)

/**
 * @brief Module state
 */
static datetime_state_t datetime_state = DATETIME_UNINIT;
/**
 * @brief This enum is used to help in iterating through the time units when calculating the new time (see datetime_get())
 */
typedef enum
{
    DT_USEC = 0,
    DT_SEC,
    DT_MIN,
    DT_HR,
    DT_DAY,
    DT_MONTH,
    DT_YEAR,
    DT_PARAMS_MAX
} datetime_params_t;

/**
 * @brief These dividers are used to calculate the carryover from one time unit to another
 */
static uint32_t clock_dividers[DT_PARAMS_MAX] = {
    1000000U,   /*!< 1M usecs in a sec */
    60U,        /*!< 60 secs in a min */
    60U,        /*!< 60 mins in an hr */
    24U,        /*!< 24 hrs in a day */
    31U,        /*!< 31 days in a month TODO: programatically determine this */
    12U,        /*!< 12 months in a year */
    0xffffffffU /*!< Doesn't matter because year is the largest time unit */
};

/**
 * @brief Datetime info stored, to be modified by datetime_set() and retrieved by datetime_get()
 */
static datetime_t datetime = {0, 0, 0, 0, 0, 0, 0};

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
sysret_t datetime_init(void)
{
    ASSERT(datetime_state == DATETIME_UNINIT);

    /* NOTE: frequency of RTC clock tick is determined by:
     *   f_rtc [kHz] = 32.768 / (PRESCALER + 1 )
     */
    rtc_config.prescaler = 32U; /* we want a tick with rate of 1 kHz, TODO: magic number */
    rtc_period =  33000.0f / 32.768f; /* TODO: magic number */ /* save RTC period in usec */
    datetime_state = DATETIME_UNSET;
    return nrf_drv_rtc_init(&rtc, &rtc_config, rtc_handler);
}

/**
 * @brief Set datetime and start RTC ticks
 * 
 * @param datetime_in datetime info to set
 * @return sysret_t Error code, what went wrong?
 */
sysret_t datetime_set(datetime_t* datetime_in)
{
    ASSERT(datetime_in != NULL);

    datetime_err_code_t ret = RET_ERR;

    if(datetime_state == DATETIME_UNSET) {
        datetime.year = datetime_in->year;
        datetime.month = datetime_in->month;
        datetime.day = datetime_in->day;
        datetime.hr = datetime_in->hr;
        datetime.min = datetime_in->min;
        datetime.sec = datetime_in->sec;
        datetime.usec =  datetime_in->usec;

        /* enable OVRFLW event and interrupt */
        nrf_drv_rtc_overflow_enable(&rtc, true);

        /* enable RTC instance */
        nrf_drv_rtc_enable(&rtc);

        datetime_state = DATETIME_SET;
        ret = RET_OK;
    }

    return ret;
}

/**
 * @brief Reset datetime and stop RTC ticks
 * 
 * @return sysret_t Error code, what went wrong?
 */
sysret_t datetime_reset(void)
{
    nrf_drv_rtc_overflow_disable(&rtc);
    nrf_drv_rtc_disable(&rtc);

    return RET_OK;
}

/**
 * @brief Get datetime values
 * 
 * @param datetime_out pointer to datetime struct to store datetime info
 * @return sysret_t Error code, what went wrong?
 */
sysret_t datetime_get(datetime_t* datetime_out)
{
    ASSERT(datetime_out != NULL);

    sysret_t ret = RET_ERR;

    if(datetime_state == DATETIME_SET)
    {
        /* get value of RTC COUNTER register */
        uint32_t rtc_counter = nrf_rtc_counter_get(rtc.p_reg);

        /* get number of milliseconds since last time set */
        uint32_t carryover = (OVRFLW_MULTIPLIER * overflow_counter) + rtc_counter;
        float32_t carryover_flt = (float32_t)(OVRFLW_MULTIPLIER * overflow_counter + rtc_counter) * rtc_period;

        /* if time unit overflows, carry that overflow over to next time unit */
        uint8_t overflow = 0;

        for(datetime_params_t i = DT_USEC ; i < DT_PARAMS_MAX ; i++) {
            uint32_t divider = clock_dividers[i];

            /* determine how much to add to this time unit */
            uint32_t time_addition = 0U;

            /* carryover_flt can get to a point when it becomes greater than the max 32-bit integer, so use uin64_t types */
            time_addition = (i == DT_USEC) ? (uint32_t)((uint64_t)carryover_flt % (uint64_t)divider) : (carryover % divider) ;

            /* determine the carryover to the next time unit */
            /* carryover_flt can get to a point when it becomes greater than the max 32-bit integer, so use uin64_t types */
            carryover = (i == DT_USEC) ? (uint32_t)((uint64_t)carryover_flt / (uint64_t)divider) : (carryover / divider);

            /* add to time, determine if there are any overflows */
            uint32_t new_time = 0U;
            switch(i) {
                case DT_USEC:
                    new_time = datetime.usec + time_addition;
                    datetime_out->usec = new_time % divider;
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

        ret = RET_OK;
    }

    return ret;
}

/**
 * @brief Get status of Datetime module
 * 
 * @return sysret_t Module status
 */
sysret_t datetime_test(void)
{
    if(datetime_state == DATETIME_UNSET)
        return RET_DRV_UNINIT;
    else
        return RET_OK;
}