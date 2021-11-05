#if defined _dbx_core_included
 #endinput
#endif
#define _dbx_core_included


#define MAX_SLOTS			24
#define PLAYER_TAB			MAXPLAYERS+1
#define PREFIX				"\x0CMONEYMOD\x0B |"
#define IsPlayer(%1) 		(1 <= %1 <= MaxClients && IsClientInGame(%1))

const Float:g_fMoneyPercent = 	15.0;

native hnsGetBandana(iClient);

enum
{
	CHANCE,
	ACTIVE,
	PASSIVE
};

enum dataMoneyOperations
{
	MONEY_ADD,
	MONEY_REMOVE
};

enum playerActivesTypes
{
	activeFireball,
	activeFrostball,
	activeStealth,
	activeGrab,
	activeFirecamp,
	activePush
};

enum playerChancesTypes
{
	chancePistol,
	chanceSmg,
	chanceGrenade,
	chanceDrop,
	chanceFlashbang,
	chanceBonusBullet,
	chanceCriticalHit
};

enum playerPassiveTypes
{
	passiveBonusHealth,
	passiveBonusArmor,
	passiveNofalldamage,
	passiveHeadsplashReduction,
	passiveRegeneration,
	passiveMoney,
	passiveLuck,
	passiveFrostnadeBreaker,
	passiveLifesteal
};

enum playerStats {
    dataMoney,
    dataKills,
    dataTotalMoney,
    dataTotalDamage,
	dataTempDamage,
	dataTempMoney,
	dataMaxObjects,
	dataChances[playerChancesTypes],
	dataActives[playerActivesTypes],
	dataPassives[playerPassiveTypes]
};

enum playerUpgradesConstants {
	DEFAULTVALUE, INCBYVALUE, COST, INCPRICEBYPERC, MAX
};
enum dataStatOperations{
	STATs_ADD,
	STATs_REMOVE
};

enum playerStatistics {
	statsKilled,
	statsKilledSplash,
	statsKilledHead,
	statsAssists,
	statsDeaths,
	statsRoundsSurvived,
	statsTotalMoney,
	statsTotalMoneyDmg,
	statsTotalMoneyBlock,
	statsBoxes,
	statsBoxesOpened
};

new String:g_sStatistics[playerStatistics][] = {
	"  Kills ",
	"  HeadSplash ",
	"  HeadShot ",
	"  Assists ",
	"  Deaths ",
	"  Rounds survived ",
	"  Money collected ",
	"  from damage ",
	"  from blocks ",
	"  Boxes looted ",
	"  Opened "
};

new const String:g_sActivesList[playerActivesTypes][32] = {
	"Fireball",
	"Flashball",
	"Stealth",
	"Grab",
	"Firecamp",
	"Push"
};

new const String:g_sActivesVal[playerActivesTypes][32] = {
	"HP",
	"units",
	"seconds",
	"",
	"seconds",
	"units"
};

new const Float:g_fActivesData[playerActivesTypes][4] = {
	{0.0, 90.0, 130.0, 150.0},
	{0.0, 100.0, 120.0, 140.0},
	{0.0, 5.0, 10.0, 15.0},
	{0.0, 3000.0, 3500.0, 4500.0},
	{0.0, 15.0, 25.0, 45.0},
	{0.0, 125.0, 150.0, 175.0}
};

new const String:g_sActivesBonus[playerActivesTypes][64] = {
	"Max. Damage",
	"Max. Distance",
	"Effect time",
	"Grab speed",
	"Effect time",
	"Max. Distance"
};

new const String:g_sActivesDesc[playerActivesTypes][64] = {
	"Damages on explosion",
	"Blinds on explosion",
	"You start to disappear",
	"Hooks an enemy on hit",
	"Heals allies for 30 secs",
	"Pushes all enemies away",
};

//  DEFAULTVALUE, INCBYVALUE, COST, INCPRICEBYPERC, MAX
new const g_iActivesConstants[playerActivesTypes][playerUpgradesConstants] = {
	{0, 5, 700, 25, 3},
	{0, 5, 500, 25, 3},
	{0, 5, 500, 25, 3},
	{0, 4, 500, 25, 3},
	{0, 5, 650, 25, 3},
	{0, 5, 650, 25, 3}
};
//START_VALUE, INCBYVALUE, COST, INCPRICEBYPERC, MAX
new const g_iPassivesConstants[playerPassiveTypes][playerUpgradesConstants] = {
	{0, 	10, 	400, 	15, 	5},
	{0, 	20, 	200, 	20, 	5},
	{0, 	10,		200,	20,		5},
	{0, 	10,		300,	15,		5},
	{0, 	1, 		200, 	15, 	5},
	{0, 	5, 		500, 	20, 	3},
	{1,		1,		500,	10,		5},
	{0,		5,		350,	15,		4},
	{0,		5,		450,	10,		4}
};

new const String:g_sPassiveInfo[playerPassiveTypes][] = {
	"●Every point, you get bonus 10HP\n \n●Maximum 50HP Bonus",
	"●Every point, you get bonus 20Kev\n  Maxing bonus: helmet\n \n●Maximum 100 Kevlar",
	"●Every point, reduces your fall\n  damage by 10%\n \n●Maximum reduction is 50%",
	"●Every point, reduces your\n  headsplash damage by 10%\n \n●Maximum reduction is 50%",
	"●Every point, you get +1HP every\n  5 seconds\n \n●Maximum regeneration is 5HP\n  every 5 seconds",
	"●Every point, you get 5% more\n  money from  damage,\n  blocks, boxes etc\n \n●Maximum bonus is 20%",
	"●Every point, you get +1 Luck\n  increasing  DropRate, Drop Rarity,\n  Higher JackPot chance,\n  Better RandomBlock results etc\n \n●Maximum Luck is 5",
	"●Every point, frozen time reduced\n  by 5%\n \n●Maximum reduction is 20%",
	"●Every point, 5% of your damage\n  heals you\n \n●Maximum lifesteal is 20%"
};

new const String:g_sPassivesList[playerPassiveTypes][32] = {
	"Health",
	"Armor: Kevlar",
	"Armor: Falling",
	"Armor: Headsplash",
	"Regeneration",
	"Money",
	"Luck",
	"Frostnade Breaker",
	"Lifesteal"
};

new const g_iPassivesPlus[playerPassiveTypes] = {1,1,0,0,1,1,1,0,1};

new const String:g_sPassivesDesc[playerPassiveTypes][64] = {
	"More basic Health",
	"More basic Armor",
	"Less Fall damage",
	"Less Headsplash damage",
	"Recovers HP over time",
	"Passive Money bonus",
	"General Luck bonus",
	"Freezetime reduction",
	"Heals on attack"
};

new const String:g_sPassivesVal[playerPassiveTypes][16] = {
	"HP",
	"Armor",
	"%",
	"%",
	"HP/5 sec",
	"%",
	"Luck",
	"%",
	"%"
};

new const String:g_sPassivesValBought[playerPassiveTypes][32] = {
	"",
	"",
	"%",
	"%",
	"HP",
	"%",
	"",
	"%",
	"%",
};

enum {
	TASK_COUNT,
	TASK_REGEN,
	TASK_GRAB,
	TASK_BLANK
};

new g_iMmClientHud;

new g_ePlayerData[PLAYER_TAB][playerStats];

new g_iRoundSkill[PLAYER_TAB][playerActivesTypes];

new g_iOldButtons[PLAYER_TAB];

new g_iSkillChosenMenu[PLAYER_TAB];

new g_iPendingMoney[PLAYER_TAB];
new g_iPendingQuant[PLAYER_TAB];
#define AUCTIONS_SLOTS 3
new g_iPendingItem[PLAYER_TAB][AUCTIONS_SLOTS];
new g_iPendingItemQuant[PLAYER_TAB][AUCTIONS_SLOTS];

new g_iTeamChange[PLAYER_TAB];

new Handle:g_hTask[PLAYER_TAB][TASK_BLANK+1];

new g_iStatsPlayer[PLAYER_TAB];
new Float:g_fPlayerKDA[PLAYER_TAB];

_mmSkillGetLevel(iClient, iGroup, iWhich){
	new iVal = iGroup == CHANCE ? g_ePlayerData[iClient][dataChances][iWhich] : iGroup == ACTIVE ? g_ePlayerData[iClient][dataActives][iWhich] : g_ePlayerData[iClient][dataPassives][iWhich];
	return iVal;
}

_mmGetMaxHealth(iClient){
	new iBonus = _mmSkillGetLevel(iClient, PASSIVE, passiveBonusHealth)*g_iPassivesConstants[passiveBonusHealth][INCBYVALUE];

	return iBonus+100;
}

_mmSetMoney(iClient, dataMoneyOperations:iOperation, iAmount){
	if(iOperation==MONEY_ADD){
		if(_mmSkillGetLevel(iClient, PASSIVE, passiveMoney)>0){
			new iBonus = _mmSkillGetLevel(iClient, PASSIVE, passiveMoney)*g_iPassivesConstants[passiveMoney][INCBYVALUE];
			new iOld = iAmount;
			
			iAmount += RoundFloat(float(iAmount) * (float(iBonus)/100.0));
			
			PrintToChat(iClient, "\x01\x0B\x01 %s\x0D %d%c MONEY Skill Bonus: +\x05%d\x0D$", PREFIX, iBonus, '%', iAmount-iOld);
		}
		if(hnsGetBandana(iClient)==2){
			new iOld = iAmount;
			
			iAmount += RoundFloat(float(iAmount) * 0.25);
			
			PrintToChat(iClient, "\x01\x0B\x01 %s\x0D 25%c Green Bandana bonus: +\x05%d\x0D$", PREFIX, '%', iAmount-iOld);
		}
		_statChange(iClient, STATs_ADD, statsTotalMoney, iAmount);
	}
	g_ePlayerData[iClient][dataMoney]=iOperation==MONEY_ADD ? g_ePlayerData[iClient][dataMoney]+iAmount : g_ePlayerData[iClient][dataMoney]-iAmount;
	g_ePlayerData[iClient][dataTotalMoney]=iOperation==MONEY_ADD ? g_ePlayerData[iClient][dataTotalMoney]+iAmount : g_ePlayerData[iClient][dataTotalMoney]-iAmount;
	_mmRefreshMoney(iClient);
}

_mmRefreshMoney(iClient){
	SetEntProp(iClient, Prop_Send, "m_iAccount", g_ePlayerData[iClient][dataMoney]);
}
//STOCKINGZ XD
stock bool:IsValidClient(iClient)
	return (1 <= iClient <= MaxClients && IsValidEntity(iClient) && IsClientConnected(iClient) && IsClientInGame(iClient) && !IsFakeClient(iClient))

stock bool isValidRef(iReference){
	new iEnt = EntRefToEntIndex(iReference);
	return (iEnt > MaxClients && IsValidEntity(iEnt))
}

stock bool:TraceEntityFilterPlayer(entity, contentsMask)
    return entity > GetMaxClients();

stock resetRendering(iClient) {
    if(IsClientConnected(iClient) && IsPlayerAlive(iClient)) {
        SetEntityRenderMode(iClient, RENDER_NORMAL);
        SetEntityRenderFx(iClient, RENDERFX_NONE);
        SetEntityRenderColor(iClient, 255, 255, 255, 255);
    }
}

void AddInFrontOf(float vecOrigin[3], float vecAngle[3], float units, float output[3])
{
	float vecAngVectors[3];
	vecAngVectors = vecAngle;
	GetAngleVectors(vecAngVectors, vecAngVectors, NULL_VECTOR, NULL_VECTOR);
	for (int i; i < 3; i++)
	output[i] = vecOrigin[i] + (vecAngVectors[i] * units);
}

void AddBehindOf(float vecOrigin[3], float vecAngle[3], float units, float output[3])
{
	float vecAngVectors[3];
	vecAngVectors = vecAngle;
	GetAngleVectors(vecAngVectors, vecAngVectors, NULL_VECTOR, NULL_VECTOR);

	for (int i; i < 3; i++){
		vecAngVectors[i]*=-1.0;
		output[i] = vecOrigin[i] + (vecAngVectors[i] * units);
	}
}

stock int CreateParticleWithParent(const char[] effect, const float origin[3], int parent) {
    int ent = CreateEntityByName("info_particle_system");
    DispatchKeyValue(ent , "start_active", "0");
    DispatchKeyValue(ent, "effect_name", effect);
    DispatchSpawn(ent);
   
    TeleportEntity(ent, origin, NULL_VECTOR, NULL_VECTOR);
    SetVariantString("!activator");
    AcceptEntityInput(ent, "SetParent", parent, ent, 0);
   
    ActivateEntity(ent);
    AcceptEntityInput(ent, "Start");
   
    return ent;
}


stock GetAimOrigin(client, Float:hOrigin[3]) {
    new Float:vAngles[3], Float:fOrigin[3];
    GetClientEyePosition(client,fOrigin);
    GetClientEyeAngles(client, vAngles);

    new Handle:trace = TR_TraceRayFilterEx(fOrigin, vAngles, MASK_SHOT, RayType_Infinite, TraceEntityFilterPlayer);

    if(TR_DidHit(trace))
    {
        TR_GetEndPosition(hOrigin, trace);
        CloseHandle(trace);
        return 1;
    }

    CloseHandle(trace);
    return 0;
}


stock bool:trExplosion(iEnt, iBitMask, any:iData) return !(iEnt==iData||1<=iEnt<=MaxClients);

stock ScreenFade(iClient, iFlags = FFADE_PURGE, iaColor[4] = {0, 0, 0, 0}, Float:fDur = 0.0, iHoldTime = 0){
	new Handle:hScreenFade = StartMessageOne("Fade", iClient);
	PbSetInt(hScreenFade, "duration", RoundFloat(fDur * 500));
	PbSetInt(hScreenFade, "hold_time", iHoldTime * 500);
	PbSetInt(hScreenFade, "flags", iFlags);
	PbSetColor(hScreenFade, "clr", iaColor);
	EndMessage();
}

stock PrintHudMessage(client, char[] channel, char[] color, char[] color2, char[] effect, char[] fadein, char[] fadeout, char[] fxtime, char[] holdtime, char[] message, char[] x, char[] y){
	if(!isValidRef(g_iMmClientHud)){
		int ent = CreateEntityByName("game_text");
		DispatchKeyValue(ent, "channel", channel);
		DispatchKeyValue(ent, "color", color);
		DispatchKeyValue(ent, "color2", color2);
		DispatchKeyValue(ent, "effect", effect);
		DispatchKeyValue(ent, "fadein", fadein);
		DispatchKeyValue(ent, "fadeout", fadeout);
		DispatchKeyValue(ent, "fxtime", fxtime);
		DispatchKeyValue(ent, "holdtime", holdtime);
		DispatchKeyValue(ent, "message", message);
		DispatchKeyValue(ent, "spawnflags", "0");
		DispatchKeyValue(ent, "x", x);
		DispatchKeyValue(ent, "y", y);
		DispatchSpawn(ent);

		g_iMmClientHud = EntIndexToEntRef(ent);
	}
	else{
		DispatchKeyValue(g_iMmClientHud, "channel", channel);
		DispatchKeyValue(g_iMmClientHud, "color", color);
		DispatchKeyValue(g_iMmClientHud, "color2", color2);
		DispatchKeyValue(g_iMmClientHud, "effect", effect);
		DispatchKeyValue(g_iMmClientHud, "fadein", fadein);
		DispatchKeyValue(g_iMmClientHud, "fadeout", fadeout);
		DispatchKeyValue(g_iMmClientHud, "fxtime", fxtime);
		DispatchKeyValue(g_iMmClientHud, "holdtime", holdtime);
		DispatchKeyValue(g_iMmClientHud, "message", message);
		DispatchKeyValue(g_iMmClientHud, "x", x);
		DispatchKeyValue(g_iMmClientHud, "y", y);
		
		SetVariantString("!activator");

		AcceptEntityInput(g_iMmClientHud, "display", client);
	}
}

stock HexToRGB(hexValue, iColor[3]){
	iColor[0] = ((hexValue >> 16) & 0xFF);
	iColor[1] = ((hexValue >> 8) & 0xFF);
	iColor[2] = ((hexValue >> 0) & 0xFF);
}

stock MT_GetRandomInt(min, max){
    return RoundToNearest(GetURandomFloat() * float(max - min) + float(min));
}

stock Float:MT_GetRandomFloat(Float:min, Float:max){
    return GetURandomFloat() * (max - min) + min;
}

stock Float:FloatMin(Float:f1, Float:f2) return f1 < f2 ? f1 : f2;
stock Float:FloatMax(Float:f1, Float:f2) return f1 > f2 ? f1 : f2;