// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

// This codes are modified from The Android Open Source Project.
// (http://androidxref.com/4.0.4/xref/frameworks/base/libs/utils/String8.cpp)

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

#include "xwalk/sysapps/device_capabilities/android/media_codec_string8.h"

namespace xwalk {
namespace sysapps {

static SharedBuffer* gEmptyStringBuf = NULL;
static char* gEmptyString = NULL;

static inline char* getEmptyString()
{
    gEmptyStringBuf->acquire();
    return gEmptyString;
}

void initialize_string8()
{
    SharedBuffer* buf = SharedBuffer::alloc(1);
    char* str = (char*)buf->data();
    *str = 0;
    gEmptyStringBuf = buf;
    gEmptyString = str;
}

String8::String8()
    : mString(getEmptyString())
{ }

void String8::setTo(const String8& other)
{
    SharedBuffer::bufferFromData(other.mString)->acquire();
    SharedBuffer::bufferFromData(mString)->release();
    mString = other.mString;
}

}  // namespace sysapps
}  // namespace xwalk
