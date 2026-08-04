//------------------------------------------------------------------------------------------------
// TerritoryHandlers — привязка RPC к серверным функциям
// Регистрируется в PluginManager
//------------------------------------------------------------------------------------------------

modded class PluginManager
{
	ref TerritoryMenuRPC m_TerritoryRPC;

	override void Init()
	{
		super.Init();
		m_TerritoryRPC = new TerritoryMenuRPC();
		RegisterPlugin("TerritoryMenuRPC", false, true);
	}
}

//------------------------------------------------------------------------------------------------
// Класс который обрабатывает входящие RPC от клиентов
//------------------------------------------------------------------------------------------------

class TerritoryMenuRPC
{
	void TerritoryMenuRPC()
	{
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "ClaimFlag", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "InvitePlayer", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "RemoveInvite", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "SetName", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "SetRadius", this, SingleplayerExecutionType.Server);

		// Загружаем сохранённые территории
		TerritoryManager.GetInstance().Load();
		Print("[TerritoryFlags] RPC handlers registered, data loaded");
	}

	//------------------------------------------------------------------------------------------------------------------
	void ClaimFlag(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		// Ищем ближайший флаг к игроку (в радиусе 3м)
		Land_Construction_Flag_Floor flag = FindNearbyFlag(player);
		if (!flag) return;

		TerritoryRPC.ClaimFlag(player, flag);
	}

	//------------------------------------------------------------------------------------------------------------------
	void InvitePlayer(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		Param2<string, string> data;
		if (!ctx.Read(data)) return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		TerritoryRPC.InvitePlayer(player, data.param1, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void RemoveInvite(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		Param2<string, string> data;
		if (!ctx.Read(data)) return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		TerritoryRPC.RemoveInvite(player, data.param1, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SetName(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		Param2<string, string> data;
		if (!ctx.Read(data)) return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		TerritoryRPC.SetName(player, data.param1, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SetRadius(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		Param2<string, float> data;
		if (!ctx.Read(data)) return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		TerritoryRPC.SetRadius(player, data.param1, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	Land_Construction_Flag_Floor FindNearbyFlag(PlayerBase player)
	{
		array<Object> objects = new array<Object>;
		GetGame().GetObjectsAtPosition(player.GetPosition(), 3.0, objects, NULL);

		foreach (Object obj : objects)
		{
			Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(obj);
			if (flag) return flag;
		}
		return null;
	}
}
