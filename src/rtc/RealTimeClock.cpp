#include <rtc/RealTimeClock.h>
#include <util/CStringStream.h>
#include <comms/serial_usb.h>

struct _CSStreamInit
{
    _CSStreamInit(const StringStreamBase* stream, const char* target)
    {
        target = stream->GetCString();
    }
};

RealTimeClock::RealTimeClock(uint8_t sda_pin, uint8_t scl_pin, uint8_t int_pin, i2c_inst_t* i2c_inst, void* user_data)
    : GPIODevice(int_pin, Pull::UP, GPIO_IRQ_EDGE_FALL, user_data)
{
    stdio_init_all();

    gpio_init(sda_pin);
    gpio_init(scl_pin);
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);

    i2c_init(i2c_inst, 400 * 1000);

    ds3231_init(&rtc, i2c_inst, 0, 0);

    Disable(); // By default, IRQs are not needed.
}

void RealTimeClock::UpdateDateAndTime()
{
    ds3231_read_current_time(&rtc, &date_time);
}

const char* RealTimeClock::GetPrettyDate(DateFormat date_format) const
{
    static CStringStream<32> ss;
    static const _CSStreamInit ss_init(&ss, date_str);
    switch (date_format)
    {
        case DD_MM: ss << date_time.date << '/' << date_time.month; break;
        case DD_MM_YY: ss << date_time.date << '/' << date_time.month << '/' << date_time.year; break;
        case DD_MM_YYYY: ss << date_time.date << '/' << date_time.month << '/' << date_time.century << date_time.year; break;
        case MM_DD: ss << date_time.month << '/' << date_time.date; break;
        case MM_DD_YY: ss << date_time.month << '/' << date_time.date << '/' << date_time.year; break;
        case MM_DD_YYYY: ss << date_time.month << '/' << date_time.date << '/' << date_time.century << date_time.year; break;
        case DD_MMM: ss << date_time.date << ' ' << dateconstants::months_short[date_time.month - 1]; break;
        case DD_MMM_YYYY: ss << date_time.date << ' ' << dateconstants::months_short[date_time.month - 1] << ", " << date_time.century << date_time.year; break;
        case MMM_DD: ss << dateconstants::months_short[date_time.month - 1] << ' ' << date_time.date; break;
        case MMM_DD_YYYY: ss << dateconstants::months_short[date_time.month - 1] << ' ' << date_time.date << ", " << date_time.century << date_time.year; break;
        case DD_Month: ss << date_time.date << ' ' << dateconstants::months[date_time.month - 1]; break;
        case DD_Month_YYYY: ss << date_time.date << ' ' << dateconstants::months[date_time.month - 1] << ", " << date_time.century << date_time.year; break;
        case Month_DD: ss << dateconstants::months[date_time.month - 1] << ' ' << date_time.date; break;
        case Month_DD_YYYY: ss << dateconstants::months[date_time.month - 1] << ' ' << date_time.date << ", " << date_time.century << date_time.year; break;
        case DDD_DD_MMM: ss << dateconstants::days_short[date_time.day - 1] << ", " << date_time.date << ' ' << dateconstants::months_short[date_time.month - 1]; break;
        case DDD_DD_MMM_YYYY: ss << dateconstants::days_short[date_time.day - 1] << ", " << date_time.date << ' ' << dateconstants::months_short[date_time.month - 1] << ", " << date_time.century << date_time.year; break;
        case DDD_MMM_DD: ss << dateconstants::days_short[date_time.day - 1] << ", " << dateconstants::months_short[date_time.month - 1] << ' ' << date_time.date; break;
        case DDD_MMM_DD_YYYY: ss << dateconstants::days_short[date_time.day - 1] << ", " << dateconstants::months_short[date_time.month - 1] << ' ' << date_time.date << ", " << date_time.century << date_time.year; break;
        case DDD_DD_Month: ss << dateconstants::days_short[date_time.day - 1] << ", " << date_time.date << ' ' << dateconstants::months[date_time.month - 1]; break;
        case DDD_DD_Month_YYYY: ss << dateconstants::days_short[date_time.day - 1] << ", " << date_time.date << ' ' << dateconstants::months[date_time.month - 1] << ", " << date_time.century << date_time.year; break;
        case DDD_Month_DD: ss << dateconstants::days_short[date_time.day - 1] << ", " << dateconstants::months[date_time.month - 1] << ' ' << date_time.date; break;
        case DDD_Month_DD_YYYY: ss << dateconstants::days_short[date_time.day - 1] << ", " << dateconstants::months[date_time.month - 1] << ' ' << date_time.date << ", " << date_time.century << date_time.year; break;
        case Day_DD_MMM: ss << dateconstants::days[date_time.day - 1] << ", " << date_time.date << ' ' << dateconstants::months_short[date_time.month - 1]; break;
        case Day_DD_MMM_YYYY: ss << dateconstants::days[date_time.day - 1] << ", " << date_time.date << ' ' << dateconstants::months_short[date_time.month - 1] << ", " << date_time.century << date_time.year; break;
        case Day_MMM_DD: ss << dateconstants::days[date_time.day - 1] << ", " << dateconstants::months_short[date_time.month - 1] << ' ' << date_time.date; break;
        case Day_MMM_DD_YYYY: ss << dateconstants::days[date_time.day - 1] << ", " << dateconstants::months_short[date_time.month - 1] << ' ' << date_time.date << ", " << date_time.century << date_time.year; break;
        case Day_DD_Month: ss << dateconstants::days[date_time.day - 1] << ", " << date_time.date << ' ' << dateconstants::months[date_time.month - 1]; break;
        case Day_DD_Month_YYYY: ss << dateconstants::days[date_time.day - 1] << ", " << date_time.date << ' ' << dateconstants::months[date_time.month - 1] << ", " << date_time.century << date_time.year; break;
        case Day_Month_DD: ss << dateconstants::days[date_time.day - 1] << ", " << dateconstants::months[date_time.month - 1] << ' ' << date_time.date; break;
        case Day_Month_DD_YYYY: ss << dateconstants::days[date_time.day - 1] << ", " << dateconstants::months[date_time.month - 1] << ' ' << date_time.date << ", " << date_time.century << date_time.year; break;
    }

    return ss.GetCString();
}

const char* RealTimeClock::GetPrettyTime(TimeFormat time_format) const
{
    static CStringStream<16> ss;
    static const _CSStreamInit ss_init(&ss, time_str);
    switch (time_format)
    {
        case HH_MM: ss << date_time.hours << ':' << date_time.minutes; break;
        case HH_MM_SS: ss << date_time.hours << ':' << date_time.minutes << ':' << date_time.seconds; break;
    }
    if (rtc.am_pm_mode)
        ss << (date_time.am_pm ? "PM" : "AM");
        
    return ss.GetCString();
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
            .century = static_cast<uint8_t>(year / 100),
            .year = static_cast<uint8_t>(year % 100)
        };
        return ds3231_configure_time(&rtc, &date) == 0;
    }
    return false;
}