public class UE52VFX_CrowBuild : ModuleRules
{
	public UE52VFX_CrowBuild(ReadOnlyTargetInfo Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputSystem", "EnhancedInput" });
	}
}