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

		TerritoryManager.GetInstance().Load();
		Print("[TerritoryFlags] RPC handlers registered, data loaded");
	}

	//------------------------------------------------------------------------------------------------------------------
	void ClaimFlag(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> data;
		PlayerBase player;
		int netID;
		Object obj;
		Land_Construction_Flag_Floor flag;

		if (type != CallType.Server) return;
		if (!ctx.Read(data)) return;

		player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		netID = data.param1.ToInt();
		obj = GetGame().GetObjectByNetworkID(netID);
		flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.ClaimFlag(player, flag);
	}

	//------------------------------------------------------------------------------------------------------------------
	void InvitePlayer(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param2<string, string> data;
		PlayerBase player;
		int netID;
		Object obj;
		Land_Construction_Flag_Floor flag;

		if (type != CallType.Server) return;
		if (!ctx.Read(data)) return;

		player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		netID = data.param1.ToInt();
		obj = GetGame().GetObjectByNetworkID(netID);
		flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.InvitePlayer(player, flag, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void RemoveInvite(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param2<string, string> data;
		PlayerBase player;
		int netID;
		Object obj;
		Land_Construction_Flag_Floor flag;

		if (type != CallType.Server) return;
		if (!ctx.Read(data)) return;

		player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		netID = data.param1.ToInt();
		obj = GetGame().GetObjectByNetworkID(netID);
		flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.RemoveInvite(player, flag, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SetName(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param2<string, string> data;
		PlayerBase player;
		int netID;
		Object obj;
		Land_Construction_Flag_Floor flag;

		if (type != CallType.Server) return;
		if (!ctx.Read(data)) return;

		player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		netID = data.param1.ToInt();
		obj = GetGame().GetObjectByNetworkID(netID);
		flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.SetName(player, flag, data.param2);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SetRadius(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param2<string, float> data;
		PlayerBase player;
		int netID;
		Object obj;
		Land_Construction_Flag_Floor flag;

		if (type != CallType.Server) return;
		if (!ctx.Read(data)) return;

		player = PlayerBase.Cast(GetGame().GetPlayerByIdentity(sender));
		if (!player) return;

		netID = data.param1.ToInt();
		obj = GetGame().GetObjectByNetworkID(netID);
		flag = Land_Construction_Flag_Floor.Cast(obj);
		if (!flag) return;

		TerritoryRPC.SetRadius(player, flag, data.param2);
	}
}