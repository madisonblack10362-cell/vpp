//------------------------------------------------------------------------------------------------------------------
// Серверная обработка RPC запросов от клиента
//------------------------------------------------------------------------------------------------------------------

class DZM_ServerHandler
{
        void DZM_ServerHandler()
        {
                GetRPCManager().AddRPC("DZM_TF", "RequestOpenMenu", this, SingleplayerExecutionType.Server);
                GetRPCManager().AddRPC("DZM_TF", "DoClaimFlag", this, SingleplayerExecutionType.Server);
                GetRPCManager().AddRPC("DZM_TF", "DoInvitePlayer", this, SingleplayerExecutionType.Server);
                GetRPCManager().AddRPC("DZM_TF", "DoKickPlayer", this, SingleplayerExecutionType.Server);
                GetRPCManager().AddRPC("DZM_TF", "DoSetName", this, SingleplayerExecutionType.Server);
                GetRPCManager().AddRPC("DZM_TF", "DoSetRadius", this, SingleplayerExecutionType.Server);
                DZM_TerritoryManager.Get();
                Print("[DZM_TerritoryFlags] Server RPC handler ready");
        }

        //------------------------------------------------------------------------------------------------------------------
        void RefreshMenu(string netID, PlayerIdentity sender)
        {
                DZM_TerritoryData td;
                string data;
                string isOwner;
                int i;

                if (!sender) return;

                td = DZM_TerritoryManager.Get().GetByNetID(netID);

                data = netID + "|";

                if (td && td.Claimed)
                {
                        data += td.OwnerSteamID + "|";
                        data += td.OwnerName + "|";
                        data += td.DisplayName + "|";
                        data += td.Radius.ToString() + "|";
                        data += "1|";
                        if (td.IsOwner(sender.GetPlainId()))
                        {
                                isOwner = "1";
                        }
                        else
                        {
                                isOwner = "0";
                        }
                        data += isOwner + "|";
                        data += td.Friends.Count().ToString() + "|";
                        for (i = 0; i < td.Friends.Count(); i++)
                        {
                                if (i > 0) data += ";";
                                data += td.Friends[i];
                        }
                }
                else
                {
                        data += "|||||0|0|0|";
                }

                GetRPCManager().SendRPC("DZM_TF", "OpenMenu", new Param1<string>(data), true, sender);
        }

        //------------------------------------------------------------------------------------------------------------------
        void RequestOpenMenu(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
        {
                Param1<string> p;

                if (type != CallType.Server) return;
                if (!ctx.Read(p)) return;
                if (!sender) return;

                RefreshMenu(p.param1, sender);
        }

        //------------------------------------------------------------------------------------------------------------------
        void DoClaimFlag(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
        {
                Param1<string> p;
                PlayerBase player;
                string netID;
                TerritoryFlag flag;
                EntityAI obj;
                bool ok;

                if (type != CallType.Server) return;
                if (!ctx.Read(p)) return;
                if (!sender) return;

                player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
                if (!player) return;

                netID = p.param1;
                obj = GetGame().GetEntityByNetworkId(netID.ToInt());
                flag = TerritoryFlag.Cast(obj);
                if (!flag) return;

                ok = DZM_TerritoryManager.Get().ClaimFlag(netID, sender.GetPlainId(), sender.GetName(), flag.GetPosition());
                if (ok)
                {
                        player.DZM_SendMessage("Территория захвачена! Радиус: 100м");
                }
                else
                {
                        player.DZM_SendMessage("Не удалось захватить территорию");
                }

                RefreshMenu(netID, sender);
        }

        //------------------------------------------------------------------------------------------------------------------
        void DoInvitePlayer(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
        {
                Param2<string, string> p;
                PlayerBase player;
                PlayerBase targetPlayer;
                string netID;
                string targetName;
                string targetID;
                bool ok;

                if (type != CallType.Server) return;
                if (!ctx.Read(p)) return;
                if (!sender) return;

                player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
                if (!player) return;

                netID = p.param1;
                targetName = p.param2;

                targetPlayer = DZM_TerritoryManager.FindOnlinePlayer(targetName);
                if (!targetPlayer || !targetPlayer.GetIdentity())
                {
                        player.DZM_SendMessage("Игрок не найден: " + targetName);
                        return;
                }

                targetID = targetPlayer.GetIdentity().GetPlainId();
                ok = DZM_TerritoryManager.Get().InviteFriend(netID, sender.GetPlainId(), targetID);
                if (ok)
                {
                        player.DZM_SendMessage("Игрок приглашён: " + targetName);
                        targetPlayer.DZM_SendMessage("Вас пригласили на территорию!");
                }
                else
                {
                        player.DZM_SendMessage("Не удалось пригласить (уже в списке или ошибка)");
                }

                RefreshMenu(netID, sender);
        }

        //------------------------------------------------------------------------------------------------------------------
        void DoKickPlayer(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
        {
                Param2<string, string> p;
                PlayerBase player;
                string netID;
                string friendID;
                bool ok;

                if (type != CallType.Server) return;
                if (!ctx.Read(p)) return;
                if (!sender) return;

                player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
                if (!player) return;

                netID = p.param1;
                friendID = p.param2;

                ok = DZM_TerritoryManager.Get().KickFriend(netID, sender.GetPlainId(), friendID);
                if (ok)
                {
                        player.DZM_SendMessage("Игрок удалён из территории");
                }
                else
                {
                        player.DZM_SendMessage("Не удалось удалить");
                }

                RefreshMenu(netID, sender);
        }

        //------------------------------------------------------------------------------------------------------------------
        void DoSetName(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
        {
                Param2<string, string> p;
                PlayerBase player;
                string netID;
                string newName;
                bool ok;

                if (type != CallType.Server) return;
                if (!ctx.Read(p)) return;
                if (!sender) return;

                player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
                if (!player) return;

                netID = p.param1;
                newName = p.param2;

                ok = DZM_TerritoryManager.Get().ChangeName(netID, sender.GetPlainId(), newName);
                if (ok)
                {
                        player.DZM_SendMessage("Название изменено: " + newName);
                }

                RefreshMenu(netID, sender);
        }

        //------------------------------------------------------------------------------------------------------------------
        void DoSetRadius(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
        {
                Param3<string, string, float> p;
                PlayerBase player;
                string netID;
                float radius;
                bool ok;

                if (type != CallType.Server) return;
                if (!ctx.Read(p)) return;
                if (!sender) return;

                player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
                if (!player) return;

                netID = p.param1;
                radius = p.param3;

                ok = DZM_TerritoryManager.Get().ChangeRadius(netID, sender.GetPlainId(), radius);
                if (ok)
                {
                        player.DZM_SendMessage("Радиус изменён: " + radius.ToString() + "м");
                }

                RefreshMenu(netID, sender);
        }
};

//------------------------------------------------------------------------------------------------------------------
// Автозапуск серверного обработчика через PluginManager
//------------------------------------------------------------------------------------------------------------------
static ref DZM_ServerHandler g_DZM_Handler;

modded class PluginManager
{
        override void Init()
        {
                super.Init();
                if (GetGame().IsServer())
                {
                        if (!g_DZM_Handler)
                        {
                                g_DZM_Handler = new DZM_ServerHandler();
                        }
                }
        }
};
