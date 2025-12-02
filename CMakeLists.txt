if (NOT TARGET pico-rtc)

    add_library(pico-rtc STATIC
        src/rtc/RealTimeClock.cpp
    )

    add_subdirectory(lib/pico-ds3231/libraries)
    add_subdirectory(lib/pico-event-hardware)

    target_include_directories(pico-rtc PUBLIC
        include
    )

    target_link_libraries(pico-rtc
        pico_stdlib
        pico_ds3231
        pico-event-hardware
    )

endif()