class ZenTerritoriesConfig
{
	protected static string DirPATH = "$profile:Zenarchist";
	protected static string ConfigPATH = DirPATH + "\\ZenTerritoriesConfig.json";

	string ConfigVersion = "1";
	ref TStringArray WhiteList = 
	{ 
		"Trap",
		"Paper",
		"Fireplace",
		"WrittenNote",
		"ClaymoreMine",
		"PowerGenerator",
		"Plastic_Explosive",
		"WoodenCrate",
		"ShelterKit",
		"MediumTent",
		"LargeTent",
		"CarTent",
		"PartyTent"
	};
	ref TStringArray ServerAdmins = 
	{
		""
	}; // GUIDs
	
	float TentRadius = 0;
	float TentCESpawnLifeTime = 5600;
	float BuildBonusSledgeDamage = 300;
	bool RequireTerritory = false;
	int PublicPermissions = 280;
	int MemberPermissions = 1;
	int Notifications = 0; // 0 Chat | 1 Notifications Mod
	
	ref array< ref ZenTerritoriesNoBuildZones> NoBuildZones = new array< ref ZenTerritoriesNoBuildZones>;
	
	string NoBuildZoneMessage				= "You can't build here, you are trying to build in a designated no build zone!";
	string TerritoryConflictMessage			= "You can't build a territory this close to another territory!";
	string WithinTerritoryWarning			= "You can't build this close to an enemy territory!";
	string DeSpawnWarningMessage			= "You are building outside a territory, $ITEMNAME$ will despawn in $LIFETIME$ without a Territory Flag";
	string BuildPartWarningMessage			= "You don't have the right permissions to build in this area!";
	string DismantleWarningMessage			= "You can't dismantle anything this close to a raised flag!";
	string LowerFlagWarningMessage			= "You do not have permissions to lower the flag in this territory!";
	string TerritoryRequiredWarningMessage	= "Sorry, you are required to build a territory to be able to build";
	
	int FlagRefreshFrequency = 432000;
	
	ref map<string, int> KitLifeTimes = new map<string, int>;
	
	[NonSerialized()]
	protected int m_BlockWarnPlayer = 0;
	[NonSerialized()]
	protected string m_BlockLastMessage = "";
	[NonSerialized()]
	protected bool m_RaidHandlerLoaded = false;
	
	[NonSerialized()]
	int m_disableBaseDamage = 0;
	
	void Load()
	{
		Print("[ZenTerritories] Loading Config");
		if (GetGame().IsServer())
		{
			if (FileExist(ConfigPATH))
			{ //If config exist load File
			    JsonFileLoader<ZenTerritoriesConfig>.JsonLoadFile(ConfigPATH, this);
				if (ConfigVersion != "2")
				{
					ConfigVersion = "2";
					Save();
				}
			}
			else
			{	//File does not exist create file
				if (!FileExist(DirPATH))
				{
					MakeDirectory(DirPATH);
				}
				NoBuildZones.Insert(new ZenTerritoriesNoBuildZones(3703.5, 5985.11, 100));
				NoBuildZones.Insert(new ZenTerritoriesNoBuildZones(8345.61, 5985.93, 100));
				FlagRefreshFrequency = GetCEApi().GetCEGlobalInt("FlagRefreshFrequency");
				if (FlagRefreshFrequency <= 0)
				{
					FlagRefreshFrequency = GameConstants.REFRESHER_FREQUENCY_DEFAULT;
				}
				KitLifeTimes.Insert("fencekit", 3888000);
				KitLifeTimes.Insert("watchtowerkit", 3888000);
				KitLifeTimes.Insert("msp_", 3888000);
				KitLifeTimes.Insert("bbp_", 3888000);
				Save();
			}
		}
	}
	
	void Save()
	{
		JsonFileLoader<ZenTerritoriesConfig>.JsonSaveFile(ConfigPATH, this);
	}

	int disableBaseDamage()
	{
		return m_disableBaseDamage;
	}
	
	bool IsInWhiteList(string item)
	{
		if (WhiteList && WhiteList.Count() > 0)
		{
			for (int i = 0; i < WhiteList.Count(); i++)
			{
				item.ToLower();
				string wItem = WhiteList.Get(i);
				wItem.ToLower();
				if (item.Contains(wItem))
				{
					return true;
				}
			}
		}

		return false;
	}
	
	bool CanWarnPlayer(string message = "")
	{
		message.ToLower();
		int curTime = GetGame().GetTime();
		if ( curTime > m_BlockWarnPlayer || m_BlockLastMessage != message)
		{
			m_BlockLastMessage = message;
			m_BlockWarnPlayer = curTime + 6000;
			return true;
		}

		return false;
	}
	
	
	int PublicPermission()
	{
		return PublicPermissions;
	}
	
	int MemberPermission()
	{
		return MemberPermissions;
	}
	
	void SendNotification(string text, string icon = "ZenTerritories/images/NoBuild.paa")
	{
		if (GetGame().IsClient() && CanWarnPlayer(text))
		{
			if (Notifications == 0)
			{
				GetGame().Chat(text,"");
			} 

			#ifdef NOTIFICATIONS 
			else 
			if (Notifications == 1)
			{
				NotificationSystem.SimpleNoticiation(text, "Territory", icon, ARGB(230, 209, 60, 60), 8, NULL);
			}
			#endif
		}
	}
	
	bool CanBuildHere(vector pos, string item = "")
	{
		if (IsInWhiteList(item))
		{
			return true;
		}

		if (NoBuildZones)
		{
			for (int i = 0; i < NoBuildZones.Count(); i++)
			{
				if (NoBuildZones.Get(i) && NoBuildZones.Get(i).Check(pos))
				{
					return false;
				}
			}
		}

		return true;
	}
	
	string NiceExpireTime(float LifeTime)
	{
		float Hours = Math.Floor(LifeTime / 3600);
		
		Print("Hour: " + Hours + " RefreshFrequency:" + FlagRefreshFrequency);
		int rtnValue = 0;
		if (LifeTime < FlagRefreshFrequency)
		{
			return ""; // Means that this item wouldn't get affected by refresh anyway.
		}

		int Days = Math.Floor(Hours / 24);
		int remander = Days % 7;
		if (remander >= 2)
		{
			return Days.ToString() + " Days";
		}

		int Weeks = Math.Floor(Days / 7);
		return Weeks.ToString() + " Weeks";
	}
	
	int GetKitLifeTime(string item)
	{
		item.ToLower();
		int lt = KitLifeTimes.Get(item);
		if (lt == 0)
		{
			foreach (string key, int lifetime : KitLifeTimes)
			{
				if (item.Contains(key))
				{
					return lifetime;
				}
			}
		} else 
		{
			return lt;
		}

		return 0;
	}
}

class ZenTerritoriesNoBuildZones
{
	string Name = "";
	float X;
	float Z;
	float R;
	bool DrawOnMap = false;
	
	void ZenTerritoriesNoBuildZones(float x, float z, float r)
	{
		X = x;
		Z = z;
		R = r;
	}
	
	//Returns True If is in zone
	bool Check(vector checkPos)
	{
		if (checkPos){
			vector ZeroedHeightPos = Vector(checkPos[0], 0,checkPos[2]);
			if (vector.Distance(ZeroedHeightPos, Vector(X, 0, Z)) <= R)
			{
				return true;
			}
		}

		return false;
	}
	
	vector GetPos()
	{
		return Vector(X, GetGame().SurfaceY(X,Z),Z);
	}
}

ref ZenTerritoriesConfig m_ZenTerritoriesConfig;

//Helper function to return Config
static ZenTerritoriesConfig GetZenTerritoriesConfig()
{
	if (!m_ZenTerritoriesConfig)
	{
		m_ZenTerritoriesConfig = new ZenTerritoriesConfig;
			
		if ( GetGame().IsServer() ){
			m_ZenTerritoriesConfig.Load();
		}
	}

	return m_ZenTerritoriesConfig;
};