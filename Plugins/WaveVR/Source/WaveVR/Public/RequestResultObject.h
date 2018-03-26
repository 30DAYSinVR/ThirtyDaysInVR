// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RequestResultObject.generated.h"
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelToBP, TArray<FString>, requestPerArr, TArray<bool>, resultArr);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelNative, TArray<FString>, requestPerArr, TArray<bool>, resultArr);
//void receiveRequestResultFromNative(TArray<FString> requestPerArr, TArray<bool> resultArr);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelNative, const TArray<FString>&, requestPerArr, const TArray<bool>&, resultArr);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRequestResultDelToBP, const TArray<FString>&, requestPerArr, const TArray<bool>&, resultArr);

/**
 * 
 */
UCLASS(ClassGroup = (WaveVR))
class WAVEVR_API URequestResultObject : public UObject
{
	GENERATED_BODY()
	
public:
    // Sets default values for this component's properties
    URequestResultObject();
    virtual ~URequestResultObject();

    UFUNCTION()
    void receiveRequestResultFromNative(const TArray<FString>& requestPerArr, const TArray<bool>& resultArr);

    UFUNCTION()
    void receiveSignal();
    UFUNCTION()
    void removeSignal();

    static FRequestResultDelNative onRequestResultNative;

    void setResult(bool result);

    UPROPERTY(BlueprintAssignable, Category = "WaveVR|Permission")
    FRequestResultDelToBP onRequestResult;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WaveVR|Permission")
    bool Result;
};
