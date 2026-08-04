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
	private const string SAVE_FILE = "$profile:TerritoryFlags.json";

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
		data.TerritoryName = ownerName + "'s Base";
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

	//------------------------------------------------------------------------------------------------------------------
	// JSON сохранение/загрузка
	//------------------------------------------------------------------------------------------------------------------
	void Save()
	{
		FileHandle fh = OpenFile(SAVE_FILE, FileMode.WRITE);
		if (!fh) return;
		FPrintln(fh, "{\"territories\": [");
		for (int i = 0; i < m_Territories.Count(); i++)
		{
			ref TerritoryData td = m_Territories[i];
			string posStr = td.Position[0].ToString() + "," + td.Position[1].ToString() + "," + td.Position[2].ToString();
			string invitedStr = "";
			for (int j = 0; j < td.InvitedIDs.Count(); j++)
			{
				if (j > 0) invitedStr += "|";
				invitedStr += td.InvitedIDs[j] + ":" + td.InvitedNames[j];
			}
			FPrint(fh, "  {\"flagID\":\"" + td.FlagID + "\",");
			FPrint(fh, "\"ownerID\":\"" + td.OwnerID + "\",");
			FPrint(fh, "\"ownerName\":\"" + td.OwnerName + "\",");
			FPrint(fh, "\"pos\":\"" + posStr + "\",");
			FPrint(fh, "\"radius\":" + td.Radius.ToString() + ",");
			FPrint(fh, "\"territoryName\":\"" + td.TerritoryName + "\",");
			FPrint(fh, "\"invited\":\"" + invitedStr + "\",");
			FPrint(fh, "\"active\":" + td.Active.ToString() + "}");
			if (i < m_Territories.Count() - 1) FPrintln(fh, ",");
			else FPrintln(fh, "");
		}
		FPrintln(fh, "]}");
		CloseFile(fh);
	}

	void Load()
	{
		m_Territories.Clear();
		string line;
		FileHandle fh = OpenFile(SAVE_FILE, FileMode.READ);
		if (!fh) { Print("[TerritoryFlags] No save file, starting fresh"); return; }
		string content = "";
		while (FGets(fh, line) >= 0)
			content += line;
		CloseFile(fh);
		// Простой парс JSON вручную (без внешних библиотек)
		parseTerritoriesJSON(content);
		Print("[TerritoryFlags] Loaded " + m_Territories.Count() + " territories");
	}

	private void parseTerritoriesJSON(string json)
	{
		// Убираем пробелы и переносы для простоты парсинга
		json.Replace(" ", "");
		json.Replace("\n", "");
		json.Replace("\r", "");

		int territoriesStart = json.IndexOf("[{  }");
		// Ищем начало массива territories
		int arrStart = json.IndexOf("[");
		int arrEnd = json.LastIndexOf("]");
		if (arrStart == -1 || arrEnd == -1) return;

		string arrContent = json.Substring(arrStart + 1, arrEnd - arrStart - 1);

		// Парсим каждый объект { ... }
		int pos = 0;
		while (pos < arrContent.Length())
		{
			int objStart = arrContent.IndexOf("{", pos);
			if (objStart == -1) break;
			int objEnd = arrContent.IndexOf("}", objStart);
			if (objEnd == -1) break;

			string obj = arrContent.Substring(objStart + 1, objEnd - objStart - 1);
			ref TerritoryData td = parseOneTerritory(obj);
			if (td) m_Territories.Insert(td);

			pos = objEnd + 1;
		}
	}

	private TerritoryData parseOneTerritory(string obj)
	{
		ref TerritoryData td = new TerritoryData();

		td.FlagID = extractJSONString(obj, "flagID");
		td.OwnerID = extractJSONString(obj, "ownerID");
		td.OwnerName = extractJSONString(obj, "ownerName");
		td.TerritoryName = extractJSONString(obj, "territoryName");

		string posStr = extractJSONString(obj, "pos");
		td.Position = parseVector(posStr);

		td.Radius = parseFloat(extractJSONString(obj, "radius"));
		if (td.Radius <= 0) td.Radius = 100.0;

		td.Active = extractJSONString(obj, "active") == "true" || extractJSONString(obj, "active") == "1";

		string invitedStr = extractJSONString(obj, "invited");
		if (invitedStr.Length() > 0)
		{
			array<string> pairs = new array<string>;
			invitedStr.Split("|", pairs);
			foreach (string pair : pairs)
			{
				array<string> parts = new array<string>;
				pair.Split(":", parts);
				if (parts.Count() >= 2)
				{
					td.InvitedIDs.Insert(parts[0]);
					td.InvitedNames.Insert(parts[1]);
				}
			}
		}

		if (td.FlagID.Length() > 0 && td.OwnerID.Length() > 0)
			return td;
		return null;
	}

	private string extractJSONString(string json, string key)
	{
		string search = "\"" + key + "\":\"";
		int start = json.IndexOf(search);
		if (start == -1)
		{
			// Попробуем без кавычек (для чисел и bool)
			search = "\"" + key + "\":";
			start = json.IndexOf(search);
			if (start == -1) return "";
			start += search.Length();
			int end = start;
			while (end < json.Length() && json.Get(end) != ',' && json.Get(end) != '}') end++;
			return json.Substring(start, end - start);
		}
		start += search.Length();
		int end = json.IndexOf("\"", start);
		if (end == -1) return "";
		return json.Substring(start, end - start);
	}

	private vector parseVector(string str)
	{
		array<string> parts = new array<string>;
		str.Split(",", parts);
		if (parts.Count() >= 3)
			return Vector(parseFloat(parts[0]), parseFloat(parts[1]), parseFloat(parts[2]));
		return "0 0 0";
	}

	//------------------------------------------------------------------------------------------------------------------
	// Получить список всех территорий (для отладки/админки)
	//------------------------------------------------------------------------------------------------------------------
	int GetTerritoryCount() { return m_Territories.Count(); }

	array<ref TerritoryData> GetAllTerritories()
	{
		return m_Territories;
	}
}
