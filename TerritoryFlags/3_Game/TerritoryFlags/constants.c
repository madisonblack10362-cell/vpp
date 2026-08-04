class TerritoryConst
{
        static float RADIUS = 100.0;
        static float FLAGDOWNSTATE = 0.9;
        static float FLAGUPSTATE = 0.2;
};

class TerritoryPerm
{
        static int OWNER = 1;
        static int BUILD = 2;
        static int DEPLOY = 4;
        static int DISMANTLE = 8;
        static int LOWERFLAG = 16;
        static int ADDMEMBER = 32;
        static int REMOVEMEMBER = 64;
};