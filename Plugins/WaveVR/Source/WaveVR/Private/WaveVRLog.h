#pragma once
#include "WaveVRPrivatePCH.h"
#include "HeadMountedDisplay.h"

#define WAVEVR_LOG_ANDROID_LOG (PLATFORM_ANDROID)

#if WAVEVR_LOG_ANDROID_LOG
#include <android/Log.h>
#define LOGV(TAG, fmt, ...) __android_log_print(ANDROID_LOG_VERBOSE, #TAG, fmt, __VA_ARGS__)
#define LOGD(TAG, fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, #TAG, fmt, __VA_ARGS__)
#define LOGI(TAG, fmt, ...) __android_log_print(ANDROID_LOG_INFO, #TAG, fmt, __VA_ARGS__)
#define LOGW(TAG, fmt, ...) __android_log_print(ANDROID_LOG_WARN, #TAG, fmt, __VA_ARGS__)
#define LOGE(TAG, fmt, ...) __android_log_print(ANDROID_LOG_ERROR, #TAG, fmt, __VA_ARGS__)
#define USEWIDE(str) str
#else  // !WAVEVR_LOG_ANDROID_LOG
// fallback to UE_LOG
#define LOGV(CategoryName, Format, ...) UE_LOG(CategoryName, Verbose, TEXT(Format), __VA_ARGS__)
#define LOGD(CategoryName, Format, ...) UE_LOG(CategoryName, Display, TEXT(Format), __VA_ARGS__)
#define LOGI(CategoryName, Format, ...) UE_LOG(CategoryName, Log, TEXT(Format), __VA_ARGS__)
#define LOGW(CategoryName, Format, ...) UE_LOG(CategoryName, Warning, TEXT(Format), __VA_ARGS__)
#define LOGE(CategoryName, Format, ...) UE_LOG(CategoryName, Error, TEXT(Format), __VA_ARGS__)
#define USEWIDE(str) *FString(str)  // Performance may not good.
#endif

#define WAVEVR_LOG_SHOW_ALL_ENTRY  0
#define WAVEVR_LOG_ENTRY_RENDER    (WAVEVR_LOG_SHOW_ALL_ENTRY || 0)
#define WAVEVR_LOG_ENTRY_LIFECYCLE (WAVEVR_LOG_SHOW_ALL_ENTRY || 1)
#define WAVEVR_LOG_ENTRY_HMD       (WAVEVR_LOG_SHOW_ALL_ENTRY || 0)
#define WAVEVR_LOG_ENTRY_STEREO    (WAVEVR_LOG_SHOW_ALL_ENTRY || 0)
#define WAVEVR_LOG_ENTRY_SCENEVIEW (WAVEVR_LOG_SHOW_ALL_ENTRY || 0)

#ifndef __FUNCTION__
// The g++ __func__ is too long to read.
#define WVR_FUNCTION_STRING USEWIDE(__func__)
#else
#define WVR_FUNCTION_STRING USEWIDE(__FUNCTION__)
#endif

#if WAVEVR_LOG_SHOW_ALL_ENTRY && WAVEVR_SUPPORTED_PLATFORMS
#define LOG_FUNC() LOGD(LogHMD, "%s", WVR_FUNCTION_STRING);
#else
#define LOG_FUNC()
#endif
#define LOG_FUNC_IF(expr) do { constexpr decltype(expr) var = (expr); if (var) { LOGD(LogHMD, "%s", WVR_FUNCTION_STRING); } } while (0)
