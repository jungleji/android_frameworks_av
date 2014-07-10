#ifndef _BLURAY_LOGD_

#define _BLURAY_LOGD_
#include<android/log.h>

#define LOG_TAG ""
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)

#endif
