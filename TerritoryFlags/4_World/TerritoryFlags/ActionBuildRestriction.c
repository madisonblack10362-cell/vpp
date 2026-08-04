//------------------------------------------------------------------------------------------------
// ActionBuildRestriction — блокирует строительство в чужих территориях
//------------------------------------------------------------------------------------------------

modded class ActionBuildPart
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!super.ActionCondition(player, target, item)) return false;

		if (!player.GetIdentity()) return true; // На всякий случай

		Object targetObj = target.GetObject();
		if (!targetObj) return true;

		string steamID = player.GetIdentity().GetPlainId();
		vector buildPos = targetObj.GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(buildPos, steamID))
		{
			string blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(buildPos, steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 3.0, "Территория",
					"Нельзя строить! Территория: " + blocker,
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

		if (!player.GetIdentity()) return true;

		string steamID = player.GetIdentity().GetPlainId();
		vector buildPos;
		Object targetObj = target.GetObject();
		if (targetObj) buildPos = targetObj.GetPosition();
		else buildPos = player.GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(buildPos, steamID))
		{
			string blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(buildPos, steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 3.0, "Территория",
					"Нельзя строить! Территория: " + blocker,
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

		if (!player.GetIdentity()) return true;

		string steamID = player.GetIdentity().GetPlainId();
		vector deployPos = player.GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(deployPos, steamID))
		{
			string blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(deployPos, steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
					player.GetIdentity(), 3.0, "Территория",
					"Нельзя размещать! Территория: " + blocker,
					"set:dayz_gui icon");
			return false;
		}
		return true;
	}
}
