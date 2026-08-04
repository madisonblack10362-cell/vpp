class ActionTerritoryClearMembers extends ActionInteractBase
{
	protected bool CanClearAll = false;

	void ActionTerritoryClearMembers()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
	}

	override string GetText()
	{
		if (CanClearAll)
		{
			return "Очистить участников";
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

		CanClearAll = (theFlag.GetMemberCount() > 0);
		bool canClear = !CanClearAll && theFlag.CheckPlayerPermission(ident.GetId(), TerritoryPerm.REMOVEMEMBER);
		bool canSetOwner = theFlag.CanReceiveNewOwner();

		return (theFlag.CheckPlayerPermission(ident.GetId(), TerritoryPerm.OWNER) || canClear || canSetOwner);
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
			theFlag.ResetMembers();
			thePlayer.Zen_SendMessage("Список участников очищен.");
		}
	}
};
