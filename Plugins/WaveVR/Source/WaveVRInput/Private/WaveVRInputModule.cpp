// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "WaveVRInput.h"
#include "WaveVRInputModule.h"
#include "IWaveVRPlugin.h"

//-------------------------------------------------------------------------------------------------
// FWaveVRInputModule
//-------------------------------------------------------------------------------------------------


void FWaveVRInputModule::StartupModule()
{
    IInputDeviceModule::StartupModule();
    UE_LOG(LogWaveVRInput, Log, TEXT("StartupModule"));
}


TSharedPtr< class IInputDevice > FWaveVRInputModule::CreateInputDevice( const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler )
{
	if (IWaveVRPlugin::IsAvailable())
	{
                return TSharedPtr< class IInputDevice >(new FWaveVRInput(InMessageHandler));
	}
	else
	{
		UE_LOG(LogWaveVRInput, Log, TEXT("WaveVRInput module is enabled, but WaveVRHMD module is not available."));
	}
	return nullptr;
}
IMPLEMENT_MODULE( FWaveVRInputModule, WaveVRInput )
