APP_PLATFORM := android-19
APP_STL := gnustl_static
APP_OPTIM        := release
APP_ABI          := arm64-v8a
APP_SHORT_COMMANDS := true

# Keep RTTI/exceptions because the source relies on dynamic_cast and existing
# exception-compatible library behavior.
APP_CPPFLAGS += -frtti -fexceptions -std=c++14 -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-math-errno
APP_CFLAGS += -O3 -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-math-errno

# Remove unreachable code/data at link time to reduce binary size and memory pressure.
APP_LDFLAGS += -Wl,--gc-sections
#APP_ABI          := armeabi-v7a x86
