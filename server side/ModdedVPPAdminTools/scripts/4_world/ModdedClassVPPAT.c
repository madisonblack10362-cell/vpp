modded class PlayerManager
{
    override void HealPlayers(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Server)
        {
            Param1<ref array<string>> data; // player id's
            if(!ctx.Read(data)) return;
        
            #ifndef DIAG_DEVELOPER
                if (sender == null) return;
            #endif
            
            string adminID  = sender.GetPlainId();
            if (!GetPermissionManager().VerifyPermission(adminID, "PlayerManager:HealPlayers")) return;
            
            array<string> ids = data.param1;
            if (ids.Count() < 1) return;
            
            foreach(string id : ids)
            {
                PlayerBase targetPlayer;
                if (GetPermissionManager().VerifyPermission(adminID, "PlayerManager:HealPlayers", id))
                {
                    targetPlayer = GetPermissionManager().GetPlayerBaseByID(id);
                    if (targetPlayer != null)
                    {
                        targetPlayer.SetHealth(targetPlayer.GetMaxHealth("", ""));
                        targetPlayer.SetHealth("","Blood", targetPlayer.GetMaxHealth("","Blood"));
                        targetPlayer.SetHealth("","Shock", targetPlayer.GetMaxHealth("","Shock"));
                        targetPlayer.GetStatHeatComfort().Set(targetPlayer.GetStatHeatComfort().GetMax());
                        targetPlayer.GetStatTremor().Set(targetPlayer.GetStatTremor().GetMin());
                        targetPlayer.GetStatWet().Set(targetPlayer.GetStatWet().GetMin());
                        targetPlayer.GetStatEnergy().Set(targetPlayer.GetStatEnergy().GetMax());
                        targetPlayer.GetStatWater().Set(targetPlayer.GetStatWater().GetMax());
                        targetPlayer.GetStatDiet().Set(targetPlayer.GetStatDiet().GetMax());
                        targetPlayer.GetStatSpecialty().Set(targetPlayer.GetStatSpecialty().GetMax());
                        targetPlayer.GetStatHeatBuffer().Set(targetPlayer.GetStatHeatBuffer().GetMax());
                        targetPlayer.HealBrokenLegs();

                        targetPlayer.RemoveAgent(eAgents.CHOLERA);
                            // Print( "[HEAL]: Агент CHOLERA снят!" );
                        targetPlayer.RemoveAgent(eAgents.INFLUENZA);
                            // Print( "[HEAL]: Агент INFLUENZA снят!" );
                        targetPlayer.RemoveAgent(eAgents.SALMONELLA);
                            // Print( "[HEAL]: Агент SALMONELLA снят!" );
                        targetPlayer.RemoveAgent(eAgents.BRAIN);
                            // Print( "[HEAL]: Агент BRAIN снят!" );
                        targetPlayer.RemoveAgent(eAgents.FOOD_POISON);
                            // Print( "[HEAL]: Агент FOOD_POISON снят!" );
                        targetPlayer.RemoveAgent(eAgents.CHEMICAL_POISON);
                            // Print( "[HEAL]: Агент CHEMICAL_POISON снят!" );
                        targetPlayer.RemoveAgent(eAgents.WOUND_AGENT);
                            // Print( "[HEAL]: Агент WOUND_AGENT снят!" );
                        targetPlayer.RemoveAgent(eAgents.NERVE_AGENT);
                            // Print( "[HEAL]: Агент NERVE_AGENT снят!" );
                        targetPlayer.RemoveAgent(eAgents.HEAVYMETAL);
                            // Print( "[HEAL]: Агент HEAVYMETAL снят!" );

                        // targetPlayer.RemoveAllAgents();

                        targetPlayer.GetStaminaHandler().SetStamina(targetPlayer.GetStaminaHandler().GetStaminaCap());
                        // Print( "[HEAL]: Стамина восстановлена" );

                        // targetPlayer.GetStatContaminationRadiation().Set(0); // радиационное заражение
                        // targetPlayer.GetStatAccumulatedRadiation().Set(0);   // накопленная радиация
                        // Print( "[HEAL]: Радиоактивное заражение и накопленная радиация убраны" );

                        if (targetPlayer.GetBleedingManagerServer())
                        {
                            int attempts = 0; //fail safe, so we dont get stuck lol
                            int cuts = targetPlayer.GetBleedingManagerServer().m_BleedingSources.Count();

                            while(cuts > 0)
                            {
                                attempts++;
                                if (attempts > 15)
                                    return;

                                if (targetPlayer.GetBleedingManagerServer())
                                {
                                    int bit = targetPlayer.GetBleedingManagerServer().GetMostSignificantBleedingSource();
                                    if(bit != 0)
                                    {
                                        targetPlayer.GetBleedingManagerServer().RemoveBleedingSourceNonInfectious(bit);
                                        cuts--;
                                    }
                                }
                            }
                        }
                        GetSimpleLogger().Log(string.Format("\"%1\" (steamid=%2) healed player (steamid=%3)", sender.GetName(), sender.GetPlainId(), id));
                    }
                }
            }
            GetWebHooksManager().PostData(AdminActivityMessage, new AdminActivityMessage(sender.GetPlainId(), sender.GetName(), "[PlayerManager] Healing "+ ids.Count() +" Players"));
        }
    }
}

modded class HealPlayerChatModule
{
    override void ExecuteCommand(PlayerBase caller, array<Man> targets, array<string> args)
    {
        if(caller == null) return;

        string callerID   = caller.VPlayerGetSteamId();
        string callerName = caller.VPlayerGetName();

        foreach(Man target : targets)
        {
            PlayerBase playerTarget = PlayerBase.Cast(target);
            if(playerTarget != null)
            {
                string targetName = playerTarget.VPlayerGetName();
                string targetID   = playerTarget.VPlayerGetSteamId();

                playerTarget.SetHealth(playerTarget.GetMaxHealth("", ""));
                playerTarget.SetHealth("","Blood", playerTarget.GetMaxHealth("","Blood"));
                playerTarget.SetHealth("","Shock", playerTarget.GetMaxHealth("","Shock"));
                playerTarget.GetStatHeatComfort().Set(playerTarget.GetStatHeatComfort().GetMax());
                playerTarget.GetStatTremor().Set(playerTarget.GetStatTremor().GetMin());
                playerTarget.GetStatWet().Set(playerTarget.GetStatWet().GetMin());
                playerTarget.GetStatEnergy().Set(playerTarget.GetStatEnergy().GetMax());
                playerTarget.GetStatWater().Set(playerTarget.GetStatWater().GetMax());
                playerTarget.GetStatDiet().Set(playerTarget.GetStatDiet().GetMax());
                playerTarget.GetStatSpecialty().Set(playerTarget.GetStatSpecialty().GetMax());
                playerTarget.HealBrokenLegs();
                playerTarget.GetStaminaHandler().SetStamina(playerTarget.GetStaminaHandler().GetStaminaCap());
                    // Print("[HEAL]: Стамина восстановлена");

                playerTarget.RemoveAgent(eAgents.CHOLERA);
                    // Print( "[HEAL]: Агент CHOLERA снят!" );
                playerTarget.RemoveAgent(eAgents.INFLUENZA);
                    // Print( "[HEAL]: Агент INFLUENZA снят!" );
                playerTarget.RemoveAgent(eAgents.SALMONELLA);
                    // Print( "[HEAL]: Агент SALMONELLA снят!" );
                playerTarget.RemoveAgent(eAgents.BRAIN);
                    // Print( "[HEAL]: Агент BRAIN снят!" );
                playerTarget.RemoveAgent(eAgents.FOOD_POISON);
                    // Print( "[HEAL]: Агент FOOD_POISON снят!" );
                playerTarget.RemoveAgent(eAgents.CHEMICAL_POISON);
                    // Print( "[HEAL]: Агент CHEMICAL_POISON снят!" );
                playerTarget.RemoveAgent(eAgents.WOUND_AGENT);
                    // Print( "[HEAL]: Агент WOUND_AGENT снят!" );
                playerTarget.RemoveAgent(eAgents.NERVE_AGENT);
                    // Print( "[HEAL]: Агент NERVE_AGENT снят!" );
                playerTarget.RemoveAgent(eAgents.HEAVYMETAL);
                    // Print( "[HEAL]: Агент HEAVYMETAL снят!" );

                // playerTarget.RemoveAllAgents();
                    // Print( "[HEAL]: Все агенты убраны" );
                
                // playerTarget.GetStatContaminationRadiation().Set(0); // радиационное заражение
                // playerTarget.GetStatAccumulatedRadiation().Set(0);   // накопленная радиация
                    // Print( "[HEAL]: Радиоактивное заражение и накопленная радиация убраны" );


                if (playerTarget.GetBleedingManagerServer())
                {
                    int attempts = 0; // Fail safe, so we don't get stuck
                    int cuts = playerTarget.GetBleedingManagerServer().m_BleedingSources.Count();

                    while(cuts > 0)
                    {
                        attempts++;
                        if (attempts > 15)
                            return;

                        if (playerTarget.GetBleedingManagerServer())
                        {
                            int bit = playerTarget.GetBleedingManagerServer().GetMostSignificantBleedingSource();
                            if(bit != 0)
                            {
                                playerTarget.GetBleedingManagerServer().RemoveBleedingSourceNonInfectious(bit);
                                cuts--;
                            }
                        }
                    }
                }
                GetSimpleLogger().Log(string.Format("\"%1\" (steamid=%2) /heal used on: \"%3\" (steamid=%4)", callerName, callerID, targetName, targetID));
                GetWebHooksManager().PostData(AdminActivityMessage, new AdminActivityMessage(callerID, callerName, "Chat Command Manager: /heal command used on: " + targetName + " ID: " + targetID));
            }
        }
    }
}
