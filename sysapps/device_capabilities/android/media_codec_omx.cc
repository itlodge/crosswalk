// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xwalk/sysapps/device_capabilities/android/media_codec_omx.h"

#include <dlfcn.h>
#include <stdio.h>

namespace xwalk {
namespace sysapps {

// Class CodecList::Codec

CodecList::Codec::Codec(const char* name, const char* role)
    : name_(),
      role_() {
  size_t name_len = strlen(name);
  size_t role_len = strlen(role);
  name_ = (char*)malloc(sizeof(char) * name_len + 1);
  if (name_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  role_ = (char*)malloc(sizeof(char) * role_len + 1);
  if (role_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  strncpy(name_, name, name_len);
  *(name_ + name_len) = '\0';
  strncpy(role_, role, role_len);
  *(role_ + role_len) = '\0';
}

CodecList::Codec::Codec(const Codec& codec)
    : name_(),
      role_() {
  size_t name_len = strlen(codec.name_);
  size_t role_len = strlen(codec.role_);
  name_ = (char*)malloc(sizeof(char) * name_len + 1);
  if (name_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  role_ = (char*)malloc(sizeof(char) * role_len + 1);
  if (role_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  strncpy(name_, codec.name_, name_len);
  *(name_ + name_len) = '\0';
  strncpy(role_, codec.role_, role_len);
  *(role_ + role_len) = '\0';
}

const CodecList::Codec& CodecList::Codec::operator=(
    const CodecList::Codec& codec) {
  if (this == &codec) {
    return *this;
  }
  CodecList::Codec tmp(codec);

  char *swap = tmp.name_;
  tmp.name_ = name_;
  name_ = swap;

  swap = tmp.role_;
  tmp.role_ = role_;
  role_ = swap;

  return *this;
}

CodecList::Codec::~Codec() {
  if (name_ != NULL) {
    free(name_);
    name_ = NULL;
  }
  if (role_ != NULL) {
    free(role_);
    role_ = NULL;
  }
}

// Class CodecList

CodecList::CodecList() {
  const char* kDefaultCodecs[][2] = {
    { "OMX.google.aac.decoder", "audio_decoder.aac" },
    { "OMX.google.amrnb.decoder", "audio_decoder.amrnb" },
    { "OMX.google.amrwb.decoder", "audio_decoder.amrwb" },
    { "OMX.google.h264.decoder", "video_decoder.avc" },
    { "OMX.google.g711.alaw.decoder", "audio_decoder.g711alaw" },
    { "OMX.google.g711.mlaw.decoder", "audio_decoder.g711mlaw" },
    { "OMX.google.h263.decoder", "video_decoder.h263" },
    { "OMX.google.mpeg4.decoder", "video_decoder.mpeg4" },
    { "OMX.google.mp3.decoder", "audio_decoder.mp3" },
    { "OMX.google.vorbis.decoder", "audio_decoder.vorbis" },
    { "OMX.google.vpx.decoder", "video_decoder.vpx" }
  };
  size_ = sizeof(kDefaultCodecs) / sizeof(kDefaultCodecs[0]);
  capacity_ = size_;
  codecs_ = new Codec[capacity_];
  if (codecs_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  for (size_t i = 0; i < size_; ++i) {
    Codec codec(kDefaultCodecs[i][0], kDefaultCodecs[i][1]);
    codecs_[i] = codec;
  }
}

CodecList::~CodecList() {
  if (codecs_ != NULL) {
    delete[] codecs_;
    codecs_ = NULL;
  }
}

void CodecList::Enlarge() {
  Codec* old_codecs = codecs_;
  codecs_ = new Codec[capacity_ * 2];

  if (codecs_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  for (size_t i = 0; i < size_; ++i) {
    codecs_[i] = old_codecs[i];
  }
  capacity_ *= 2;

  if (old_codecs != NULL) {
    delete[] old_codecs;
    old_codecs = NULL;
  }
}

CodecString CodecList::ToString() {
  CodecString codecs_str;

  for (size_t i = 0; i < size_; ++i) {
    if (i > 0) {
      codecs_str.Append(',');
    }
    codecs_str.Append(codecs_[i].name());
    codecs_str.Append('|');
    for (const char* p = codecs_[i].role(); *p != '\0'; ++p) {
      if (*p == '.') {
        break;
      } else if (*p == '_') {
        codecs_str.Append('|');
      } else {
        codecs_str.Append(*p);
      }
    }
  }
  return codecs_str;
}

CodecString CodecList::GetCodecs() {
  // This function locates at media_codecs_string8.cc.
  // Use a global shared buffer to store string, so need
  // to initialize the buffer here.
  initialize_string8();

  // Get the codecs from the device vendor.
  void *handle = dlopen("libstagefrighthw.so", RTLD_NOW);
  if (handle == NULL) {
    return this->ToString();
  }
  typedef OMXPluginBase *(*CreateOMXPluginFunc)();
  CreateOMXPluginFunc createOMXPlugin =
    (CreateOMXPluginFunc)dlsym(handle, "createOMXPlugin");
  if (!createOMXPlugin) {
    createOMXPlugin = (CreateOMXPluginFunc)dlsym(
        handle, "_ZN7android15createOMXPluginEv");
  }
  OMXPluginBase *plugin = (*createOMXPlugin)();

  if (!plugin) {
    return this->ToString();
  }
  OMX_U32 index = -1;
  char name[128];
  OMX_ERRORTYPE err;
  while ((err = plugin->enumerateComponents(name, sizeof(name), ++index))
      == OMX_ErrorNone) {
    Vector<String8> roles;
    if ((err = plugin->getRolesOfComponent(name, &roles))
        == OMX_ErrorNone) {
      Codec codec(name, roles[0].string());
      this->Add(codec);
    } else {
      fprintf(stderr, "Error occured when getting codec roles, 0x%x\n", err);
    }
  }
  if (err != OMX_ErrorNoMore) {
    fprintf(stderr, "Error occured when getting codec name, 0x%x\n", err);
  }
  dlclose(handle);

  return this->ToString();
}

// Class CodecString

CodecString::CodecString()
    : data_(),
      size_(0),
      capacity_(128) {
  data_ = (char*)malloc(sizeof(char) * capacity_);
  if (data_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  *(data_) = '\0';
}

CodecString::CodecString(const CodecString& str)
    : data_(),
      size_(str.size_),
      capacity_(str.capacity_) {
  data_ = (char*)malloc(sizeof(char) * capacity_);
  if (data_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  strncpy(data_, str.data_, size_);
  *(data_ + size_) = '\0';
}

const CodecString& CodecString::operator=(const CodecString& str) {
  if (this == &str) {
    return *this;
  }
  CodecString tmp(str);

  char* swap = tmp.data_;
  tmp.data_ = data_;
  data_ = swap;

  size_ = tmp.size_;
  capacity_ = tmp.capacity_;

  return *this;
}

CodecString::~CodecString() {
  if (data_ != NULL) {
    free(data_);
    data_ = NULL;
  }
}

void CodecString::Append(const char* str) {
  if (str == NULL) {
    return;
  }
  size_t len = strlen(str);
  while (size_ + len >= capacity_) {
    this->Enlarge();
  }
  strncpy(data_ + size_, str, len);
  size_ += len;
  *(data_ + size_) = '\0';
}

void CodecString::Append(char ch) {
  while (size_ + 1 >= capacity_) {
    this->Enlarge();
  }
  *(data_ + size_) = ch;
  ++size_;
  *(data_ + size_) = '\0';
}

void CodecString::Enlarge() {
  char* old_data = data_;
  data_ = (char*)malloc(sizeof(char) * capacity_ * 2);
  if (data_ == NULL) {
    fprintf(stderr, "Failed to allocate memory!\n");
    exit(1);
  }
  memcpy(data_, old_data, sizeof(char) * size_);
  capacity_ *= 2;
  if (old_data != NULL) {
    free(old_data);
    old_data = NULL;
  }
}

}  // namespace sysapps
}  // namespace xwalk
