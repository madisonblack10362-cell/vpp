//------------------------------------------------------------------------------------------------------------------// Хелпер для отправки сообщений игроку//------------------------------------------------------------------------------------------------------------------

modded class PlayerBase
{
        void DZM_ShowLocalMessage(string message)
        {
                if (GetGame().GetPlayer())
                {
                        GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", message, ""));
                }
        }

        void DZM_SendMessage(string message)
        {
                Param1<string> p;
                p = new Param1<string>("");
                if (GetGame().IsServer() && IsAlive() && !IsPlayerDisconnected() && message != "")
                {
                        p.param1 = message;
                        GetGame().RPCSingleParam(this, ERPCs.RPC_USER_ACTION_MESSAGE, p, true, GetIdentity());
                }
        }
};
