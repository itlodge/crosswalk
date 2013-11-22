// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.xwalk.runtime.extension.api.device_capabilities;

import org.json.JSONObject;
import org.xwalk.runtime.extension.XWalkExtensionContext;

public class DeviceCapabilitiesCodecs {
    private static XWalkMediaCodec mMediaCodec;

    public DeviceCapabilitiesCodecs(DeviceCapabilities instance,
                                    XWalkExtensionContext context) {
        mMediaCodec = XWalkMediaCodec.getInstance(instance);
    }

    public JSONObject getInfo() {
        return mMediaCodec.getCodecsInfo();
    }
}
