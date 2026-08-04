//------------------------------------------------------------------------------------------------------------------
// Расширение ванильного TerritoryFlag через CE storage
// Хранит только UUID для привязки к менеджеру при перезапуске
//------------------------------------------------------------------------------------------------------------------

modded class TerritoryFlag
{
        protected string m_DZM_UUID = "";

        //------------------------------------------------------------------------------------------------------------------
        string DZM_GetUUID()
        {
                return m_DZM_UUID;
        }

        //------------------------------------------------------------------------------------------------------------------
        override void OnStoreSave(ParamsWriteContext ctx)
        {
                super.OnStoreSave(ctx);
                ctx.Write(m_DZM_UUID);
        }

        //------------------------------------------------------------------------------------------------------------------
        override bool OnStoreLoad(ParamsReadContext ctx, int version)
        {
                string savedUUID;
                string currentNetID;

                if (!super.OnStoreLoad(ctx, version)) return false;

                savedUUID = "";
                if (ctx.Read(savedUUID))
                {
                        m_DZM_UUID = savedUUID;
                }

                if (GetGame().IsServer() && m_DZM_UUID.Length() > 0)
                {
                        currentNetID = GetNetworkID().ToString();
                        DZM_TerritoryManager.Get().RegisterFlag(currentNetID, m_DZM_UUID);
                        m_DZM_UUID = currentNetID;
                }

                return true;
        }

        //------------------------------------------------------------------------------------------------------------------
        override void EEInit()
        {
                super.EEInit();

                if (GetGame().IsServer())
                {
                        string netID;
                        netID = GetNetworkID().ToString();
                        if (m_DZM_UUID.Length() == 0)
                        {
                                DZM_TerritoryManager.Get().RegisterFlag(netID, "");
                                m_DZM_UUID = netID;
                        }
                }
        }

        //------------------------------------------------------------------------------------------------------------------
        override void EEKilled(Object killer)
        {
                super.EEKilled(killer);
                DZM_Cleanup();
        }

        //------------------------------------------------------------------------------------------------------------------
        override void EEDelete(EntityAI owner)
        {
                super.EEDelete(owner);
                DZM_Cleanup();
        }

        //------------------------------------------------------------------------------------------------------------------
        void DZM_Cleanup()
        {
                string netID;
                if (GetGame().IsServer())
                {
                        netID = GetNetworkID().ToString();
                        DZM_TerritoryManager.Get().UnregisterFlag(netID);
                }
        }

        //------------------------------------------------------------------------------------------------------------------
        override void SetActions()
        {
                super.SetActions();
                AddAction(ActionDZM_OpenMenu);
        }
};
