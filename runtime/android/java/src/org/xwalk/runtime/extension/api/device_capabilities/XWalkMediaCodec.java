// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.xwalk.runtime.extension.api.device_capabilities;

import java.util.Set;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.os.Build;

public abstract class XWalkMediaCodec {
    protected static final String TAG = "XWalkMediaCodec";

    /** Android MediaCodec API reports codec with namespace instead of common codec name.
     *  For example: "OMX.google.h263.decoder", what this API want to report is "h263".
     *
     *  The below static lists are used to parse out the common codec name. The codec names
     *  are referenced by below links, it maybe adjusted on demand.
     *
     *  http://www.chromium.org/audio-video
     *  https://developer.mozilla.org/en-US/docs/HTML/Supported_media_formats
     */
    protected static final String[] AUDIO_CODEC_NAMES =
            {"ALAC", "MP3", "AMRNB", "AMRWB", "AAC", "G711", "VORBIS", "WMA", "FLAC", "OPUS"};
    protected static final String[] VIDEO_CODEC_NAMES =
            {"H263", "H264", "MPEG4", "AVC", "WMV", "VP8", "Theora"};

    protected Set<AudioCodecElement> mAudioCodecsSet;
    protected Set<VideoCodecElement> mVideoCodecsSet;

    protected DeviceCapabilities mDeviceCapabilities;
    private static XWalkMediaCodec mInstance;

    protected class AudioCodecElement {
        public String codecName;

        public AudioCodecElement(String name) {
            codecName = name;
        }

        // Objects of this class need to be added to a HashSet, so we implement equals()
        // and hashCode() to support comparison operation.
        public boolean equals(Object obj) {
            return this.codecName.equals(((AudioCodecElement)obj).codecName);
        }

        public int hashCode() {
            return codecName.hashCode();
        }
    }

    protected class VideoCodecElement {
        public String codecName;
        public boolean isEncoder;
        public boolean hwAccel;

        public VideoCodecElement(String name, boolean encoder, boolean hardware) {
            codecName = name;
            isEncoder = encoder;
            hwAccel = hardware;
        }

        // Objects of this class need to be added to a HashSet, so we implement equals()
        // and hashCode() to support comparison operation.
        public boolean equals(Object obj) {
            return this.codecName.equals(((VideoCodecElement)obj).codecName)
                    && this.isEncoder == ((VideoCodecElement)obj).isEncoder
                            && this.hwAccel == ((VideoCodecElement)obj).hwAccel;
        }

        public int hashCode() {
            return codecName.hashCode();
        }
    }

    public static XWalkMediaCodec getInstance(DeviceCapabilities instance) {
        if (mInstance == null) {
            if (Build.VERSION.SDK_INT >= 16)
                mInstance = new MediaCodec(instance);
            else if (Build.VERSION.SDK_INT == 15)
                mInstance = new MediaCodecICS(instance);
            else
                mInstance = new MediaCodecNull(instance);
        }
        return mInstance;
    }

    public JSONObject getCodecsInfo() {
        JSONObject outputObject = new JSONObject();
        JSONArray audioCodecsArray = new JSONArray();
        JSONArray videoCodecsArray = new JSONArray();

        try {
            for (AudioCodecElement codecToAdd : mAudioCodecsSet) {
                JSONObject codecsObject = new JSONObject();
                codecsObject.put("format", codecToAdd.codecName);
                audioCodecsArray.put(codecsObject);
            }
            for (VideoCodecElement codecToAdd : mVideoCodecsSet) {
                JSONObject codecsObject = new JSONObject();
                codecsObject.put("format", codecToAdd.codecName);
                codecsObject.put("encode", codecToAdd.isEncoder);
                codecsObject.put("hwAccel", codecToAdd.hwAccel);
                videoCodecsArray.put(codecsObject);
            }

            outputObject.put("audioCodecs", audioCodecsArray);
            outputObject.put("videoCodecs", videoCodecsArray);
        } catch (JSONException e) {
            return mDeviceCapabilities.setErrorMessage(e.toString());
        }

        return outputObject;
    }

    protected abstract void getCodecsList();
}
