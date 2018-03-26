// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "RequestResultObject.h"

FRequestResultDelNative URequestResultObject::onRequestResultNative;

URequestResultObject::URequestResultObject() {
    Result = false;
    UE_LOG(LogTemp, Warning, TEXT("URequestResultObject constructor"));
    //URequestResultObject::onRequestResultNative.AddDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}

URequestResultObject::~URequestResultObject() {
    //URequestResultObject::onRequestResultNative.RemoveDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}

void URequestResultObject::receiveRequestResultFromNative(const TArray<FString>& requestPerArr, const TArray<bool>& resultArr) {
    int size = requestPerArr.Num();
    UE_LOG(LogTemp, Warning, TEXT("receiveRequestResultFromNative in URequestResultObject ----- %d"), size);

    for (int i = 0; i < size; i++) {
        UE_LOG(LogTemp, Warning, TEXT("receiveRequestResultFromNative in URequestResultObject ----- %s %d"), *requestPerArr[i], resultArr[i]);
    }

    onRequestResult.Broadcast(requestPerArr, resultArr);
    removeSignal();
}

void URequestResultObject::setResult(bool result) {
    Result = result;
}

void URequestResultObject::receiveSignal() {
    URequestResultObject::onRequestResultNative.AddDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}

void URequestResultObject::removeSignal() {
    URequestResultObject::onRequestResultNative.RemoveDynamic(this, &URequestResultObject::receiveRequestResultFromNative);
}
