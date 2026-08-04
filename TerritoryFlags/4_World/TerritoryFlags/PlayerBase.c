modded class PlayerBase
{
	void Zen_SendMessage(string message)
	{
		Param1<string> m_MessageParam = new Param1<string>("");
		if (GetGame().IsServer() && m_MessageParam && IsAlive() && !IsPlayerDisconnected() && message != "")
		{
			m_MessageParam.param1 = message;
			GetGame().RPCSingleParam(this, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam, true, GetIdentity());
		}
	}

	void Zen_DisplayClientMessage(string message)
	{
		if (GetGame().GetPlayer())
		{
			GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", message, ""));
		}
	}
}
