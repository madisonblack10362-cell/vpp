//--------------------------------------------------------------------------------------------------------------
// DZM Flag Extension — добавляет UUID и экшен меню к флагштокам
//--------------------------------------------------------------------------------------------------------------

modded class TerritoryFlag
{
	protected string m_DZM_UUID = "";

	void TerritoryFlag()
	{
		Print("[DZM_TerritoryFlags] TerritoryFlag constructor called");
	}

	string DZM_GetUUID()
	{
		return m_DZM_UUID;
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_DZM_UUID);
	}

	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		string savedUUID;
		if (!super.OnStoreLoad(ctx, version)) return false;
		savedUUID = "";
		if (ctx.Read(savedUUID))
		{
			m_DZM_UUID = savedUUID;
		}
		if (GetGame().IsServer() && m_DZM_UUID.Length() > 0)
		{
			DZM_TerritoryManager.Get().RegisterFlag(m_DZM_UUID, GetPosition());
		}
		return true;
	}

	override void EEInit()
	{
		super.EEInit();
		Print("[DZM_TerritoryFlags] EEInit called for TerritoryFlag at " + GetPosition().ToString());
		if (GetGame().IsServer())
		{
			if (m_DZM_UUID.Length() == 0)
			{
				m_DZM_UUID = DZM_TerritoryManager.GenerateUUID();
			}
			DZM_TerritoryManager.Get().RegisterFlag(m_DZM_UUID, GetPosition());
		}
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		DZM_Cleanup();
	}

	void DZM_Cleanup()
	{
		if (GetGame().IsServer())
		{
			DZM_TerritoryManager.Get().UnregisterFlag(m_DZM_UUID);
		}
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionDZM_OpenMenu);
		Print("[DZM_TerritoryFlags] SetActions - ActionDZM_OpenMenu added to TerritoryFlag");
	}
};

//--------------------------------------------------------------------------------------------------------------
// Fallback: добавляем экшен через BaseBuildingBase на случай если другой мод
// перезаписывает TerritoryFlag.SetActions() без вызова super
//--------------------------------------------------------------------------------------------------------------

modded class BaseBuildingBase
{
	override void SetActions()
	{
		super.SetActions();
		if (IsInherited(TerritoryFlag))
		{
			AddAction(ActionDZM_OpenMenu);
			Print("[DZM_TerritoryFlags] SetActions - ActionDZM_OpenMenu added via BaseBuildingBase fallback");
		}
	}
};
