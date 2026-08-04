modded class ActionDeployObject : ActionContinuousBase
{
        protected int m_LastSync = 0;
        protected bool m_CanPlaceHere = false;
        protected vector m_LastCheckLocation = vector.Zero;

        override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
        {
                if (!super.ActionCondition(player, target, item))
                {
                        return false;
                }

                if (GetGame().IsDedicatedServer())
                {
                        return true;
                }

                PlayerBase thePlayer = PlayerBase.Cast(player);
                if (thePlayer && thePlayer.GetHologramLocal() && thePlayer.GetHologramLocal().GetProjectionEntity())
                {
                        vector projPos = thePlayer.GetHologramLocal().GetProjectionEntity().GetPosition();
                        if (vector.Distance(m_LastCheckLocation, projPos) > 0.4)
                        {
                                string theGUID = "";
                                if (thePlayer.GetIdentity())
                                {
                                        theGUID = thePlayer.GetIdentity().GetId();
                                }

                                m_LastCheckLocation = projPos;
                                EntityAI kit = item;

                                if (kit.GetType() == "TerritoryFlagKit")
                                {
                                        array<Object> objects = new array<Object>;
                                        array<CargoBase> proxyCargos = new array<CargoBase>;
                                        GetGame().GetObjectsAtPosition(projPos, TerritoryConst.RADIUS * 2, objects, proxyCargos);
                                        TerritoryFlag theFlag;

                                        for (int x = 0; x < objects.Count(); x++)
                                        {
                                                if (Class.CastTo(theFlag, objects.Get(x)))
                                                {
                                                        if (theFlag.HasRaisedFlag())
                                                        {
                                                                thePlayer.Zen_DisplayClientMessage("Слишком близко к другой территории!");
                                                                m_CanPlaceHere = false;
                                                                return false;
                                                        }
                                                }
                                        }

                                        m_CanPlaceHere = true;
                                        return true;
                                }

                                if (!TerritoryFlag.HasTerritoryPermAtPos(theGUID, TerritoryPerm.DEPLOY, projPos))
                                {
                                        thePlayer.Zen_DisplayClientMessage("Нельзя размещать на чужой территории!");
                                        m_CanPlaceHere = false;
                                        return false;
                                }

                                m_CanPlaceHere = true;
                                return true;
                        }
                        else
                        {
                                return m_CanPlaceHere;
                        }
                }

                return true;
        }
}

modded class ActionBuildPart : ActionContinuousBase
{
        override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
        {
                if (!super.ActionCondition(player, target, item))
                {
                        return false;
                }

                if (GetGame().IsServer())
                {
                        return true;
                }

                Object targetObj = target.GetObject();
                if (!targetObj)
                {
                        return true;
                }

                PlayerBase thePlayer = PlayerBase.Cast(player);
                if (!thePlayer || !thePlayer.GetIdentity())
                {
                        return true;
                }

                if (!TerritoryFlag.HasTerritoryPermAtPos(thePlayer.GetIdentity().GetId(), TerritoryPerm.BUILD, targetObj.GetPosition()))
                {
                        thePlayer.Zen_DisplayClientMessage("Нельзя строить на чужой территории!");
                        return false;
                }

                return true;
        }
}

modded class ActionDismantlePart : ActionContinuousBase
{
        override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
        {
                if (!super.ActionCondition(player, target, item))
                {
                        return false;
                }

                if (GetGame().IsServer())
                {
                        return true;
                }

                Object targetObj = target.GetObject();
                if (!targetObj)
                {
                        return true;
                }

                PlayerBase thePlayer = PlayerBase.Cast(player);
                if (!thePlayer || !thePlayer.GetIdentity())
                {
                        return true;
                }

                if (!TerritoryFlag.HasTerritoryPermAtPos(thePlayer.GetIdentity().GetId(), TerritoryPerm.DISMANTLE, targetObj.GetPosition()))
                {
                        thePlayer.Zen_DisplayClientMessage("Нельзя разбирать на чужой территории!");
                        return false;
                }

                return true;
        }
}

modded class ActionLowerFlag : ActionContinuousBase
{
        override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
        {
                if (!super.ActionCondition(player, target, item))
                {
                        return false;
                }

                TerritoryFlag theFlag = TerritoryFlag.Cast(target.GetObject());
                PlayerBase thePlayer = PlayerBase.Cast(player);

                if (theFlag && thePlayer && thePlayer.GetIdentity())
                {
                        if (vector.Distance(theFlag.GetPosition(), thePlayer.GetPosition()) > 2.6)
                        {
                                return false;
                        }

                        if (!theFlag.CheckPlayerPermission(thePlayer.GetIdentity().GetId(), TerritoryPerm.LOWERFLAG))
                        {
                                thePlayer.Zen_DisplayClientMessage("Нет прав для опускания флага!");
                                return false;
                        }

                        return true;
                }
        }
}

modded class ActionConstructor
{
        override void RegisterActions(TTypenameArray actions)
        {
                super.RegisterActions(actions);
                actions.Insert(ActionTerritoryClaim);
                actions.Insert(ActionTerritoryInvite);
                actions.Insert(ActionTerritoryJoin);
                actions.Insert(ActionTerritoryClearMembers);
        }
};
