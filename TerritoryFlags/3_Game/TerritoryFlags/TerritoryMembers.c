class TerritoryMembers
{
        ref map<string, int> m_Members;

        void TerritoryMembers()
        {
                m_Members = new map<string, int>;
        }

        bool CheckId(string guid)
        {
                if (!m_Members)
                {
                        m_Members = new map<string, int>;
                }

                if (m_Members.Contains(guid))
                {
                        return true;
                }

                return false;
        }

        bool AddMember(string guid, int permission = 0)
        {
                if (permission == 0)
                {
                        permission = TerritoryPerm.BUILD + TerritoryPerm.DEPLOY + TerritoryPerm.DISMANTLE + TerritoryPerm.LOWERFLAG;
                }

                if (!CheckId(guid))
                {
                        m_Members.Insert(guid, permission);
                        return true;
                }

                return false;
        }

        bool RemoveMember(string guid)
        {
                if (CheckId(guid))
                {
                        m_Members.Remove(guid);
                        return true;
                }

                return false;
        }

        bool CheckPermission(string guid, int permission)
        {
                if (CheckId(guid))
                {
                        int perms = m_Members.Get(guid);
                        if (perms == 1)
                        {
                                return true;
                        }

                        int CalcedPerms = perms & permission;
                        if (CalcedPerms == permission)
                        {
                                return true;
                        }
                }

                return false;
        }

        int Count()
        {
                if (!m_Members)
                {
                        return 0;
                }

                return m_Members.Count();
        }

        void Clear()
        {
                if (!m_Members)
                {
                        m_Members = new map<string, int>;
                }
                else
                {
                        m_Members.Clear();
                }
        }

        array<string> GetMemberArray()
        {
                if (!m_Members)
                {
                        m_Members = new map<string, int>;
                }

                return m_Members.GetKeyArray();
        }
};