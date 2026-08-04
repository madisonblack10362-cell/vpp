#define _ARMA_

class CfgPatches
{
	class ZenUtilities
	{
		requiredVersion = 0.1;
		units[] = {""};
		requiredAddons[] = {"DZ_Data","DZ_Scripts","DZ_Gear_Books"};
	};
};
class CfgMods
{
	class ZenUtilities
	{
		dir = "ZenUtilities";
		picture = "";
		action = "";
		hideName = 1;
		hidePicture = 1;
		name = "ZenUtilities";
		credits = "Zenarchist";
		author = "Zenarchist";
		authorID = "0";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"ZenUtilities/scripts/3_game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"ZenUtilities/scripts/4_world"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"ZenUtilities/scripts/5_mission"};
			};
		};
	};
};
class CfgVehicles{};
