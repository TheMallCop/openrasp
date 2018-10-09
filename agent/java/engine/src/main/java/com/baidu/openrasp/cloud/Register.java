/*
 * Copyright 2017-2018 Baidu Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.baidu.openrasp.cloud;

import com.baidu.openrasp.cloud.model.CloudCacheModel;
import com.baidu.openrasp.cloud.model.CloudRequestUrl;
import com.baidu.openrasp.cloud.model.GenericResponse;
import com.baidu.openrasp.config.Config;
import com.baidu.openrasp.tool.OSUtil;
import com.baidu.openrasp.tool.model.ApplicationModel;
import com.google.gson.Gson;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

/**
 * @description: 云控注册接口
 * @author: anyang
 * @create: 2018/10/08 11:58
 */
public class Register {
    public static void register() throws Exception {
        Map<String, Object> params = GenerateParameters();
        String content = new Gson().toJson(params);
        String url = CloudRequestUrl.CLOUD_REGISTER_URL;
        GenericResponse response = new CloudHttp().request(url, content);
        if (response != null) {
            if (response.getStatus() != null && response.getStatus() == 0) {
                System.out.println("[OpenRASP] Cloud Control Registered Successed");
            } else {
                System.out.println("[OpenRASP] Cloud Control Registered Failed");
                throw new Exception();
            }
        }
    }

    private static Map<String, Object> GenerateParameters() {
        Map<String, Object> params = new HashMap<String, Object>();
        params.put("id", CloudCacheModel.getInstance().getRaspId());
        params.put("version", ApplicationModel.getRaspVersion());
        params.put("host_name", OSUtil.getHostName());
        params.put("language", "java");
        params.put("language_version", System.getProperty("java.version"));
        params.put("server_type", ApplicationModel.getServerName());
        params.put("server_version", ApplicationModel.getVersion());
        String raspHome = Config.getConfig().getBaseDirectory();
        params.put("rasp_home", raspHome);
        return params;
    }
}
