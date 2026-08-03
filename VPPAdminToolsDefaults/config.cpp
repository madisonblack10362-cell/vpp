class CfgPatches
{
	class DZM_VPPAdminToolsDefaults
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts",
			"DZM_VPPAdminTools"
		};
	};
};
class CfgMods
{
	class DZM_VPPAdminToolsDefaults
	{
		dir="VPPAdminToolsDefaults";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="VPPAdminToolsDefaults";
		credits="";
		author="";
		authorID="0";
		version=1;
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Core",
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class engineScriptModule
			{
				value="";
				files[]=
				{
					"VPPAdminToolsDefaults/1_Core"
				};
			};
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"VPPAdminToolsDefaults/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"VPPAdminToolsDefaults/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"VPPAdminToolsDefaults/5_Mission"
				};
			};
		};
	};
};
