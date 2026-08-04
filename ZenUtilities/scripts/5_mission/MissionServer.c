modded class MissionServer
{
	// Invoke on player connect
	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);

		if (player && identity)
		{
			ZenModLogger.Log("Player logged in: " + identity.GetName() + " (" + identity.GetPlainId() + ")" + " (pos=" + player.GetPosition() + ")", "login");
		}
	}

	// On disconnect
	void InvokeOnDisconnect(PlayerBase player)
	{
		super.InvokeOnDisconnect(player);

		if (player && player.GetIdentity())
		{
			ZenModLogger.Log("Player logged out: " + player.GetIdentity().GetName() + " (" + player.GetIdentity().GetPlainId() + ")" + " (pos=" + player.GetPosition() + ")", "login");
		}
	}
};