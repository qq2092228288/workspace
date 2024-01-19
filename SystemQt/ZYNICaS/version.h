#ifndef VERSION_H
#define VERSION_H

#define FILE_VERSION            1,0,0,1
#define FILE_VERSION_STR        "1.0.0.1"
#define PRODUCT_VERSION         FILE_VERSION
#define PRODUCT_VERSION_STR     FILE_VERSION_STR
#define INTERNAL_NAME           "YZNICaS"
#define ORIGINAL_FILE_NAME      "YZNICaS.exe"
#define ORGANIZATION_NAME       "Changsha Yuanze Medical Equipment Co., Ltd"
#define ORGANIZATION_DOMAIN     "www.zeyaotebco.com"

#ifdef _WIN32
    #define APP_ID              "65F60647-7D49-4181-AD4E-C00F0CE577B9"
#elif linux
    #define APP_ID              "linux version app_id"
#endif

#endif // VERSION_H
