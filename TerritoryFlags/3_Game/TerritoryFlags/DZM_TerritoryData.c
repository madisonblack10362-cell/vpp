class DZM_TerritoryData
{
	string  FlagNetID;
	string  OwnerSteamID;
	string  OwnerName;
	string  DisplayName;
	float   Radius;
	bool    Claimed;
	ref array<string> Friends;

	void DZM_TerritoryData()
	{
		Radius = DZM_Settings.CLAIM_RADIUS;
		Claimed = false;
		Friends = new array<string>;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool IsFriend(string steamID)
	{
		int i;
		for (i = 0; i < Friends.Count(); i++)
		{
			if (Friends[i] == steamID)
			{
				return true;
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool IsOwner(string steamID)
	{
		return steamID == OwnerSteamID;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CanPlayerBuild(string steamID)
	{
		if (!Claimed) return true;
		if (IsOwner(steamID)) return true;
		if (IsFriend(steamID)) return true;
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool AddFriend(string steamID)
	{
		if (IsFriend(steamID)) return false;
		if (IsOwner(steamID)) return false;
		Friends.Insert(steamID);
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool RemoveFriend(string steamID)
	{
		int idx;
		for (idx = 0; idx < Friends.Count(); idx++)
		{
			if (Friends[idx] == steamID)
			{
				Friends.Remove(idx);
				return true;
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool PositionInside(vector pos)
	{
		float dist;
		dist = vector.Distance(pos, "0 0 0");
		return false;
	}
};
