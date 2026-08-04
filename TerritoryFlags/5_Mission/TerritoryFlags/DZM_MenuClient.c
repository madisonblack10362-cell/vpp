class DZM_TerritoryMenu extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected TextWidget m_TitleText;
	protected ButtonWidget m_BtnClose;
	protected Widget m_ClaimPanel;
	protected ButtonWidget m_BtnClaim;
	protected Widget m_MainPanel;
	protected EditBoxWidget m_NameInput;
	protected EditBoxWidget m_RadiusInput;
	protected EditBoxWidget m_InviteInput;
	protected ButtonWidget m_BtnSetName;
	protected ButtonWidget m_BtnSetRadius;
	protected ButtonWidget m_BtnInvite;
	protected TextListboxWidget m_PlayerList;
	protected ButtonWidget m_BtnKick;
	protected string m_FlagNetID;
	protected bool m_IsOwner;
	protected bool m_IsClaimed;
	protected ref array<string> m_FriendIDs = new array<string>;

	void DZM_TerritoryMenu()
	{
		GetRPCManager().AddRPC("DZM_TF", "OpenMenu", this, SingleplayerExecutionType.Client);
	}

	void Show()
	{
		if (m_Root) { Hide(); }
		m_Root = GetGame().GetWorkspace().CreateWidgets("GUI/Layouts/TerritoryMenu/TerritoryMenu.layout");
		if (!m_Root) { Print("[DZM] layout not found"); return; }
		m_Root.SetHandler(this);
		m_TitleText = TextWidget.Cast(m_Root.FindAnyWidget("TitleText"));
		m_BtnClose = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnClose"));
		m_ClaimPanel = m_Root.FindAnyWidget("ClaimPanel");
		m_BtnClaim = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnClaim"));
		m_MainPanel = m_Root.FindAnyWidget("MainPanel");
		m_NameInput = EditBoxWidget.Cast(m_Root.FindAnyWidget("NameInput"));
		m_RadiusInput = EditBoxWidget.Cast(m_Root.FindAnyWidget("RadiusInput"));
		m_InviteInput = EditBoxWidget.Cast(m_Root.FindAnyWidget("InviteInput"));
		m_BtnSetName = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSetName"));
		m_BtnSetRadius = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnSetRadius"));
		m_BtnInvite = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnInvite"));
		m_PlayerList = TextListboxWidget.Cast(m_Root.FindAnyWidget("PlayerList"));
		m_BtnKick = ButtonWidget.Cast(m_Root.FindAnyWidget("BtnKick"));
		m_MainPanel.Show(false);
		m_ClaimPanel.Show(true);
		DZM_ApplyStyles();
	}

	void Hide()
	{
		if (m_Root)
		{
			m_Root.Unlink();
			delete m_Root;
			m_Root = null;
		}
	}

	void OpenMenu(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> data;
		if (!ctx.Read(data)) return;
		DZM_ParseData(data.param1);
	}

	void DZM_ParseData(string data)
	{
		array<string> parts;
		string ownerID;
		string ownerName;
		string terrName;
		float radius;
		int claimed;
		int isOwner;
		int friendCount;
		string friendsStr;
		array<string> friendArr;
		int i;

		if (!m_Root) Show();
		parts = new array<string>;
		data.Split("|", parts);
		if (parts.Count() < 8) return;

		m_FlagNetID = parts[0];
		ownerID = parts[1];
		ownerName = parts[2];
		terrName = parts[3];
		radius = parts[4].ToFloat();
		claimed = parts[5].ToInt();
		isOwner = parts[6].ToInt();

		if (claimed == 1)
		{
			m_IsClaimed = true;
		}
		else
		{
			m_IsClaimed = false;
		}

		if (isOwner == 1)
		{
			m_IsOwner = true;
		}
		else
		{
			m_IsOwner = false;
		}

		if (!m_IsClaimed)
		{
			m_ClaimPanel.Show(true);
			m_MainPanel.Show(false);
			if (m_TitleText) m_TitleText.SetText("Флаг - Свободен");
			return;
		}

		m_ClaimPanel.Show(false);
		m_MainPanel.Show(true);
		if (m_TitleText) m_TitleText.SetText("Территория: " + terrName);
		if (m_NameInput)
		{
			m_NameInput.SetText(terrName);
			m_NameInput.Enable(m_IsOwner);
		}
		if (m_RadiusInput)
		{
			m_RadiusInput.SetText(radius.ToString());
			m_RadiusInput.Enable(m_IsOwner);
		}
		if (m_BtnSetName) m_BtnSetName.Show(m_IsOwner);
		if (m_BtnSetRadius) m_BtnSetRadius.Show(m_IsOwner);
		if (m_BtnInvite) m_BtnInvite.Show(m_IsOwner);
		if (m_BtnKick) m_BtnKick.Show(m_IsOwner);
		if (m_InviteInput) m_InviteInput.Show(m_IsOwner);

		m_PlayerList.ClearItems();
		m_FriendIDs.Clear();
		friendCount = parts[7].ToInt();
		if (friendCount > 0 && parts.Count() >= 9)
		{
			friendsStr = parts[8];
			friendArr = new array<string>;
			friendsStr.Split(";", friendArr);
			for (i = 0; i < friendArr.Count(); i++)
			{
				m_PlayerList.AddItem(friendArr[i], 0, 0, 0);
				m_FriendIDs.Insert(friendArr[i]);
			}
		}
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		string name;
		float r;
		int row;
		string targetID;

		if (w == m_BtnClose)
		{
			Hide();
			return true;
		}
		if (w == m_BtnClaim)
		{
			GetRPCManager().SendRPC("DZM_TF", "DoClaimFlag", new Param1<string>(m_FlagNetID), true);
			return true;
		}
		if (w == m_BtnSetName && m_NameInput)
		{
			name = m_NameInput.GetText();
			GetRPCManager().SendRPC("DZM_TF", "DoSetName", new Param2<string, string>(m_FlagNetID, name), true);
			return true;
		}
		if (w == m_BtnSetRadius && m_RadiusInput)
		{
			r = m_RadiusInput.GetText().ToFloat();
			GetRPCManager().SendRPC("DZM_TF", "DoSetRadius", new Param3<string, string, float>(m_FlagNetID, "", r), true);
			return true;
		}
		if (w == m_BtnInvite && m_InviteInput)
		{
			name = m_InviteInput.GetText();
			if (name.Length() > 0)
			{
				GetRPCManager().SendRPC("DZM_TF", "DoInvitePlayer", new Param2<string, string>(m_FlagNetID, name), true);
				m_InviteInput.SetText("");
			}
			return true;
		}
		if (w == m_BtnKick)
		{
			row = m_PlayerList.GetSelectedRow();
			if (row >= 0 && row < m_FriendIDs.Count())
			{
				targetID = m_FriendIDs[row];
				GetRPCManager().SendRPC("DZM_TF", "DoKickPlayer", new Param2<string, string>(m_FlagNetID, targetID), true);
			}
			return true;
		}
		return false;
	}

	void DZM_ApplyStyles()
	{
		if (!m_Root) return;
		m_Root.SetPos(0.5, 0.3);
		m_Root.SetSize(0.35, 0.45);
		m_Root.SetColor(ARGB(220, 20, 20, 20));

		if (m_BtnClose)
		{
			m_BtnClose.SetPos(0.88, 0.01);
			m_BtnClose.SetSize(0.1, 0.06);
			m_BtnClose.SetText("X");
			m_BtnClose.SetColor(ARGB(255, 180, 40, 40));
			m_BtnClose.SetTextColor(ARGB(255, 255, 255, 255));
		}
		if (m_BtnClaim)
		{
			m_BtnClaim.SetPos(0.1, 0.2);
			m_BtnClaim.SetSize(0.8, 0.15);
			m_BtnClaim.SetText("ЗАХВАТИТЬ ТЕРРИТОРИЮ");
			m_BtnClaim.SetColor(ARGB(255, 40, 120, 40));
			m_BtnClaim.SetTextColor(ARGB(255, 255, 255, 255));
		}
		if (m_NameInput)
		{
			m_NameInput.SetPos(0.25, 0.05);
			m_NameInput.SetSize(0.5, 0.08);
			m_NameInput.SetColor(ARGB(255, 40, 40, 40));
			m_NameInput.SetTextColor(ARGB(255, 220, 220, 220));
		}
		if (m_BtnSetName)
		{
			m_BtnSetName.SetPos(0.78, 0.05);
			m_BtnSetName.SetSize(0.18, 0.08);
			m_BtnSetName.SetText("Применить");
			m_BtnSetName.SetColor(ARGB(255, 60, 60, 80));
			m_BtnSetName.SetTextColor(ARGB(255, 255, 255, 255));
		}
		if (m_RadiusInput)
		{
			m_RadiusInput.SetPos(0.25, 0.17);
			m_RadiusInput.SetSize(0.5, 0.08);
			m_RadiusInput.SetColor(ARGB(255, 40, 40, 40));
			m_RadiusInput.SetTextColor(ARGB(255, 220, 220, 220));
		}
		if (m_BtnSetRadius)
		{
			m_BtnSetRadius.SetPos(0.78, 0.17);
			m_BtnSetRadius.SetSize(0.18, 0.08);
			m_BtnSetRadius.SetText("Применить");
			m_BtnSetRadius.SetColor(ARGB(255, 60, 60, 80));
			m_BtnSetRadius.SetTextColor(ARGB(255, 255, 255, 255));
		}
		if (m_InviteInput)
		{
			m_InviteInput.SetPos(0.25, 0.29);
			m_InviteInput.SetSize(0.5, 0.08);
			m_InviteInput.SetColor(ARGB(255, 40, 40, 40));
			m_InviteInput.SetTextColor(ARGB(255, 220, 220, 220));
		}
		if (m_BtnInvite)
		{
			m_BtnInvite.SetPos(0.78, 0.29);
			m_BtnInvite.SetSize(0.18, 0.08);
			m_BtnInvite.SetText("Пригласить");
			m_BtnInvite.SetColor(ARGB(255, 40, 100, 160));
			m_BtnInvite.SetTextColor(ARGB(255, 255, 255, 255));
		}
		if (m_PlayerList)
		{
			m_PlayerList.SetPos(0.05, 0.45);
			m_PlayerList.SetSize(0.9, 0.35);
			m_PlayerList.SetColor(ARGB(200, 30, 30, 30));
			m_PlayerList.SetTextColor(ARGB(255, 220, 220, 220));
		}
		if (m_BtnKick)
		{
			m_BtnKick.SetPos(0.3, 0.85);
			m_BtnKick.SetSize(0.4, 0.08);
			m_BtnKick.SetText("Удалить выбранного");
			m_BtnKick.SetColor(ARGB(255, 160, 40, 40));
			m_BtnKick.SetTextColor(ARGB(255, 255, 255, 255));
		}
	}
};

static ref DZM_TerritoryMenu g_DZM_Menu;

DZM_TerritoryMenu DZM_GetMenu()
{
	if (!g_DZM_Menu) g_DZM_Menu = new DZM_TerritoryMenu();
	return g_DZM_Menu;
}
