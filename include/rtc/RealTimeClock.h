#pragma once

#include <hardware/GPIODevice.h>
#include <hardware/SerialUSB.h>

extern "C"
{
#include <ds3231.h>
}

namespace dateconstants
{
    constexpr const char* days[] = {
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday"
    };

    constexpr const char* days_short[] = {
        "Mon",
        "Tue",
        "Wed",
        "Thu",
        "Fri",
        "Sat",
        "Sun"
    };

    constexpr const char* months[] = {
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"
    };

    constexpr const char* months_short[] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };
}

struct _CSStreamInit;

class RealTimeClock : public GPIODevice
{
private:
    static constexpr size_t date_str_size = 32;
    static constexpr size_t time_str_size = 16;

    ds3231_t rtc;
    ds3231_data_t date_time;
    char date_str[date_str_size];
    char time_str[time_str_size];

public:
    enum DateFormat
    {
        DD_MM,              //      23/11
        DD_MM_YY,           //      23/11/25
        DD_MM_YYYY,         //      23/11/2025
        MM_DD,              //      11/23
        MM_DD_YY,           //      11/23/25
        MM_DD_YYYY,         //      11/23/2025
        DD_MMM,             //      23 Nov
        DD_MMM_YYYY,        //      23 Nov, 2025
        MMM_DD,             //      Nov 23
        MMM_DD_YYYY,        //      Nov 23, 2025
        DD_Month,           //      23 November
        DD_Month_YYYY,      //      23 November, 2025
        Month_DD,           //      November 23
        Month_DD_YYYY,      //      November 23, 2025
        DDD_DD_MMM,         //      Sun, 23 Nov
        DDD_DD_MMM_YYYY,    //      Sun, 23 Nov, 2025
        DDD_MMM_DD,         //      Sun, Nov 23
        DDD_MMM_DD_YYYY,    //      Sun, Nov 23, 2025
        DDD_DD_Month,       //      Sun, 23 November
        DDD_DD_Month_YYYY,  //      Sun, 23 November, 2025
        DDD_Month_DD,       //      Sun, November 23
        DDD_Month_DD_YYYY,  //      Sun, November 23, 2025
        Day_DD_MMM,         //      Sunday, 23 Nov
        Day_DD_MMM_YYYY,    //      Sunday, 23 Nov, 2025
        Day_MMM_DD,         //      Sunday, Nov 23
        Day_MMM_DD_YYYY,    //      Sunday, Nov 23, 2025
        Day_DD_Month,       //      Sunday, 23 November
        Day_DD_Month_YYYY,  //      Sunday, 23 November, 2025
        Day_Month_DD,       //      Sunday, November 23
        Day_Month_DD_YYYY   //      Sunday, November 23, 2025
    };

    enum TimeFormat
    {
        HH_MM,              //      8:07 PM     or 20:07
        HH_MM_SS            //      8:07:59 PM  or 20:07:59
    };

    RealTimeClock(uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin, i2c_inst_t* i2c_inst = i2c0);
    ~RealTimeClock() = default;

    void UpdateDateAndTime();

    inline const ds3231_data_t& GetDateAndTime() const
    {
        return date_time;
    }

    const char* GetPrettyDate(DateFormat date_format);
    const char* GetPrettyTime(TimeFormat time_format);

    inline const char* GetDateString() const
    {
        return date_str;
    }
    inline const char* GetTimeString() const
    {
        return time_str;
    }

    bool Use24HourTime(bool military_time = true);
    bool SetTime(ds3231_data_t new_time);
    bool SetAlarm1(ds3231_alarm_1_t alarm, ALARM_1_MASKS mode);
    bool SetAlarm2(ds3231_alarm_2_t alarm, ALARM_2_MASKS mode);
    bool SyncTime(const Command& command);

    inline bool IsActivated() const override
    {
        return !gpio_get(gpio_pin);
    }

    friend _CSStreamInit;
};