// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.xwalk.runtime.extension.api.device_capabilities;

import org.chromium.base.JNINamespace;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

@JNINamespace("xwalk::sysapps")
public class MediaCodecICS extends XWalkMediaCodec {
    public MediaCodecICS(DeviceCapabilities instance) {
    }

    @Override
    public JSONObject getCodecsInfo() {
        JSONObject out = new JSONObject();
        JSONArray audioCodecsArray = new JSONArray();
        JSONArray videoCodecsArray = new JSONArray();

        int tmp = -1;
        tmp = nativeGetInt();
        try {
            JSONObject obj = new JSONObject();
            obj.put("format", tmp);
            obj.put("encode", false);
            obj.put("hwAccel", false);
            videoCodecsArray.put(obj);

            out.put("audioCodecs", audioCodecsArray);
            out.put("videoCodecs", videoCodecsArray);
        } catch (JSONException e) {
            return mDeviceCapabilities.setErrorMessage(e.toString());
        }

        return out;
    }

    private native int nativeGetInt();
}
