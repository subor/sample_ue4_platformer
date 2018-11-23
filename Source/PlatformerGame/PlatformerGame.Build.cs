// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class PlatformerGame : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string LibPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "RuyiSDK/lib/")); }
	}

	public PlatformerGame(ReadOnlyTargetRules Target) : base(Target)
	{
		bUseRTTI = true;
		bEnableExceptions = true;

		//PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		Definitions.Add("BOOST_ALL_NO_LIB");
		//Definitions.Add("BOOST_ALL_DYN_LINK");

		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore", 
				   "GameMenuBuilder",
				"Json",
				//"DesktopPlatform",
				"ImageWrapper",
				"RHI",
				"RenderCore",
				"UMG",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"PlatformerGameLoadingScreen",
			}
		); 

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Slate",
				"SlateCore",
			}
		);

		PublicIncludePaths.AddRange(
			new string[] 
			{
				"PlatformerGame/RuyiSDK/include",
				"PlatformerGame/RuyiSDK/include/Generated/CommonType",
				// ... add public include paths required here ...
			}
			);

		PrivateIncludePaths.AddRange(new string[] { 
			"PlatformerGame/Private/UI/Menu",
			//"PlatformerGame/include",
			});

		PublicAdditionalLibraries.Add(Path.Combine(LibPath, "RuyiSDK.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibPath, "zmq", "libzmq.lib"));

		PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_chrono-vc141-mt-1_64.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_date_time-vc141-mt-1_64.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_system-vc141-mt-1_64.lib"));
		PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "libboost_thread-vc141-mt-1_64.lib"));
		// PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "boost_chrono-vc141-mt-1_64.lib"));
		// PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "boost_date_time-vc141-mt-1_64.lib"));
		// PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "boost_system-vc141-mt-1_64.lib"));
		// PublicAdditionalLibraries.Add(Path.Combine(LibPath, "boost", "boost_thread-vc141-mt-1_64.lib"));

		PrivateIncludePathModuleNames.AddRange(
			new string[]
			{
				"DesktopPlatform",
			}
			);
	}
}
