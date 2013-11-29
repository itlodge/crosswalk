// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This codes are modified from The Android Open Source Project.
// (http://androidxref.com/4.0.4/xref/frameworks/base/include/utils/
//  SharedBuffer.h)

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

#ifndef XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_SHAREDBUFFER_H_
#define XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_SHAREDBUFFER_H_

#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>

namespace xwalk {
namespace sysapps {

class SharedBuffer {
public:
    enum {
        eKeepStorage = 0x00000001
    };

    static SharedBuffer* alloc(size_t size);
    static ssize_t dealloc(const SharedBuffer *released);
    static const SharedBuffer* bufferFromData(const void* data)
    { return ((const SharedBuffer*)data) - 1; }

    static const SharedBuffer *sharedBuffer(const void *data)
    { return data ? reinterpret_cast<const SharedBuffer *>(data)-1 : 0; }

    void* data() 
    { return this + 1; }
    
    void acquire() const;
    int32_t release(uint32_t flags = 0) const;

    bool onlyOwner() const
    { return (mRefs == 1); }

private:
    inline SharedBuffer() { }
    inline ~SharedBuffer() { }
    inline SharedBuffer(const SharedBuffer&);

   // 16 bytes. must be sized to preserve correct alingment.
    mutable int32_t mRefs;
    size_t mSize;
    uint32_t mReserved[2];
};

}  // namespace sysapps
}  // namespace xwalk

#endif // XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_SHAREDBUFFER_H_
