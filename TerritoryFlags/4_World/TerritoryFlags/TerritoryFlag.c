//------------------------------------------------------------------------------------------------
// TerritoryFlag — моддед класс ванильного строительного флага
// При первом взаимодействии заявляет территорию
//------------------------------------------------------------------------------------------------

modded class Land_Construction_Flag_Floor
{
        protected string m_TerritoryOwnerID;
        protected string m_TerritoryOwnerName;
        protected bool   m_TerritoryClaimed = false;
        protected string m_FlagNetworkID;

        //------------------------------------------------------------------------------------------------------------------
        // Получить строковый ID флага для TerritoryManager
        //------------------------------------------------------------------------------------------------------------------
        string GetFlagTerritoryID()
        {
                if (m_FlagNetworkID.Length() == 0)
                        m_FlagNetworkID = GetNetworkID().ToString();
                return m_FlagNetworkID;
        }

        //------------------------------------------------------------------------------------------------------------------
        // Заявить территорию (вызывается при первом взаимодействии)
        //------------------------------------------------------------------------------------------------------------------
        bool ClaimTerritory(PlayerBase player)
        {
                if (m_TerritoryClaimed) return false;
                if (!player || !player.GetIdentity()) return false;

                string steamID = player.GetIdentity().GetPlainId();
                string name = player.GetIdentity().GetName();
                string flagID = GetFlagTerritoryID();

                // Проверяем нет ли уже территории в радиусе
                TerritoryManager tm = TerritoryManager.GetInstance();
                if (!tm.CanBuild(GetPosition(), steamID) || GetBlockerForPlayer(steamID).Length() > 0)
                {
                        // Можно строить только если это наша территория или свободная зона
                        // CanBuild вернёт false если мы в чужой зоне, но для владельца — true
                        if (!tm.CanBuild(GetPosition(), steamID))
                        {
                                string blocker = tm.GetBlockerOwnerName(GetPosition(), steamID);
                                NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                                player.GetIdentity(), 4.0, "Territory",
                                                "Cannot claim here! Inside: " + blocker,
                                                "set:dayz_gui icon");
                                return false;
                        }
                }

                // Проверяем что наш флаг не внутри нашей же территории
                // (допускается — можно иметь несколько флагов)

                m_TerritoryOwnerID = steamID;
                m_TerritoryOwnerName = name;
                m_TerritoryClaimed = true;

                tm.RegisterTerritory(flagID, steamID, name, GetPosition(), 100.0);

                SetSynchDirty();

                NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                player.GetIdentity(), 4.0, "Territory",
                                "Territory claimed! Radius: 100m",
                                "set:dayz_gui icon");

                Print("[TerritoryFlags] Flag claimed by " + name + " at " + GetPosition().ToString());
                return true;
        }

        //------------------------------------------------------------------------------------------------------------------
        // Проверяет блокирует ли чужая территория в данной точке для данного игрока
        //------------------------------------------------------------------------------------------------------------------
        string GetBlockerForPlayer(string steamID)
        {
                TerritoryManager tm = TerritoryManager.GetInstance();
                return tm.GetBlockerOwnerName(GetPosition(), steamID);
        }

        //------------------------------------------------------------------------------------------------------------------
        // Получить данные территории этого флага
        //------------------------------------------------------------------------------------------------------------------
        TerritoryData GetTerritoryData()
        {
                return TerritoryManager.GetInstance().GetTerritoryByFlag(GetFlagTerritoryID());
        }

        //------------------------------------------------------------------------------------------------------------------
        // Является ли игрок владельцем или приглашённым
        //------------------------------------------------------------------------------------------------------------------
        bool IsTerritoryMember(string steamID)
        {
                if (steamID == m_TerritoryOwnerID) return true;
                TerritoryData td = GetTerritoryData();
                if (!td) return false;
                return td.IsPlayerAllowed(steamID);
        }

        //------------------------------------------------------------------------------------------------------------------
        // Серверное сохранение
        //------------------------------------------------------------------------------------------------------------------
        override void OnStoreSave(ParamsWriteContext ctx)
        {
                super.OnStoreSave(ctx);
                ctx.Write(m_TerritoryClaimed);
                ctx.Write(m_TerritoryOwnerID);
                ctx.Write(m_TerritoryOwnerName);
        }

        override bool OnStoreLoad(ParamsReadContext ctx, int version)
        {
                if (!super.OnStoreLoad(ctx, version)) return false;
                if (!ctx.Read(m_TerritoryClaimed)) return false;
                if (!ctx.Read(m_TerritoryOwnerID)) return false;
                if (!ctx.Read(m_TerritoryOwnerName)) return false;

                // Перезапускаем менеджер и перерегистрируем
                if (m_TerritoryClaimed && m_TerritoryOwnerID.Length() > 0)
                {
                        // Обновляем позицию флага в данных территории (на случай если флаг перенесли)
                        TerritoryManager tm = TerritoryManager.GetInstance();
                        tm.Load();
                        string flagID = GetFlagTerritoryID();
                        TerritoryData td = tm.GetTerritoryByFlag(flagID);
                        if (td)
                        {
                                td.Position = GetPosition();
                        }
                        else
                        {
                                tm.RegisterTerritory(flagID, m_TerritoryOwnerID, m_TerritoryOwnerName, GetPosition());
                        }
                }
                return true;
        }

        //------------------------------------------------------------------------------------------------------------------
        // При удалении флага — снять территорию
        //------------------------------------------------------------------------------------------------------------------
        void EEKilled(Object killer)
        {
                super.EEKilled(killer);
                if (m_TerritoryClaimed)
                {
                        TerritoryManager.GetInstance().UnregisterTerritory(GetFlagTerritoryID());
                        Print("[TerritoryFlags] Flag destroyed, territory removed: " + GetFlagTerritoryID());
                }
        }

        //------------------------------------------------------------------------------------------------------------------
        // Добавляем действие Territory Menu
        //------------------------------------------------------------------------------------------------------------------
        override array<string> GetActions()
        {
                array<string> actions = super.GetActions();
                actions.Insert("ActionTerritoryMenu");
                return actions;
        }

        //------------------------------------------------------------------------------------------------------------------
        // Деструктор
        //------------------------------------------------------------------------------------------------------------------
        void ~Land_Construction_Flag_Floor()
        {
                // Территория уже удалена в EEKilled, но на всякий случай
        }
}
