modded class ItemBase extends InventoryItem
{
	protected int m_MaxLifeTimeSynced;
	
	void ItemBase()
	{
		RegisterNetSyncVariableInt("m_MaxLifeTimeSynced");
	}

	protected void OnTServerInit()
	{
		m_MaxLifeTimeSynced = this.GetLifetimeMax();
		SetSynchDirty();
	}
	
	int GetTSyncedLifeTime()
	{
		if (GetGame().IsDedicatedServer())
		{
			return this.GetLifetimeMax();
		}

		return m_MaxLifeTimeSynced;
	}
	
	override void AfterStoreLoad()
    {
        super.AfterStoreLoad();
		if (GetGame().IsDedicatedServer())
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Call(this.OnTServerInit);
		}        
    }
}