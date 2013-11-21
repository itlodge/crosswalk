// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.xwalk.runtime.extension.api.device_capabilities;

import java.util.ArrayList;
import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;

import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONArray;
import org.xwalk.runtime.extension.XWalkExtensionContext;

import android.util.Log;
import android.os.Build;
import android.content.res.AssetManager;

public class DeviceCapabilitiesCodecs {
    private static final String MEDIA_CODECS_FILE = "/system/etc/media_codecs.xml";
    private static final String TAG = "DeviceCapabilitiesCodecs";

    private DeviceCapabilities mDeviceCapabilities;
    private XWalkExtensionContext mContext;

    private boolean mEncode;
    private boolean mHwAccel;
    private String mFormat;

    public DeviceCapabilitiesCodecs(DeviceCapabilities instance,
                                    XWalkExtensionContext context) {
        mDeviceCapabilities = instance;
        mContext = context;
    }

    public JSONObject getInfo() {
        if (Build.VERSION.SDK_INT < 16) {
            Log.d(TAG, getCodecInfoList().toString());
            return getCodecInfoList();
        }
        JSONObject outputObject = new JSONObject();
        JSONArray audioCodecsArray = new JSONArray();
        JSONArray videoCodecsArray = new JSONArray();

        int numCodecs = android.media.MediaCodecList.getCodecCount();
        try {
            for (int i = 0; i < numCodecs; i++) {
                android.media.MediaCodecInfo codeInfo = 
                    android.media.MediaCodecList.getCodecInfoAt(i);
                String[] formats = codeInfo.getSupportedTypes();
                for (String format : formats) {
                    JSONObject codecsObject = new JSONObject();

                    String[] msgArray = format.split("/", 2);
                    // FIXME get the value of hardware-accelerate
                    codecsObject.put("format", codeInfo.getName());
                    if (msgArray[0].equals("audio")) {
                        audioCodecsArray.put(codecsObject);
                    } else if (msgArray[0].equals("video")) {
                        codecsObject.put("encode", codeInfo.isEncoder());
                        codecsObject.put("hwAccel", false);
                        videoCodecsArray.put(codecsObject);
                    }
                }
            }
            outputObject.put("audioCodecs", audioCodecsArray);
            outputObject.put("videoCodecs", videoCodecsArray);
        } catch (JSONException e) {
            return setErrorMessage(e.toString());
        }

        return outputObject;
    }

    private JSONObject setErrorMessage(String error) {
        JSONObject jsonObject = new JSONObject();
        try {
            jsonObject.put("error", error);
        } catch (JSONException e) {
            Log.e(TAG, e.toString());
        }
        return jsonObject;
    }

    private class MediaCodecInfo {
        private String mName;
        private String[] mSupportedTypes;
        private boolean mIsEncoder;

        public MediaCodecInfo(String name, String[] types, boolean isEncoder) {
            mName = name;
            mSupportedTypes = types;
            mIsEncoder = isEncoder;
        }

        public final String getName() {
            return mName;
        }

        public final String[] getSupportedTypes() {
            return mSupportedTypes;
        }

        public final boolean isEncoder() {
            return mIsEncoder;
        }
    }

    private JSONObject getCodecInfoList() {
        ArrayList<MediaCodecInfo> codecInfoList = new ArrayList<MediaCodecInfo>();
        JSONObject outputObject = new JSONObject();

        try {
            AssetManager assetManager = mContext.getContext().getAssets();
            InputStream codecStream = assetManager.open("media_codecs.xml");

            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder builder = factory.newDocumentBuilder();
            Document codecDoc = builder.parse(codecStream);
            codecDoc.getDocumentElement().normalize();

            codecInfoList.addAll(getInfoByType(codecDoc, true));
            codecInfoList.addAll(getInfoByType(codecDoc, false));

            JSONArray audioCodecsArray = new JSONArray();
            JSONArray videoCodecsArray = new JSONArray();

            for (MediaCodecInfo info : codecInfoList) {
                JSONObject codecObj = new JSONObject();
                String name = info.getName();
                if (name.endsWith(".decoder") || name.endsWith(".encoder")) {
                    name = name.substring(0, name.lastIndexOf('.'));
                    name = name.substring(name.lastIndexOf('.') + 1);
                } else if (name.endsWith("Encoder")) {
                    name = name.substring(0, name.indexOf("Encoder") - 1);
                } else if (name.endsWith("Decoder")) {
                    name = name.substring(0, name.indexOf("Decoder") - 1);
                } else if (name.contains(".VideoEncoder.") || 
                        name.contains(".VideoDecoder.")) {
                    name = name.substring(name.lastIndexOf('.') + 1); 
                }
                codecObj.put("format", name);
                String[] types = info.getSupportedTypes();
                String type = (types[0].substring(0, types[0].indexOf('/')));
                if (type.equals("audio")) {
                   audioCodecsArray.put(codecObj); 
                } else {
                   codecObj.put("encode", info.isEncoder());
                   codecObj.put("hwAccel", false);
                   videoCodecsArray.put(codecObj);
                }
            }
            outputObject.put("audioCodecs", audioCodecsArray);
            outputObject.put("videoCodecs", videoCodecsArray);
        } catch (Exception e) {
            return setErrorMessage(e.toString());   
        }
        return outputObject;
    }

    private ArrayList<MediaCodecInfo> getInfoByType(Document codecDoc, 
                                                           boolean isEncoder) {
        ArrayList<MediaCodecInfo> codecInfoList = new ArrayList<MediaCodecInfo>();
        NodeList codecList = null;
        if (isEncoder) {
            codecList = codecDoc.getElementsByTagName("Encoders");
        } else {
            codecList = codecDoc.getElementsByTagName("Decoders");
        }
        for (int i = 0; i < codecList.getLength(); ++i) {
           if (!codecList.item(i).hasChildNodes()) {
               continue;
           }
           NodeList infoNodeList = codecList.item(i).getChildNodes();
           for (int j = 0; j < infoNodeList.getLength(); ++j) { 
               if (!infoNodeList.item(j).hasAttributes()) {
                   continue;
               }
               Element infoElement = (Element)infoNodeList.item(j);
               String name = infoElement.getAttribute("name");
               if (name.equals("")) {
                   continue;
               }
               ArrayList<String> types = new ArrayList<String>();
               String type = infoElement.getAttribute("type");
               if (type.equals("")) {
                   if (!infoNodeList.item(j).hasChildNodes()) {    
                       continue;
                   }
                   NodeList innerList = infoNodeList.item(j).getChildNodes();
                   for (int k = 0; k < innerList.getLength(); ++k) {
                       if (!innerList.item(k).getNodeName().equals("Type")) {
                           continue;
                       }
                       if (!innerList.item(k).hasAttributes()) {
                           continue;
                       }
                       Element innerElement = (Element)innerList.item(k);
                       String typeName = innerElement.getAttribute("name");
                       if (typeName.equals("")) {
                           continue;
                       }
                       types.add(typeName);
                   }
                   if (types.isEmpty()) {
                       continue;
                   }
               } else {
                   types.add(type);
               }

               MediaCodecInfo info = new MediaCodecInfo(name, 
                       types.toArray(new String[types.size()]), isEncoder);
               codecInfoList.add(info); 
           }
        }
        return codecInfoList;
    }
}

