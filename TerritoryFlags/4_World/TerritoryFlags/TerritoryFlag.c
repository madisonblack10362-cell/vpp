modded class TerritoryFlag extends BaseBuildingBase
{
	static ref array<vector> FLAG_LOCATIONS = new ref array<vector>;

	protected bool m_CanAddMember = false;
	protected bool m_AwaitingReset = false;
	protected string m_TerritoryOwner = "";
	protected ref TerritoryMembers m_TerritoryMembers = new TerritoryMembers;

	void TerritoryFlag()
	{
		RegisterNetSyncVariableBool("m_CanAddMember");
	}

	void ~TerritoryFlag()
	{
		if (m_AwaitingReset)
		{
			GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(this.ResetAllowMemberToBeAdded);
		}
	}

	//----------------------------------------------------------------------------------------------
	string GetTerritoryOwner()
	{
		return m_TerritoryOwner;
	}

	//----------------------------------------------------------------------------------------------
	bool IsTerritoryOwner(string guid)
	{
		if (!m_TerritoryOwner || m_TerritoryOwner == "")
		{
			return false;
		}

		return m_TerritoryOwner == guid;
	}

	//----------------------------------------------------------------------------------------------
	bool CanReceiveNewOwner()
	{
		if (!m_TerritoryOwner || m_TerritoryOwner == "")
		{
			return true;
		}

		return false;
	}

	//----------------------------------------------------------------------------------------------
	int GetMemberCount()
	{
		return m_TerritoryMembers.Count();
	}

	//----------------------------------------------------------------------------------------------
	bool IsTerritoryMember(string guid)
	{
		if (m_TerritoryOwner == "")
		{
			return true;
		}

		if (guid == m_TerritoryOwner)
		{
			return true;
		}

		if (m_TerritoryMembers.CheckId(guid))
		{
			return true;
		}

		return false;
	}

	//----------------------------------------------------------------------------------------------
	void SetTerritoryOwner(string guid)
	{
		m_TerritoryOwner = guid;
		SyncTerritory();
	}

	//----------------------------------------------------------------------------------------------
	void AddMember(string guid)
	{
		if (guid == m_TerritoryOwner)
		{
			return;
		}

		m_TerritoryMembers.AddMember(guid);
		AllowMemberToBeAdded(false);
		SyncTerritory();
	}

	//----------------------------------------------------------------------------------------------
	void RemoveMember(string guid)
	{
		if (guid == m_TerritoryOwner)
		{
			return;
		}

		m_TerritoryMembers.RemoveMember(guid);
		SyncTerritory();
	}

	//----------------------------------------------------------------------------------------------
	bool CanAddMember()
	{
		return m_CanAddMember;
	}

	//----------------------------------------------------------------------------------------------
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

	//----------------------------------------------------------------------------------------------
	void ResetAllowMemberToBeAdded()
	{
		m_AwaitingReset = false;
		m_CanAddMember = false;
		SetSynchDirty();
	}

	//----------------------------------------------------------------------------------------------
	void ResetMembers()
	{
		Print("[TerritoryFlags] ResetMembers m_TerritoryOwner: " + m_TerritoryOwner);
		m_TerritoryMembers.Clear();
		SyncTerritory();
	}

	//----------------------------------------------------------------------------------------------
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

	//----------------------------------------------------------------------------------------------
	bool CheckPlayerPermission(string guid, int permission)
	{
		if (guid == m_TerritoryOwner)
		{
			return true;
		}

		if (HasRaisedFlag())
		{
			return m_TerritoryMembers.CheckPermission(guid, permission);
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------
	void SyncTerritory(PlayerIdentity identity = NULL)
	{
		if (GetGame().IsServer())
		{
			SetSynchDirty();
			RPCSingleParam(RPC_TERRITORY_SEND_DATA, new Param2<string, TerritoryMembers>(m_TerritoryOwner, m_TerritoryMembers), true, identity);
		}
		else
		{
			RPCSingleParam(RPC_TERRITORY_REQUEST_DATA, new Param1<bool>(true), true, NULL);
		}
	}

	//----------------------------------------------------------------------------------------------
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);

		Param2<string, TerritoryMembers> data;

		if (rpc_type == RPC_TERRITORY_SEND_DATA && GetGame().IsClient())
		{
			if (ctx.Read(data))
			{
				m_TerritoryOwner = data.param1;
				m_TerritoryMembers = TerritoryMembers.Cast(data.param2);
			}

			return;
		}

		if (rpc_type == RPC_TERRITORY_REQUEST_DATA && GetGame().IsServer())
		{
			if (sender)
			{
				Print("[TerritoryFlags] SyncTerritory request from " + sender.GetName());
				SyncTerritory(sender);
			}
			else
			{
				SyncTerritory();
			}

			return;
		}

		if (rpc_type == RPC_TERRITORY_ADD_MEMBER && GetGame().IsServer())
		{
			if (ctx.Read(data))
			{
				if (CanAddMember() && sender)
				{
					AddMember(data.param1);
					AllowMemberToBeAdded(false);
				}

				SyncTerritory();
			}

			return;
		}
	}

	//----------------------------------------------------------------------------------------------
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		ctx.Write(m_TerritoryOwner);
		ctx.Write(m_TerritoryMembers);
	}

	//----------------------------------------------------------------------------------------------
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
			Print("[TerritoryFlags] Loaded owner: " + m_TerritoryOwner + " pos: " + GetPosition().ToString());
		}

		return true;
	}

	//----------------------------------------------------------------------------------------------
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();

		if (GetGame().IsDedicatedServer())
		{
			FLAG_LOCATIONS.Insert(GetPosition());
		}
	}

	//----------------------------------------------------------------------------------------------
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionTerritoryClaim);
		AddAction(ActionTerritoryInvite);
		AddAction(ActionTerritoryJoin);
		AddAction(ActionTerritoryClearMembers);
	}

	//----------------------------------------------------------------------------------------------
	static bool HasTerritoryPermAtPos(string guid, int perm, vector pos)
	{
		if (GetGame().IsDedicatedServer())
		{
			return true;
		}

		if (guid == "")
		{
			return false;
		}

		if (pos == vector.Zero)
		{
			return false;
		}

		array<Object> objects = new array<Object>;
		array<CargoBase> proxyCargos = new array<CargoBase>;
		GetGame().GetObjectsAtPosition(pos, TerritoryConst.RADIUS, objects, proxyCargos);
		TerritoryFlag theFlag;

		if (objects)
		{
			for (int i = 0; i < objects.Count(); i++)
			{
				if (Class.CastTo(theFlag, objects.Get(i)))
				{
					if (theFlag.HasRaisedFlag())
					{
						return theFlag.CheckPlayerPermission(guid, perm);
					}
				}
			}
		}

		return true;
	}
}
