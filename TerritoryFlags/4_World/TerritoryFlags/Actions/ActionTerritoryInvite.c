class ActionTerritoryInvite extends ActionInteractBase
{
	protected bool Inviting = true;

	void ActionTerritoryInvite()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
	}

	override string GetText()
	{
		if (Inviting)
		{
			return "Пригласить игрока";
		}

		return "Отменить приглашение";
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

		Inviting = !theFlag.CanAddMember();
		return theFlag.CheckPlayerPermission(ident.GetId(), TerritoryPerm.ADDMEMBER);
	}

	override void OnExecuteServer(ActionData action_data)
	{
		if (!action_data || !action_data.m_Target)
		{
			return;
		}

		TerritoryFlag theFlag = TerritoryFlag.Cast(action_data.m_Target.GetObject());
		if (theFlag)
		{
			theFlag.AllowMemberToBeAdded(Inviting);

			if (Inviting)
			{
				if (action_data.m_Player)
				{
					PlayerBase p = PlayerBase.Cast(action_data.m_Player);
					if (p) p.Zen_SendMessage("Приглашение активно! Игрок должен принять у флага.");
				}
			}
		}
	}
};
