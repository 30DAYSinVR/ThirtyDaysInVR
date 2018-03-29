// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "WaveVRHMD.h"
#include "WaveVRBlueprintFunctionLibrary.h"

UWaveVRBlueprintFunctionLibrary::UWaveVRBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

WVR_DeviceType GetType(EWVR_DeviceType Type)
{
    WVR_DeviceType ResType = WVR_DeviceType_HMD;
    switch(Type) {
        case EWVR_DeviceType::DeviceType_HMD:
            ResType = WVR_DeviceType_HMD;
            break;
        case EWVR_DeviceType::DeviceType_Controller_Right:
            ResType = WVR_DeviceType_Controller_Right;
            break;
        case EWVR_DeviceType::DeviceType_Controller_Left:
            ResType = WVR_DeviceType_Controller_Left;
            break;
    }
    return ResType;
}

bool UWaveVRBlueprintFunctionLibrary::GetDevicePose(FVector& OutPosition, FRotator& OutOrientation, EWVR_DeviceType Type) {

    PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
    PoseManagerImp::Device* device = PoseMngr->GetDevice(GetType(Type));
    bool bIsPoseValid = device->pose.pose.isValidPose;
    OutPosition = bIsPoseValid ? device->position : FVector::ZeroVector;
    OutOrientation = bIsPoseValid ? device->rotation : FRotator::ZeroRotator;

    return bIsPoseValid;
}

FVector UWaveVRBlueprintFunctionLibrary::GetDeviceVelocity(EWVR_DeviceType type)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	PoseManagerImp::Device* device = PoseMngr->GetDevice(GetType(type));
	WVR_Vector3f_t velocity = device->pose.pose.velocity;
	return FVector(velocity.v[0], velocity.v[1], velocity.v[2]);
}

FVector UWaveVRBlueprintFunctionLibrary::GetDeviceAngularVelocity(EWVR_DeviceType type)
{
	PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
	PoseManagerImp::Device* device = PoseMngr->GetDevice(GetType(type));
	WVR_Vector3f_t angularv = device->pose.pose.angularVelocity;
	return FVector(angularv.v[0], angularv.v[1], angularv.v[2]);
}

bool UWaveVRBlueprintFunctionLibrary::IsDevicePoseValid(EWVR_DeviceType Type)
{
    PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
    return PoseMngr->IsDevicePoseValid(GetType(Type));
}

int UWaveVRBlueprintFunctionLibrary::GetNumOfDoF(EWVR_DeviceType Type)
{
    PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
    return PoseMngr->GetNumOfDoF(GetType(Type));
}

bool UWaveVRBlueprintFunctionLibrary::IsDeviceConnected(EWVR_DeviceType Type)
{
    PoseManagerImp* PoseMngr = PoseManagerImp::GetInstance();
    return PoseMngr->IsDeviceConnected(GetType(Type));
}

bool UWaveVRBlueprintFunctionLibrary::IsInputFocusCapturedBySystem()
{
#if WAVEVR_SUPPORTED_PLATFORMS
    return WVR_IsInputFocusCapturedBySystem();
#else
    return false;
#endif
}

float UWaveVRBlueprintFunctionLibrary::getDeviceBatteryPercentage(EWVR_DeviceType type) {
	WVR_DeviceType device = WVR_DeviceType::WVR_DeviceType_HMD;
	float result = -1.0f;
	switch (type) {
	case EWVR_DeviceType::DeviceType_HMD:
		device = WVR_DeviceType::WVR_DeviceType_HMD;
		break;
	case EWVR_DeviceType::DeviceType_Controller_Right:
		device = WVR_DeviceType::WVR_DeviceType_Controller_Right;
		break;
	case EWVR_DeviceType::DeviceType_Controller_Left:
		device = WVR_DeviceType::WVR_DeviceType_Controller_Left;
		break;
	}
	if (FWaveVRHMD::IsWVRInitialized()) {
#if WAVEVR_SUPPORTED_PLATFORMS
		result = WVR_GetDeviceBatteryPercentage(device);
#endif
	}
	return result;
}

bool UWaveVRBlueprintFunctionLibrary::GetRenderTargetSize(FIntPoint & OutSize)
{
	LOG_FUNC();

	uint32 width = 1600, height = 900;
	if (FWaveVRHMD::IsWVRInitialized()) {
#if WAVEVR_SUPPORTED_PLATFORMS
		WVR_GetRenderTargetSize(&width, &height);
#endif
		OutSize.X = width;
		OutSize.Y = height;
		return true;
	} else {
		return false;
	}
}

FString UWaveVRBlueprintFunctionLibrary::GetRenderModelName(EWVR_Hand hand) {
    FString retString = FString(TEXT("Generic"));
#if WAVEVR_SUPPORTED_PLATFORMS
    char *str = new char[30];
    FString str_name = FString(TEXT("GetRenderModelName")); //Controller getParameter
    if (hand == EWVR_Hand::Hand_Controller_Right) {
        uint32_t num2 = WVR_GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_ANSI(*str_name), str, 30);
    }
    else {
        uint32_t num2 = WVR_GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_ANSI(*str_name), str, 30);
    }

    retString = FString(ANSI_TO_TCHAR(str));
#endif

    return retString;
}

AActor * UWaveVRBlueprintFunctionLibrary::LoadCustomControllerModel(EWVR_DeviceType device, EWVR_DOF dof, FTransform transform) {
    // auto cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/WaveVR/Blueprints/Controller_BP_3DoF_Finch"));
    // asyc load asset (materials, texture ...)
    //auto cls = StaticLoadObject(UObject::StaticClass(), nullptr, TEXT("/WaveVR/Materials/Touch_Icon_Sprite"));
    FString GenericControllerModel = FString(TEXT("/WaveVR/Blueprints/WVR_GENERIC_CONTROLLER"));
    FString ControllerName = FString(TEXT(""));
    char *str = new char[30];
    FString str_name = FString(TEXT("GetRenderModelName")); //Controller getParameter
    FString BPClassName = FString(TEXT("/WaveVR/Blueprints/"));
#if WAVEVR_SUPPORTED_PLATFORMS
    if (device == EWVR_DeviceType::DeviceType_Controller_Right) {
        uint32_t num2 = WVR_GetParameters(WVR_DeviceType_Controller_Right, TCHAR_TO_ANSI(*str_name), str, 30);
    }
    else if (device == EWVR_DeviceType::DeviceType_Controller_Left) {
        uint32_t num2 = WVR_GetParameters(WVR_DeviceType_Controller_Left, TCHAR_TO_ANSI(*str_name), str, 30);
    }

    ControllerName.Append(FString(ANSI_TO_TCHAR(str)));

    if (dof == EWVR_DOF::DOF_3) {
        ControllerName.Append(FString(TEXT("_3DOF_MC_")));
    }
    else if (dof == EWVR_DOF::DOF_6) {
        ControllerName.Append(FString(TEXT("_6DOF_MC_")));
    }
    else {
        WVR_NumDoF tmp = WVR_NumDoF::WVR_NumDoF_3DoF;
        if (device == EWVR_DeviceType::DeviceType_Controller_Right) {
            tmp = WVR_GetDegreeOfFreedom(WVR_DeviceType_Controller_Right);
        }
        else if (device == EWVR_DeviceType::DeviceType_Controller_Left) {
            tmp = WVR_GetDegreeOfFreedom(WVR_DeviceType_Controller_Left);
        }
        if (tmp == WVR_NumDoF::WVR_NumDoF_3DoF) {
            ControllerName.Append(FString(TEXT("_3DOF_MC_")));
        }
        else {
            ControllerName.Append(FString(TEXT("_6DOF_MC_")));
        }
    }

    if (device == EWVR_DeviceType::DeviceType_Controller_Right) {
        ControllerName.Append(FString(TEXT("R")));
    }
    else if (device == EWVR_DeviceType::DeviceType_Controller_Left) {
        ControllerName.Append(FString(TEXT("L")));
    }

    FString tmpName = ControllerName;

    ControllerName.Append(FString(TEXT(".")));
    ControllerName.Append(tmpName);
    ControllerName.Append(FString(TEXT("_C")));
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *ControllerName);
    UE_LOG(LogTemp, Warning, TEXT("ControllerName = %s"), *ControllerName);

    BPClassName.Append(ControllerName);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *BPClassName);
    UE_LOG(LogTemp, Warning, TEXT("BPClassName = %s"), *BPClassName);
#else
    GenericControllerModel.Append(FString(TEXT("_3DOF_MC_R")));
    FString tmpName = GenericControllerModel;
    GenericControllerModel.Append(FString(TEXT(".")));
    GenericControllerModel.Append(tmpName);
    GenericControllerModel.Append(FString(TEXT("_C")));
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *GenericControllerModel);
    UE_LOG(LogTemp, Warning, TEXT("ControllerName = %s"), *GenericControllerModel);

    BPClassName.Append(GenericControllerModel);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, *BPClassName);
    UE_LOG(LogTemp, Warning, TEXT("BPClassName = %s"), *BPClassName);
#endif
    //load blueprints
    //UClass* cls = LoadObject< UClass >(nullptr, TEXT("/WaveVR/Blueprints/Controller_BP_3DoF_Finch.Controller_BP_3DoF_Finch_C"));

    UClass* cls1 = LoadObject< UClass >(nullptr, *BPClassName);

    if (cls1 != nullptr) {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("cls1 loaded "));
        UE_LOG(LogTemp, Warning, TEXT("UWaveVRBlueprintFunctionLibrary::BeginPlay load model BP"));
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("cls1 is null "));
        UE_LOG(LogTemp, Warning, TEXT("cls1 is null"));
        return nullptr;
    }

    //UBlueprint * bp = Cast<UBlueprint>(cls);
    /*
    if (!bp) {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Failed to load UClass 2  "));
    return nullptr;
    }
    else {
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UClass LOADED!!!! 2 " + bp->GetName()));
    }*/

    //TSubclassOf<class UObject> MyItemBlueprint;

    //MyItemBlueprint = (UClass*)bp->GeneratedClass;
    UWorld* const World = GWorld->GetWorld();
    UObject* SpawnObject = nullptr;
    AActor* SpawnActor = nullptr;
    if (World) {
        FActorSpawnParameters SpawnParams;
        //SpawnParams.Instigator = this;
        SpawnActor = World->SpawnActor<AActor>(cls1, transform.GetLocation(), transform.Rotator(), SpawnParams);
        //SpawnObject = World->SpawnActor<UObject>(cls, { 0,0,0 }, { 0,0,0 }, SpawnParams);
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("NO WORLD!!!!"));
    }
    return SpawnActor;
}
