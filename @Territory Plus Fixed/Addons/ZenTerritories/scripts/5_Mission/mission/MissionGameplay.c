modded class MissionGameplay extends MissionBase
{	
	void MissionGameplay() 
	{
		GetRPCManager().AddRPC("ZENTERRITORY", "RPCZenTerritoriesModSettings", this, SingeplayerExecutionType.Both);
	}
	
	override void OnMissionStart()
	{
		super.OnMissionStart();
		GetRPCManager().SendRPC("ZENTERRITORY", "RPCZenTerritoriesModSettings", new Param1< ZenTerritoriesConfig >(NULL), true, NULL);
	}

	void RPCZenTerritoriesModSettings(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target) 
	{
		Param1< ZenTerritoriesConfig > data; 
		if (!ctx.Read(data)) 
			return;

		m_ZenTerritoriesConfig = data.param1;
		
		#ifdef BASICMAP
		if (GetZenTerritoriesConfig().NoBuildZones)
		{
			BasicMap().ClearMarkers("TerritoryNoBuildZones");
			bool SomeZonesOnTheMap = false;

			for (int i = 0; i < GetZenTerritoriesConfig().NoBuildZones.Count(); i++)
			{
				if (GetZenTerritoriesConfig().NoBuildZones.Get(i) && GetZenTerritoriesConfig().NoBuildZones.Get(i).DrawOnMap)
				{
					BasicMapCircleMarker tmpMarker = new BasicMapCircleMarker("", GetZenTerritoriesConfig().NoBuildZones.Get(i).GetPos(), "ZenTerritories\\images\\NoBuild.paa", {189, 38, 78},150);
					tmpMarker.SetRadius(GetZenTerritoriesConfig().NoBuildZones.Get(i).R);
					tmpMarker.SetShowCenterMarker(true);
					tmpMarker.SetHideIntersects(true);
					tmpMarker.SetCanEdit(false);
					tmpMarker.SetGroup("TerritoryNoBuildZones");
					BasicMap().AddMarker("TerritoryNoBuildZones",tmpMarker);
					SomeZonesOnTheMap = true;
				}
			}

			if (SomeZonesOnTheMap)
			{
				BasicMap().RegisterGroup("TerritoryNoBuildZones", new BasicMapGroupMetaData("TerritoryNoBuildZones", "No Build Areas"), NULL);
			}
		}
		#endif
	}
}