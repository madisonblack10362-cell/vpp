//------------------------------------------------------------------------------------------------------------------
// Action для открытия меню территории при взгляде на флаг//------------------------------------------------------------------------------------------------------------------

class ActionDZM_OpenMenu extends ActionInteractBase
{
        void ActionDZM_OpenMenu()
        {
                m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_ATTACHITEM;
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
                string netID;
                PlayerBase pb;

                flag = TerritoryFlag.Cast(action_data.m_Target.GetObject());
                pb = PlayerBase.Cast(action_data.m_Player);
                if (!flag || !pb || !pb.GetIdentity()) return;

                netID = flag.GetNetworkID().ToString();
                GetRPCManager().SendRPC("DZM_TF", "RequestOpenMenu", new Param1<string>(netID), true, pb.GetIdentity());
        }
};
