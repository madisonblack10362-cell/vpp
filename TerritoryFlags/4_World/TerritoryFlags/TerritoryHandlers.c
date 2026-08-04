//------------------------------------------------------------------------------------------------
// TerritoryHandlers — привязка RPC к серверным функциям
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
class TerritoryMenuRPC
{
	void TerritoryMenuRPC()
	{
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "ClaimFlag", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "InvitePlayer", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "RemoveInvite", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "SetName", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "SetRadius", this, SingleplayerExecutionType.Server);

		// Загружаем сохранённые территории ОДИН РАЗ
		TerritoryManager.GetInstance().Load();
		Print("[TerritoryFlags] RPC handlers registered, data loaded");
	}

	//------------------------------------------------------------------------------------------------------------------
	void ClaimFlag(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		Param1<string> data;
		if (!ctx.Read(data)) return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		// Ищем флаг по networkID который клиент прислал
		int netID = data.param1.ToInt();
		Object obj = GetGame().GetObjectByNetworkID(netID);
		Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(obj);
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

		// data.param1 = current networkID флага
		int netID = data.param1.ToInt();
		Object obj = GetGame().GetObjectByNetworkID(netID);
		Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.InvitePlayer(player, flag, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void RemoveInvite(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		Param2<string, string> data;
		if (!ctx.Read(data)) return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		int netID = data.param1.ToInt();
		Object obj = GetGame().GetObjectByNetworkID(netID);
		Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.RemoveInvite(player, flag, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SetName(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		Param2<string, string> data;
		if (!ctx.Read(data)) return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		int netID = data.param1.ToInt();
		Object obj = GetGame().GetObjectByNetworkID(netID);
		Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.SetName(player, flag, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SetRadius(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (type != CallType.Server) return;
		Param2<string, float> data;
		if (!ctx.Read(data)) return;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		int netID = data.param1.ToInt();
		Object obj = GetGame().GetObjectByNetworkID(netID);
		Land_Construction_Flag_Floor flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.SetRadius(player, flag, data.param2);
	}
}
