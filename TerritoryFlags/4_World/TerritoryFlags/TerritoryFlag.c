//------------------------------------------------------------------------------------------------
// TerritoryFlag — моддed класс ванильного строительного флага
//------------------------------------------------------------------------------------------------

modded class Land_Construction_Flag_Floor
{
	protected string m_TerritoryOwnerID;
	protected string m_TerritoryOwnerName;
	protected bool   m_TerritoryClaimed = false;
	protected string m_TerritoryUUID;

	//------------------------------------------------------------------------------------------------------------------
	string GetFlagTerritoryID()
	{
		if (m_TerritoryUUID.Length() > 0) return m_TerritoryUUID;
		return GetNetworkID().ToString();
	}

	//------------------------------------------------------------------------------------------------------------------
	string GetFlagNetID()
	{
		return GetNetworkID().ToString();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool ClaimTerritory(PlayerBase player)
	{
		string steamID;
		string name;
		TerritoryManager tm;
		string blocker;

		if (m_TerritoryClaimed) return false;
		if (!player || !player.GetIdentity()) return false;

		steamID = player.GetIdentity().GetPlainId();
		name = player.GetIdentity().GetName();

		tm = TerritoryManager.GetInstance();
		if (!tm.CanBuild(GetPosition(), steamID))
		{
			blocker = tm.GetBlockerOwnerName(GetPosition(), steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
				player.GetIdentity(), 4.0, "Территория",
				"Нельзя захватить тут! Территория: " + blocker,
				"set:dayz_gui icon");
			return false;
		}

		m_TerritoryUUID = GetNetworkID().ToString();
		m_TerritoryOwnerID = steamID;
		m_TerritoryOwnerName = name;
		m_TerritoryClaimed = true;

		tm.RegisterTerritory(m_TerritoryUUID, steamID, name, GetPosition(), 100.0);

		NotificationSystem.SendNotificationToPlayerIdentityExtended(
			player.GetIdentity(), 4.0, "Территория",
			"Территория захвачена! Радиус: 100м",
			"set:dayz_gui icon");

		Print("[TerritoryFlags] Flag claimed by " + name + " UUID=" + m_TerritoryUUID);
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	TerritoryData GetTerritoryData()
	{
		return TerritoryManager.GetInstance().GetTerritoryByFlag(GetFlagTerritoryID());
	}

	//------------------------------------------------------------------------------------------------------------------
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_TerritoryClaimed);
		ctx.Write(m_TerritoryOwnerID);
		ctx.Write(m_TerritoryOwnerName);
		ctx.Write(m_TerritoryUUID);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		string savedUUID;
		TerritoryManager tm;
		string currentNetID;
		TerritoryData td;

		if (!super.OnStoreLoad(ctx, version)) return false;
		if (!ctx.Read(m_TerritoryClaimed)) return false;
		if (!ctx.Read(m_TerritoryOwnerID)) return false;
		if (!ctx.Read(m_TerritoryOwnerName)) return false;

		savedUUID = "";
		if (ctx.Read(savedUUID))
		{
			m_TerritoryUUID = savedUUID;
		}

		if (m_TerritoryClaimed && m_TerritoryOwnerID.Length() > 0)
		{
			tm = TerritoryManager.GetInstance();
			currentNetID = GetNetworkID().ToString();

			if (m_TerritoryUUID.Length() > 0)
			{
				td = tm.GetTerritoryByFlag(m_TerritoryUUID);
				if (td)
				{
					tm.UpdateFlagID(m_TerritoryUUID, currentNetID, GetPosition());
					m_TerritoryUUID = currentNetID;
					Print("[TerritoryFlags] Reconnected territory " + m_TerritoryOwnerName);
				}
				else
				{
					tm.RegisterTerritory(currentNetID, m_TerritoryOwnerID, m_TerritoryOwnerName, GetPosition());
					m_TerritoryUUID = currentNetID;
					Print("[TerritoryFlags] Save lost, re-registered for " + m_TerritoryOwnerName);
				}
			}
			else
			{
				tm.RegisterTerritory(currentNetID, m_TerritoryOwnerID, m_TerritoryOwnerName, GetPosition());
				m_TerritoryUUID = currentNetID;
			}
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		CleanupTerritory();
	}

	void EEDelete(EntityAI owner)
	{
		super.EEDelete(owner);
		CleanupTerritory();
	}

	protected void CleanupTerritory()
	{
		string fid;
		if (m_TerritoryClaimed)
		{
			fid = GetFlagTerritoryID();
			if (fid.Length() > 0)
			{
				TerritoryManager.GetInstance().UnregisterTerritory(fid);
				Print("[TerritoryFlags] Flag destroyed, territory removed: " + fid);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	override array<string> GetActions()
	{
		array<string> actions;
		actions = super.GetActions();
		actions.Insert("ActionTerritoryMenu");
		return actions;
	}
}
