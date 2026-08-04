//------------------------------------------------------------------------------------------------
// ActionTerritoryMenu — кастомное действие для открытия меню территории
// Появляется только при взгляде на строительный флаг
//------------------------------------------------------------------------------------------------

class ActionTerritoryMenu: ActionSingleUseBase
{
        void ActionTerritoryMenu() { m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACT; }

        override void CreateConditionComponents()
        {
                m_ConditionItem = new CCNonExistent;
                m_ConditionTarget = new CCTCursor(UAMaxDistances.DEFAULT);
        }

        override string GetText()
        {
                return "Меню территории";
        }

        override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
        {
                Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(target.GetObject());
                if (!flag) return false;
                return true;
        }

        override void Start(ActionData action_data)
        {
                super.Start(action_data);

                Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(action_data.m_Target.GetObject());
                if (flag)
                {
                        TerritoryRPC.OpenMenu(action_data.m_Player, flag);
                }
        }
}

// GetActions override добавлен в TerritoryFlag.c