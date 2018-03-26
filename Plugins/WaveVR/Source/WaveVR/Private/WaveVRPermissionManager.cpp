// Fill out your copyright notice in the Description page of Project Settings.
/**
* Created by devinyu on 2017/9/27.
*/

#include "WaveVRPrivatePCH.h"
#include "WaveVRPermissionManager.h"

#if WAVEVR_SUPPORTED_PLATFORMS
#include "AndroidApplication.h"
#include "AndroidJNI.h"
#include <android/log.h>

#define L_TAG "WVR_PermissionMgr"
extern JavaVM* mJavaVM;
extern JNIEnv* JavaENV;
extern jclass mJavaclazz;
extern jobject mJavaObj;
#endif

UWaveVRPermissionManager* UWaveVRPermissionManager::mInstance;

#pragma region Class UWaveVRPermissionManager
UWaveVRPermissionManager::UWaveVRPermissionManager()
{
#if WAVEVR_SUPPORTED_PLATFORMS
    __android_log_print(ANDROID_LOG_ERROR, L_TAG, "UWaveVRPermissionManager constructor");
#endif
	isProcessing = false;
}

UWaveVRPermissionManager* UWaveVRPermissionManager::getInstance() {
	if (mInstance == nullptr) {
		//mInstance = new UWaveVRPermissionManager();
		mInstance = NewObject<UWaveVRPermissionManager>(UWaveVRPermissionManager::StaticClass());
	}

	return mInstance;
}

URequestResultObject* UWaveVRPermissionManager::requestPermissions(TArray<FString> permissions)
{
#if WAVEVR_SUPPORTED_PLATFORMS
    __android_log_print(ANDROID_LOG_ERROR, L_TAG, "requestPermissions ttttttttttt");
#endif
	return getInstance()->requestPermissionsInternal(permissions);
}

URequestResultObject* UWaveVRPermissionManager::requestPermissionsInternal(TArray<FString> permissions)
{
    URequestResultObject* mResultObject = NewObject<URequestResultObject>(URequestResultObject::StaticClass());
    mResultObject->setResult(false);
#if WAVEVR_SUPPORTED_PLATFORMS
	__android_log_print(ANDROID_LOG_ERROR, L_TAG, "requestPermissionsInternal");
	if (isProcessing) return mResultObject;

	if (!AndroidJavaENVCheck()) {
		__android_log_print(ANDROID_LOG_ERROR, L_TAG, "ERROR: AndroidJavaENVCheck fails ");
		return mResultObject;
	}

	isProcessing = true;

	jmethodID methodid = JavaENV->GetMethodID(mJavaclazz, "requestPermissions", "([Ljava/lang/String;)Z");

	if (!methodid)
	{
		__android_log_print(ANDROID_LOG_ERROR, L_TAG, "ERROR: requestPermissions Method cant be found T_T ");
		return mResultObject;
	}
    UE_LOG(LogTemp, Warning, TEXT("URequestResultObject receiveSignal"));
    mResultObject->receiveSignal();
	int len = permissions.Num();
	jstring str;
	jobjectArray permissionArray = 0;
	permissionArray = JavaENV->NewObjectArray(len, JavaENV->FindClass("java/lang/String"), 0);

	for (int i = 0; i < len; i++)
	{
		str = JavaENV->NewStringUTF(TCHAR_TO_ANSI(*permissions[i]));//JavaENV->NewStringUTF(sa[i]);
		JavaENV->SetObjectArrayElement(permissionArray, i, str);
	}
	bool ret = JavaENV->CallBooleanMethod(mJavaObj, methodid, permissionArray);

	__android_log_print(ANDROID_LOG_INFO, L_TAG, "requestPermissionsInternal, ret = %d", ret);
    mResultObject->setResult(true);
	return mResultObject;
#else
    UE_LOG(LogHMD, Log, TEXT("requestPermissionsInternal"));
    return mResultObject;
#endif
}

bool UWaveVRPermissionManager::isPermissionGranted(FString permission)
{
#if WAVEVR_SUPPORTED_PLATFORMS
    __android_log_print(ANDROID_LOG_ERROR, L_TAG, "requestPermissions");
#endif
	return getInstance()->isPermissionGrantedInternal(permission);
}

bool UWaveVRPermissionManager::isPermissionGrantedInternal(FString permission)
{
#if WAVEVR_SUPPORTED_PLATFORMS
	__android_log_print(ANDROID_LOG_ERROR, L_TAG, "isPermissionGrantedInternal");

	if (!AndroidJavaENVCheck()) {
		__android_log_print(ANDROID_LOG_ERROR, L_TAG, "ERROR: AndroidJavaENVCheck fails ");
		return false;
	}
	jmethodID methodid = JavaENV->GetMethodID(mJavaclazz, "isPermissionGranted", "(Ljava/lang/String;)Z");

	if (!methodid)
	{
		__android_log_print(ANDROID_LOG_ERROR, L_TAG, "ERROR: isPermissionGranted Method cant be found T_T ");
		return false;
	}

	bool ret = JavaENV->CallBooleanMethod(mJavaObj, methodid, JavaENV->NewStringUTF(TCHAR_TO_ANSI(*permission)));

	__android_log_print(ANDROID_LOG_ERROR, L_TAG, "isPermissionGrantedInternal, ret = %d", ret);

	AndroidJavaENVFinish();

	return ret;
#else
    UE_LOG(LogHMD, Log, TEXT("isPermissionGrantedInternal"));
    return false;
#endif
}

bool UWaveVRPermissionManager::shouldGrantPermissionInternal(FString permission)
{
#if WAVEVR_SUPPORTED_PLATFORMS
	__android_log_print(ANDROID_LOG_ERROR, L_TAG, "shouldGrantPermissionInternal");

	if (!AndroidJavaENVCheck()) {
		__android_log_print(ANDROID_LOG_ERROR, L_TAG, "ERROR: AndroidJavaENVCheck fails ");
		return false;
	}
	jmethodID methodid = JavaENV->GetMethodID(mJavaclazz, "shouldGrantPermission", "(Ljava/lang/String;)Z");

	if (!methodid)
	{
		__android_log_print(ANDROID_LOG_ERROR, L_TAG, "ERROR: shouldGrantPermission Method cant be found T_T ");
		return false;
	}

	bool ret = JavaENV->CallBooleanMethod(mJavaObj, methodid, JavaENV->NewStringUTF(TCHAR_TO_ANSI(*permission)));

	__android_log_print(ANDROID_LOG_ERROR, L_TAG, "shouldGrantPermissionInternal, ret = %d", ret);

	AndroidJavaENVFinish();

	return ret;
#else
    UE_LOG(LogHMD, Log, TEXT("shouldGrantPermissionInternal"));
    return false;
#endif
}

bool UWaveVRPermissionManager::shouldGrantPermission(FString permission)
{
#if WAVEVR_SUPPORTED_PLATFORMS
    __android_log_print(ANDROID_LOG_ERROR, L_TAG, "requestPermissions");
#endif
	return getInstance()->shouldGrantPermissionInternal(permission);
}

#if WAVEVR_SUPPORTED_PLATFORMS
bool UWaveVRPermissionManager::AndroidJavaENVCheck() {
	if (!mJavaclazz || !mJavaObj || !JavaENV || !mJavaVM) {
		__android_log_print(ANDROID_LOG_ERROR, L_TAG, "Cant find Java class/method.");
		return false;
	}

	mJavaAttached = false;
	switch (mJavaVM->GetEnv((void**)&JavaENV, JNI_VERSION_1_6)) {
	case JNI_OK:
        //__android_log_print(ANDROID_LOG_DEBUG, L_TAG, "GetEnv is JNI_OK");
		break;
	case JNI_EDETACHED:
        //__android_log_print(ANDROID_LOG_DEBUG, L_TAG, "GetEnv is JNI_EDETACHED");
		if (mJavaVM->AttachCurrentThread(&JavaENV, nullptr) != JNI_OK) {
            __android_log_print(ANDROID_LOG_ERROR, L_TAG, "failed to attach current thread");
			return false;
		}
		mJavaAttached = true;
		break;
	case JNI_EVERSION:
        //__android_log_print(ANDROID_LOG_DEBUG, L_TAG, "GetEnv is JNI_EVERSION");
		break;
	}

	return true;
}

void UWaveVRPermissionManager::AndroidJavaENVFinish() {
	if (mJavaAttached) {
		mJavaVM->DetachCurrentThread();
	}
}
#endif
#pragma endregion

