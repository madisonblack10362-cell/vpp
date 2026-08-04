//------------------------------------------------------------------------------------------------------------------
// Блокировка строительства/размещения/разборки на чужих территориях
//------------------------------------------------------------------------------------------------------------------

modded class ActionDeployObject
{
        protected bool m_DZM_LastResult = true;
        protected vector m_DZM_LastPos = vector.Zero;

        override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
        {
                string steamID;
                vector projPos;
                PlayerBase pb;

                if (!super.ActionCondition(player, target, item)) return false;
                if (GetGame().IsDedicatedServer()) return true;

                pb = PlayerBase.Cast(player);
                if (!pb || !pb.GetIdentity()) return true;

                if (!pb.GetHologramLocal()) return true;
                if (!pb.GetHologramLocal().GetProjectionEntity()) return true;

                projPos = pb.GetHologramLocal().GetProjectionEntity().GetPosition();
                if (vector.Distance(m_DZM_LastPos, projPos) < 0.5) return m_DZM_LastResult;

                m_DZM_LastPos = projPos;
                steamID = pb.GetIdentity().GetPlainId();

                if (item && item.GetType() == "TerritoryFlagKit")
                {
                        if (!DZM_TerritoryManager.Get().CheckFlagPlacementAllowed(projPos))
                        {
                                pb.Zen_DisplayClientMessage("Слишком близко к чужой территории!");
                                m_DZM_LastResult = false;
                                return false;
                        }
                        m_DZM_LastResult = true;
                        return true;
                }

                if (!DZM_TerritoryManager.Get().CheckBuildAllowed(projPos, steamID))
                {
                        string blocker;
                        blocker = DZM_TerritoryManager.Get().FindBlockerName(projPos, steamID);
                        pb.Zen_DisplayClientMessage("Нельзя строить здесь! Территория: " + blocker);
                        m_DZM_LastResult = false;
                        return false;
                }

                m_DZM_LastResult = true;
                return true;
        }
}

modded class ActionBuildPart
{
        override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
        {
                string steamID;
                Object targetObj;
                vector buildPos;
                string blocker;
                PlayerBase pb;

                if (!super.ActionCondition(player, target, item)) return false;
                if (GetGame().IsServer()) return true;

                targetObj = target.GetObject();
                if (!targetObj) return true;

                pb = PlayerBase.Cast(player);
                if (!pb || !pb.GetIdentity()) return true;

                steamID = pb.GetIdentity().GetPlainId();
                buildPos = targetObj.GetPosition();

                if (!DZM_TerritoryManager.Get().CheckBuildAllowed(buildPos, steamID))
                {
                        blocker = DZM_TerritoryManager.Get().FindBlockerName(buildPos, steamID);
                        pb.Zen_DisplayClientMessage("Нельзя строить! Территория: " + blocker);
                        return false;
                }
                return true;
        }
}

modded class ActionDismantlePart
{
        override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
        {
                string steamID;
                Object targetObj;
                vector buildPos;
                string blocker;
                PlayerBase pb;

                if (!super.ActionCondition(player, target, item)) return false;
                if (GetGame().IsServer()) return true;

                targetObj = target.GetObject();
                if (!targetObj) return true;

                pb = PlayerBase.Cast(player);
                if (!pb || !pb.GetIdentity()) return true;

                steamID = pb.GetIdentity().GetPlainId();
                buildPos = targetObj.GetPosition();

                if (!DZM_TerritoryManager.Get().CheckBuildAllowed(buildPos, steamID))
                {
                        blocker = DZM_TerritoryManager.Get().FindBlockerName(buildPos, steamID);
                        pb.Zen_DisplayClientMessage("Нельзя разбирать! Территория: " + blocker);
                        return false;
                }
                return true;
        }
}
