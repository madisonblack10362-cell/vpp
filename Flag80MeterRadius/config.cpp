class CfgPatches
{
	class Flag80MeterRadius
	{
		units[]={};
		weapons[]={};
		requiredVersion=0.1;
		requiredAddons[]={};
	};
};
class CfgMods
{
	class Flag80MeterRadius
	{
		dir="Flag80MeterRadius";
		name="Flag80MeterRadius";
		credits="Deceased";
		author="DaemonForge";
		version="0.1";
		extra=0;
		type="mod";
		dependencies[]=
		{
			"Game"
		};
		class defs
		{
			class gameScriptModule
			{
				value="";
				files[]=
				{
					"Flag80MeterRadius/scripts/3_Game"
				};
			};
		};
	};
};
