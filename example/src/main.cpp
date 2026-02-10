#include <rtc/RealTimeClock.h>

#include <hardware/Button.h>

#define USE_PRINT 1

#if USE_PRINT
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

void command_callback(const Event* ev, void* usr)
{
    auto event = ev->GetEventAsType<CommandEvent>();
    auto& cmd = event->GetCommand();
    auto rtc = (RealTimeClock*)usr;
    if (cmd.Is("synctime"))
    {
        LOG("%s\n", cmd.command_arguments);
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        rtc->SyncTime(cmd);
    }
}

int main()
{
    stdio_init_all();
    i2c_init(i2c0, 400 * 1000);
    
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_put(PICO_DEFAULT_LED_PIN, 1);

    SerialUSB usb("/");

    RealTimeClock rtc(4, 5, 6);
    rtc.Use24HourTime(true);
    rtc.UpdateDateAndTime();

    static constexpr ds3231_alarm_2_t discarded = {
        .minutes = 0,
        .hours = 0,
        .am_pm = 0,
        .day = 0,
        .date = 0
    };

    rtc.SetAlarm2(discarded, ON_EVERY_MINUTE);

    std::ignore = rtc.AddAction([](const Event* ev, void*){
        puts("CLOCK HAS FIRED");
    });

    int id = usb.AddAction(&command_callback, &rtc);

    Button log_button = 16;

    int id_log = log_button.AddAction([](const Event* ev, void* ptr){
        RealTimeClock* rtc_v = (RealTimeClock*)ptr;
        ButtonEvent* ev_b = ev->GetEventAsType<ButtonEvent>();
        if (ev_b->WasPressed())
        {
            LOG("Date: %s\tTime: %s\n", rtc_v->GetPrettyDate(RealTimeClock::DateFormat::MM_DD_YYYY), rtc_v->GetPrettyTime(RealTimeClock::TimeFormat::HH_MM_SS));
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
        }
        else
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
    }, &rtc);

    RealTimeClock::DateFormat fmt = RealTimeClock::StringToDateFormat("MMM_DD");

    gpio_put(PICO_DEFAULT_LED_PIN, 0);
    while (1)
    {
        Event::HandleEvents();
        bool d = usb.DetectCommandsOverUSB();
        //gpio_put(PICO_DEFAULT_LED_PIN, d);
        rtc.UpdateDateAndTime();
    }

}