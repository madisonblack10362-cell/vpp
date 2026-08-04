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
	void RequestOpenMenu(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> p;
		PlayerBase player;
		string netID;
		DZM_TerritoryData td;
		string data;
		string isOwner;
		int i;

		if (type != CallType.Server) return;
		if (!ctx.Read(p)) return;
		if (!sender) return;

		player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		netID = p.param1;
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
	void DoClaimFlag(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> p;
		PlayerBase player;
		string netID;
		TerritoryFlag flag;
		Object obj;
		bool ok;

		if (type != CallType.Server) return;
		if (!ctx.Read(p)) return;
		if (!sender) return;

		player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		netID = p.param1;
		obj = GetGame().GetObjectByNetworkID(netID.ToInt());
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

		RequestOpenMenu(type, ctx, sender, target);
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

		RequestOpenMenu(type, ctx, sender, target);
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

		RequestOpenMenu(type, ctx, sender, target);
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

		RequestOpenMenu(type, ctx, sender, target);
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

		RequestOpenMenu(type, ctx, sender, target);
	}
};

//------------------------------------------------------------------------------------------------------------------
// Автозапуск серверного обработчика через PluginManager
//------------------------------------------------------------------------------------------------------------------
static ref DZM_ServerHandler g_DZM_Handler;

modded class PluginManager
{
	ref DZM_ServerHandler m_DZM_RPC;

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
