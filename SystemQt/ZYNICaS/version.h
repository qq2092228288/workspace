#ifndef VERSION_H
#define VERSION_H

#define FILE_VERSION            4,5,1,1123
#define FILE_VERSION_STR        "4.5.1.1123"
#define PRODUCT_VERSION         FILE_VERSION
#define PRODUCT_VERSION_STR     FILE_VERSION_STR
#define INTERNAL_NAME           "YZNICaS"
#define ORIGINAL_FILE_NAME      "YZNICaS.exe"
#define ORGANIZATION_DOMAIN     "www.zeyaotebco.com"

#ifdef _WIN32
    #define APP_ID              "92FB908B-79F3-4B31-9499-5E57A7239285"
#elif linux
    #define APP_ID              "92FB908B-79F3-4B31-9499-5E57A7239285"
#endif

#endif // VERSION_H
