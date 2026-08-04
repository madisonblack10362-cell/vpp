//------------------------------------------------------------------------------------------------
// TerritoryRPC — серверная обработка RPC (работает с объектом флага, не с ID строкой)
//------------------------------------------------------------------------------------------------

class TerritoryRPC
{
	//------------------------------------------------------------------------------------------------------------------
	static void OpenMenu(PlayerBase player, Land_Construction_Flag_Floor flag)
	{
		string steamID;
		string netID;
		string data;
		TerritoryData td;
		string ownerFlag;
		int i;

		if (!player || !player.GetIdentity() || !flag) return;

		steamID = player.GetIdentity().GetPlainId();
		netID = flag.GetFlagNetID();
		td = flag.GetTerritoryData();

		data = netID + "|";

		if (td)
		{
			data += td.OwnerID + "|";
			data += td.OwnerName + "|";
			data += td.TerritoryName + "|";
			data += td.Radius.ToString() + "|";
			data += "1|";
			if (td.IsOwner(steamID))
				ownerFlag = "1";
			else
				ownerFlag = "0";
			data += ownerFlag + "|";
			data += td.InvitedIDs.Count().ToString() + "|";
			for (i = 0; i < td.InvitedIDs.Count(); i++)
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
	static void ClaimFlag(PlayerBase player, Land_Construction_Flag_Floor flag)
	{
		if (!flag) return;
		flag.ClaimTerritory(player);
		OpenMenu(player, flag);
	}

	//------------------------------------------------------------------------------------------------------------------
	static void InvitePlayer(PlayerBase player, Land_Construction_Flag_Floor flag, string targetName)
	{
		string steamID;
		string flagUUID;
		PlayerBase target;
		string targetID;
		bool ok;

		if (!player || !player.GetIdentity() || !flag) return;

		steamID = player.GetIdentity().GetPlainId();
		flagUUID = flag.GetFlagTerritoryID();

		target = FindPlayerByName(targetName);
		if (!target)
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
				player.GetIdentity(), 3.0, "Территория",
				"Игрок не найден: " + targetName,
				"set:dayz_gui icon");
			return;
		}

		targetID = target.GetIdentity().GetPlainId();
		ok = TerritoryManager.GetInstance().InvitePlayer(flagUUID, steamID, targetID, targetName);

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
	static void RemoveInvite(PlayerBase player, Land_Construction_Flag_Floor flag, string targetSteamID)
	{
		string steamID;
		string flagUUID;
		bool ok;

		if (!player || !player.GetIdentity() || !flag) return;

		steamID = player.GetIdentity().GetPlainId();
		flagUUID = flag.GetFlagTerritoryID();

		ok = TerritoryManager.GetInstance().RemoveInvite(flagUUID, steamID, targetSteamID);
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
	static void SetName(PlayerBase player, Land_Construction_Flag_Floor flag, string name)
	{
		string flagUUID;

		if (!player || !player.GetIdentity() || !flag) return;

		flagUUID = flag.GetFlagTerritoryID();
		TerritoryManager.GetInstance().SetTerritoryName(flagUUID, player.GetIdentity().GetPlainId(), name);
		NotificationSystem.SendNotificationToPlayerIdentityExtended(
			player.GetIdentity(), 3.0, "Территория",
			"Название: " + name,
			"set:dayz_gui icon");
		OpenMenu(player, flag);
	}

	//------------------------------------------------------------------------------------------------------------------
	static void SetRadius(PlayerBase player, Land_Construction_Flag_Floor flag, float radius)
	{
		string flagUUID;

		if (!player || !player.GetIdentity() || !flag) return;

		flagUUID = flag.GetFlagTerritoryID();
		TerritoryManager.GetInstance().SetRadius(flagUUID, player.GetIdentity().GetPlainId(), radius);
		NotificationSystem.SendNotificationToPlayerIdentityExtended(
			player.GetIdentity(), 3.0, "Территория",
			"Радиус: " + radius.ToString() + "м",
			"set:dayz_gui icon");
		OpenMenu(player, flag);
	}

	//------------------------------------------------------------------------------------------------------------------
	static PlayerBase FindPlayerByName(string name)
	{
		array<Man> players;
		string search;
		Man m;
		PlayerBase p;

		if (name.Length() == 0) return null;

		players = new array<Man>;
		GetGame().GetPlayers(players);
		search = name;
		search.ToLower();

		for (int i = 0; i < players.Count(); i++)
		{
			m = players[i];
			p = PlayerBase.Cast(m);
			if (p && p.GetIdentity())
			{
				if (p.GetIdentity().GetName().ToLower() == search)
					return p;
			}
		}

		for (int i = 0; i < players.Count(); i++)
		{
			m = players[i];
			p = PlayerBase.Cast(m);
			if (p && p.GetIdentity())
			{
				if (p.GetIdentity().GetName().ToLower().Contains(search))
					return p;
			}
		}
		return null;
	}
}