// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// This codes are modified from The Android Open Source Project
// (http://androidxref.com/4.0.4/xref/frameworks/base/include/utils/String8.h)

/*
* Copyright (C) 2005 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_STRING8_H_
#define XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_STRING8_H_

#include "xwalk/sysapps/device_capabilities/android/media_codec_sharedbuffer.h"

namespace xwalk {
namespace sysapps {

void initialize_string8();

class String8 {
public:
    String8();

    String8(const String8& o)
        : mString(o.mString)
    { SharedBuffer::bufferFromData(mString)->acquire(); }

    ~String8()
    { SharedBuffer::bufferFromData(mString)->release(); }

    String8& operator=(const String8& other)
    {
        setTo(other);
        return *this;
    }

    const char* string() const
    { return mString; }

    void setTo(const String8& other);

private:
    const char* mString;
};

}  // namespace sysapps
}  // namespace xwalk

#endif // XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_STRING8_H_
