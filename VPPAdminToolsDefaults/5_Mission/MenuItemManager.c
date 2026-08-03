//------------------------------------------------------------------------------------------------
// VPP Admin Tools — Override default Item Manager values
//   Quantity:  1000 (instead of MAX)
//   Spawn in:  On Ground (instead of In Inventory)
//------------------------------------------------------------------------------------------------

modded class MenuItemManager
{
	override void OnCreate(Widget RootW)
	{
		super.OnCreate(RootW);

		Print("[VPPDefaults] OnCreate fired");

		if (!M_SUB_WIDGET)
		{
			Print("[VPPDefaults] ERROR: M_SUB_WIDGET is null");
			return;
		}

		// === Quantity: 1000 instead of MAX ===
		EditBoxWidget qty = EditBoxWidget.Cast(M_SUB_WIDGET.FindAnyWidget("InputQuantity"));
		if (qty)
		{
			qty.SetText("1000");
			Print("[VPPDefaults] Quantity -> 1000");
		} else {
			Print("[VPPDefaults] ERROR: InputQuantity not found");
		}

		// === Placement dropdown: On Ground ===
		if (m_PlacementDropDown)
		{
			m_PlacementDropDown.SetIndex(1);
			Print("[VPPDefaults] Dropdown index -> 1");
		} else {
			Print("[VPPDefaults] WARNING: m_PlacementDropDown is null");
		}
	}
};
