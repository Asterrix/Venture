// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class VentureEditorTarget : TargetRules
{
	public VentureEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
		ExtraModuleNames.Add("Venture");
	}
}
