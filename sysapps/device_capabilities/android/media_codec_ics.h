#ifndef XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_ICS_H_
#define XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_ICS_H_

#include "base/android/jni_helper.h"

namespace xwalk {
namespace sysapps {

class MediaCodecICS {
public:
  MediaCodecICS(JNIEnv* env);

  jint GetInt(JNIEnv* env, jobject obj);
};

bool RegisterMediaCodecICS(JNIEnv* env);

}
}

#endif
