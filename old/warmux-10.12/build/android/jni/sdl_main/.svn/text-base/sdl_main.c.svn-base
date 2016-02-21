#ifdef ANDROID

#include <unistd.h>
#include <jni.h>
#include <android/log.h>
#include "SDL_thread.h"
#include "SDL_main.h"

/* JNI-C wrapper stuff */

#ifdef __cplusplus
#define C_LINKAGE "C"
#else
#define C_LINKAGE
#endif

static int isSdcardUsed = 0;

#ifndef SDL_JAVA_PACKAGE_PATH
#error You have to define SDL_JAVA_PACKAGE_PATH to your package path with dots replaced with underscores, for example "com_example_SanAngeles"
#endif
#define JAVA_EXPORT_NAME2(name,package) Java_##package##_##name
#define JAVA_EXPORT_NAME1(name,package) JAVA_EXPORT_NAME2(name,package)
#define JAVA_EXPORT_NAME(name) JAVA_EXPORT_NAME1(name,SDL_JAVA_PACKAGE_PATH)

extern C_LINKAGE void
JAVA_EXPORT_NAME(DemoRenderer_nativeInit) ( JNIEnv*  env, jobject thiz )
{
  char * argv[] = { "sdl" }; //, "-d", "network" };

  if (isSdcardUsed) {
    chdir("/sdcard/Warmux");
  } else {
    chdir("/data/data/org.warmux/files");
  }

  // Redirect output
  freopen("./stdout.txt", "wt", stdout);
  freopen("./stderr.txt", "wt", stderr);

  // Launch
  main(sizeof(argv)/sizeof(char*), argv);
}

extern C_LINKAGE void
JAVA_EXPORT_NAME(Settings_nativeIsSdcardUsed) ( JNIEnv*  env, jobject thiz, jint flag )
{
  isSdcardUsed = flag;
}

extern C_LINKAGE void
JAVA_EXPORT_NAME(Settings_nativeSetEnv) ( JNIEnv*  env, jobject thiz, jstring j_name, jstring j_value )
{
  jboolean iscopy;
  const char *name = (*env)->GetStringUTFChars(env, j_name, &iscopy);
  const char *value = (*env)->GetStringUTFChars(env, j_value, &iscopy);
  setenv(name, value, 1);
  (*env)->ReleaseStringUTFChars(env, j_name, name);
  (*env)->ReleaseStringUTFChars(env, j_value, value);
}

#undef JAVA_EXPORT_NAME
#undef JAVA_EXPORT_NAME1
#undef JAVA_EXPORT_NAME2
#undef C_LINKAGE

#endif
