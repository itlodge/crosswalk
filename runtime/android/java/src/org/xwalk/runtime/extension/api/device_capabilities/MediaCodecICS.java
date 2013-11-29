// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.xwalk.runtime.extension.api.device_capabilities;

import java.util.HashSet;

import android.util.Log;

import org.json.JSONObject;

import org.chromium.base.JNINamespace;

@JNINamespace("xwalk::sysapps")
public class MediaCodecICS extends XWalkMediaCodec {
    public MediaCodecICS(DeviceCapabilities instance) {
        mDeviceCapabilities = instance;

        mAudioCodecsSet = new HashSet<AudioCodecElement>();
        mVideoCodecsSet = new HashSet<VideoCodecElement>();

        getCodecsList();
    }

    @Override
    protected void getCodecsList() {
        String[] codecs = nativeGetCodecs().split(",");
        for (String codec : codecs) {
            String[] attrs = codec.split("\\|");
            String name = attrs[0].toUpperCase();
            String type = attrs[1];

            if (type.equals("audio")) {
                for (String audioCodecName : AUDIO_CODEC_NAMES) {
                    if (!name.contains(audioCodecName)) {
                        continue;
                    }
                    mAudioCodecsSet.add(new AudioCodecElement(audioCodecName));
                    break;
                } 
            } else {
                for (String videoCodecName : VIDEO_CODEC_NAMES) {
                    if (!name.contains(videoCodecName)) {
                        continue;
                    }
                    boolean isEncoder = attrs[2].equals("encoder") ? true : false;
                    //FIXME (fyraimar) Get the right hwAccel value
                    mVideoCodecsSet.add(new VideoCodecElement(
                            videoCodecName, isEncoder, false));
                    break;
                }
            }
        }
    }

    private native String nativeGetCodecs();
}
