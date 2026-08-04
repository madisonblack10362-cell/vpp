//--------------------------------------------------------------------------------------------------------------
// DZM Menu Action
//--------------------------------------------------------------------------------------------------------------

class ActionDZM_OpenMenu extends ActionInteractBase
{
	void ActionDZM_OpenMenu()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
		Print("[DZM_TerritoryFlags] ActionDZM_OpenMenu constructor called");
	}

	override string GetText()
	{
		return "Меню территории";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		TerritoryFlag flag;
		flag = TerritoryFlag.Cast(target.GetObject());
		if (!flag) return false;
		return true;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		TerritoryFlag flag;
		PlayerBase pb;
		string uuid;
		flag = TerritoryFlag.Cast(action_data.m_Target.GetObject());
		pb = PlayerBase.Cast(action_data.m_Player);
		if (!flag || !pb || !pb.GetIdentity()) return;
		uuid = flag.DZM_GetUUID();
		if (uuid.Length() == 0) return;
		GetRPCManager().SendRPC("DZM_TF", "RequestOpenMenu", new Param1<string>(uuid), true, pb.GetIdentity());
	}
};
