modded class TentBase extends ItemBase
{
	override void EEOnCECreate() 
	{
		super.EEOnCECreate();
		this.SetLifetime(GetZenTerritoriesConfig().TentCESpawnLifeTime);
	}
	
	void ResetTentLifeTime()
	{
		float OldLifeTime = GetLifetime();
		float MaxLifetime = GetLifetimeMax();
		this.SetLifetime(MaxLifetime);
		float NewLifeTime = GetLifetime();
		GetCEApi().RadiusLifetimeReset(GetPosition(), GetZenTerritoriesConfig().TentRadius);
	}
	
	override void ToggleAnimation(string selection)
	{
		super.ToggleAnimation(selection);
		if (GetGame().IsServer() && GetZenTerritoriesConfig().TentRadius >= 0)
		{
			ResetTentLifeTime();
		}
	}
};