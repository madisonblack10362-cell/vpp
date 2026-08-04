modded class PlayerBase
{
	// Sends a text message to the client (Zen_ to prevent conflicts with other mods that might use the same method name)
	void Zen_SendMessage(string message)
	{
		Param1<string> m_MessageParam = new Param1<string>("");
		if (GetGame().IsServer() && m_MessageParam && IsAlive() && !IsPlayerDisconnected() && message != "")
		{
			m_MessageParam.param1 = message;
			GetGame().RPCSingleParam(this, ERPCs.RPC_USER_ACTION_MESSAGE, m_MessageParam, true, GetIdentity());
		}
	}

	// Print a client-side white text message
	void Zen_DisplayClientMessage(string message)
	{
		if (GetGame().GetPlayer())
		{
			GetGame().GetMission().OnEvent(ChatMessageEventTypeID, new ChatMessageEventParams(CCDirect, "", message, ""));
		}
	}

	// Check for death
	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (source && GetIdentity())
		{
			PlayerBase player = PlayerBase.Cast(source);

			if (!player)
			{
				player = PlayerBase.Cast(source.GetHierarchyRootPlayer());
			}

			if (player && player.GetIdentity())
			{
				if (player.GetIdentity().GetPlainId() != this.GetIdentity().GetPlainId())
					ZenModLogger.Log("PLAYER PVP HIT: " + player.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ") hit " + this.GetIdentity().GetName() + " (" + this.GetIdentity().GetPlainId() + ") with " + ammo + " (health=" + this.GetHealth().ToString() + ")", "gunfight");
			}
		}
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (!GetIdentity())
			return;

		LogPlayerDeathItems(this);

		if (!killer)
		{
			ZenModLogger.Log("Player death: " + this.GetIdentity().GetName() + " (" + this.GetIdentity().GetPlainId() + ") was killed by a null object / unknown (pos=" + GetPosition() + ")", "pvp");
			return;
		}

		// Player melee kill
		PlayerBase player = PlayerBase.Cast(killer);
		if (player && player.GetIdentity())
		{
			ZenModLogger.Log("PLAYER PVP KILL: " + player.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ") killed " + this.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ") with " + killer.GetType() + " (pos=" + GetPosition() + ") (killerHealth=" + player.GetHealth().ToString() + ")", "pvp");
			return;
		}

		EntityAI killerAI = EntityAI.Cast(killer);
		if (killerAI)
		{
			Man man = killerAI.GetHierarchyRootPlayer();

			if (!man)
			{
				ZenModLogger.Log("Player death: " + player.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ") killed " + this.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ") with " + killer.GetType() + " (pos=" + GetPosition() + ") (killerHealth=" + player.GetHealth().ToString() + ")", "pvp");
				return;
			}

			player = PlayerBase.Cast(man);
			if (player && player.GetIdentity())
			{
				ZenModLogger.Log("PLAYER PVP KILL: " + player.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ") killed " + this.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ") with " + killer.GetType() + " (pos=" + GetPosition() + ") (killerHealth=" + player.GetHealth().ToString() + ")", "pvp");
				return;
			}
		}

		ZenModLogger.Log("Player death: " + this.GetIdentity().GetName() + " (" + this.GetIdentity().GetPlainId() + ") was killed by " + killer.GetType() + " (pos=" + GetPosition() + ")", "pvp");
	}

	void LogPlayerDeathItems(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		string logMsg = "items=";
		array<EntityAI> itemsArray = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, itemsArray);

		if (itemsArray.Count() == 0)
		{
			logMsg = "No items detected";
		}
		else
		{
			for (int i = 0; i < itemsArray.Count(); i++)
			{
				EntityAI item = itemsArray.Get(i);
				if (item && !item.GetType().Contains("SurvivorM") && !item.GetType().Contains("SurvivorF"))
				{
					float quant = item.GetQuantity();

					Ammunition_Base ammo = Ammunition_Base.Cast(item);
					if (ammo) // Item is ammunition, treat it differently to itembase
					{
						quant = ammo.GetAmmoCount();
					}

					logMsg = logMsg + "\"" + item.GetType() + "(" + item.GetQuantity() + "/" + item.GetHealth() + ")\",";
				}
			}
		}

		ZenModLogger.Log("PlayerInventory (" + player.GetIdentity().GetPlainId() + "/" + player.GetIdentity().GetName() + ")\n" + logMsg, "pvp");
	}
};