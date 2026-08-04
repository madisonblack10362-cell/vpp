class ActionTerritoryClaim extends ActionInteractBase
{
	protected bool CanClaim = false;

	void ActionTerritoryClaim()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
	}

	override string GetText()
	{
		if (CanClaim)
		{
			return "Захватить территорию";
		}

		return "Сбросить владельца";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		PlayerIdentity ident = PlayerIdentity.Cast(player.GetIdentity());
		TerritoryFlag theFlag = TerritoryFlag.Cast(target.GetObject());

		if (!ident || !theFlag)
		{
			return false;
		}

		float state = theFlag.GetAnimationPhase("flag_mast");
		if (!theFlag.FindAttachmentBySlotName("Material_FPole_Flag"))
		{
			return false;
		}

		if (state >= TerritoryConst.FLAGUPSTATE)
		{
			return false;
		}

		CanClaim = theFlag.CanReceiveNewOwner();
		return true;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Target || !action_data.m_Player)
		{
			return;
		}

		PlayerBase thePlayer = PlayerBase.Cast(action_data.m_Player);
		TerritoryFlag theFlag = TerritoryFlag.Cast(action_data.m_Target.GetObject());

		if (theFlag && thePlayer && thePlayer.GetIdentity())
		{
			if (CanClaim && theFlag.CanReceiveNewOwner())
			{
				thePlayer.Zen_SendMessage("Вы захватили территорию! Другие не могут строить рядом.");
				theFlag.SetTerritoryOwner(thePlayer.GetIdentity().GetId());
			}
			else
			{
				theFlag.SetTerritoryOwner("");
			}

			theFlag.ResetMembers();
		}
	}
};