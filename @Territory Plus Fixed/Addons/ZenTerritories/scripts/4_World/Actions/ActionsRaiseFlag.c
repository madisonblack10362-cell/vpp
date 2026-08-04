modded class ActionRaiseFlag: ActionContinuousBase
{	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (super.ActionCondition(player, target, item))
		{
			TerritoryFlag theFlag = TerritoryFlag.Cast(target.GetObject());
			PlayerBase thePlayer = PlayerBase.Cast(player);

			if (theFlag && thePlayer && thePlayer.GetIdentity())
			{
				theFlag.SyncTerritoryRateLimited();
				if (vector.Distance(theFlag.GetPosition(), thePlayer.GetPosition()) < UAMisc.MAX_DISTANCE_FROM_FLAG)
				{
					return true;
				}
			}
		}

		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		TerritoryFlag totem = TerritoryFlag.Cast(action_data.m_Target.GetObject());

		if (totem)
		{
			totem.AnimateFlagEx(totem.GetAnimationPhase("flag_mast") - UAMisc.BONUS_RAISE_FLAG_STEP_INCREMENT, action_data.m_Player);
			totem.AddRefresherTime01(UAMisc.BONUS_RAISE_FLAG_STEP_INCREMENT);
		}
	}
};