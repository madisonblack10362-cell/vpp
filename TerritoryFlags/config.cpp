class CfgPatches
{
	class DZM_TerritoryFlags
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};
class CfgMods
{
	class DZM_TerritoryFlags
	{
		dir="TerritoryFlags";
		picture="";
		action="";
		hideName=1;
		hidePicture=1;
		name="TerritoryFlags";
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
					"TerritoryFlags/1_Core"
				};
			};
			class gameScriptModule
			{
				value="";
				files[]=
			{
					"TerritoryFlags/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
			{
					"TerritoryFlags/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
			{
					"TerritoryFlags/5_Mission"
				};
			};
		};
	};
};
