// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveVRPrivatePCH.h"
#include "HeadMountedDisplay.h"  // For LogHMD
#include "PoseManagerImp.h"

const WVR_DeviceType PoseManagerImp::DeviceTypes[WVR_DEVICE_COUNT_LEVEL_1] = {
    WVR_DeviceType_HMD,
    WVR_DeviceType_Controller_Right,
    WVR_DeviceType_Controller_Left
};

PoseManagerImp* PoseManagerImp::GetInstance()
{
    static PoseManagerImp* mInst = new PoseManagerImp();
    return mInst;
}

PoseManagerImp::Device* PoseManagerImp::GetDevice(WVR_DeviceType type)
{
    Device* device = nullptr;
    switch(type) {
        case WVR_DeviceType_HMD:
            device = hmd;
            break;
        case WVR_DeviceType_Controller_Right:
            device = controllerRight;
            break;
        case WVR_DeviceType_Controller_Left:
            device = controllerLeft;
            break;
    }
    return device;
}

PoseManagerImp::PoseManagerImp()
{
    UE_LOG(LogHMD, Log, TEXT("@@@@@PoseManager Constructure"));
    hmd = new Device(WVR_DeviceType_HMD);
    controllerLeft = new Device(WVR_DeviceType_Controller_Left);
    controllerRight = new Device(WVR_DeviceType_Controller_Right);
#if WAVEVR_SUPPORTED_PLATFORMS
    Hmddof = WVR_GetDegreeOfFreedom(WVR_DeviceType::WVR_DeviceType_HMD);
    Controllerdof = WVR_GetDegreeOfFreedom(WVR_DeviceType::WVR_DeviceType_Controller_Right);
#endif
}

PoseManagerImp::~PoseManagerImp()
{
    delete hmd;
    delete controllerLeft;
    delete controllerRight;
}

bool PoseManagerImp::IsDeviceConnected(WVR_DeviceType type) {
#if WAVEVR_SUPPORTED_PLATFORMS
    return WVR_IsDeviceConnected(type);
#else
    return true;
#endif
}

bool PoseManagerImp::IsDevicePoseValid(WVR_DeviceType type) {
#if WAVEVR_SUPPORTED_PLATFORMS
    WVR_PoseState_t poseState;
    WVR_GetPoseState(type, WVR_PoseOriginModel_OriginOnGround, 0, &poseState);
    return poseState.isValidPose;
#else
    return true;
#endif
}

int PoseManagerImp::GetNumOfDoF(WVR_DeviceType type) {
#if WAVEVR_SUPPORTED_PLATFORMS

    switch (type)
    {
        case WVR_DeviceType::WVR_DeviceType_HMD:
            if (Hmddof == WVR_NumDoF::WVR_NumDoF_6DoF)
                return 6;  // 6 DoF
            else if (Hmddof == WVR_NumDoF::WVR_NumDoF_3DoF)
                return 3;  // 3 DoF
	case WVR_DeviceType::WVR_DeviceType_Controller_Right:
	case WVR_DeviceType::WVR_DeviceType_Controller_Left:
            if (Controllerdof == WVR_NumDoF::WVR_NumDoF_6DoF)
                return 6;  // 6 DoF
            else if (Controllerdof == WVR_NumDoF::WVR_NumDoF_3DoF)
                return 3;  // 3 DoF
        default:
            return 0;// abnormal case
    }
#else
    return 3;
#endif
}

void PoseManagerImp::SetTrackingOriginPoses(EHMDTrackingOrigin::Type NewOrigin) {
    switch (NewOrigin)
        {
        case EHMDTrackingOrigin::Eye:
            Origin = WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnHead;
            break;
        case EHMDTrackingOrigin::Floor:
            Origin = WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnGround;
            break;
        }
}

EHMDTrackingOrigin::Type PoseManagerImp::GetTrackingOriginPoses() {
    EHMDTrackingOrigin::Type rv = EHMDTrackingOrigin::Eye;

    switch (Origin)
        {
        case WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnHead:
            rv = EHMDTrackingOrigin::Eye;
            break;
        case WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnGround:
            rv = EHMDTrackingOrigin::Floor;
            break;
        }

	return rv;
}

/*Update all poses and return the assigned device position and Orientation*/
void PoseManagerImp::UpdatePoses()
{
    FRenderCommandFence Fence_Pose; //Avoid GetSyncPose twice in one submit.
    //SCOPED_NAMED_EVENT(UpdetePoses, FColor::Orange);
    Fence_Pose.BeginFence();
    Fence_Pose.Wait();
    //check(bWVRInitialized);

    //Update Devices pose from runtime
#if WAVEVR_SUPPORTED_PLATFORMS
    WVR_GetSyncPose(Origin, currFrame.poses, WVR_DEVICE_COUNT_LEVEL_1);
#endif

    for (uint32_t i = 0; i < /*DeviceTypes.Length*/WVR_DEVICE_COUNT_LEVEL_1; i++)
    {
        bool _hasType = false;

        for (uint32_t j = 0; j < /*poses.Length*/WVR_DEVICE_COUNT_LEVEL_1; j++)
        {
            WVR_DevicePosePair_t _pose = currFrame.poses[j];

            if (_pose.type == DeviceTypes [i])
            {
                _hasType = true;
                currFrame.deviceIndexMap[i] = j;

                if (currFrame.connected [i] != _pose.pose.isValidPose)
                {
                    currFrame.connected [i] = _pose.pose.isValidPose; 
#if DEBUG
                    UE_LOG(LogHMD, Log, TEXT("device %d is connected? (%b)"), DeviceTypes[i], currFrame.connected[i]);
#endif
                }

                if (currFrame.connected[i])
                {
                    FVector LocalCurrentPosition;
                    FQuat LocalCurrentOrientation;
                    CoordinatTransform(_pose.pose.poseMatrix, LocalCurrentOrientation, LocalCurrentPosition);
                    currFrame.DeviceOrientation[i] = LocalCurrentOrientation;
                    currFrame.DevicePosition[i] = LocalCurrentPosition;
                }

                break;
            }
        }

        // no such type
        if (!_hasType)
        {
            if (currFrame.connected [i] == true)
            {
                currFrame.connected [i] = false;
#if DEBUG
                UE_LOG(LogHMD, Log, TEXT("device %d is disconnected?"), DeviceTypes[i]);
#endif
            }
        }
    }

    UpdateDevice(); //HMD , L-controller, R-controller
    PrintDeviceInfo();
}

void PoseManagerImp::CoordinatTransform(const WVR_Matrix4f& InPose, FQuat& OutOrientation, FVector& OutPosition) const {

	FMatrix Pose = ToFMatrix(InPose);
	FQuat Orientation(Pose);

	OutOrientation.X = -Orientation.Z;
	OutOrientation.Y = Orientation.X;
	OutOrientation.Z = Orientation.Y;
 	OutOrientation.W = -Orientation.W;

	OutOrientation.Normalize();

        FVector position = FVector(Pose.M[3][0], Pose.M[3][1], Pose.M[3][2]);
	OutPosition = CoordinatUtil::getVector3(position);
	//OutPosition = FVector(-Pose.M[3][2], Pose.M[3][0], Pose.M[3][1]);
}

void PoseManagerImp::UpdateDevice() {
    //1 m = 100cm.
    float POSITION_UNIT_FACTOR = 100.0;
    Device* devices[3] = {hmd, controllerLeft, controllerRight};
    for (auto dev : devices) {
        check(dev);
        dev->connected = currFrame.connected[dev->index];
        dev->pose = currFrame.poses[currFrame.deviceIndexMap[dev->index]];
        dev->orientation = currFrame.DeviceOrientation[dev->index];
        dev->rotation = currFrame.DeviceOrientation[dev->index].Rotator();
        dev->position = currFrame.DevicePosition[dev->index] * POSITION_UNIT_FACTOR;
    }
}

void PoseManagerImp::PrintDeviceInfo() {
#if DEBUG
    Device* devices[3] = {hmd, controllerLeft, controllerRight};
    for(auto dev : devices) {
        if (dev == hmd) {
            UE_LOG(LogHMD, Log, TEXT("HMD Info: "));
        } else if(dev == controllerLeft){
            UE_LOG(LogHMD, Log, TEXT("Left Controller Info: "));
        } else if(dev == controllerRight){
            UE_LOG(LogHMD, Log, TEXT("Right Controller Info: "));
        }
        if(dev->connected){
            UE_LOG(LogHMD, Log, TEXT("Device is connected"));
        }else{
            UE_LOG(LogHMD, Log, TEXT("Device is NOT connected"));
        }
        UE_LOG(LogHMD, Log, TEXT("Orientation (W,X,Y,Z) is (%f, %f, %f, %f)"),dev->orientation.W, dev->orientation.X, dev->orientation.Y, dev->orientation.Z);
        UE_LOG(LogHMD, Log, TEXT("Position(X,Y,Z) is (%f, %f, %f)"), dev->position.X, dev->position.Y, dev->position.Z);
        UE_LOG(LogHMD, Log, TEXT("Rotator(Pitch,Roll,Yaw) is(%f, %f, %f)"), dev->rotation.Pitch, dev->rotation.Roll, dev->rotation.Yaw);
    }
#endif
}
