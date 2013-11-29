// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This codes are modified from The Android Open Source Project.
// (http://androidxref.com/4.0.4/xref/frameworks/base/include/utils/Vector.h)

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

#ifndef XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_VECTOR_H_
#define XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_VECTOR_H_

#include "xwalk/sysapps/device_capabilities/android/media_codec_string8.h"
#include "xwalk/sysapps/device_capabilities/android/media_codec_typehelper.h"

namespace xwalk {
namespace sysapps {

class VectorImpl {
public:
    enum { // flags passed to the ctor
        HAS_TRIVIAL_CTOR    = 0x00000001,
        HAS_TRIVIAL_DTOR    = 0x00000002,
        HAS_TRIVIAL_COPY    = 0x00000004,
    };

    VectorImpl(size_t itemSize, uint32_t flags)
        : mStorage(0), mCount(0), mFlags(flags), mItemSize(itemSize)
    { }

    VectorImpl(const VectorImpl& rhs);

    virtual ~VectorImpl() { }

    VectorImpl& operator = (const VectorImpl& rhs);

    const void *arrayImpl() const 
    { return mStorage; }

    void finish_vector()
    {
        release_storage();
        mStorage = 0;
        mCount = 0;
    }

    void release_storage()
    {
        if (mStorage) {
            const SharedBuffer* sb = SharedBuffer::sharedBuffer(mStorage);
            if (sb->release(SharedBuffer::eKeepStorage) == 1) {
                _do_destroy(mStorage, mCount);
                SharedBuffer::dealloc(sb);
            }
        }
    }

protected:
    virtual void do_construct(void* storage, size_t num) const = 0;
    virtual void do_destroy(void* storage, size_t num) const = 0;
    virtual void do_copy(void* dest, const void* from, size_t num) const = 0;
    virtual void do_splat(void* dest, const void* item, size_t num) const = 0;
    virtual void do_move_forward(void* dest, const void* from, size_t num) const = 0;
    virtual void do_move_backward(void* dest, const void* from, size_t num) const = 0;

    // take care of FBC...
    virtual void reservedVectorImpl1() { }
    virtual void reservedVectorImpl2() { } 
    virtual void reservedVectorImpl3() { }
    virtual void reservedVectorImpl4() { }
    virtual void reservedVectorImpl5() { }
    virtual void reservedVectorImpl6() { }
    virtual void reservedVectorImpl7() { }
    virtual void reservedVectorImpl8() { }

private:
    void _do_destroy(void* storage, size_t num) const
    {
        if (!(mFlags & HAS_TRIVIAL_DTOR)) {
            do_destroy(storage, num);
        }
    }

    void *mStorage;   // base address of the vector
    size_t mCount;     // number of items

    const uint32_t mFlags;
    const size_t mItemSize;
};

template <class TYPE>
class Vector : private VectorImpl {
public:
    Vector()
        : VectorImpl(sizeof(TYPE),  
                     ((traits<TYPE>::has_trivial_ctor   ? HAS_TRIVIAL_CTOR   : 0)
                     |(traits<TYPE>::has_trivial_dtor   ? HAS_TRIVIAL_DTOR   : 0)
                     |(traits<TYPE>::has_trivial_copy   ? HAS_TRIVIAL_COPY   : 0))
                    )
    { }

    Vector(const Vector<TYPE>& rhs);

    virtual ~Vector()
    { finish_vector(); }

    const Vector<TYPE>& operator = (const Vector<TYPE>& rhs) const;
    Vector<TYPE>& operator = (const Vector<TYPE>& rhs);

    const TYPE& operator [] (size_t index) const
    { return *(array() + index); }

    const TYPE* array() const
    { return static_cast<const TYPE *>(arrayImpl()); } 

protected:
    virtual void do_construct(void* storage, size_t num) const 
    { construct_type(reinterpret_cast<TYPE*>(storage), num); } 

    virtual void do_destroy(void* storage, size_t num) const 
    { destroy_type(reinterpret_cast<TYPE*>(storage), num); }

    virtual void do_copy(void* dest, const void* from, size_t num) const 
    { 
        copy_type(reinterpret_cast<TYPE*>(dest), 
                  reinterpret_cast<const TYPE*>(from), num); 
    }

    virtual void do_splat(void* dest, const void* item, size_t num) const 
    { 
        splat_type(reinterpret_cast<TYPE*>(dest), 
                   reinterpret_cast<const TYPE*>(item), num); 
    }

    virtual void do_move_forward(void* dest, const void* from, size_t num) const 
    { 
        move_forward_type(reinterpret_cast<TYPE*>(dest), 
                          reinterpret_cast<const TYPE*>(from), num); 
    }

    virtual void do_move_backward(void* dest, const void* from, size_t num) const 
    { 
        move_backward_type(reinterpret_cast<TYPE*>(dest),
                           reinterpret_cast<const TYPE*>(from), num);
    }
};

}  // namespace sysapps
}  // namespace xwalk

#endif  // XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_VECTOR_H_
