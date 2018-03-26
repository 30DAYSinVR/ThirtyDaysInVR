// Fill out your copyright notice in the Description page of Project Settings.
/**
* Created by devinyu on 2017/9/27.
*/

#pragma once

#include "CoreMinimal.h"
#include "RequestResultObject.h"
#include "UObject/NoExportTypes.h"
#include "WaveVRPermissionManager.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (WaveVR))
class WAVEVR_API UWaveVRPermissionManager : public UObject
{
	GENERATED_BODY()
	
public:
	UWaveVRPermissionManager();

    UFUNCTION(BlueprintCallable, Category = "WaveVR|Permission")
    static bool isPermissionGranted(FString permission);

	UFUNCTION(BlueprintCallable, Category = "WaveVR|Permission")
    static bool shouldGrantPermission(FString permission);

    UFUNCTION(BlueprintCallable, Category = "WaveVR|Permission")
    static URequestResultObject* requestPermissions(TArray<FString> permissions);

	static UWaveVRPermissionManager* getInstance();

private:
	bool isPermissionGrantedInternal(FString permission);
	bool shouldGrantPermissionInternal(FString permission);
    URequestResultObject* requestPermissionsInternal(TArray<FString> permissions);
	bool isProcessing;

#if WAVEVR_SUPPORTED_PLATFORMS
	bool AndroidJavaENVCheck();
	void AndroidJavaENVFinish();
	bool mJavaAttached;
#endif
	static UWaveVRPermissionManager* mInstance;
};
