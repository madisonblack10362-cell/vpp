class CfgPatches
{
	class ZenTerritories
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]=
		{
			"JM_CF_Scripts"
		};
	};
};
class CfgMods
{
	class ZenTerritories
	{
		dir="ZenTerritories";
		name="ZenTerritories";
		credits="Deceased";
		author="DaemonForge/Zenarchist/StrykerX1/ReViQo";
		authorID="0";
		version="0.1";
		extra=0;
		type="mod";
		hideName=1;
		hidePicture=1;
		dependencies[]=
		{
			"Game",
			"World",
			"Mission"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"ZenTerritories/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value="";
				files[]=
				{
					"ZenTerritories/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value="";
				files[]=
				{
					"ZenTerritories/scripts/5_Mission"
				};
			};
		};
	};
};
