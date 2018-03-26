// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "WaveVRPrivatePCH.h"

//Implement of UWaveVRPoseManager
class PoseManagerImp
{
  public:
    static const WVR_DeviceType DeviceTypes[WVR_DEVICE_COUNT_LEVEL_1];
    /*poses derived from WVR_GetSyncPose directly*/
    class RawFramePoses
    {
      public:
        RawFramePoses()
        {
            for (int i = 0; i < WVR_DEVICE_COUNT_LEVEL_1; i++)
            {
                connected[i] = false;
                deviceIndexMap[i] = 0;
                DeviceOrientation[i] = FQuat::Identity;
            }
            FMemory::Memzero(DevicePosition, WVR_DEVICE_COUNT_LEVEL_1 * sizeof(FVector));
        }

        WVR_DevicePosePair_t poses[WVR_DEVICE_COUNT_LEVEL_1]; // HMD, R, L controllers.
        bool connected [WVR_DEVICE_COUNT_LEVEL_1];
        int deviceIndexMap[WVR_DEVICE_COUNT_LEVEL_1];
        FVector DevicePosition[WVR_DEVICE_COUNT_LEVEL_1];
        FQuat DeviceOrientation[WVR_DEVICE_COUNT_LEVEL_1];
    };
    class Device
    {
      public:
        Device(WVR_DeviceType _type)
        {
            type = _type;
            for (int i = 0; i < WVR_DEVICE_COUNT_LEVEL_1; i++)
            {
                if (DeviceTypes[i] == type)
                {
                    index = i;
                    break;
                }
            }
            connected = false;
        }

        WVR_DeviceType type;
        int index;
        bool connected;
        WVR_DevicePosePair_t pose;
        FVector position;
        FQuat orientation;
        FRotator rotation;
    };
  public:
    static PoseManagerImp* GetInstance();
    virtual ~PoseManagerImp();
    void UpdatePoses(); //Keep update per game frame

  public:
    Device* GetDevice(WVR_DeviceType type);
    bool IsDeviceConnected(WVR_DeviceType type);
    bool IsDevicePoseValid(WVR_DeviceType type);
    int GetNumOfDoF(WVR_DeviceType type);
    void SetTrackingOriginPoses(EHMDTrackingOrigin::Type NewOrigin);
    EHMDTrackingOrigin::Type GetTrackingOriginPoses();

  private:
    PoseManagerImp(); //prevent new instance directly

    void CoordinatTransform(const WVR_Matrix4f_t& Pose, FQuat& OutOrientation, FVector& OutPosition) const;
    void UpdateDevice();
    void PrintDeviceInfo();
    //Transform due to col/raw major switch
    static FORCEINLINE FMatrix ToFMatrix(const WVR_Matrix4f_t& tm)
    {
        return FMatrix(
            FPlane(tm.m[0][0], tm.m[1][0], tm.m[2][0], tm.m[3][0]),
            FPlane(tm.m[0][1], tm.m[1][1], tm.m[2][1], tm.m[3][1]),
            FPlane(tm.m[0][2], tm.m[1][2], tm.m[2][2], tm.m[3][2]),
            FPlane(tm.m[0][3], tm.m[1][3], tm.m[2][3], tm.m[3][3]));
    }

  private:
    RawFramePoses currFrame;
    Device* hmd = nullptr;
    Device* controllerLeft = nullptr;
    Device* controllerRight = nullptr;
    WVR_PoseOriginModel Origin = WVR_PoseOriginModel::WVR_PoseOriginModel_OriginOnGround;
    WVR_NumDoF Hmddof = WVR_NumDoF::WVR_NumDoF_3DoF;
    WVR_NumDoF Controllerdof = WVR_NumDoF::WVR_NumDoF_3DoF;
};

class CoordinatUtil {
  public:
      static FVector2D getVector2(const FVector2D& wvr_axis) {
          FVector2D vec2D = FVector2D::ZeroVector;

          vec2D.X = -wvr_axis.X;
          vec2D.Y = wvr_axis.Y;
          return vec2D;
      }
      static FVector getVector3(const FVector& wvr_position) {
          FVector vec = FVector::ZeroVector;

          vec.X = - wvr_position.Z;
          vec.Y = wvr_position.X;
          vec.Z = wvr_position.Y;
          return vec;
      }
};
