// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_ICS_H_
#define XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_ICS_H_

#include "base/android/jni_helper.h"
#include "xwalk/sysapps/device_capabilities/android/media_codec_omx.h"

namespace xwalk {
namespace sysapps {

// This class is designed for getting the codecs for 
// Android 4.0(Ice Cream Sandwich).
class MediaCodecICS {
 public:
  explicit MediaCodecICS(JNIEnv* env);

  jstring GetCodecs(JNIEnv* env, jobject obj);
};

bool RegisterMediaCodecICS(JNIEnv* env);

}  // namespace sysapps
}  // namespace xwalk

#endif  // XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_ICS_H_
