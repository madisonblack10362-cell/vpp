//--------------------------------------------------------------------------------------------------------------
// DZM_TerritoryManager
//--------------------------------------------------------------------------------------------------------------

class DZM_TerritoryManager
{
	private static ref DZM_TerritoryManager s_Instance;
	private ref map<string, ref DZM_TerritoryData> m_Data;
	private string m_SavePath;

	static DZM_TerritoryManager Get()
	{
		if (!s_Instance)
		{
			s_Instance = new DZM_TerritoryManager();
			s_Instance.Init();
		}
		return s_Instance;
	}

	void Init()
	{
		m_Data = new map<string, ref DZM_TerritoryData>;
		m_SavePath = "$profile:DZM_TerritoryFlags.sav";
		if (GetGame().IsServer())
		{
			LoadFromFile();
		}
		Print("[DZM_TerritoryFlags] Manager initialized, territories: " + m_Data.Count().ToString());
	}

	static string GenerateUUID()
	{
		int t;
		string s;
		t = GetGame().GetTime();
		s = t.ToString();
		s += "_";
		s += Math.RandomInt(0, 999999).ToString();
		return s;
	}

	DZM_TerritoryData GetByID(string id)
	{
		if (m_Data.Contains(id))
		{
			return m_Data.Get(id);
		}
		return null;
	}

	void RegisterFlag(string uuid, vector pos)
	{
		DZM_TerritoryData td;
		if (m_Data.Contains(uuid))
		{
			td = m_Data.Get(uuid);
			td.FlagPosition = pos;
			return;
		}
		td = new DZM_TerritoryData();
		td.FlagNetID = uuid;
		td.FlagPosition = pos;
		m_Data.Set(uuid, td);
	}

	void UnregisterFlag(string uuid)
	{
		if (m_Data.Contains(uuid))
		{
			m_Data.Remove(uuid);
			SaveToFile();
			Print("[DZM_TerritoryFlags] Unregistered flag: " + uuid);
		}
	}

	bool ClaimFlag(string uuid, string steamID, string playerName)
	{
		DZM_TerritoryData td;
		td = GetByID(uuid);
		if (!td) return false;
		if (td.Claimed) return false;
		td.OwnerSteamID = steamID;
		td.OwnerName = playerName;
		td.DisplayName = playerName + " - База";
		td.Claimed = true;
		td.Radius = DZM_Settings.CLAIM_RADIUS;
		SaveToFile();
		return true;
	}

	bool CheckBuildAllowed(vector pos, string steamID)
	{
		int i;
		array<string> keys;
		DZM_TerritoryData td;
		float dist;
		keys = m_Data.GetKeyArray();
		for (i = 0; i < keys.Count(); i++)
		{
			td = m_Data.Get(keys[i]);
			if (!td || !td.Claimed) continue;
			dist = vector.Distance(pos, td.FlagPosition);
			if (dist <= td.Radius)
			{
				if (!td.CanPlayerBuild(steamID))
				{
					return false;
				}
			}
		}
		return true;
	}

	string FindBlockerName(vector pos, string steamID)
	{
		int i;
		array<string> keys;
		DZM_TerritoryData td;
		float dist;
		keys = m_Data.GetKeyArray();
		for (i = 0; i < keys.Count(); i++)
		{
			td = m_Data.Get(keys[i]);
			if (!td || !td.Claimed) continue;
			dist = vector.Distance(pos, td.FlagPosition);
			if (dist <= td.Radius)
			{
				if (!td.CanPlayerBuild(steamID))
				{
					return td.DisplayName;
				}
			}
		}
		return "";
	}

	bool CheckFlagPlacementAllowed(vector pos)
	{
		int i;
		array<string> keys;
		DZM_TerritoryData td;
		float dist;
		keys = m_Data.GetKeyArray();
		for (i = 0; i < keys.Count(); i++)
		{
			td = m_Data.Get(keys[i]);
			if (!td || !td.Claimed) continue;
			dist = vector.Distance(pos, td.FlagPosition);
			if (dist <= td.Radius * 2)
			{
				return false;
			}
		}
		return true;
	}

	DZM_TerritoryData FindTerritoryAtPos(vector pos)
	{
		int i;
		array<string> keys;
		DZM_TerritoryData td;
		float dist;
		keys = m_Data.GetKeyArray();
		for (i = 0; i < keys.Count(); i++)
		{
			td = m_Data.Get(keys[i]);
			if (!td || !td.Claimed) continue;
			dist = vector.Distance(pos, td.FlagPosition);
			if (dist <= td.Radius)
			{
				return td;
			}
		}
		return null;
	}

	bool InviteFriend(string uuid, string ownerID, string friendID)
	{
		DZM_TerritoryData td;
		td = GetByID(uuid);
		if (!td) return false;
		if (!td.IsOwner(ownerID)) return false;
		bool result;
		result = td.AddFriend(friendID);
		if (result) SaveToFile();
		return result;
	}

	bool KickFriend(string uuid, string ownerID, string friendID)
	{
		DZM_TerritoryData td;
		td = GetByID(uuid);
		if (!td) return false;
		if (!td.IsOwner(ownerID)) return false;
		bool result;
		result = td.RemoveFriend(friendID);
		if (result) SaveToFile();
		return result;
	}

	bool ChangeName(string uuid, string ownerID, string newName)
	{
		DZM_TerritoryData td;
		td = GetByID(uuid);
		if (!td) return false;
		if (!td.IsOwner(ownerID)) return false;
		if (newName.Length() == 0) return false;
		td.DisplayName = newName;
		SaveToFile();
		return true;
	}

	bool ChangeRadius(string uuid, string ownerID, float newRadius)
	{
		DZM_TerritoryData td;
		td = GetByID(uuid);
		if (!td) return false;
		if (!td.IsOwner(ownerID)) return false;
		td.Radius = Math.Clamp(newRadius, DZM_Settings.MIN_RADIUS, DZM_Settings.MAX_RADIUS);
		SaveToFile();
		return true;
	}

	void SaveToFile()
	{
		FileHandle fh;
		int i;
		int j;
		array<string> keys;
		DZM_TerritoryData td;
		fh = OpenFile(m_SavePath, FileMode.WRITE);
		if (!fh) return;
		FPrintln(fh, "DZM_TF_V3");
		keys = m_Data.GetKeyArray();
		for (i = 0; i < keys.Count(); i++)
		{
			td = m_Data.Get(keys[i]);
			if (!td) continue;
			FPrintln(fh, "T");
			FPrintln(fh, td.FlagNetID);
			FPrintln(fh, td.FlagPosition[0].ToString());
			FPrintln(fh, td.FlagPosition[1].ToString());
			FPrintln(fh, td.FlagPosition[2].ToString());
			FPrintln(fh, td.OwnerSteamID);
			FPrintln(fh, td.OwnerName);
			FPrintln(fh, td.DisplayName);
			FPrintln(fh, td.Radius.ToString());
			if (td.Claimed)
			{
				FPrintln(fh, "1");
			}
			else
			{
				FPrintln(fh, "0");
			}
			FPrintln(fh, td.Friends.Count().ToString());
			for (j = 0; j < td.Friends.Count(); j++)
			{
				FPrintln(fh, td.Friends[j]);
			}
		}
		CloseFile(fh);
	}

	void LoadFromFile()
	{
		FileHandle fh;
		ref array<string> lines;
		string line;
		int idx;
		DZM_TerritoryData td;
		int friendCount;
		int fi;
		float px;
		float py;
		float pz;
		fh = OpenFile(m_SavePath, FileMode.READ);
		if (!fh)
		{
			Print("[DZM_TerritoryFlags] No save file, starting fresh");
			return;
		}
		lines = new array<string>;
		while (FGets(fh, line) >= 0)
		{
			line.Replace("\r", "");
			line.Replace("\n", "");
			lines.Insert(line);
		}
		CloseFile(fh);
		if (lines.Count() == 0) return;
		if (lines[0] != "DZM_TF_V3")
		{
			Print("[DZM_TerritoryFlags] Unknown save version, resetting");
			return;
		}
		idx = 1;
		while (idx < lines.Count())
		{
			if (lines[idx] == "T")
			{
				idx++;
				if (idx + 10 > lines.Count()) break;
				td = new DZM_TerritoryData();
				td.FlagNetID = lines[idx]; idx++;
				px = lines[idx].ToFloat(); idx++;
				py = lines[idx].ToFloat(); idx++;
				pz = lines[idx].ToFloat(); idx++;
				td.FlagPosition = Vector(px, py, pz);
				td.OwnerSteamID = lines[idx]; idx++;
				td.OwnerName = lines[idx]; idx++;
				td.DisplayName = lines[idx]; idx++;
				td.Radius = lines[idx].ToFloat();
				if (td.Radius <= 0) td.Radius = DZM_Settings.CLAIM_RADIUS;
				idx++;
				if (lines[idx] == "1")
				{
					td.Claimed = true;
				}
				else
				{
					td.Claimed = false;
				}
				idx++;
				friendCount = lines[idx].ToInt();
				idx++;
				for (fi = 0; fi < friendCount && idx < lines.Count(); fi++)
				{
					td.Friends.Insert(lines[idx]);
					idx++;
				}
				m_Data.Set(td.FlagNetID, td);
			}
			else
			{
				idx++;
			}
		}
		Print("[DZM_TerritoryFlags] Loaded " + m_Data.Count().ToString() + " territories");
	}

	static PlayerBase FindOnlinePlayer(string name)
	{
		array<Man> players;
		Man m;
		PlayerBase p;
		int i;
		string pName;
		if (name.Length() == 0) return null;
		players = new array<Man>;
		GetGame().GetPlayers(players);
		for (i = 0; i < players.Count(); i++)
		{
			m = players[i];
			p = PlayerBase.Cast(m);
			if (p && p.GetIdentity())
			{
				pName = p.GetIdentity().GetName();
				if (pName == name)
				{
					return p;
				}
			}
		}
		for (i = 0; i < players.Count(); i++)
		{
			m = players[i];
			p = PlayerBase.Cast(m);
			if (p && p.GetIdentity())
			{
				pName = p.GetIdentity().GetName();
				if (pName.Contains(name))
				{
					return p;
				}
			}
		}
		return null;
	}
};
