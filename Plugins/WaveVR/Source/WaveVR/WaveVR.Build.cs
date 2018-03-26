using System.IO;
using System.Diagnostics;

namespace UnrealBuildTool.Rules
{
    public class WaveVR : ModuleRules
    {
        public WaveVR(ReadOnlyTargetRules Target) : base(Target)
        {
            Debug.Print("Build the WaveVR Plugin");
            var EngineDir = Path.GetFullPath(BuildConfiguration.RelativeEnginePath);

            var openglDrvPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\OpenGLDrv\Private");
            var openglPath = Path.Combine(EngineDir, @"Source\ThirdParty\OpenGL");
            var rendererPrivatePath = Path.Combine(EngineDir, @"Source\Runtime\Renderer\Private");

            PublicIncludePathModuleNames.Add("Launch");

            PrivateIncludePaths.AddRange(
                new string[] {
                    openglDrvPrivatePath,
                    openglPath,
                    rendererPrivatePath, //***because its not build-in module now*** // "../../../../../Source/Runtime/Renderer/Private",
                    // ... add other private include paths required here ...
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "RHI",
                    "RenderCore",
                    "Renderer",
                    "ShaderCore",
                    "InputCore",
                    "HeadMountedDisplay",
                    "Slate",
                    "SlateCore",
                    "OpenGLDrv",
                    "WVR"
                }
            );
            if(UEBuildConfiguration.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }

            Definitions.Add("DEBUG=0");    // Adding your definition here, will add a global Preprocessor value for cpp

            AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenGL");

            if (Target.Platform == UnrealTargetPlatform.Android)
            {
                string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, BuildConfiguration.RelativeEnginePath);
                AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(PluginPath, "WaveVR_UPL.xml")));
            }
        }
    }
}
