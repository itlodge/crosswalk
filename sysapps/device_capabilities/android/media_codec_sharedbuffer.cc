// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This codes are modified from The Android Open Source Project.
// (http://androidxref.com/4.0.4/xref/frameworks/base/libs/utils/
//  SharedBuffer.cpp)

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

#include "xwalk/sysapps/device_capabilities/android/media_codec_sharedbuffer.h"

namespace xwalk {
namespace sysapps {

static int android_atomic_cas(int32_t old_value, int32_t new_value,
                       volatile int32_t *ptr)
{
    int32_t prev;
    __asm__ __volatile__ ("lock; cmpxchgl %1, %2"
                          : "=a" (prev)
                          : "q" (new_value), "m" (*ptr), "0" (old_value)
                          : "memory");
    return prev != old_value;
}

static int android_atomic_release_cas(int32_t old_value,
                               int32_t new_value,
                               volatile int32_t *ptr)
{
    /* Stores are not reordered with other stores. */
    return android_atomic_cas(old_value, new_value, ptr);
}

static int32_t android_atomic_inc(volatile int32_t* addr) {
    int32_t oldValue;
    do {
        oldValue = *addr;
    } while (android_atomic_release_cas(oldValue, oldValue + 1, addr));
    return oldValue;
}

static int32_t android_atomic_dec(volatile int32_t* addr) {
    int32_t oldValue;
    do {
        oldValue = *addr;
    } while (android_atomic_release_cas(oldValue, oldValue - 1, addr));
    return oldValue;
}
 
SharedBuffer* SharedBuffer::alloc(size_t size)
{
    SharedBuffer* sb = static_cast<SharedBuffer *>(malloc(sizeof(SharedBuffer) + size));
    if (sb) {
        sb->mRefs = 1;
        sb->mSize = size;
    }
    return sb;
}

ssize_t SharedBuffer::dealloc(const SharedBuffer *released)
{
    if (released->mRefs != 0) { // XXX: invalid operation
        return -1; 
    }
    free(const_cast<SharedBuffer*>(released));
    return 0;
}

void SharedBuffer::acquire() const
{
    android_atomic_inc(&mRefs);
}

int32_t SharedBuffer::release(uint32_t flags) const
{
    int32_t prev = 1;
    if (onlyOwner() || ((prev = android_atomic_dec(&mRefs)) == 1)) {
        mRefs = 0;
        if ((flags & eKeepStorage) == 0) {
            free(const_cast<SharedBuffer*>(this));
        }
    }
    return prev;
}

}  // namespace sysapps
}  // namespace xwalk
