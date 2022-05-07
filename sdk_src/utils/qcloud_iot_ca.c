/*
 * Tencent is pleased to support the open source community by making IoT Hub
 available.
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

 * Licensed under the MIT License (the "License"); you may not use this file
 except in
 * compliance with the License. You may obtain a copy of the License at
 * http://opensource.org/licenses/MIT

 * Unless required by applicable law or agreed to in writing, software
 distributed under the License is
 * distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND,
 * either express or implied. See the License for the specific language
 governing permissions and
 * limitations under the License.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "qcloud_iot_ca.h"

#include <stdlib.h>

#include "qcloud_iot_common.h"
#include "qcloud_iot_import.h"

typedef struct _RegionDomain_ {
    const char *region;
    const char *domain;
} RegionDomain;

/*mqtt domain*/
static RegionDomain sg_iot_mqtt_domain[] = {
    {.region = "china", .domain = QCLOUD_IOT_MQTT_DIRECT_DOMAIN},    /* China */
    {.region = "us-east", .domain = QCLOUD_IOT_MQTT_US_EAST_DOMAIN}, /* Eastern US*/
};

/*dynreg domain*/
static RegionDomain sg_iot_dyn_reg_domain[] = {
    {.region = "china", .domain = DYN_REG_SERVER_URL},           /* China */
    {.region = "us-east", .domain = DYN_REG_SERVER_US_EAST_URL}, /* Eastern US*/
};

/*log domain*/
static RegionDomain sg_iot_log_domain[] = {
    {.region = "china", .domain = LOG_UPLOAD_SERVER_DOMAIN},           /* China */
    {.region = "us-east", .domain = LOG_UPLOAD_SERVER_US_EAST_DOMAIN}, /* Eastern US*/
};

#ifndef AUTH_WITH_NOTLS
static const char *iot_ca_crt = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIC/TCCAeWgAwIBAgIUBFJaPndSi/R/e3kCSOSkXYSssuUwDQYJKoZIhvcNAQEL\r\n"
    "BQAwDTELMAkGA1UEAwwCY2EwIBcNMjIwNTA2MTIxMzI5WhgPMjEyMjA0MTIxMjEz\r\n"
    "MjlaMA0xCzAJBgNVBAMMAmNhMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\r\n"
    "AQEAqsugvpIhF8NnNlCrSUO9nfWRvECpfnsRaNYury3T3y/5IIcBdD6oqtkwlF9F\r\n"
    "jJqXH+STDWf6cdmQu0HfJtrh5FB3Uwlwg91v4OIjzlP1jsEIeq1T7XQCRteP3NBq\r\n"
    "lMwkfusW74MPxmiOGfflvKZwGNPPXnUY2NAj3WU6JHdUMMUI8Gso8ST4vCa5nYBG\r\n"
    "W1IDvD1HPjkR+lTwpVapLmbGw5CSTvHQ/WBm+ml5CmRzjxlYHI7b8HXPrMwnODRJ\r\n"
    "qGUC8oAsDa4kT7BaDkpCkLlUOogxkqZryRvWVqCqDpJG+O+W3yNC2tBT2tKWdQaD\r\n"
    "5dX+o64J+1z0Xa3/Um20MGMdKQIDAQABo1MwUTAdBgNVHQ4EFgQUhLFU/u88AoaI\r\n"
    "Wgd5YJQQ72SzVCEwHwYDVR0jBBgwFoAUhLFU/u88AoaIWgd5YJQQ72SzVCEwDwYD\r\n"
    "VR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAco1INDeed+UpaKFqzNq1\r\n"
    "9SvZsy9hxq8O0hj62iv/nJr33I20uSsrVGOU9mBKukgazf+XCQEwF9MI4J7BvsuM\r\n"
    "EZAbJFDd3y7K8Yag4HNULLoiWcFUTBAD0OwmdcpNKJjWvbaY1IJGmp1j+hcrfjm5\r\n"
    "SB9r6Cj7heuLbdk1rnuf1nyL0sB85vr+4Mek0GfhAEQP2TMRgOmp0jgr2XPPzMDM\r\n"
    "OSRNcJeBdmNuA5+mCDBC2j9dUH28rfEh/6aIW4vz0y15LcxEsg5/Aehdr6yp2i85\r\n"
    "4uH3qPgfttpEsuafBYMUrU2D3HgXVono8FeimFNZ+qhifY5h1xt/amv+sppyBopf\r\n"
    "lw==\r\n"
    "-----END CERTIFICATE-----"};


#ifdef OTA_USE_HTTPS

static const char *iot_https_ca_crt = {
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\r\n"
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n"
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\r\n"
    "MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n"
    "YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\r\n"
    "aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\r\n"
    "jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\r\n"
    "xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\r\n"
    "1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\r\n"
    "snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\r\n"
    "U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\r\n"
    "9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\r\n"
    "BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\r\n"
    "AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\r\n"
    "yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\r\n"
    "38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\r\n"
    "AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\r\n"
    "DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\r\n"
    "HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\r\n"
    "-----END CERTIFICATE-----\r\n"
    "-----BEGIN CERTIFICATE-----\r\n"
    "MIIEaTCCA1GgAwIBAgILBAAAAAABRE7wQkcwDQYJKoZIhvcNAQELBQAwVzELMAkG\r\n"
    "A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\r\n"
    "b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw0xNDAyMjAxMDAw\r\n"
    "MDBaFw0yNDAyMjAxMDAwMDBaMGYxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\r\n"
    "YWxTaWduIG52LXNhMTwwOgYDVQQDEzNHbG9iYWxTaWduIE9yZ2FuaXphdGlvbiBW\r\n"
    "YWxpZGF0aW9uIENBIC0gU0hBMjU2IC0gRzIwggEiMA0GCSqGSIb3DQEBAQUAA4IB\r\n"
    "DwAwggEKAoIBAQDHDmw/I5N/zHClnSDDDlM/fsBOwphJykfVI+8DNIV0yKMCLkZc\r\n"
    "C33JiJ1Pi/D4nGyMVTXbv/Kz6vvjVudKRtkTIso21ZvBqOOWQ5PyDLzm+ebomchj\r\n"
    "SHh/VzZpGhkdWtHUfcKc1H/hgBKueuqI6lfYygoKOhJJomIZeg0k9zfrtHOSewUj\r\n"
    "mxK1zusp36QUArkBpdSmnENkiN74fv7j9R7l/tyjqORmMdlMJekYuYlZCa7pnRxt\r\n"
    "Nw9KHjUgKOKv1CGLAcRFrW4rY6uSa2EKTSDtc7p8zv4WtdufgPDWi2zZCHlKT3hl\r\n"
    "2pK8vjX5s8T5J4BO/5ZS5gIg4Qdz6V0rvbLxAgMBAAGjggElMIIBITAOBgNVHQ8B\r\n"
    "Af8EBAMCAQYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUlt5h8b0cFilT\r\n"
    "HMDMfTuDAEDmGnwwRwYDVR0gBEAwPjA8BgRVHSAAMDQwMgYIKwYBBQUHAgEWJmh0\r\n"
    "dHBzOi8vd3d3Lmdsb2JhbHNpZ24uY29tL3JlcG9zaXRvcnkvMDMGA1UdHwQsMCow\r\n"
    "KKAmoCSGImh0dHA6Ly9jcmwuZ2xvYmFsc2lnbi5uZXQvcm9vdC5jcmwwPQYIKwYB\r\n"
    "BQUHAQEEMTAvMC0GCCsGAQUFBzABhiFodHRwOi8vb2NzcC5nbG9iYWxzaWduLmNv\r\n"
    "bS9yb290cjEwHwYDVR0jBBgwFoAUYHtmGkUNl8qJUC99BM00qP/8/UswDQYJKoZI\r\n"
    "hvcNAQELBQADggEBAEYq7l69rgFgNzERhnF0tkZJyBAW/i9iIxerH4f4gu3K3w4s\r\n"
    "32R1juUYcqeMOovJrKV3UPfvnqTgoI8UV6MqX+x+bRDmuo2wCId2Dkyy2VG7EQLy\r\n"
    "XN0cvfNVlg/UBsD84iOKJHDTu/B5GqdhcIOKrwbFINihY9Bsrk8y1658GEV1BSl3\r\n"
    "30JAZGSGvip2CTFvHST0mdCF/vIhCPnG9vHQWe3WVjwIKANnuvD58ZAWR65n5ryA\r\n"
    "SOlCdjSXVWkkDoPWoC209fN5ikkodBpBocLTJIg1MGCUF7ThBCIxPTsvFwayuJ2G\r\n"
    "K1pp74P1S8SqtCr4fKGxhZSM9AyHDPSsQPhZSZg=\r\n"
    "-----END CERTIFICATE-----"};
#endif

#endif

const char *iot_ca_get()
{
#ifndef AUTH_WITH_NOTLS
    return iot_ca_crt;
#else
    return NULL;
#endif
}

const char *iot_https_ca_get()
{
#if ((!defined(AUTH_WITH_NOTLS)) && (defined OTA_USE_HTTPS))
    return iot_https_ca_crt;
#else
    return NULL;
#endif
}

const char *iot_get_mqtt_domain(const char *region)
{
    const char *pDomain = NULL;
    int         i;

    if (!region) {
        goto end;
    }

    for (i = 0; i < sizeof(sg_iot_mqtt_domain) / sizeof(sg_iot_mqtt_domain[0]); i++) {
        if (0 == strcmp(region, sg_iot_mqtt_domain[i].region)) {
            pDomain = sg_iot_mqtt_domain[i].domain;
            break;
        }
    }

end:
    if (!pDomain) {
        pDomain = sg_iot_mqtt_domain[0].domain;
    }

    return pDomain;
}

const char *iot_get_dyn_reg_domain(const char *region)
{
    const char *pDomain = NULL;
    int         i;

    if (!region) {
        goto end;
    }

    for (i = 0; i < sizeof(sg_iot_dyn_reg_domain) / sizeof(sg_iot_dyn_reg_domain[0]); i++) {
        if (0 == strcmp(region, sg_iot_dyn_reg_domain[i].region)) {
            pDomain = sg_iot_dyn_reg_domain[i].domain;
            break;
        }
    }

end:
    if (!pDomain) {
        pDomain = sg_iot_dyn_reg_domain[0].domain;
    }

    return pDomain;
}

const char *iot_get_log_domain(const char *region)
{
    const char *pDomain = NULL;
    int         i;

    if (!region) {
        goto end;
    }

    for (i = 0; i < sizeof(sg_iot_log_domain) / sizeof(sg_iot_log_domain[0]); i++) {
        if (0 == strcmp(region, sg_iot_log_domain[i].region)) {
            pDomain = sg_iot_log_domain[i].domain;
            break;
        }
    }

end:
    if (!pDomain) {
        pDomain = sg_iot_log_domain[0].domain;
    }

    return pDomain;
}

#ifdef __cplusplus
}
#endif
