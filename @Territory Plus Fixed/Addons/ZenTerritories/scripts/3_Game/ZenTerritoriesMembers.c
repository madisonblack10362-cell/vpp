class ZenTerritoryMembers
{
	ref map<string, int> m_Members;
		
	void ZenTerritoryMembers()
	{
		m_Members = new map<string, int>;
	}
	
	bool CheckId(string guid)
	{
		if (!m_Members)
		{
			m_Members = new map<string, int>;
		}

		if (m_Members.Contains(guid))
		{
			return true;
		}

		return false;
	}
	
	bool Check(string guid, int permission)
	{
		if (CheckId(guid))
		{
			int perms = m_Members.Get(guid);
			if (perms)
			{
				int CalcedPerms = perms & permission;
				if ( CalcedPerms == permission || perms == 1)
				{
					return true;
				}
			}
		} 

		return false;
	}
	
	bool AddMember(string guid, int permission = -1)
	{
		if (permission == -1)
		{
			permission = GetZenTerritoriesConfig().MemberPermission();
		}

		if (!CheckId(guid))
		{
			m_Members.Insert(guid, permission);
			return true;
		}

		return false;
	}
	
	bool RemoveMember(string guid)
	{
		if (CheckId(guid))
		{
			m_Members.Remove(guid);
			return true;
		}

		return false;
	}
	
	array<string> GetMemberArray()
	{
		if (!m_Members)
		{
			m_Members = new map<string, int>;;
		}

		return m_Members.GetKeyArray();
	}
	
	int Count()
	{
		if (!m_Members)
		{
			return 0;
		}

		return m_Members.Count();
	}
	
	void Clear()
	{
		if (!m_Members)
		{
			m_Members = new map<string, int>;
		} else 
		{
			m_Members.Clear();
		}
		
	}
}

class TerritoryConst
{
	static float FLAGDOWNSTATE = 0.9;
	static float FLAGUPSTATE = 0.2;
}