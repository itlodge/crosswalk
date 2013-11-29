// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xwalk/sysapps/device_capabilities/android/media_codec_ics.h"

#include "base/android/jni_android.h"
#include "jni/MediaCodecICS_jni.h"

namespace xwalk {
namespace sysapps {

MediaCodecICS::MediaCodecICS(JNIEnv* env) { }

jstring GetCodecs(JNIEnv* env, jobject obj) {
    CodecList list;
    CodecString codecs = list.GetCodecs();

    return env->NewStringUTF(codecs.c_str());
}

bool RegisterMediaCodecICS(JNIEnv* env) {
  return RegisterNativesImpl(env) >= 0;
}

} // namespace sysapps
} // namespace xwalk
