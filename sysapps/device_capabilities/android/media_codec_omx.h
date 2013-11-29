// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_OMX_H_
#define XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_OMX_H_

#include "xwalk/sysapps/device_capabilities/android/media_codec_vector.h"

namespace xwalk {
namespace sysapps {

typedef char* OMX_STRING;
typedef unsigned int OMX_U32;

typedef enum OMX_ERRORTYPE {
  OMX_ErrorNone = 0,
  OMX_ErrorNoMore = 0x8000100E
} OMX_ERRORTYPE;

// This struct is used to fetch the codecs provided by the device vendor.
struct OMXPluginBase {
  OMXPluginBase() {}
  virtual ~OMXPluginBase() {}
  // Need two dummy virtual functions to fill the virtual table.
  virtual void dummy1() = 0;
  virtual void dummy2() = 0;
  virtual OMX_ERRORTYPE enumerateComponents(
      OMX_STRING name,
      size_t size,
      OMX_U32 index) = 0;
  virtual OMX_ERRORTYPE getRolesOfComponent(
      const char *name,
      Vector<String8> *roles) = 0;

 private:
  OMXPluginBase(const OMXPluginBase &);
  OMXPluginBase &operator=(const OMXPluginBase &);
};

class CodecString;

class CodecList {
 public:
  CodecList();
  ~CodecList();
  // Return all the codes. The string is with this form:
  // "{name}|{audio/video}|{encoder/decoder},{name}...".
  // For example, "omx.google.aac.decoder|audio|decoder,
  // omx.google.h264.encoder|video|encoder".
  CodecString GetCodecs();

 private:
  class Codec {
   public:
    Codec() : name_(NULL), role_(NULL) { }
    Codec(const char* name, const char* role);
    Codec(const Codec& codec);
    const Codec& operator=(const Codec& codec);
    ~Codec();
    const char* name() const { return name_; }
    const char* role() const { return role_; }

   private:
    char* name_;
    char* role_;
  };

 private:
  CodecList(const CodecList&);
  const CodecList& operator=(const CodecList&);

  void Add(const Codec& codec) {
    while ((size_ + 1) >= capacity_) {
      this->Enlarge();
    }
    codecs_[size_] = codec;
    ++size_;
  }

  void Enlarge();
  CodecString ToString();

  Codec* codecs_;
  size_t size_;
  size_t capacity_;
};

class CodecString {
 public:
  CodecString();
  CodecString(const CodecString& str);
  const CodecString& operator=(const CodecString& str);
  ~CodecString();
  void Append(const char* str);
  void Append(char ch);

  const char* c_str() const { return data_; }

 private:
  void Enlarge();

  char* data_;
  size_t size_;
  size_t capacity_;
};

}  // namespace sysapps
}  // namespace xwalk

#endif  // XWALK_SYSAPPS_DEVICE_CAPABLILITIES_ANDROID_MEDIA_CODEC_OMX_H_
