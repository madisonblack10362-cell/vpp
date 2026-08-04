//------------------------------------------------------------------------------------------------
// TerritoryRPC — серверная обработка RPC (работает с объектом флага, не с ID строкой)
//------------------------------------------------------------------------------------------------

class TerritoryRPC
{
        // Открыть меню флага (сервер отправляет данные клиенту)
        //------------------------------------------------------------------------------------------------------------------
        static void OpenMenu(PlayerBase player, Land_Construction_Flag_Floor flag)
        {
                if (!player || !player.GetIdentity() || !flag) return;

                string steamID = player.GetIdentity().GetPlainId();
                // Для RPC протокола ВСЕГДА отправляем текущий networkID (клиент использует его для обратных RPC)
                string netID = flag.GetFlagNetID();
                TerritoryData td = flag.GetTerritoryData();

                // Формат: netID|ownerID|ownerName|territoryName|radius|claimed|isOwner|invitedCount|id1:name1;id2:name2
                string data = netID + "|";

                if (td)
                {
                        data += td.OwnerID + "|";
                        data += td.OwnerName + "|";
                        data += td.TerritoryName + "|";
                        data += td.Radius.ToString() + "|";
                        data += "1|";
                        string ownerFlag;
                        if (td.IsOwner(steamID))
                                ownerFlag = "1";
                        else
                                ownerFlag = "0";
                        data += ownerFlag + "|";
                        data += td.InvitedIDs.Count().ToString() + "|";
                        for (int i = 0; i < td.InvitedIDs.Count(); i++)
                        {
                                if (i > 0) data += ";";
                                data += td.InvitedIDs[i] + ":" + td.InvitedNames[i];
                        }
                }
                else
                {
                        data += "|||0|0|0|";
                }

                GetRPCManager().SendRPC("RPC_TerritoryFlags", "OpenMenu", new Param1<string>(data), true, player.GetIdentity());
        }

        //------------------------------------------------------------------------------------------------------------------
        // Заявить территорию
        //------------------------------------------------------------------------------------------------------------------
        static void ClaimFlag(PlayerBase player, Land_Construction_Flag_Floor flag)
        {
                if (!flag) return;
                flag.ClaimTerritory(player);
                // Отправляем обновлённые данные
                OpenMenu(player, flag);
        }

        //------------------------------------------------------------------------------------------------------------------
        // Инвайт игрока
        //------------------------------------------------------------------------------------------------------------------
        static void InvitePlayer(PlayerBase player, Land_Construction_Flag_Floor flag, string targetName)
        {
                if (!player || !player.GetIdentity() || !flag) return;
                string steamID = player.GetIdentity().GetPlainId();
                string flagUUID = flag.GetFlagTerritoryID();

                PlayerBase target = FindPlayerByName(targetName);
                if (!target)
                {
                        NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                        player.GetIdentity(), 3.0, "Территория",
                                        "Игрок не найден: " + targetName,
                                        "set:dayz_gui icon");
                        return;
                }

                string targetID = target.GetIdentity().GetPlainId();
                bool ok = TerritoryManager.GetInstance().InvitePlayer(flagUUID, steamID, targetID, targetName);

                if (ok)
                {
                        NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                        player.GetIdentity(), 3.0, "Территория",
                                        "Приглашён: " + targetName,
                                        "set:dayz_gui icon");

                        NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                        target.GetIdentity(), 4.0, "Территория",
                                        "Вас пригласили на территорию!",
                                        "set:dayz_gui icon");

                        TerritoryManager.GetInstance().Save();
                }
                else
                {
                        NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                        player.GetIdentity(), 3.0, "Территория",
                                        "Уже приглашён или ошибка",
                                        "set:dayz_gui icon");
                }

                OpenMenu(player, flag);
        }

        //------------------------------------------------------------------------------------------------------------------
        // Удалить инвайт
        //------------------------------------------------------------------------------------------------------------------
        static void RemoveInvite(PlayerBase player, Land_Construction_Flag_Floor flag, string targetSteamID)
        {
                if (!player || !player.GetIdentity() || !flag) return;
                string steamID = player.GetIdentity().GetPlainId();
                string flagUUID = flag.GetFlagTerritoryID();

                bool ok = TerritoryManager.GetInstance().RemoveInvite(flagUUID, steamID, targetSteamID);
                if (ok)
                {
                        NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                        player.GetIdentity(), 3.0, "Территория",
                                        "Приглашение удалено",
                                        "set:dayz_gui icon");
                        TerritoryManager.GetInstance().Save();
                }
                OpenMenu(player, flag);
        }

        //------------------------------------------------------------------------------------------------------------------
        // Сменить название
        //------------------------------------------------------------------------------------------------------------------
        static void SetName(PlayerBase player, Land_Construction_Flag_Floor flag, string name)
        {
                if (!player || !player.GetIdentity() || !flag) return;
                string flagUUID = flag.GetFlagTerritoryID();
                TerritoryManager.GetInstance().SetTerritoryName(flagUUID, player.GetIdentity().GetPlainId(), name);
                NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                player.GetIdentity(), 3.0, "Территория",
                                "Название: " + name,
                                "set:dayz_gui icon");
                OpenMenu(player, flag);
        }

        //------------------------------------------------------------------------------------------------------------------
        // Сменить радиус
        //------------------------------------------------------------------------------------------------------------------
        static void SetRadius(PlayerBase player, Land_Construction_Flag_Floor flag, float radius)
        {
                if (!player || !player.GetIdentity() || !flag) return;
                string flagUUID = flag.GetFlagTerritoryID();
                TerritoryManager.GetInstance().SetRadius(flagUUID, player.GetIdentity().GetPlainId(), radius);
                NotificationSystem.SendNotificationToPlayerIdentityExtended(
                                player.GetIdentity(), 3.0, "Территория",
                                "Радиус: " + radius.ToString() + "м",
                                "set:dayz_gui icon");
                OpenMenu(player, flag);
        }

        //------------------------------------------------------------------------------------------------------------------
        // Поиск игрока по имени (точное, потом частичное)
        //------------------------------------------------------------------------------------------------------------------
        static PlayerBase FindPlayerByName(string name)
        {
                if (name.Length() == 0) return null;

                array<Man> players = new array<Man>;
                GetGame().GetPlayers(players);
                string search = name;
                search.ToLower();

                // Сначала точное совпадение
                foreach (Man m : players)
                {
                        PlayerBase p = PlayerBase.Cast(m);
                        if (p && p.GetIdentity())
                        {
                                if (p.GetIdentity().GetName().ToLower() == search)
                                        return p;
                        }
                }

                // Потом частичное
                foreach (Man m : players)
                {
                        PlayerBase p = PlayerBase.Cast(m);
                        if (p && p.GetIdentity())
                        {
                                if (p.GetIdentity().GetName().ToLower().Contains(search))
                                        return p;
                        }
                }
                return null;
        }
}