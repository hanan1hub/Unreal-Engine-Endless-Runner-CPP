using UnrealBuildTool;
using System.Collections.Generic;

public class CPP_EndlessRunnerEditorTarget : TargetRules
{
    public CPP_EndlessRunnerEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;

        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
        CppStandard = CppStandardVersion.Cpp20;

        ExtraModuleNames.Add("CPP_EndlessRunner");
    }
}
