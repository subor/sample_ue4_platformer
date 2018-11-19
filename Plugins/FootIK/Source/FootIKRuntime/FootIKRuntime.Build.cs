// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FootIKRuntime : ModuleRules
{
	public FootIKRuntime(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivatePCHHeaderFile = "Private/FootIKRuntimePrivatePCH.h";

        PublicDependencyModuleNames.AddRange(
			new string[] { 
				"Core", 
				"CoreUObject", 
				"Engine",
				"AnimGraphRuntime"
			}
			);
	}
}
