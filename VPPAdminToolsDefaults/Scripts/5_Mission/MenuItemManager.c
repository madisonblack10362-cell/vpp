//------------------------------------------------------------------------------------------------
// VPP Admin Tools — Override default Item Manager values
//   Quantity:  1000 (instead of MAX)
//   Spawn in:  On Ground / Na zemle (instead of In Inventory)
//------------------------------------------------------------------------------------------------

modded class MenuItemManager
{
	override void OnMenuShow()
	{
		super.OnMenuShow();

		// Quantity: 1000 instead of MAX
		if (m_InputQuantity)
			m_InputQuantity.SetText("1000");

		// Placement dropdown: ON_GROUND (index 1) instead of IN_INVENTORY (index 0)
		if (m_PlacementDropDown)
		{
			m_PlacementDropDown.SetText(m_PlacementLabels[1]);
			m_PlacementDropDown.SetIndex(1);
		}
	}
};
