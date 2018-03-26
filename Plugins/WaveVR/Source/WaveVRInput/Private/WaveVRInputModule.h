// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "IWaveVRInputModule.h"
#include "IInputDevice.h"

//-------------------------------------------------------------------------------------------------
// FWaveVRInputModule
//-------------------------------------------------------------------------------------------------
class FWaveVRInputModule : public IWaveVRInputModule
{
	// IInputDeviceModule overrides
	virtual void StartupModule() override;
	virtual TSharedPtr< class IInputDevice > CreateInputDevice( const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler ) override;
};
