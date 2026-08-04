//------------------------------------------------------------------------------------------------
// ActionBuildRestriction — блокирует строительство в чужих территориях
// Перехватываем ключевые строительные действия
//------------------------------------------------------------------------------------------------

modded class ActionBuildPart
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item)) return false;

		string steamID = player.GetIdentity().GetPlainId();
		vector buildPos = target.GetObject().GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(buildPos, steamID))
		{
			string blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(buildPos, steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 3.0, "Territory",
					"Cannot build here! Territory: " + blocker,
					"set:dayz_gui icon");
			return false;
		}
		return true;
	}
}

modded class ActionBuild
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item)) return false;

		string steamID = player.GetIdentity().GetPlainId();
		vector buildPos;
		if (target.GetObject()) buildPos = target.GetObject().GetPosition();
		else buildPos = player.GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(buildPos, steamID))
		{
			string blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(buildPos, steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 3.0, "Territory",
					"Cannot build here! Territory: " + blocker,
					"set:dayz_gui icon");
			return false;
		}
		return true;
	}
}

modded class ActionDeployObject
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item)) return false;

		string steamID = player.GetIdentity().GetPlainId();
		vector deployPos = player.GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(deployPos, steamID))
		{
			string blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(deployPos, steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 3.0, "Territory",
					"Cannot deploy here! Territory: " + blocker,
					"set:dayz_gui icon");
			return false;
		}
		return true;
	}
}
