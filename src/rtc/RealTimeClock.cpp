#include <rtc/RealTimeClock.h>

// utility macros to be used only here
#define FORMAT_DATE(fmt, ...) chars_written = snprintf(date_str, date_str_size, fmt, __VA_ARGS__)
#define FORMAT_TIME(fmt, ...) chars_written = snprintf(time_str, time_str_size, fmt, __VA_ARGS__)
#define CENTURY date_time.century + 20
#define MONTH_SHORT dateconstants::months_short[date_time.month - 1]
#define MONTH dateconstants::months[date_time.month - 1]
#define DAY_SHORT dateconstants::days_short[date_time.day - 1]
#define DAY dateconstants::days[date_time.day - 1]

RealTimeClock::RealTimeClock(uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin, i2c_inst_t* i2c_inst)
    : GPIODevice(int_pin, Pull::UP, GPIO_IRQ_EDGE_FALL)
{
    gpio_init(sda_pin);
    gpio_init(scl_pin);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);

    ds3231_init(&rtc, i2c_inst, 0, 0);

    Disable(); // By default, IRQs are not needed.
}

void RealTimeClock::UpdateDateAndTime()
{
    ds3231_read_current_time(&rtc, &date_time);
}

const char* RealTimeClock::GetPrettyDate(DateFormat date_format)
{
    int chars_written;
    switch (date_format)
    {
        case DD_MM: FORMAT_DATE("%u/%u", date_time.date, date_time.month); break;
        case DD_MM_YY: FORMAT_DATE("%u/%u/%u", date_time.date, date_time.month, date_time.year); break;
        case DD_MM_YYYY: FORMAT_DATE("%u/%u/%u%u", date_time.date, date_time.month, CENTURY, date_time.year); break;
        case MM_DD: FORMAT_DATE("%u/%u", date_time.month, date_time.date); break;
        case MM_DD_YY: FORMAT_DATE("%u/%u/%u", date_time.month, date_time.date, date_time.year); break;
        case MM_DD_YYYY: FORMAT_DATE("%u/%u/%u%u", date_time.month, date_time.date, CENTURY, date_time.year); break;
        case DD_MMM: FORMAT_DATE("%u %s", date_time.date, MONTH_SHORT); break;
        case DD_MMM_YYYY: FORMAT_DATE("%u %s, %u%u", date_time.date, MONTH_SHORT); break;
        case MMM_DD: FORMAT_DATE("%s %u", MONTH_SHORT, date_time.date); break;
        case MMM_DD_YYYY: FORMAT_DATE("%s %u, %u%u", MONTH_SHORT, date_time.date, CENTURY, date_time.year); break;
        case DD_Month: FORMAT_DATE("%u %s", date_time.date, MONTH); break;
        case DD_Month_YYYY: FORMAT_DATE("%u %s, %u%u", date_time.date, MONTH, CENTURY, date_time.year); break;
        case Month_DD: FORMAT_DATE("%s %u", MONTH, date_time.date); break;
        case Month_DD_YYYY: FORMAT_DATE("%s %u, %u%u", MONTH, date_time.date, CENTURY, date_time.year); break;
        case DDD_DD_MMM: FORMAT_DATE("%s, %u %s", DAY_SHORT, date_time.date, MONTH_SHORT); break;
        case DDD_DD_MMM_YYYY: FORMAT_DATE("%s, %u %s, %u%u", DAY_SHORT, date_time.date, MONTH_SHORT, CENTURY, date_time.year); break;
        case DDD_MMM_DD: FORMAT_DATE("%s, %s %u", DAY_SHORT, MONTH_SHORT, date_time.date); break;
        case DDD_MMM_DD_YYYY: FORMAT_DATE("%s, %s %u, %u%u", DAY_SHORT, MONTH_SHORT, date_time.date, CENTURY, date_time.year);
        case DDD_DD_Month: FORMAT_DATE("%s, %u %s", DAY_SHORT, date_time.date, MONTH); break;
        case DDD_DD_Month_YYYY: FORMAT_DATE("%s, %u %s, %u%u", DAY_SHORT, date_time.date, MONTH, CENTURY, date_time.year); break;
        case DDD_Month_DD: FORMAT_DATE("%s, %s %u", DAY_SHORT, MONTH, date_time.date); break;
        case DDD_Month_DD_YYYY: FORMAT_DATE("%s, %s %u, %u%u", DAY_SHORT, MONTH, date_time.date, CENTURY, date_time.year); break;
        case Day_DD_MMM: FORMAT_DATE("%s, %u %s", DAY, date_time.date, MONTH_SHORT); break;
        case Day_DD_MMM_YYYY: FORMAT_DATE("%s, %u %s, %u%u", DAY, date_time.date, MONTH_SHORT, CENTURY, date_time.year); break;
        case Day_MMM_DD: FORMAT_DATE("%s, %s %u", DAY, MONTH_SHORT, date_time.date); break;
        case Day_MMM_DD_YYYY: FORMAT_DATE("%s, %s %u, %u%u", DAY, MONTH_SHORT, date_time.date, CENTURY, date_time.year); break;
        case Day_DD_Month: FORMAT_DATE("%s, %u %s", DAY, date_time.date, MONTH); break;
        case Day_DD_Month_YYYY: FORMAT_DATE("%s, %u %s, %u%u", DAY, date_time.date, MONTH, CENTURY, date_time.year); break;
        case Day_Month_DD: FORMAT_DATE("%s, %s %u", DAY, MONTH, date_time.date); break;
        case Day_Month_DD_YYYY: FORMAT_DATE("%s, %s %u, %u%u", DAY, MONTH, date_time.date, CENTURY, date_time.year); break;
        default: return GetPrettyDate(DateFormat::DD_MM_YYYY);
    }

    return date_str;
}

const char* RealTimeClock::GetPrettyTime(TimeFormat time_format)
{
    int chars_written;
    switch (time_format)
    {
        case HH_MM: FORMAT_TIME("%02u:%02u", date_time.hours, date_time.minutes); break;
        case HH_MM_SS: FORMAT_TIME("%02u:%02u:%02u", date_time.hours, date_time.minutes, date_time.seconds); break;
        default: return GetPrettyTime(TimeFormat::HH_MM);
    }
    if (rtc.am_pm_mode)
        snprintf(time_str + chars_written, time_str_size, "%s", date_time.am_pm ? "PM" : "AM");
        
    return time_str;
}

bool RealTimeClock::Use24HourTime(bool military_time)
{
    return ds3231_enable_am_pm_mode(&rtc, !military_time) == 0;
}

bool RealTimeClock::SetTime(ds3231_data_t new_time)
{
    return ds3231_configure_time(&rtc, &new_time) == 0;
}

bool RealTimeClock::SetAlarm1(ds3231_alarm_1_t alarm, ALARM_1_MASKS mode)
{
    Enable();
    return ds3231_set_alarm_1(&rtc, &alarm, mode) == 0;
}

bool RealTimeClock::SetAlarm2(ds3231_alarm_2_t alarm, ALARM_2_MASKS mode)
{
    Enable();
    return ds3231_set_alarm_2(&rtc, &alarm, mode) == 0;
}

bool RealTimeClock::SyncTime(const Command& command)
{
    char buff[128];
    strncpy(buff, command.command_arguments, max_command_segment_length);
    
    uint32_t year, month, day, weekday, hour, minute, second; // Expecting 24-hour time
    if (sscanf(buff, "%d %d %d %d %d %d %d", &year, &month, &day, &weekday, &hour, &minute, &second) == 7)
    {
        ds3231_data_t date = {
            .seconds = static_cast<uint8_t>(second),
            .minutes = static_cast<uint8_t>(minute),
            .hours = static_cast<uint8_t>(hour),
            .am_pm = hour >= 12,
            .day = static_cast<uint8_t>(weekday),
            .date = static_cast<uint8_t>(day),
            .month = static_cast<uint8_t>(month),
            .century = static_cast<uint8_t>(year / 2100), // this is a single bit. 0 for 2000-2099, 1 for 2100-2199
            .year = static_cast<uint8_t>(year % 100)
        };
        return ds3231_configure_time(&rtc, &date) == 0;
    }
    return false;
}