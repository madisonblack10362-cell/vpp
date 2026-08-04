//------------------------------------------------------------------------------------------------
// TerritoryFlag — моддed класс ванильного строительного флага
//------------------------------------------------------------------------------------------------

modded class Land_Construction_Flag_Floor
{
	protected string m_TerritoryOwnerID;
	protected string m_TerritoryOwnerName;
	protected bool   m_TerritoryClaimed = false;

	// m_TerritoryUUID — persistent ID территории, сохраняется в CE storage.
	// Присваивается при клейме (равен networkID на момент клейма).
	// Нужен потому что networkID меняется после рестарта сервера.
	protected string m_TerritoryUUID;

	//------------------------------------------------------------------------------------------------------------------
	// Получить persistent ID территории (для TerritoryManager)
	//------------------------------------------------------------------------------------------------------------------
	string GetFlagTerritoryID()
	{
		if (m_TerritoryUUID.Length() > 0) return m_TerritoryUUID;
		// Для неклейменых флагов — текущий networkID
		return GetNetworkID().ToString();
	}

	//------------------------------------------------------------------------------------------------------------------
	// Получить текущий networkID как строку (для RPC протокола)
	//------------------------------------------------------------------------------------------------------------------
	string GetFlagNetID()
	{
		return GetNetworkID().ToString();
	}

	//------------------------------------------------------------------------------------------------------------------
	// Заявить территорию
	//------------------------------------------------------------------------------------------------------------------
	bool ClaimTerritory(PlayerBase player)
	{
		if (m_TerritoryClaimed) return false;
		if (!player || !player.GetIdentity()) return false;

		string steamID = player.GetIdentity().GetPlainId();
		string name = player.GetIdentity().GetName();

		// Проверяем что позиция не внутри чужой территории
		TerritoryManager tm = TerritoryManager.GetInstance();
		if (!tm.CanBuild(GetPosition(), steamID))
		{
			string blocker = tm.GetBlockerOwnerName(GetPosition(), steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 4.0, "Территория",
					"Нельзя захватить тут! Территория: " + blocker,
					"set:dayz_gui icon");
			return false;
		}

		// Сохраняем persistent UUID (текущий networkID) для survivals через рестарт
		m_TerritoryUUID = GetNetworkID().ToString();
		m_TerritoryOwnerID = steamID;
		m_TerritoryOwnerName = name;
		m_TerritoryClaimed = true;

		tm.RegisterTerritory(m_TerritoryUUID, steamID, name, GetPosition(), 100.0);

		NotificationSystem.SendNotificationToPlayerIdentityExtended(
				player.GetIdentity(), 4.0, "Территория",
				"Территория захвачена! Радиус: 100м",
				"" );

		Print("[TerritoryFlags] Flag claimed by " + name + " UUID=" + m_TerritoryUUID);
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	// Получить данные территории этого флага
	//------------------------------------------------------------------------------------------------------------------
	TerritoryData GetTerritoryData()
	{
		return TerritoryManager.GetInstance().GetTerritoryByFlag(GetFlagTerritoryID());
	}

	//------------------------------------------------------------------------------------------------------------------
	// CE сохранение — сохраняем UUID чтобы пересоединить после рестарта
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
		if (!super.OnStoreLoad(ctx, version)) return false;
		if (!ctx.Read(m_TerritoryClaimed)) return false;
		if (!ctx.Read(m_TerritoryOwnerID)) return false;
		if (!ctx.Read(m_TerritoryOwnerName)) return false;

		// m_TerritoryUUID может отсутствовать в старых сохранениях
		string savedUUID = "";
		if (ctx.Read(savedUUID))
		{
			m_TerritoryUUID = savedUUID;
		}

		if (m_TerritoryClaimed && m_TerritoryOwnerID.Length() > 0)
		{
			TerritoryManager tm = TerritoryManager.GetInstance();
			string currentNetID = GetNetworkID().ToString();

			if (m_TerritoryUUID.Length() > 0)
			{
				// Пересоединяем: ищем территорию по старому UUID, обновляем на новый networkID
				TerritoryData td = tm.GetTerritoryByFlag(m_TerritoryUUID);
				if (td)
				{
					tm.UpdateFlagID(m_TerritoryUUID, currentNetID, GetPosition());
					m_TerritoryUUID = currentNetID;
					Print("[TerritoryFlags] Reconnected territory " + m_TerritoryOwnerName);
				}
				else
				{
					// JSON потерян — перерегистрируем с дефолтами
					tm.RegisterTerritory(currentNetID, m_TerritoryOwnerID, m_TerritoryOwnerName, GetPosition());
					m_TerritoryUUID = currentNetID;
					Print("[TerritoryFlags] Save lost, re-registered territory for " + m_TerritoryOwnerName);
				}
			}
			else
			{
				// Старое сохранение без UUID — перерегистрируем
				tm.RegisterTerritory(currentNetID, m_TerritoryOwnerID, m_TerritoryOwnerName, GetPosition());
				m_TerritoryUUID = currentNetID;
			}
		}
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	// При удалении флага — снять территорию
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
		if (m_TerritoryClaimed)
		{
			string fid = GetFlagTerritoryID();
			if (fid.Length() > 0)
			{
				TerritoryManager.GetInstance().UnregisterTerritory(fid);
				Print("[TerritoryFlags] Flag destroyed, territory removed: " + fid);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// Добавляем действие «Меню территории»
	//------------------------------------------------------------------------------------------------------------------
	override array<string> GetActions()
	{
		array<string> actions = super.GetActions();
		actions.Insert("ActionTerritoryMenu");
		return actions;
	}
}