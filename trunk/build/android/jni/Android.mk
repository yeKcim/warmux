
# The namespace in Java file, with dots replaced with underscores
SDL_JAVA_PACKAGE_PATH := org_warmux

# Android Dev Phone G1 has trackball instead of cursor keys, and
# sends trackball movement events as rapid KeyDown/KeyUp events,
# this will make Up/Down/Left/Right key up events with X frames delay,
# so if application expects you to press and hold button it will process the event correctly.
# TODO: create a libsdl config file for that option and for key mapping/on-screen keyboard
SDL_TRACKBALL_KEYUP_DELAY := 1

# If the application designed for higher screen resolution enable this to get the screen
# resized in HW-accelerated way, however it eats a tiny bit of CPU
SDL_VIDEO_RENDER_RESIZE := 0

LOCAL_ARM_MODE := arm
include $(call all-subdir-makefiles)
