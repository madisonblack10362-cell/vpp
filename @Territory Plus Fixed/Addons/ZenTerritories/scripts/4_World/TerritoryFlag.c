modded class TerritoryFlag extends BaseBuildingBase
{
	// Store territory flag locations
	static ref array<vector> FLAG_LOCATIONS = new ref array<vector>;

	protected bool m_CanAddMember		= false;
	protected bool m_AwaitingReset		= false;
	protected string m_TerritoryOwner	= "";
	protected bool m_isRequestingSync	= false;
	protected int m_LastSyncTime		= 0;
	ref ZenTerritoryMembers m_TerritoryMembers = new ZenTerritoryMembers;
	
	void TerritoryFlag()
	{
		RegisterNetSyncVariableBool("m_CanAddMember");
	}

	void ~TerritoryFlag()
	{
		if (m_isRequestingSync)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.DoFirstSync);
		}
	}
	
	override void EEInit()
	{
		super.EEInit();

		if (GetGame().IsClient())
		{
			DayZPlayer player = DayZPlayer.Cast(GetGame().GetPlayer());
			if (player)
			{
				m_isRequestingSync = true;
				int Distance = vector.Distance(player.GetPosition(), GetPosition()) + 1;
				int Time = Distance * 5;
				GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.DoFirstSync, Time, false);
			}

			AnimateFlag(1 - GetRefresherTime01());
		}
	}

	TStringArray TerritoryMembers()
	{
		return m_TerritoryMembers.GetMemberArray();
	}

	void DoFirstSync()
	{
		m_isRequestingSync = false;
		SyncTerritory();
	}

	bool IsTerritoryOwner(string guid)
	{
		if (!m_TerritoryOwner || m_TerritoryOwner == "")
		{
			return false;
		}

		return (m_TerritoryOwner == guid);
	}
	
	bool CanReceiveNewOwner()
	{
		if (!m_TerritoryOwner || m_TerritoryOwner == "")
		{
			return true;
		}

		return false;
	}
	
	int GetMemberCount()
	{
		return TerritoryMembers().Count();
	}
	
	void ResetMembers()
	{
		Print("[ZenTerritories] ResetMembers m_TerritoryOwner: " + m_TerritoryOwner);
		m_TerritoryMembers.Clear();
		SyncTerritory();
	}
	
	bool CanAddMember()
	{
		return m_CanAddMember;
	}
	
	void SetTerritoryOwner(string guid)
	{
		m_TerritoryOwner = guid;
		SyncTerritory();
	}
	
	void AllowMemberToBeAdded(bool state = true)
	{
		m_CanAddMember = state;
		if (m_AwaitingReset && GetGame().IsServer())
		{
			m_AwaitingReset = false;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.ResetAllowMemberToBeAdded);
		}

		if (state && GetGame().IsServer())
		{
			m_AwaitingReset = true;
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(this.ResetAllowMemberToBeAdded, 300 * 1000);
		}

		SetSynchDirty();
	}
	
	void ResetAllowMemberToBeAdded()
	{
		m_AwaitingReset = false;
		m_CanAddMember = false;
		SetSynchDirty();
	}
	
	void AddMember(string guid)
	{
		if (guid == m_TerritoryOwner)
		{
			return;
		} else 
		{
			m_TerritoryMembers.AddMember(guid);
			AllowMemberToBeAdded(false);
			SyncTerritory();
		}
	}
	
	void RemoveMember(string guid)
	{
		if (guid == m_TerritoryOwner)
		{
			return;
		} else 
		{
			m_TerritoryMembers.RemoveMember(guid);
			SyncTerritory();
		}
	}
	
	bool IsTerritoryMember(string guid)
	{
		if (m_TerritoryOwner == "")
		{
			return true;
		} else 
		if (guid == m_TerritoryOwner)
		{
			return true;
		} else 
		if (m_TerritoryMembers.CheckId(guid))
		{
			return true;
		} else 
		{
			return false;
		}
	}
	
	void AddMemberClient(string guid)
	{
		if (CanAddMember())
		{
			RPCSingleParam(ZENTERRITORYRPCs.ADD_MEMBER, new Param2< string, ZenTerritoryMembers >(guid, m_TerritoryMembers), true, NULL);
		}
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{   
		super.OnStoreSave(ctx);
		ctx.Write(m_TerritoryOwner);
		ctx.Write(m_TerritoryMembers);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{
		if (!super.OnStoreLoad(ctx, version)) 
		{
			return false;
		}

		if (!ctx.Read(m_TerritoryOwner)) 
		{
			return false;
		}
		
		if (!ctx.Read(m_TerritoryMembers)) 
		{
			return false;
		}
		
		if (GetGame().IsServer())
		{
			Print("[ZenTerritories] m_TerritoryOwner: " + m_TerritoryOwner + " Pos: " + GetPosition());
		}
		
		return true;
	}
	
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();

		if (GetGame().IsDedicatedServer())
		{
			FLAG_LOCATIONS.Insert(GetPosition());
		}
	}

	void SyncTerritoryRateLimited()
	{
		if (GetGame().IsServer()) 
		{
			return;
		}

		int curTime = GetGame().GetTime();
		if (m_LastSyncTime < curTime)
		{
			m_LastSyncTime = curTime + 100000;
			SyncTerritory();
		}
	}
	
	void SyncTerritory(PlayerIdentity identity = NULL)
	{
		if (GetGame().IsServer()) 
		{
			SetSynchDirty();
			RPCSingleParam(ZENTERRITORYRPCs.SEND_DATA, new Param2< string, ZenTerritoryMembers >(m_TerritoryOwner, m_TerritoryMembers), true, identity);
		} else 
		if (GetGame().IsClient())
		{
			RPCSingleParam(ZENTERRITORYRPCs.REQUEST_DATA, new Param1<bool>(true), true, NULL);
		}
	}
	
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		Param2< string, ZenTerritoryMembers > data;
		if (rpc_type == ZENTERRITORYRPCs.SEND_DATA && GetGame().IsClient()) 
		{
			if (ctx.Read(data))	
			{
				m_TerritoryOwner = data.param1;
				m_TerritoryMembers = ZenTerritoryMembers.Cast(data.param2);
			}

			return;
		}

		if (rpc_type == ZENTERRITORYRPCs.REQUEST_DATA && GetGame().IsServer()) 
		{
			if (sender)
			{
				Print("[ZenTerritories] SyncTerritory Request From Client (" + sender.GetId() +") " + sender.GetName());
				SyncTerritory(sender);
			} else 
			{
				Print("[ZenTerritories] SyncTerritory Request From Client With No sender");
				SyncTerritory();
			}

			return;
		}

		if (rpc_type == ZENTERRITORYRPCs.ADD_MEMBER && GetGame().IsServer())
		{
			if (ctx.Read(data))
			{
				if (CanAddMember() && sender)
				{
					if (TerritoryMembers().Count() == data.param2.Count() && sender.GetId() == data.param1) // To ensure client and server were in sync
					{ 
						AddMember(data.param1);
						AllowMemberToBeAdded(false);
					}

					SyncTerritory();
				}
			}

			return;
		}
	}
		
	bool HasRaisedFlag()
	{
		if (FindAttachmentBySlotName("Material_FPole_Flag"))
		{
			float state = GetAnimationPhase("flag_mast");
			if (state <= TerritoryConst.FLAGDOWNSTATE)
			{
				return true;
			}
		}

		return false;
	}
		
	bool CheckPlayerPermission(string guid, int permission)
	{
		int publicPerms = GetZenTerritoriesConfig().PublicPermission();
		int PermsCheck = publicPerms & permission;

		if (PermsCheck == permission) // Has public perms
		{ 
			return true;
		}
		
		if (GetZenTerritoriesConfig().ServerAdmins.Find(guid) != -1) // Is server admin
		{
			return true;
		}
		
		if (guid == m_TerritoryOwner) // Is Owner
		{ 
			return true;
		}
		
		if (HasRaisedFlag()) // Is member or not
		{
			return m_TerritoryMembers.Check(guid, permission); 
		}

		return true;
	}
	
	
	override void SetActions()
	{
		super.SetActions();

		// Custom flag actions
		AddAction(ActionAddTerritoryMember);
		AddAction(ActionResetTerritoryMembers);
		AddAction(ActionAcceptMembership);
	}

	static vector m_LastCheckLocation = vector.Zero;
	static int m_LastCheckLocationNextTime = 0;
	static bool m_CachedHasTerritoryPerm = false;
	
	static bool HasTerritoryPermAtPos(string GUID, int Perm, vector Pos, bool CheckTerritoryOverlap = false)
	{
		if (GetGame().IsDedicatedServer()) // To Pervent Lag Server side from stuff being placed. I know not super secure but good enough
		{ 
			return true;
		}

		if (GUID == "")
		{
			return false;
		}

		int curTime = GetGame().GetTime();
		if (vector.Distance(m_LastCheckLocation, Pos) <= 0.05 && m_LastCheckLocationNextTime > curTime)
		{
			return m_CachedHasTerritoryPerm;
		}

		m_LastCheckLocation = Pos;
		m_LastCheckLocationNextTime = curTime + 5000;
		if (Pos == vector.Zero)
		{
			m_CachedHasTerritoryPerm = false;
			return m_CachedHasTerritoryPerm;
		} else 
		{
			array<Object> objects = new array<Object>;
			array<CargoBase> proxyCargos = new array<CargoBase>;
			float theRadius = GameConstants.REFRESHER_RADIUS;

			if (CheckTerritoryOverlap) 
			{
				theRadius = GameConstants.REFRESHER_RADIUS * 2;
			}

			GetGame().GetObjectsAtPosition(Pos, theRadius, objects, proxyCargos);
			TerritoryFlag theFlag;

			if (objects)
			{
				for (int i = 0; i < objects.Count(); i++)
				{
					if (Class.CastTo(theFlag, objects.Get(i)))
					{
						theFlag.SyncTerritoryRateLimited();
						m_CachedHasTerritoryPerm = theFlag.CheckPlayerPermission(GUID, Perm) && !CheckTerritoryOverlap;
						return m_CachedHasTerritoryPerm;
					}
				}
			}
		}

		m_CachedHasTerritoryPerm = true;
		return m_CachedHasTerritoryPerm;
	}
}