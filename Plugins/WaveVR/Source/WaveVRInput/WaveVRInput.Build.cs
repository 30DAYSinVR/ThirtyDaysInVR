// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class WaveVRInput : ModuleRules
    {
        public WaveVRInput(ReadOnlyTargetRules Target) : base(Target)
        {
            var EngineDir = Path.GetFullPath(BuildConfiguration.RelativeEnginePath);
            var openglDrvPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\OpenGLDrv\Private");
            var openglPath = Path.Combine(EngineDir, @"Source\ThirdParty\OpenGL");
            var rendererPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\Renderer\Private");

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "WaveVR",
                    "WVR",
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "InputCore",
                    "InputDevice",
                    "HeadMountedDisplay",
                    "OpenGLDrv"
                });

            PrivateIncludePaths.AddRange(
                new string[] {
                    "WaveVR/Private",
                    openglDrvPrivatePath,
                    openglPath,
                    rendererPrivatePath
                });
            if(UEBuildConfiguration.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }
        }
    }
}
