#if defined _dbx_chances_included
 #endinput
#endif
#define _dbx_chances_included

new const String:g_sChancesList[playerChancesTypes][32] = {
	"Pistols",
	"Smgs",
	"HeGrenade",
	"Droprate",
	"DoubleFlashbang",
	"BonusBullet",
	"CriticalStab"
};

new const String:g_sChancesDesc[playerChancesTypes][64] = {
	"Spawn with Pistol",
	"Spawn with Smg",
	"Spawn with Grenade",
	"Your box drop chance",
	"(T) Spawn with 2 flashbangs",
	"Sometimes you get 2 bullets",
	"Chance for +50% DMG stab"
};

new const g_iChancesConstants[playerChancesTypes][playerUpgradesConstants] = {
	{4, 2, 1000, 50, 2},
	{4, 2, 1000, 50, 2},
	{5, 2, 500, 50, 3},
	{25, 5, 400, 30, 4},
	{0, 25, 300, 30, 4},
	{0, 2, 600, 50, 3},
	{0, 5, 600, 40, 4}
};
/*
new const String:g_sChancesVal[playerChancesTypes][16] = {
	"Chance",
	"Chance",
	"Chance",
	"Chance",
	"Chance",
	"Chance",
	"Chance"
};*/

#define CHANCE_PISTOLS_MAX 7
new const String:g_sChancesPistol[CHANCE_PISTOLS_MAX][2][] = {
	{"weapon_usp_silencer", "USP" },
	{"weapon_fiveseven", "FivenSeveN" },
	{"weapon_glock",	"Glock" },
	{"weapon_hkp2000",	"HKP2000" },
	{"weapon_p250",		"P250" },
	{"weapon_tec9",		"Tec-9" },
	{"weapon_cz75a",	"CZ75" }
};

#define CHANCE_SMGS_MAX 5
new const String:g_sChancesSmg[CHANCE_SMGS_MAX][2][] = {
	{"weapon_mp7", 		"MP7" },
	{"weapon_mp9", 		"MP9" },
	{"weapon_ump45",	"Ump45" },
	{"weapon_p90",		"P90" },
	{"weapon_mac10",	"Mac10" }
};

new g_iWeaponsCount[2][3];

public _skillChanceWeapon(iClient, iLevel, playerChancesTypes:iWhich){
	new iReward;
	new iCurrent;
	new iChance;
	new String:sClassReward[64], String:sDisplayReward[64];
	new bool:bPistol=false;
	new bool:bGrenade=false;
	
	switch(iWhich){
		case chancePistol:{
			iChance = g_iChancesConstants[chancePistol][DEFAULTVALUE] + g_iChancesConstants[chancePistol][INCBYVALUE]*iLevel;
			bPistol=true;
		}
		case chanceSmg:{
			iChance = g_iChancesConstants[chanceSmg][DEFAULTVALUE] + g_iChancesConstants[chanceSmg][INCBYVALUE]*iLevel;
		}
		case chanceGrenade:{
			iChance = g_iChancesConstants[chanceGrenade][DEFAULTVALUE] + g_iChancesConstants[chanceGrenade][INCBYVALUE]*iLevel;
			bGrenade=true;
		}
	}
	
	if(!bGrenade){
		new iPist =  _:CHANCE_PISTOLS_MAX;
		new iSmg = _:CHANCE_SMGS_MAX;
		iCurrent = Math_GetRandomInt(0, 100);
		
		if(iChance >= iCurrent){	
			new iFound = GetRandomInt(0, bPistol ? iPist-1 : iSmg-1);
			//PrintToChatAll("SUCC:%d>%d",iChance,iCurrent);
			g_iWeaponsCount[GetClientTeam(iClient)-2][bPistol ? 0 : 1]++;
				
			FormatEx(sClassReward, sizeof sClassReward, bPistol ? g_sChancesPistol[iFound][0] : g_sChancesSmg[iFound][0]);
			FormatEx(sDisplayReward, sizeof sDisplayReward, bPistol ? g_sChancesPistol[iFound][1] : g_sChancesSmg[iFound][1]);

			iReward = GivePlayerItem(iClient, sClassReward);					
			SetEntProp(iReward, Prop_Data, "m_iClip1", 1);
			SetEntProp(iReward, Prop_Send, "m_iPrimaryReserveAmmoCount", 0);

			PrintToChat(iClient, "\x01\x0B\x01 %s\x03 You just won a\x09 %s\x07 [%d%c]", PREFIX/*, GetClientTeam(iClient) == 3 ? "\x0D" : "\x0F"*/, sDisplayReward, iChance, '%');
		}
	}
	else{
		iCurrent = Math_GetRandomInt(0, 100);
		
		if(iChance >= iCurrent){
			g_iWeaponsCount[GetClientTeam(iClient)-2][2]++;
			
			iReward = GivePlayerItem(iClient, "weapon_hegrenade");
			PrintToChat(iClient, "\x01\x0B\x01 %s\x03 You just won a\x09 Grenade\x07 [%d%c]", PREFIX/*, GetClientTeam(iClient) == 3 ? "\x0D" : "\x0F"*/, iChance, '%');
		}
	}
}