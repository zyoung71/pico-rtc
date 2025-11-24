#include <rtc/RealTimeClock.h>

int main()
{
    stdio_init_all();
    
    RealTimeClock rtc(4, 5, 6);
    rtc.UseMilitaryTime(true);
    rtc.SyncTimeOverUSB();

    while (1)
    {
        rtc.UpdateDateAndTime();
    }

}