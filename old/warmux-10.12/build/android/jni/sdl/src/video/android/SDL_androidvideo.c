/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#include <jni.h>
#include <android/log.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include <math.h>
#include <string.h> // for memset()

#include "SDL_config.h"
#include "SDL_version.h"

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "SDL_mutex.h"
#include "SDL_thread.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "../SDL_sysvideo.h"
#include "SDL_androidvideo.h"
#include "jniwrapperstuff.h"


// The device screen dimensions to draw on
int SDL_ANDROID_sWindowWidth  = 0;
int SDL_ANDROID_sWindowHeight = 0;
#ifdef SDL_ANDROID_USE_2BUFS
SDL_sem *swap_sem = NULL;
SDL_Thread *swap_thread = NULL;
#endif

// Extremely wicked JNI environment to call Java functions from C code
static JNIEnv* JavaEnv = NULL;
static jclass JavaRendererClass = NULL;
static jobject JavaRenderer = NULL;
static jmethodID JavaSwapBuffers = NULL;
static int glContextLost = 0;
extern JavaVM *jniVM;

#ifdef SDL_ANDROID_USE_2BUFS
// Should ideally loop endlessly, waiting on a semaphore,
// CallIntMethod return stored to a static value, and a loop end bool
// to properly finish the function
int ThreadSwap(void *data)
{
  __android_log_print(ANDROID_LOG_INFO, "libSDL", "doing the swap");
  //(*jniVM)->AttachCurrentThread(jniVM, &JavaEnv, NULL);
  int ret = (*JavaEnv)->CallIntMethod( JavaEnv, JavaRenderer, JavaSwapBuffers );
  //(*jniVM)->DetachCurrentThread(jniVM);
  __android_log_print(ANDROID_LOG_INFO, "libSDL", "swapped");
  SDL_SemPost(swap_sem); // Can be called again
  return ret;
}
#endif

static void appPutToBackgroundCallbackDefault(void)
{
  SDL_ANDROID_PauseAudioPlayback();
}
static void appRestoredCallbackDefault(void)
{
  SDL_ANDROID_ResumeAudioPlayback();
}

static SDL_ANDROID_ApplicationPutToBackgroundCallback_t appPutToBackgroundCallback = appPutToBackgroundCallbackDefault;
static SDL_ANDROID_ApplicationPutToBackgroundCallback_t appRestoredCallback = appRestoredCallbackDefault;

int SDL_ANDROID_CallJavaSwapBuffers()
{
  if( !glContextLost )
  {
    SDL_ANDROID_processAndroidTrackballDampening();
  }
#ifdef SDL_ANDROID_USE_2BUFS
  if (swap_thread) {
    int ret;
    __android_log_print(ANDROID_LOG_INFO, "libSDL", "doing the wait");
    // Because of the semaphore this is useless, but this is done to get return value
    SDL_WaitThread(swap_thread, &ret);
    swap_thread = NULL;
    if (!ret)
      return 0;
    __android_log_print(ANDROID_LOG_INFO, "libSDL", "done waiting");
  }
  swap_thread = SDL_CreateThread(ThreadSwap, NULL);
#else
  if( ! (*JavaEnv)->CallIntMethod( JavaEnv, JavaRenderer, JavaSwapBuffers ) )
    return 0;
#endif
  if( glContextLost )
  {
    glContextLost = 0;
    __android_log_print(ANDROID_LOG_INFO, "libSDL", "OpenGL context recreated, refreshing textures");
    SDL_ANDROID_VideoContextRecreated();
    appRestoredCallback();
  }
  return 1;
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(DemoRenderer_nativeResize) ( JNIEnv*  env, jobject  thiz, jint w, jint h )
{
  if( SDL_ANDROID_sWindowWidth == 0 )
  {
    SDL_ANDROID_sWindowWidth = w;
    SDL_ANDROID_sWindowHeight = h;
    __android_log_print(ANDROID_LOG_INFO, "libSDL", "Physical screen resolution is %dx%d", w, h);
  }
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(DemoRenderer_nativeDone) ( JNIEnv*  env, jobject  thiz )
{
  __android_log_print(ANDROID_LOG_INFO, "libSDL", "quitting...");
#if SDL_VERSION_ATLEAST(1,3,0)
  SDL_SendQuit();
#else
  SDL_PrivateQuit();
#endif
  __android_log_print(ANDROID_LOG_INFO, "libSDL", "quit OK");
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(DemoRenderer_nativeGlContextLost) ( JNIEnv*  env, jobject  thiz )
{
  __android_log_print(ANDROID_LOG_INFO, "libSDL", "OpenGL context lost, waiting for new OpenGL context");
  glContextLost = 1;
  appPutToBackgroundCallback();
#if SDL_VERSION_ATLEAST(1,3,0)
  if( ANDROID_CurrentWindow )
    SDL_SendWindowEvent(ANDROID_CurrentWindow, SDL_WINDOWEVENT_MINIMIZED, 0, 0);
#else
  SDL_PrivateAppActive(0, SDL_APPACTIVE|SDL_APPINPUTFOCUS|SDL_APPMOUSEFOCUS);
#endif
  SDL_ANDROID_VideoContextLost();
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(DemoRenderer_nativeGlContextRecreated) ( JNIEnv*  env, jobject  thiz )
{
  __android_log_print(ANDROID_LOG_INFO, "libSDL", "OpenGL context recreated, sending SDL_ACTIVEEVENT");
#if SDL_VERSION_ATLEAST(1,3,0)
  if( ANDROID_CurrentWindow )
    SDL_SendWindowEvent(ANDROID_CurrentWindow, SDL_WINDOWEVENT_RESTORED, 0, 0);
#else
  SDL_PrivateAppActive(1, SDL_APPACTIVE|SDL_APPINPUTFOCUS|SDL_APPMOUSEFOCUS);
#endif
}

JNIEXPORT void JNICALL
JAVA_EXPORT_NAME(DemoRenderer_nativeInitJavaCallbacks) ( JNIEnv*  env, jobject thiz )
{
  JavaEnv = env;
  JavaRenderer = thiz;

  JavaRendererClass = (*JavaEnv)->GetObjectClass(JavaEnv, thiz);
  JavaSwapBuffers = (*JavaEnv)->GetMethodID(JavaEnv, JavaRendererClass, "swapBuffers", "()I");

  ANDROID_InitOSKeymap();

}

int SDL_ANDROID_SetApplicationPutToBackgroundCallback(
    SDL_ANDROID_ApplicationPutToBackgroundCallback_t appPutToBackground,
    SDL_ANDROID_ApplicationPutToBackgroundCallback_t appRestored )
{
  appPutToBackgroundCallback = appPutToBackgroundCallbackDefault;
  appRestoredCallback = appRestoredCallbackDefault;

  if( appPutToBackground )
    appPutToBackgroundCallback = appPutToBackground;

  if( appRestoredCallback )
    appRestoredCallback = appRestored;
}
