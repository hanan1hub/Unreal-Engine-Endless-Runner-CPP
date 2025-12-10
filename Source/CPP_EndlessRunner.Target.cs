using UnrealBuildTool;
using System.Collections.Generic;

public class CPP_EndlessRunnerTarget : TargetRules
{
    public CPP_EndlessRunnerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;

        DefaultBuildSettings = BuildSettingsVersion.V6;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
        CppStandard = CppStandardVersion.Cpp20;

        ExtraModuleNames.Add("CPP_EndlessRunner");
    }
}
