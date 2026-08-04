//----------------------------------------------------------------------------------------------------------------------------------
// TerritoryMenu — клиентское меню управления территорией флага
//----------------------------------------------------------------------------------------------------------------------------------

class TerritoryMenu extends ScriptedWidgetEventHandler
{
	protected Widget      m_Root;
	protected Widget      m_HeaderPanel;
	protected TextWidget   m_TitleText;
	protected ButtonWidget m_BtnClose;

	// Claim
	protected Widget      m_ClaimPanel;
	protected ButtonWidget m_BtnClaim;

	// Main panel
	protected Widget      m_MainPanel;
	protected EditBoxWidget m_NameInput;
	protected EditBoxWidget m_RadiusInput;
	protected EditBoxWidget m_InviteInput;
	protected ButtonWidget m_BtnSetName;
	protected ButtonWidget m_BtnSetRadius;
	protected ButtonWidget m_BtnInvite;
	protected TextListboxWidget m_PlayerList;
	protected ButtonWidget m_BtnRemoveInvite;

	// State
	protected string  m_FlagNetID;  // текущий networkID флага (для RPC)
	protected bool    m_IsOwner;
	protected bool    m_IsClaimed;

	// Отдельный массив steamID для списка игроков (т.к. TextListboxWidget.AddItem не принимает string data)
	protected ref array<string> m_InvitedSteamIDs = new array<string>;

	void TerritoryMenu()
	{
		GetRPCManager().AddRPC("RPC_TerritoryFlags", "OpenMenu", this, SingleplayerExecutionType.Client);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Show(Widget parent)
	{
		if (m_Root) { Hide(); }

		// Путь ОТНОСИТЕЛЬНО КОРНЯ PBO (не включая имя мода!)
		m_Root = GetGame().GetWorkspace().CreateWidgets("GUI/Layouts/TerritoryMenu/TerritoryMenu.layout");
		if (!m_Root) { Print("[TerritoryMenu] ERROR: Failed to create layout"); return; }

		m_Root.SetHandler(this);

		m_HeaderPanel = m_Root.FindAnyWidget("HeaderPanel");
		m_TitleText  = TextWidget.Cast(m_Root.FindAnyWidget("TitleText"));
		m_BtnClose   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnClose"));

		m_ClaimPanel = m_Root.FindAnyWidget("ClaimPanel");
		m_BtnClaim   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnClaim"));

		m_MainPanel    = m_Root.FindAnyWidget("MainPanel");
		m_NameInput    = EditBoxWidget.Cast(m_Root.FindAnyWidget("NameInput"));
		m_RadiusInput  = EditBoxWidget.Cast(m_Root.FindAnyWidget("RadiusInput"));
		m_InviteInput  = EditBoxWidget.Cast(m_Root.FindAnyWidget("InviteInput"));
		m_BtnSetName   = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSetName"));
		m_BtnSetRadius = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSetRadius"));
		m_BtnInvite    = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnInvite"));
		m_PlayerList   = TextListboxWidget.Cast(m_Root.FindAnyWidget("PlayerList"));
		m_BtnRemoveInvite = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnRemoveInvite"));

		m_MainPanel.Show(false);
		m_ClaimPanel.Show(true);

		ApplyStyles();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Hide()
	{
		if (m_Root)
		{
			m_Root.Unlink();
			delete m_Root;
			m_Root = null;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void OpenMenu(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		ParseAndUpdate(data.param1);
	}

	//------------------------------------------------------------------------------------------------------------------
	void ParseAndUpdate(string data)
	{
		if (!m_Root) Show(null);

		// Формат: netID|ownerID|ownerName|territoryName|radius|claimed|isOwner|invitedCount|id1:name1;id2:name2
		array<string> parts = new array<string>;
		data.Split("|", parts);

		if (parts.Count() < 7) return;

		m_FlagNetID = parts[0];  // ТЕКУЩИЙ networkID для обратных RPC
		string ownerID = parts[1];
		string ownerName = parts[2];
		string terrName = parts[3];
		float radius = parts[4].ToFloat();
		int claimed  = parts[5].ToInt();
		int isOwner  = parts[6].ToInt();

		m_IsClaimed = (claimed == 1);
		m_IsOwner = (isOwner == 1);

		if (!m_IsClaimed)
		{
			m_ClaimPanel.Show(true);
			m_MainPanel.Show(false);
			if (m_TitleText) m_TitleText.SetText("Флаг - Не занят");
			return;
		}

		m_ClaimPanel.Show(false);
		m_MainPanel.Show(true);

		if (m_TitleText) m_TitleText.SetText("Территория: " + terrName);
		if (m_NameInput) m_NameInput.SetText(terrName);
		if (m_RadiusInput) m_RadiusInput.SetText(radius.ToString());

		// Парсим список приглашённых
		m_PlayerList.ClearItems();
		m_InvitedSteamIDs.Clear();
		if (parts.Count() >= 9)
		{
			int invCount = parts[7].ToInt();
			if (invCount > 0)
			{
				string invitedStr = parts[8];
				array<string> pairs = new array<string>;
				invitedStr.Split(";", pairs);
				foreach (string pair : pairs)
				{
					array<string> kv = new array<string>;
					pair.Split(":", kv);
					if (kv.Count() >= 2)
					{
						m_PlayerList.AddItem(kv[1], 0, 0, 0);
						m_InvitedSteamIDs.Insert(kv[0]);
					}
				}
			}
		}

		bool ownerControls = m_IsOwner;
		if (m_NameInput) m_NameInput.Enable(ownerControls);
		if (m_RadiusInput) m_RadiusInput.Enable(ownerControls);
		if (m_BtnSetName) m_BtnSetName.Show(ownerControls);
		if (m_BtnSetRadius) m_BtnSetRadius.Show(ownerControls);
		if (m_BtnInvite) m_BtnInvite.Show(ownerControls);
		if (m_BtnRemoveInvite) m_BtnRemoveInvite.Show(ownerControls);
	}

	//------------------------------------------------------------------------------------------------------------------
	override bool OnClick(Widget w, int x, int y, int button)
	{
		if (w == m_BtnClose)
		{
			Hide();
			return true;
		}
		if (w == m_BtnClaim)
		{
			// Отправляем ТЕКУЩИЙ networkID чтобы сервер нашёл флаг
			GetRPCManager().SendRPC("RPC_TerritoryFlags", "ClaimFlag", new Param1<string>(m_FlagNetID), true);
			return true;
		}
		if (w == m_BtnSetName && m_NameInput)
		{
			GetRPCManager().SendRPC("RPC_TerritoryFlags", "SetName", new Param2<string, string>(m_FlagNetID, m_NameInput.GetText()), true);
			return true;
		}
		if (w == m_BtnSetRadius && m_RadiusInput)
		{
			float r = m_RadiusInput.GetText().ToFloat();
			GetRPCManager().SendRPC("RPC_TerritoryFlags", "SetRadius", new Param2<string, float>(m_FlagNetID, r), true);
			return true;
		}
		if (w == m_BtnInvite && m_InviteInput)
		{
			string name = m_InviteInput.GetText();
			if (name.Length() > 0)
			{
				GetRPCManager().SendRPC("RPC_TerritoryFlags", "InvitePlayer", new Param2<string, string>(m_FlagNetID, name), true);
				m_InviteInput.SetText("");
			}
			return true;
		}
		if (w == m_BtnRemoveInvite)
		{
			int row = m_PlayerList.GetSelectedRow();
			if (row >= 0 && row < m_InvitedSteamIDs.Count())
			{
				string targetID = m_InvitedSteamIDs[row];
				GetRPCManager().SendRPC("RPC_TerritoryFlags", "RemoveInvite", new Param2<string, string>(m_FlagNetID, targetID), true);
			}
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	void ApplyStyles()
	{
		if (!m_Root) return;

		m_Root.SetPos(0.5, 0.3);
		m_Root.SetSize(0.35, 0.45);
		m_Root.SetColor(ARGB(220, 20, 20, 20));

		if (m_HeaderPanel)
		{
			m_HeaderPanel.SetPos(0, 0);
			m_HeaderPanel.SetSize(1, 0.08);
			m_HeaderPanel.SetColor(ARGB(255, 40, 40, 40));
		}

		if (m_TitleText)
		{
			m_TitleText.SetPos(0.02, 0.01);
			m_TitleText.SetSize(0.8, 0.06);
			m_TitleText.SetTextSize(16);
			m_TitleText.SetColor(ARGB(255, 255, 255, 255));
			m_TitleText.SetText("Территория");
		}

		if (m_BtnClose)
		{
			m_BtnClose.SetPos(0.9, 0.01);
			m_BtnClose.SetSize(0.08, 0.06);
			m_BtnClose.SetText("X");
			m_BtnClose.SetColor(ARGB(255, 180, 40, 40));
			m_BtnClose.SetTextColor(ARGB(255, 255, 255, 255));
		}

		StyleButton(m_BtnClaim, 0.1, 0.15, 0.8, 0.15, "ЗАХВАТИТЬ ТЕРРИТОРИЮ", ARGB(255, 40, 120, 40));

		float rowY = 0.03;
		StyleRow(m_NameInput, m_BtnSetName, rowY, "Название:", "Применить");
		StyleRow(m_RadiusInput, m_BtnSetRadius, rowY + 0.12, "Радиус (20-300м):", "Применить");
		StyleRow(m_InviteInput, m_BtnInvite, rowY + 0.24, "Пригласить игрока:", "Пригласить", ARGB(255, 40, 100, 160));

		if (m_PlayerList)
		{
			m_PlayerList.SetPos(0.05, 0.45);
			m_PlayerList.SetSize(0.9, 0.35);
			m_PlayerList.SetColor(ARGB(200, 30, 30, 30));
			m_PlayerList.SetTextColor(ARGB(255, 220, 220, 220));
		}

		StyleButton(m_BtnRemoveInvite, 0.3, 0.85, 0.4, 0.08, "Удалить выбранного", ARGB(255, 160, 40, 40));
	}

	void StyleRow(EditBoxWidget input, ButtonWidget btn, float y, string labelText, string btnText, int btnColor = 0)
	{
		if (!input || !btn) return;
		input.SetPos(0.25, y);
		input.SetSize(0.5, 0.08);
		input.SetColor(ARGB(255, 40, 40, 40));
		input.SetTextColor(ARGB(255, 220, 220, 220));

		btn.SetPos(0.78, y);
		btn.SetSize(0.15, 0.08);
		btn.SetText(btnText);
		if (btnColor != 0) btn.SetColor(btnColor);
		else btn.SetColor(ARGB(255, 60, 60, 80));
		btn.SetTextColor(ARGB(255, 255, 255, 255));
	}

	void StyleButton(ButtonWidget btn, float x, float y, float w, float h, string text, int color)
	{
		if (!btn) return;
		btn.SetPos(x, y);
		btn.SetSize(w, h);
		btn.SetText(text);
		btn.SetColor(color);
		btn.SetTextColor(ARGB(255, 255, 255, 255));
	}
}

//------------------------------------------------------------------------------------------------
static ref TerritoryMenu g_TerritoryMenu;

TerritoryMenu GetTerritoryMenu()
{
	if (!g_TerritoryMenu) g_TerritoryMenu = new TerritoryMenu();
	return g_TerritoryMenu;
}