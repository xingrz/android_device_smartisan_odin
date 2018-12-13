/*
Copyright (c) 2013, The Linux Foundation. All rights reserved.
Copyright (c) 2018, The MoKee Open Source Project

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <android-base/file.h>
#include <android-base/strings.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include "vendor_init.h"
#include "property_service.h"

using android::base::ReadFileToString;
using android::init::property_set;

const std::string version_id_file = "/sys/hwinfo/version_id";

void property_override(char const prop[], char const value[])
{
    prop_info *pi = (prop_info *) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void property_override_dual(char const system_prop[], char const vendor_prop[], char const value[])
{
    property_override(system_prop, value);
    property_override(vendor_prop, value);
}

void init_product_properties()
{
    std::string version_id;

    if (ReadFileToString(version_id_file, &version_id)) {
        if (version_id.find("version_id=primary:") == 0 ||
            version_id.find("version_id=standard:") == 0) {
            property_override_dual(
                "ro.product.model", "ro.vendor.product.model",
                "OD103");
        }
        else if (version_id.find("version_id=advanced1:") == 0 ||
                 version_id.find("version_id=advanced2:") == 0) {
            property_override_dual(
                "ro.product.model", "ro.vendor.product.model",
                "OD105");
        }
        else {
            LOG(ERROR) << "Unknown version_id: " << version_id;
        }
    }
    else {
        LOG(ERROR) << "Unable to read version_id from " << version_id_file;
    }
}

void vendor_load_properties() {
    LOG(INFO) << "Loading vendor specific properties";
    init_product_properties();
}
