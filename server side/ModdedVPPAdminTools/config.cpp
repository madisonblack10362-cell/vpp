class CfgPatches
{
	class ModdedVPPAdminTools
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZM_VPPAdminToolsScripts"
		};
	};
};
class CfgMods
{
	class ModdedVPPAdminTools_mods
	{
		dir="ModdedVPPAdminTools";
		name="ModdedVPPAdminTools";
		author="Sladya";
		version="1";
		type="mod";
		dependencies[]=
		{
			"Game",
			"World"
		};
		class defs
		{
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"ModdedVPPAdminTools/scripts/4_World"
				};
			};
		};
	};
};
