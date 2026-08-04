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
		if (InvitedIDs.Find(steamID) != -1) return false;
		InvitedIDs.Insert(steamID);
		InvitedNames.Insert(name);
		return true;
	}

	bool RemoveInvite(string steamID)
	{
		int idx = InvitedIDs.Find(steamID);
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
// TerritoryManager — серверный синглтон, управляет всеми территориями
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
	// Регистрация
	//------------------------------------------------------------------------------------------------------------------
	TerritoryData RegisterTerritory(string flagID, string ownerID, string ownerName, vector pos, float radius = 100.0)
	{
		foreach (ref TerritoryData td : m_Territories)
		{
			if (td.FlagID == flagID) return td;
		}
		ref TerritoryData data = new TerritoryData();
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
		for (int i = 0; i < m_Territories.Count(); i++)
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
	// Переименовать flagID (нужно при рестарте сервера — networkID меняется)
	//------------------------------------------------------------------------------------------------------------------
	void UpdateFlagID(string oldID, string newID, vector newPos)
	{
		foreach (ref TerritoryData td : m_Territories)
		{
			if (td.FlagID == oldID)
			{
				td.FlagID = newID;
				td.Position = newPos;
				Print("[TerritoryFlags] Updated flagID: " + oldID + " -> " + newID);
				return;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CanBuild(vector pos, string steamID)
	{
		foreach (ref TerritoryData td : m_Territories)
		{
			if (td.Active && td.IsInTerritory(pos) && !td.IsPlayerAllowed(steamID))
				return false;
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	TerritoryData GetTerritoryByFlag(string flagID)
	{
		foreach (ref TerritoryData td : m_Territories)
		{
			if (td.FlagID == flagID) return td;
		}
		return null;
	}

	//------------------------------------------------------------------------------------------------------------------
	string GetBlockerOwnerName(vector pos, string steamID)
	{
		foreach (ref TerritoryData td : m_Territories)
		{
			if (td.Active && td.IsInTerritory(pos) && !td.IsPlayerAllowed(steamID))
				return td.TerritoryName;
		}
		return "";
	}

	//------------------------------------------------------------------------------------------------------------------
	bool InvitePlayer(string flagID, string inviterID, string targetID, string targetName)
	{
		TerritoryData td = GetTerritoryByFlag(flagID);
		if (!td || !td.IsOwner(inviterID)) return false;
		return td.InvitePlayer(targetID, targetName);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool RemoveInvite(string flagID, string removerID, string targetID)
	{
		TerritoryData td = GetTerritoryByFlag(flagID);
		if (!td || !td.IsOwner(removerID)) return false;
		return td.RemoveInvite(targetID);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool SetTerritoryName(string flagID, string steamID, string name)
	{
		TerritoryData td = GetTerritoryByFlag(flagID);
		if (!td || !td.IsOwner(steamID)) return false;
		td.TerritoryName = name;
		Save();
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool SetRadius(string flagID, string steamID, float radius)
	{
		TerritoryData td = GetTerritoryByFlag(flagID);
		if (!td || !td.IsOwner(steamID)) return false;
		td.Radius = Math.Clamp(radius, 20.0, 300.0);
		Save();
		return true;
	}

	//==================================================================================================================
	// СОХРАНЕНИЕ / ЗАГРУЗКА — свой формат, не JSON (чтобы не сломаться на пробелах и спецсимволах)
	//==================================================================================================================
	// Формат:
	//   #TerritoryFlags V1
	//   TERRITORY
	//   <flagID>        — по одной строке на поле
	//   <ownerID>
	//   <ownerName>     — может содержать пробелы, без перевода строки
	//   <territoryName>  — может содержать пробелы
	//   <posX,posY,posZ>
	//   <radius>
	//   <active: 0/1>
	//   <inviteCount>
	//   <steamID>\t<playerName>  — один инвайт на строку, TAB разделитель
	//   ... (inviteCount строк)
	//   NEXT

	//------------------------------------------------------------------------------------------------------------------
	void Save()
	{
		FileHandle fh = OpenFile(SAVE_FILE, FileMode.WRITE);
		if (!fh)
		{
			Print("[TerritoryFlags] ERROR: Cannot open save file for writing: " + SAVE_FILE);
			return;
		}

		FPrintln(fh, "#TerritoryFlags V1");

		for (int i = 0; i < m_Territories.Count(); i++)
		{
			ref TerritoryData td = m_Territories[i];
			FPrintln(fh, "TERRITORY");
			FPrintln(fh, td.FlagID);
			FPrintln(fh, td.OwnerID);
			FPrintln(fh, td.OwnerName);
			FPrintln(fh, td.TerritoryName);
			FPrintln(fh, td.Position[0].ToString() + "," + td.Position[1].ToString() + "," + td.Position[2].ToString());
			FPrintln(fh, td.Radius.ToString());
			FPrintln(fh, td.Active ? "1" : "0");
			FPrintln(fh, td.InvitedIDs.Count().ToString());
			for (int j = 0; j < td.InvitedIDs.Count(); j++)
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
		m_Territories.Clear();

		FileHandle fh = OpenFile(SAVE_FILE, FileMode.READ);
		if (!fh)
		{
			Print("[TerritoryFlags] No save file, starting fresh");
			return;
		}

		// Читаем все строки
		ref array<string> lines = new array<string>;
		string line;
		while (FGets(fh, line) >= 0)
		{
			// Убираем \r и \n
			line.Replace("\r", "");
			line.Replace("\n", "");
			lines.Insert(line);
		}
		CloseFile(fh);

		if (lines.Count() == 0) return;

		// Проверяем заголовок
		if (lines[0] != "#TerritoryFlags V1")
		{
			Print("[TerritoryFlags] ERROR: Unknown save file version");
			return;
		}

		// Парсим блоки TERRITORY ... NEXT
		int idx = 1;
		while (idx < lines.Count())
		{
			if (lines[idx] == "TERRITORY")
			{
				idx++;
				ref TerritoryData td = parseTerritoryBlock(lines, idx);
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
		// Читаем 7 обязательных полей
		if (idx + 7 > lines.Count()) return null;

		ref TerritoryData td = new TerritoryData();
		td.FlagID        = lines[idx]; idx++;
		td.OwnerID       = lines[idx]; idx++;
		td.OwnerName     = lines[idx]; idx++;
		td.TerritoryName = lines[idx]; idx++;

		string posStr = lines[idx]; idx++;
		td.Position = parseVector(posStr);

		td.Radius = lines[idx].ToFloat();
		if (td.Radius <= 0) td.Radius = 100.0;
		idx++;

		td.Active = (lines[idx] == "1");
		idx++;

		// Читаем инвайты
		if (idx >= lines.Count()) return td;
		int invCount = lines[idx].ToInt();
		idx++;

		for (int i = 0; i < invCount && idx < lines.Count(); i++)
		{
			if (lines[idx] == "NEXT") break;

			// Формат: steamID<TAB>playerName
			int tabIdx = lines[idx].IndexOf("\t");
			if (tabIdx > 0)
			{
				string sid = lines[idx].Substring(0, tabIdx);
				string sname = lines[idx].Substring(tabIdx + 1);
				td.InvitedIDs.Insert(sid);
				td.InvitedNames.Insert(sname);
			}
			idx++;
		}

		// Пропускаем маркер NEXT
		if (idx < lines.Count() && lines[idx] == "NEXT") idx++;

		if (td.FlagID.Length() > 0 && td.OwnerID.Length() > 0)
			return td;

		return null;
	}

	//------------------------------------------------------------------------------------------------------------------
	private vector parseVector(string str)
	{
		array<string> parts = new array<string>;
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
