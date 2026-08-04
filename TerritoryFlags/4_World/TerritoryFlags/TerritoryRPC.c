//------------------------------------------------------------------------------------------------
// TerritoryRPC — серверная обработка RPC от клиента (инвайты, настройки, открытие меню)
//------------------------------------------------------------------------------------------------

class TerritoryRPC
{
	//------------------------------------------------------------------------------------------------------------------
	// Открыть меню флага (сервер отправляет данные клиенту)
	//------------------------------------------------------------------------------------------------------------------
	static void OpenMenu(PlayerBase player, Land_Construction_Flag_Floor flag)
	{
		if (!player || !player.GetIdentity() || !flag) return;

		string steamID = player.GetIdentity().GetPlainId();
		string flagID = flag.GetFlagTerritoryID();

		TerritoryData td = TerritoryManager.GetInstance().GetTerritoryByFlag(flagID);

		// Формируем строку данных territory для отправки клиенту
		// Формат: flagID|ownerID|ownerName|territoryName|radius|claimed|isOwner|invitedCount|id1:name1|id2:name2|...
		string data = "";
		data += flagID + "|";

		if (td)
		{
			data += td.OwnerID + "|";
			data += td.OwnerName + "|";
			data += td.TerritoryName + "|";
			data += td.Radius.ToString() + "|";
			data += "1|"; // claimed
			data += (td.IsOwner(steamID) ? "1" : "0") + "|";
			data += td.InvitedIDs.Count().ToString() + "|";
			for (int i = 0; i < td.InvitedIDs.Count(); i++)
			{
				if (i > 0) data += ";";
				data += td.InvitedIDs[i] + ":" + td.InvitedNames[i];
			}
		}
		else
		{
			// Флаг ещё не заявлен — клиент покажет кнопку "Claim"
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
		bool ok = flag.ClaimTerritory(player);
		// Отправляем обновлённые данные
		OpenMenu(player, flag);
	}

	//------------------------------------------------------------------------------------------------------------------
	// Инвайт игрока
	//------------------------------------------------------------------------------------------------------------------
	static void InvitePlayer(PlayerBase player, string flagID, string targetName)
	{
		if (!player || !player.GetIdentity()) return;
		string steamID = player.GetIdentity().GetPlainId();

		// Найти онлайн-игрока по имени
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
		bool ok = TerritoryManager.GetInstance().InvitePlayer(flagID, steamID, targetID, targetName);

		if (ok)
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 3.0, "Территория",
					"Приглашён: " + targetName,
					"set:dayz_gui icon");

			// Уведомить приглашённого
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

		// Обновить меню владельцу
		SendMenuUpdate(player, flagID);
	}

	//------------------------------------------------------------------------------------------------------------------
	// Удалить инвайт
	//------------------------------------------------------------------------------------------------------------------
	static void RemoveInvite(PlayerBase player, string flagID, string targetSteamID)
	{
		if (!player || !player.GetIdentity()) return;
		string steamID = player.GetIdentity().GetPlainId();

		bool ok = TerritoryManager.GetInstance().RemoveInvite(flagID, steamID, targetSteamID);
		if (ok)
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 3.0, "Территория",
					"Приглашение удалено",
					"set:dayz_gui icon");
			TerritoryManager.GetInstance().Save();
		}
		SendMenuUpdate(player, flagID);
	}

	//------------------------------------------------------------------------------------------------------------------
	// Сменить название
	//------------------------------------------------------------------------------------------------------------------
	static void SetName(PlayerBase player, string flagID, string name)
	{
		if (!player || !player.GetIdentity()) return;
		TerritoryManager.GetInstance().SetTerritoryName(flagID, player.GetIdentity().GetPlainId(), name);
		NotificationSystem.SendNotificationToPlayerIdentityExtended(
				player.GetIdentity(), 3.0, "Территория",
				"Название: " + name,
				"set:dayz_gui icon");
		SendMenuUpdate(player, flagID);
	}

	//------------------------------------------------------------------------------------------------------------------
	// Сменить радиус
	//------------------------------------------------------------------------------------------------------------------
	static void SetRadius(PlayerBase player, string flagID, float radius)
	{
		if (!player || !player.GetIdentity()) return;
		TerritoryManager.GetInstance().SetRadius(flagID, player.GetIdentity().GetPlainId(), radius);
		NotificationSystem.SendNotificationToPlayerIdentityExtended(
				player.GetIdentity(), 3.0, "Территория",
				"Радиус: " + radius.ToString() + "м",
				"set:dayz_gui icon");
		SendMenuUpdate(player, flagID);
	}

	//------------------------------------------------------------------------------------------------------------------
	// Хелперы
	//------------------------------------------------------------------------------------------------------------------
	static void SendMenuUpdate(PlayerBase player, string flagID)
	{
		// Находим объект флага по ID и обновляем меню
		Object obj = GetGame().GetObjectByNetworkID(flagID.ToInt());
		if (obj)
		{
			Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(obj);
			if (flag) OpenMenu(player, flag);
		}
	}

	static PlayerBase FindPlayerByName(string name)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		name.ToLower();
		foreach (Man m : players)
		{
			PlayerBase p = PlayerBase.Cast(m);
			if (p && p.GetIdentity())
			{
				if (p.GetIdentity().GetName().ToLower() == name)
					return p;
			}
		}
		// Попробуем частичное совпадение
		foreach (Man m : players)
		{
			PlayerBase p = PlayerBase.Cast(m);
			if (p && p.GetIdentity())
			{
				if (p.GetIdentity().GetName().ToLower().Contains(name))
					return p;
			}
		}
		return null;
	}
}
