//------------------------------------------------------------------------------------------------
// TerritoryData — данные одной территории
//------------------------------------------------------------------------------------------------

class TerritoryData
{
	string  FlagID;
	string  OwnerID;
	string  OwnerName;
	vector  Position;
	float   Radius;
	ref array<string> InvitedIDs  = new array<string>;
	ref array<string> InvitedNames = new array<string>;
	string  TerritoryName;
	bool    Active;

	void TerritoryData() { Radius = 100.0; Active = true; }

	bool IsPlayerAllowed(string steamID)
	{
		if (steamID == OwnerID) return true;
		return InvitedIDs.Find(steamID) != -1;
	}

	bool IsOwner(string steamID) { return steamID == OwnerID; }

	bool InvitePlayer(string steamID, string name)
	{
		int idx;
		idx = InvitedIDs.Find(steamID);
		if (idx != -1) return false;
		InvitedIDs.Insert(steamID);
		InvitedNames.Insert(name);
		return true;
	}

	bool RemoveInvite(string steamID)
	{
		int idx;
		idx = InvitedIDs.Find(steamID);
		if (idx == -1) return false;
		InvitedIDs.Remove(idx);
		InvitedNames.Remove(idx);
		return true;
	}

	bool IsInTerritory(vector pos)
	{
		return vector.Distance(pos, Position) <= Radius;
	}
}

//------------------------------------------------------------------------------------------------
// TerritoryManager — серверный синглтон
//------------------------------------------------------------------------------------------------

class TerritoryManager
{
	private static ref TerritoryManager s_Instance;
	private ref array<ref TerritoryData> m_Territories = new array<ref TerritoryData>;
	private const string SAVE_FILE = "$profile:TerritoryFlags.dat";

	static TerritoryManager GetInstance()
	{
		if (!s_Instance) s_Instance = new TerritoryManager();
		return s_Instance;
	}

	//------------------------------------------------------------------------------------------------------------------
	TerritoryData RegisterTerritory(string flagID, string ownerID, string ownerName, vector pos, float radius = 100.0)
	{
		int i;
		ref TerritoryData data;

		for (i = 0; i < m_Territories.Count(); i++)
		{
			if (m_Territories[i].FlagID == flagID) return m_Territories[i];
		}

		data = new TerritoryData();
		data.FlagID = flagID;
		data.OwnerID = ownerID;
		data.OwnerName = ownerName;
		data.Position = pos;
		data.Radius = radius;
		data.TerritoryName = ownerName + " - База";
		data.Active = true;
		m_Territories.Insert(data);
		Save();
		Print("[TerritoryFlags] Registered: " + flagID + " owner=" + ownerName);
		return data;
	}

	//------------------------------------------------------------------------------------------------------------------
	void UnregisterTerritory(string flagID)
	{
		int i;
		for (i = 0; i < m_Territories.Count(); i++)
		{
			if (m_Territories[i].FlagID == flagID)
			{
				m_Territories.Remove(i);
				Save();
				Print("[TerritoryFlags] Removed: " + flagID);
				return;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void UpdateFlagID(string oldID, string newID, vector newPos)
	{
		int i;
		for (i = 0; i < m_Territories.Count(); i++)
		{
			if (m_Territories[i].FlagID == oldID)
			{
				m_Territories[i].FlagID = newID;
				m_Territories[i].Position = newPos;
				Print("[TerritoryFlags] Updated flagID: " + oldID + " -> " + newID);
				return;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CanBuild(vector pos, string steamID)
	{
		int i;
		for (i = 0; i < m_Territories.Count(); i++)
		{
			if (m_Territories[i].Active && m_Territories[i].IsInTerritory(pos) && !m_Territories[i].IsPlayerAllowed(steamID))
				return false;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	TerritoryData GetTerritoryByFlag(string flagID)
	{
		int i;
		for (i = 0; i < m_Territories.Count(); i++)
		{
			if (m_Territories[i].FlagID == flagID) return m_Territories[i];
		}
		return null;
	}

	//------------------------------------------------------------------------------------------------------------------
	string GetBlockerOwnerName(vector pos, string steamID)
	{
		int i;
		for (i = 0; i < m_Territories.Count(); i++)
		{
			if (m_Territories[i].Active && m_Territories[i].IsInTerritory(pos) && !m_Territories[i].IsPlayerAllowed(steamID))
				return m_Territories[i].TerritoryName;
		}
		return "";
	}

	//------------------------------------------------------------------------------------------------------------------
	bool InvitePlayer(string flagID, string inviterID, string targetID, string targetName)
	{
		TerritoryData td;
		td = GetTerritoryByFlag(flagID);
		if (!td || !td.IsOwner(inviterID)) return false;
		return td.InvitePlayer(targetID, targetName);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool RemoveInvite(string flagID, string removerID, string targetID)
	{
		TerritoryData td;
		td = GetTerritoryByFlag(flagID);
		if (!td || !td.IsOwner(removerID)) return false;
		return td.RemoveInvite(targetID);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool SetTerritoryName(string flagID, string steamID, string name)
	{
		TerritoryData td;
		td = GetTerritoryByFlag(flagID);
		if (!td || !td.IsOwner(steamID)) return false;
		td.TerritoryName = name;
		Save();
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool SetRadius(string flagID, string steamID, float radius)
	{
		TerritoryData td;
		td = GetTerritoryByFlag(flagID);
		if (!td || !td.IsOwner(steamID)) return false;
		td.Radius = Math.Clamp(radius, 20.0, 300.0);
		Save();
		return true;
	}

	//==================================================================================================================
	// СОХРАНЕНИЕ / ЗАГРУЗКА
	//==================================================================================================================

	//------------------------------------------------------------------------------------------------------------------
	void Save()
	{
		FileHandle fh;
		int i;
		int j;
		ref TerritoryData td;
		string activeStr;

		fh = OpenFile(SAVE_FILE, FileMode.WRITE);
		if (!fh)
		{
			Print("[TerritoryFlags] ERROR: Cannot open save file: " + SAVE_FILE);
			return;
		}

		FPrintln(fh, "#TerritoryFlags V1");

		for (i = 0; i < m_Territories.Count(); i++)
		{
			td = m_Territories[i];
			FPrintln(fh, "TERRITORY");
			FPrintln(fh, td.FlagID);
			FPrintln(fh, td.OwnerID);
			FPrintln(fh, td.OwnerName);
			FPrintln(fh, td.TerritoryName);
			FPrintln(fh, td.Position[0].ToString() + "," + td.Position[1].ToString() + "," + td.Position[2].ToString());
			if (td.Active)
				activeStr = "1";
			else
				activeStr = "0";
			FPrintln(fh, activeStr);
			FPrintln(fh, td.Radius.ToString());
			FPrintln(fh, td.InvitedIDs.Count().ToString());
			for (j = 0; j < td.InvitedIDs.Count(); j++)
			{
				FPrintln(fh, td.InvitedIDs[j] + "\t" + td.InvitedNames[j]);
			}
			FPrintln(fh, "NEXT");
		}

		CloseFile(fh);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Load()
	{
		FileHandle fh;
		ref array<string> lines;
		string line;
		int idx;
		ref TerritoryData td;

		m_Territories.Clear();

		fh = OpenFile(SAVE_FILE, FileMode.READ);
		if (!fh)
		{
			Print("[TerritoryFlags] No save file, starting fresh");
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

		if (lines[0] != "#TerritoryFlags V1")
		{
			Print("[TerritoryFlags] ERROR: Unknown save file version");
			return;
		}

		idx = 1;
		while (idx < lines.Count())
		{
			if (lines[idx] == "TERRITORY")
			{
				idx++;
				td = parseTerritoryBlock(lines, idx);
				if (td)
				{
					m_Territories.Insert(td);
				}
			}
			else
			{
				idx++;
			}
		}

		Print("[TerritoryFlags] Loaded " + m_Territories.Count() + " territories");
	}

	//------------------------------------------------------------------------------------------------------------------
	private TerritoryData parseTerritoryBlock(array<string> lines, inout int idx)
	{
		ref TerritoryData td;
		string posStr;
		int invCount;
		int i;
		int tabIdx;
		string sid;
		string sname;

		if (idx + 7 > lines.Count()) return null;

		td = new TerritoryData();
		td.FlagID        = lines[idx]; idx++;
		td.OwnerID       = lines[idx]; idx++;
		td.OwnerName     = lines[idx]; idx++;
		td.TerritoryName = lines[idx]; idx++;

		posStr = lines[idx]; idx++;
		td.Position = parseVector(posStr);

		td.Radius = lines[idx].ToFloat();
		if (td.Radius <= 0) td.Radius = 100.0;
		idx++;

		if (lines[idx] == "1")
			td.Active = true;
		else
			td.Active = false;
		idx++;

		if (idx >= lines.Count()) return td;
		invCount = lines[idx].ToInt();
		idx++;

		for (i = 0; i < invCount && idx < lines.Count(); i++)
		{
			if (lines[idx] == "NEXT") break;

			tabIdx = lines[idx].IndexOf("\t");
			if (tabIdx > 0)
			{
				sid = lines[idx].Substring(0, tabIdx);
				sname = lines[idx].Substring(tabIdx + 1);
				td.InvitedIDs.Insert(sid);
				td.InvitedNames.Insert(sname);
			}
			idx++;
		}

		if (idx < lines.Count() && lines[idx] == "NEXT") idx++;

		if (td.FlagID.Length() > 0 && td.OwnerID.Length() > 0)
			return td;

		return null;
	}

	//------------------------------------------------------------------------------------------------------------------
	private vector parseVector(string str)
	{
		array<string> parts;
		parts = new array<string>;
		str.Split(",", parts);
		if (parts.Count() >= 3)
			return Vector(parts[0].ToFloat(), parts[1].ToFloat(), parts[2].ToFloat());
		return "0 0 0";
	}

	//------------------------------------------------------------------------------------------------------------------
	int GetTerritoryCount() { return m_Territories.Count(); }

	array<ref TerritoryData> GetAllTerritories()
	{
		return m_Territories;
	}
}