class ZenFunctions
{
	// ACTION CONDITIONS
	static const int	CAMERA_PITCH_TO_CATCH_RAIN = 45; // Camera pitch angle to catch rain
	static const float	REQUIRED_RAIN = 0.2;			 // Weather rain level required to catch rain
	static const float	REQUIRED_OVERCAST = 0.6;		 // Weather overcast level required to catch rain

	static bool HasItemType(DayZPlayer player, string item)
	{
		item.ToLower();
		array<EntityAI> itemsArray = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		for (int i = 0; i < itemsArray.Count(); i++)
		{
			if (itemsArray.Get(i))
			{
				string foundType = itemsArray.Get(i).GetType();
				foundType.ToLower();
				if (foundType == item)
					return true;
			}
		}

		return false;
	}

	static bool MoveInventory(notnull ItemBase oldItem, notnull ItemBase newItem, bool deleteOldItem = true)
	{
		if (!oldItem.GetInventory() || !newItem.GetInventory())
		{
			if (deleteOldItem)
			{
				GetGame().ObjectDelete(oldItem);
			}

			return false;
		}

		array<EntityAI> children = new array<EntityAI>;
		oldItem.GetInventory().EnumerateInventory(InventoryTraversalType.LEVELORDER, children);
		int count = children.Count();
		float itemHealth;
		bool isRuined, isLocked;
		for (int i = 0; i < count; i++)
		{
			EntityAI child = children.Get(i);
			if (child)
			{
				isRuined = false;
				isLocked = false;
				if (child.IsRuined())
				{
					itemHealth = child.GetHealth("", "");
					child.SetHealthMax("", "");
					isRuined = true;

				}

				if (child.IsLockedInSlot())
				{
					isLocked = true;
				}

				InventoryLocation child_src = new InventoryLocation;
				child.GetInventory().GetCurrentInventoryLocation(child_src);

				InventoryLocation child_dst = new InventoryLocation;
				child_dst.Copy(child_src);
				child_dst.SetParent(newItem);

				if (GameInventory.LocationCanAddEntity(child_dst))
				{
					newItem.GetInventory().TakeToDst(InventoryMode.SERVER, child_src, child_dst);

					if (isRuined)
					{
						child_dst.GetItem().SetHealth("", "", itemHealth);
					}

					if (isLocked)
					{
						ItemBase.Cast(child_dst.GetItem()).LockToParent();
					}
				}
			}
		}

		if (deleteOldItem)
		{
			GetGame().ObjectDelete(oldItem);
		}

		return true;
	}

	// Debug message - sends a server-side player message to all online players
	static void SendGlobalMessage(string msg)
	{
		array<Man> players = new array<Man>;
		GetGame().GetWorld().GetPlayerList(players);
		for (int x = 0; x < players.Count(); x++)
		{
			PlayerBase pb = PlayerBase.Cast(players.Get(x));
			if (pb)
			{
				pb.Zen_SendMessage(msg);
			}
		}
	}

	// Print a client-side white text message
	static void DebugMessage(string message)
	{
		if (GetGame().GetPlayer())
		{
			GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", message, ""));
		}
	}

	// Send a message to the given player
	static void SendPlayerMessage(PlayerBase player, string msg)
	{
		if (!player || player.IsPlayerDisconnected())
			return;

		Param1<string> m_MessageParam = new Param1<string>("");
		if (GetGame().IsDedicatedServer() && m_MessageParam && msg != "")
		{
			m_MessageParam.param1 = msg;
			GetGame().RPCSingleParam(player, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam, true, player.GetIdentity());
		}
	}

	// Is it currently raining heavily and overcast?
	static bool IsRaining()
	{
		return GetGame().GetWeather().GetRain().GetActual() >= REQUIRED_RAIN && GetGame().GetWeather().GetOvercast().GetActual() >= REQUIRED_OVERCAST;
	}

	// Client-side only. Gets camera angle.
	static int GetCameraPitch()
	{
		if (!GetGame())
			return 0;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		float pitch = 0;

		if (player)
		{
			DayZPlayerCamera camera = player.GetCurrentCamera();

			if (camera)
				pitch = camera.GetCurrentPitch();
		}

		return pitch;
	}
};