//------------------------------------------------------------------------------------------------
// ActionBuildRestriction — блокирует строительство в чужих территориях
//------------------------------------------------------------------------------------------------

modded class ActionBuildPart
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		string steamID;
		Object targetObj;
		vector buildPos;
		string blocker;

		if (!super.ActionCondition(player, target, item)) return false;
		if (!player.GetIdentity()) return true;

		targetObj = target.GetObject();
		if (!targetObj) return true;

		steamID = player.GetIdentity().GetPlainId();
		buildPos = targetObj.GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(buildPos, steamID))
		{
			blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(buildPos, steamID);
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
		string steamID;
		vector buildPos;
		Object targetObj;
		string blocker;

		if (!super.ActionCondition(player, target, item)) return false;
		if (!player.GetIdentity()) return true;

		steamID = player.GetIdentity().GetPlainId();
		targetObj = target.GetObject();
		if (targetObj)
			buildPos = targetObj.GetPosition();
		else
			buildPos = player.GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(buildPos, steamID))
		{
			blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(buildPos, steamID);
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
		string steamID;
		vector deployPos;
		string blocker;

		if (!super.ActionCondition(player, target, item)) return false;
		if (!player.GetIdentity()) return true;

		steamID = player.GetIdentity().GetPlainId();
		deployPos = player.GetPosition();

		if (!TerritoryManager.GetInstance().CanBuild(deployPos, steamID))
		{
			blocker = TerritoryManager.GetInstance().GetBlockerOwnerName(deployPos, steamID);
			NotificationSystem.SendNotificationToPlayerIdentityExtended(
				player.GetIdentity(), 3.0, "Территория",
				"Нельзя размещать! Территория: " + blocker,
				"set:dayz_gui icon");
			return false;
		}
		return true;
	}
}
