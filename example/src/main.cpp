#include <rtc/RealTimeClock.h>

void command_callback(const Event* ev, void* usr)
{
    auto event = ev->GetEventAsType<CommandEvent>();
    auto& cmd = event->GetCommand();
    auto rtc = (RealTimeClock*)usr;

    if (strncmp(cmd.command_name, "synctime", max_command_segment_length))
    {
        rtc->SyncTime(cmd);
    }
}

int main()
{
    stdio_init_all();
    
    SerialUSB usb("__cmd__");

    RealTimeClock rtc(4, 5, 6);
    rtc.Use24HourTime(true);
    
    int id = usb.AddAction(&command_callback, &rtc);
    
    while (1)
    {
        Event::HandleEvents();
        usb.DetectCommandsOverUSB();
        rtc.UpdateDateAndTime();
    }

}