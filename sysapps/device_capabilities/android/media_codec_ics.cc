#include "xwalk/sysapps/device_capabilities/android/media_codec_ics.h"

#include "base/android/jni_android.h"
#include "jni/MediaCodecICS_jni.h"

namespace xwalk {
namespace sysapps {

MediaCodecICS::MediaCodecICS(JNIEnv* env) {
}

jint GetInt(JNIEnv* env, jobject obj) {
  return 9999;
}

bool RegisterMediaCodecICS(JNIEnv* env) {
  return RegisterNativesImpl(env) >= 0;
}

} // sysapps
} // xwalk

