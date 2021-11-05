#if defined _dbx_skills_actives_included
 #endinput
#endif
#define _dbx_skills_actives_included

#include "dbx_core.inc"

const Float:FROSTBALL_EXPLO_OFFSET = 	42.0;
const Float:FIREBALL_EXPLO_OFFSET = 	80.0;
const Float:CAMP_FIRE_RADIUS = 			150.0;
const Float:CAMP_FIRE_DIFF = 			10.0;
const TICKS_FOR_FULL_STEALTH = 			10;
const SKILLS_COUNT =					6;

enum skillShots{
	skillShot_Grab,
	skillShot_Fireball,
	skillShot_Frostball
};

enum skillShots_Resources{
	skillShot_Sound,
	skillShot_Sprite,
	skillShot_Explo
};

enum skillShots_Props{
	Float:skillShot_ProjectileSpeed,
	Float:skillShot_SpriteScale,
	Float:skillShot_Sprite2Scale,
	Float:skillShot_Sprite2Dur
};

enum skillSounds_Grab{
	skillSound_Grab_Shot,
	skillSound_Grab_Pull
};

enum skillSounds_Fireball{
	skillSound_Fireball_Shot,
	skillSound_Fireball_Explode
};

enum skillCampfire_Assets{
	skillCampfire_Sprite,
	skillCampfire_Model
};

new const String:g_sSkill_Campfire_Assets[skillCampfire_Assets][] = {
	"materials/playcorehns_misc/vFX_0x0F.vmt", //firecamp.vmt
	"models/playcorehns_blocks/pole/wood.mdl"
};

new const String:g_sSkillShot_Resources[skillShots][skillShots_Resources][] = {
	{"*playcore/playcore_sfx19.mp3", 		"playcorehns_misc/vFX_0x0G.vmt",/*grab*/ 			""},
	{"",								 	"playcorehns_misc/vFX_0x0H.vmt",/*fireball*/ 		"playcorehns_misc/vFX_0x0J.vmt"}, //fireball explo
	{"", 									"playcorehns_misc/vFX_0x0I.vmt",/*frostball*/		"playcorehns_misc/vFX_0x0K.vmt"}	//frostball explo
};

new const Float:g_fSkillShot_Props[skillShots][skillShots_Props] = {
	{3000.0, 0.2, 0.0, 0.0},
	{1750.0, 0.25, 0.8, 0.67},
	{1500.0, 0.25, 0.4, 0.55}
};

new const String:g_sSkillSound_Grab[skillSounds_Grab][] = { "*playcore/playcore_sfx19.mp3", "*playcore/playcore_sfx20.mp3" };
new const String:g_sSkillSound_Fireball[skillSounds_Fireball][] = { "*playcore/playcore_sfx21.mp3", "*playcore/playcore_sfx21.mp3" };

new Float:g_fGrabTime[MAXPLAYERS+1];
new Float:g_fStealthTime[MAXPLAYERS+1];
new g_iGrabbed[MAXPLAYERS+1];
new g_iChild[2048];
new g_iSprite[2048];

new Handle:g_hCampFireTaskHandle[MAXPLAYERS+1];
new Handle:g_hStealthTimer[MAXPLAYERS+1];
new Handle:g_hStealthCast[MAXPLAYERS+1];

new bool:bFoundSkills[MAXPLAYERS+1];
new bool:bFoundWeps[MAXPLAYERS+1];

new g_iStealthTransparencyTicks[MAXPLAYERS+1];
new g_iAvailableActive[MAXPLAYERS+1][playerActivesTypes];
new g_iSkillsCount[2][SKILLS_COUNT];

_skillsPrecacheStuff(){
	new i;
	new String:sPath[128], String:sFullPath[128];
	
	//g_iExplosion = PrecacheModel("sprites/sprite_fire01.vmt");
	//g_iFrostplosion = PrecacheModel("materials/playcorehns_misc/frostball_explo00.vmt");
	
	FormatEx(sPath, sizeof sPath, g_sSkill_Campfire_Assets[skillCampfire_Sprite]);
	AddFileToDownloadsTable(sPath);
		
	ReplaceString(sPath, sizeof sPath, ".vmt", ".vtf", false);
	AddFileToDownloadsTable(sPath);
	
	PrecacheModel(g_sSkill_Campfire_Assets[skillCampfire_Model]);
	
	PrecacheModel("models/props_junk/watermelon01.mdl");

	FormatEx(sPath, sizeof sPath, "*playcore/playcore_sfx17.mp3");
	AddToStringTable(FindStringTable("soundprecache"), sPath);
	ReplaceString(sPath, sizeof sPath, "*", "sound/", false);
	PrecacheSound(sPath, true);	
	
	for(i = 0 ; i < _:skillSounds_Grab ; i++){
		FormatEx(sPath, sizeof sPath, g_sSkillSound_Grab[i]);
		AddToStringTable(FindStringTable("soundprecache"), sPath);
		
		ReplaceString(sPath, sizeof sPath, "*", "sound/", false);
		PrecacheSound(sPath, true);	
		AddFileToDownloadsTable(sPath);	
	}
	for(i = 0 ; i < _:skillSounds_Fireball ; i++){
		FormatEx(sPath, sizeof sPath, g_sSkillSound_Fireball[i]);
		AddToStringTable(FindStringTable("soundprecache"), sPath);
		
		ReplaceString(sPath, sizeof sPath, "*", "sound/", false);
		PrecacheSound(sPath, true);	
		AddFileToDownloadsTable(sPath);	
	}
	for(i = 0 ; i < _:skillShots ; i++){
		new x;
		for(x = 0 ; x < _:skillShots_Resources ; x++){
			if(strlen(g_sSkillShot_Resources[i][x])){
				switch(x){
					case skillShot_Sound:{
						FormatEx(sFullPath, sizeof sFullPath, g_sSkillShot_Resources[i][skillShot_Sound]);
						ReplaceString(sFullPath, sizeof sFullPath, "*", "sound/", false);
						
						if(!IsSoundPrecached(sFullPath)){
							FormatEx(sPath, sizeof sPath, g_sSkillShot_Resources[i][skillShot_Sound]);
							AddToStringTable(FindStringTable("soundprecache"), sPath);
							
							PrecacheSound(sFullPath, true);	
							AddFileToDownloadsTable(sFullPath);
						}
					}
					case skillShot_Sprite, skillShot_Explo:{
						FormatEx(sPath, sizeof sPath, "materials/%s", g_sSkillShot_Resources[i][x]);
						AddFileToDownloadsTable(sPath);
						
						ReplaceString(sPath, sizeof sPath, ".vmt", ".vtf");
						AddFileToDownloadsTable(sPath);
					}
				}
			}
		}
	}
	/*AddFileToDownloadsTable("materials/playcorehns_misc/grabv2.vmt");
	AddFileToDownloadsTable("materials/playcorehns_misc/grabv2.vtf");
	AddFileToDownloadsTable("materials/playcorehns_misc/fireballv2.vmt");
	AddFileToDownloadsTable("materials/playcorehns_misc/fireballv2.vtf");
	AddFileToDownloadsTable("materials/playcorehns_misc/frostball.vmt");
	AddFileToDownloadsTable("materials/playcorehns_misc/frostball.vtf");
	AddFileToDownloadsTable("materials/playcorehns_misc/frostball_explo00.vmt");
	AddFileToDownloadsTable("materials/playcorehns_misc/frostball_explo00.vtf");
	AddFileToDownloadsTable("materials/playcorehns_misc/fireball_explo00.vmt");
	AddFileToDownloadsTable("materials/playcorehns_misc/fireball_explo00.vtf");*/
}


_skillsActiveReset(iClient){
	if(g_hStealthCast[iClient] != INVALID_HANDLE){
		KillTimer(g_hStealthCast[iClient]);
		g_hStealthCast[iClient] = INVALID_HANDLE;
	}
	if(g_hStealthTimer[iClient] != INVALID_HANDLE){
		KillTimer(g_hStealthTimer[iClient]);
		g_hStealthTimer[iClient] = INVALID_HANDLE;
	}
	g_fStealthTime[iClient] = 0.0;
	g_iStealthTransparencyTicks[iClient]=0;
	
	for(new i = 0 ; i < _:playerActivesTypes ; i++){
		g_iAvailableActive[iClient][i] = 0;
	}
	resetRendering(iClient);
}

_skillsActiveGetAvailable(iClient){
	new iCount=0;
	for(new i = 0 ; i < _:playerActivesTypes ; i++){
		if(g_iAvailableActive[iClient][i] > 0) iCount++;
	}
	return iCount;
}

_skillsActivePrintDrops(iClient, iTeam){
	new iEnemies = iTeam==2 ? 1 : 0;
	
	bFoundSkills[iClient]=false;
	bFoundWeps[iClient]=false;
	
	new String:sMsg[512], String:sColor[4];
	if(g_iSkillsCount[iEnemies][0] || g_iSkillsCount[iEnemies][1] || g_iSkillsCount[iEnemies][2] || g_iSkillsCount[iEnemies][3] || g_iSkillsCount[iEnemies][4] || g_iSkillsCount[iEnemies][5]){
		PrintToChat(iClient, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");

		FormatEx(sColor, sizeof sColor, "%s", iTeam==2 ? "\x0D" : "\x0F");
		
		FormatEx(sMsg, sizeof sMsg, "\x01\x0B\x01 %s %s%s:\x08", PREFIX, sColor, iTeam==2 ? "SEEKERS Skills" : "HIDERS Skills");
		
		if(g_iSkillsCount[iEnemies][0]){
			Format(sMsg, sizeof sMsg, "%s \x06%d\x08 fireball%s", sMsg, g_iSkillsCount[iEnemies][0], g_iSkillsCount[iEnemies][0]>1 ? "s" : "");
		}
		if(g_iSkillsCount[iEnemies][1]){
			Format(sMsg, sizeof sMsg, "%s%s\x06%d\x08 flashball%s", sMsg, g_iSkillsCount[iEnemies][0]>0 ? ", " : "", g_iSkillsCount[iEnemies][1], g_iSkillsCount[iEnemies][1]>1 ? "s" : "");
		}
		if(g_iSkillsCount[iEnemies][2]){
			Format(sMsg, sizeof sMsg, "%s%s\x06%d\x08 stealth%s", sMsg, (g_iSkillsCount[iEnemies][0]||g_iSkillsCount[iEnemies][1]) ? ", " : "", g_iSkillsCount[iEnemies][2], g_iSkillsCount[iEnemies][2]>1 ? "s" : "");
		}
		if(g_iSkillsCount[iEnemies][3]){
			Format(sMsg, sizeof sMsg, "%s%s\x06%d\x08 grab%s", sMsg, (g_iSkillsCount[iEnemies][0]||g_iSkillsCount[iEnemies][1]||g_iSkillsCount[iEnemies][2]) ? ", " : "", g_iSkillsCount[iEnemies][3], g_iSkillsCount[iEnemies][3]>1 ? "s" : "");
		}
		if(g_iSkillsCount[iEnemies][4]){
			Format(sMsg, sizeof sMsg, "%s%s\x06%d\x08 firecamp%s", sMsg, (g_iSkillsCount[iEnemies][0]||g_iSkillsCount[iEnemies][1]||g_iSkillsCount[iEnemies][2]||g_iSkillsCount[iEnemies][3]) ? ", " : "", g_iSkillsCount[iEnemies][4], g_iSkillsCount[iEnemies][4]>1 ? "s" : "");
		}
		if(g_iSkillsCount[iEnemies][5]){
			Format(sMsg, sizeof sMsg, "%s%s\x06%d\x08 push%s", sMsg, (g_iSkillsCount[iEnemies][0]||g_iSkillsCount[iEnemies][1]||g_iSkillsCount[iEnemies][2]||g_iSkillsCount[iEnemies][3]||g_iSkillsCount[iEnemies][4]) ? ", " : "", g_iSkillsCount[iEnemies][5], g_iSkillsCount[iEnemies][5]>1 ? "s" : "");
		}
		PrintToChat(iClient, sMsg);
		bFoundSkills[iClient]=true;
	}
	if(g_iWeaponsCount[iEnemies][0] || g_iWeaponsCount[iEnemies][1] || g_iWeaponsCount[iEnemies][2]){
		if(bFoundSkills[iClient]==false) PrintToChat(iClient, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");

		FormatEx(sColor, sizeof sColor, "%s", iTeam==2 ? "\x0D" : "\x0F");
		
		FormatEx(sMsg, sizeof sMsg, "\x01\x0B\x01 %s %s%s: ", PREFIX, sColor, iTeam==2 ? "SEEKERS got" : "HIDERS got");
		
		if(g_iWeaponsCount[iEnemies][0]){
			Format(sMsg, sizeof sMsg, "%s \x06%d\x08 pistol%s", sMsg, g_iWeaponsCount[iEnemies][0], g_iWeaponsCount[iEnemies][0]>1 ? "s" : "");
		}
		if(g_iWeaponsCount[iEnemies][1]){
			Format(sMsg, sizeof sMsg, "%s%s\x06%d\x08 smg%s", sMsg, g_iWeaponsCount[iEnemies][0]>0 ? ", " : "", g_iWeaponsCount[iEnemies][1], g_iWeaponsCount[iEnemies][1]>1 ? "s" : "");
		}
		if(g_iWeaponsCount[iEnemies][2]){
			Format(sMsg, sizeof sMsg, "%s%s\x06%d\x08 grenade%s", sMsg, (g_iWeaponsCount[iEnemies][0]||g_iWeaponsCount[iEnemies][1]) ? " and " : "", g_iWeaponsCount[iEnemies][2], g_iWeaponsCount[iEnemies][2]>1 ? "s" : "");
		}
		PrintToChat(iClient, sMsg);
		bFoundWeps[iClient]=true;
	}

	if(bFoundSkills[iClient]==true||bFoundWeps[iClient]==true) PrintToChat(iClient, "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
}

_skillsActiveTry(iClient){
	for(new i = 0 ; i < _:playerActivesTypes ; i++){
		if(g_ePlayerData[iClient][dataActives][i]>0){
			new iChance = g_iActivesConstants[i][DEFAULTVALUE] + g_iActivesConstants[i][INCBYVALUE]*g_ePlayerData[iClient][dataActives][i];
			
			new iCur = Math_GetRandomInt(0, 100);
			//PrintToChatAll("Cur %d Chance %d (%s - Level %d)", iCur, iChance, g_sActivesList[i], g_ePlayerData[iClient][dataActives][i]);
			
			if(iCur<=iChance){
				PrintToChat(iClient, "\x01\x0B\x01 %s\x03 You rolled skill:\x09 %s\x07 [%d%c]", PREFIX, g_sActivesList[i], iChance, '%');
				g_iAvailableActive[iClient][i]=1;
				g_iSkillsCount[GetClientTeam(iClient)-2][i]++;
			}
		}
	}
}

_createActivesPanel(iClient){	
	new Handle:hMenu = CreateMenu(_handlerActivesPanel);
	
	new String:sTitle[64], String:sSkill[32];
	FormatEx(sTitle, sizeof sTitle, "USE SKILL 「%d」\n \n", _skillsActiveGetAvailable(iClient));
	SetMenuTitle(hMenu, sTitle);

	for(new i = 0 ; i < _:playerActivesTypes ; i++){
		FormatEx(sSkill, sizeof sSkill, "%s%s", g_iAvailableActive[iClient][i] ? "⭑" : " ", g_sActivesList[i]);
		
		AddMenuItem(hMenu, g_sActivesList[i], sSkill, g_iAvailableActive[iClient][i]  ? ITEMDRAW_DEFAULT : ITEMDRAW_DISABLED);
	}
	
	SetMenuOptionFlags(hMenu, MENUFLAG_NO_SOUND|MENUFLAG_BUTTON_EXIT);
	DisplayMenu(hMenu, iClient, 0);
}

new const String:g_sActiveCastColor[playerActivesTypes][] = {
	"196 128 32",
	"255 255 255",
	"225 225 225",
	"196 164 0",
	"225 32 0",
	"0 16 128"
};

_handlerActivesPanel(Handle:hMenu, MenuAction:mAction, iClient, iParam){
	if(mAction == MenuAction_Select){
		if(IsPlayerAlive(iClient)){
			new String:sChosen[16];
			GetMenuItem(hMenu, iParam, sChosen, sizeof sChosen);
			
			new iSkill = _skillActiveGetByName(sChosen);
			
			if(g_iAvailableActive[iClient][iSkill]){
				new String:sCasted[64];
				FormatEx(sCasted, sizeof sCasted, "\n\n\n\n\n\nCasted %s", sChosen);
				if(playerActivesTypes:iSkill==activeFirecamp)
					Format(sCasted, sizeof sCasted, "%s\nDuration: %.0fs", sCasted, g_fActivesData[activeFirecamp][g_ePlayerData[iClient][dataActives][activeFirecamp]]);
				
				switch(iSkill){
					case activeFrostball, activeFireball, activeGrab:{
						g_iAvailableActive[iClient][iSkill]=0;
						_skillsShootProjectile(iClient, playerActivesTypes:iSkill==activeFrostball ? skillShot_Frostball : playerActivesTypes:iSkill==activeFireball ? skillShot_Fireball : skillShot_Grab);
						PrintHudMessage(iClient, "4", "128 128 128", g_sActiveCastColor[iSkill], "2", "0.035", "0.5", "1.0", "0.5", sCasted, "-1", "-1");
						PrintHudMessage(iClient, "4", "128 128 128", g_sActiveCastColor[iSkill], "2", "0.035", "0.5", "1.0", "0.5", sCasted, "-1", "-1");
					}
					case activeFirecamp:{
						new iCasted = _skillsCreateCampfire(iClient);
						
						if(iCasted==1){
							g_iAvailableActive[iClient][activeFirecamp]=0;
							PrintHudMessage(iClient, "4", "128 128 128", g_sActiveCastColor[activeFirecamp], "2", "0.035", "0.5", "1.0", "0.5", sCasted, "-1", "-1");
							PrintHudMessage(iClient, "4", "128 128 128", g_sActiveCastColor[activeFirecamp], "2", "0.035", "0.5", "1.0", "0.5", sCasted, "-1", "-1");
						}
					}
					case activeStealth:{
						g_iAvailableActive[iClient][activeStealth]=0;
						_skillsUseStealth(iClient);
					}
					case activePush:{
						g_iAvailableActive[iClient][activePush]=0;
						_skillsUsePush(iClient);
						PrintHudMessage(iClient, "4", "128 128 128", g_sActiveCastColor[activePush], "2", "0.035", "0.5", "1.0", "0.5", sCasted, "-1", "-1");
						PrintHudMessage(iClient, "4", "128 128 128", g_sActiveCastColor[activePush], "2", "0.035", "0.5", "1.0", "0.5", sCasted, "-1", "-1");
					}
				}
				//PrintToChatAll("elo");
			}
			//_createActivesPanel(iClient);
		}
	}
}

_skillActiveGetByName(String:sSkill[]){
	for(new i = 0 ; i < _:playerActivesTypes ; i++){
		if(StrEqual(sSkill, g_sActivesList[i])) return i;
	}
	return -1;
}

_skillsShootProjectile(iClient, skillShots:iSkill){
	new Float:fEyes[3], Float:fAng[3], Float:fVelocity[3], Float:fFinal[3];
	GetClientEyePosition(iClient, fEyes);
	GetClientEyeAngles(iClient, fAng);
			
	AddInFrontOf(fEyes, fAng, 10.0, fFinal);

	GetClientEyeAngles(iClient, fAng);
	GetEntPropVector(iClient, Prop_Data, "m_vecVelocity", fVelocity);
			
	GetAngleVectors(fAng, fVelocity, NULL_VECTOR, NULL_VECTOR);
	NormalizeVector(fVelocity, fVelocity);
	if(iSkill==skillShot_Grab){
		ScaleVector(fVelocity, g_fActivesData[activeGrab][g_ePlayerData[iClient][dataActives][activeGrab]]);
	}
	else
		ScaleVector(fVelocity, g_fSkillShot_Props[iSkill][skillShot_ProjectileSpeed]);
	
	_skillsShoot(skillShots:iSkill, iClient, fFinal, fVelocity, fAng);
}

_skillsShoot(skillShots:iSkill, iClient, Float:fOri[3], Float:fVel[3], Float:fAng[3]){
	new iProjectile = CreateEntityByName("decoy_projectile");
	DispatchSpawn(iProjectile);
	
	SetEntPropEnt(iProjectile, Prop_Send, "m_hOwnerEntity", iClient);
	SetEntityModel(iProjectile, "models/props_junk/watermelon01.mdl");
	SetEntityRenderMode(iProjectile, RENDER_NONE);

	SetEntityGravity(iProjectile, 0.001);
	
	EmitAmbientSound(g_sSkillShot_Resources[iSkill][skillShot_Sound], fOri, iClient, SNDLEVEL_RAIDSIREN, _, 1.0);
	
	TeleportEntity(iProjectile, fOri, fAng, fVel);
	
	new iSprite = CreateEntityByName("env_sprite");
	if(iSprite != -1){
		new String:sScale[5];
		FloatToString(g_fSkillShot_Props[iSkill][skillShot_SpriteScale], sScale, sizeof sScale);
		
		DispatchKeyValue(iSprite, "spawnflags", "1"); 
		DispatchKeyValue(iSprite, "scale", sScale); 
		DispatchKeyValue(iSprite, "model", g_sSkillShot_Resources[iSkill][skillShot_Sprite]); 
		DispatchSpawn(iSprite);
		SetEntityRenderMode(iSprite, RENDER_TRANSADD);
		
		g_iChild[iProjectile] = iSprite;
	}
	
	switch(iSkill){
		case skillShot_Grab:{
			TE_SetupBeamFollow(iSprite, g_iPrecachedTrail[1][0], g_iPrecachedTrail[2][0], 1.75, 7.0, 9.0, 3500, {255, 0, 0, 196});
			TE_SendToAll();
			TE_SetupBeamFollow(iSprite, g_iPrecachedTrail[1][0], g_iPrecachedTrail[2][0], 0.75, 7.0, 9.0, 3000, {0, 196, 0, 255});
			TE_SendToAll();
			
			SDKHook(iProjectile, SDKHook_StartTouch, _skillOnGrabTouch);
		}
		case skillShot_Fireball:{
			TE_SetupBeamFollow(iSprite, g_iPrecachedTrail[1][0], g_iPrecachedTrail[1][1], 1.75, 8.0, 8.0, 3500, {225, 0, 0, 225});
			TE_SendToAll();
			TE_SetupBeamFollow(iSprite, g_iPrecachedTrail[1][0], g_iPrecachedTrail[1][1], 1.0, 8.0, 8.0, 3000, {255, 192, 0, 255});
			TE_SendToAll();	
			
			SDKHook(iProjectile, SDKHook_StartTouch, _skillOnFireballTouch);
		}
		case skillShot_Frostball:{
			TE_SetupBeamFollow(iSprite, g_iPrecachedTrail[1][0], g_iPrecachedTrail[1][1], 1.75, 8.0, 8.0, 3500, {0, 0, 255, 225});
			TE_SendToAll();
			TE_SetupBeamFollow(iSprite, g_iPrecachedTrail[1][0], g_iPrecachedTrail[1][1], 1.0, 8.0, 8.0, 3000, {0, 255, 0, 255});
			TE_SendToAll();	
			
			SDKHook(iProjectile, SDKHook_StartTouch, _skillOnFrostballTouch);
		}
	}
}

_skillCreateExplo(skillShots:iSkill, Float:fPos[3]){
	new iSprite = CreateEntityByName("env_sprite");
	if(iSprite!=-1){
		new String:sScale[5];
		FloatToString(g_fSkillShot_Props[iSkill][skillShot_Sprite2Scale], sScale, sizeof sScale);
		
		//PrintToChatAll(g_sSkillShot_Resources[iSkill][skillShot_Explo]);
		DispatchKeyValue(iSprite, "model", g_sSkillShot_Resources[iSkill][skillShot_Explo]);
		DispatchKeyValue(iSprite, "scale", sScale);
	
		DispatchSpawn(iSprite);
		ActivateEntity(iSprite);

		TeleportEntity(iSprite, fPos, NULL_VECTOR, NULL_VECTOR);
		
		g_iSprite[iSprite]=iSprite;
		CreateTimer(g_fSkillShot_Props[iSkill][skillShot_Sprite2Dur], taskKillSprite, iSprite);
	}
}

public _skillOnFrostballTouch(iFrostball, iClient) {
	if(!IsValidEntity(iFrostball))
		return;

	SDKUnhook(iFrostball, SDKHook_StartTouch, _skillOnFireballTouch);
	
	//new iAttacker = GetEntPropEnt(iFrostball, Prop_Send, "m_hOwnerEntity");
	
	new Float:fOri[3], Float:fTr[3], Float:fAng[3], Float:fExplo[3], Float:fSub=0.0;
	GetEntPropVector(iFrostball, Prop_Data, "m_vecOrigin", fOri);
	GetEntPropVector(iFrostball, Prop_Send, "m_angRotation", fAng);
	AddBehindOf(fOri, fAng, 48.0, fExplo);
	AddVectors(fOri, fTr, fTr);
	
	fTr[2]-=FROSTBALL_EXPLO_OFFSET;
	TR_TraceRayFilter(fOri, fTr, MASK_SHOT, RayType_EndPoint, trExplosion, iFrostball);
	if(TR_DidHit()){
		TR_GetEndPosition(fTr);
		fSub = FloatAbs(FROSTBALL_EXPLO_OFFSET-(fOri[2]-fTr[2]));
	}

	fExplo[2] += fSub;
	_skillCreateExplo(skillShot_Frostball, fExplo);
	
	EmitAmbientSound(g_sSkillSound_Fireball[skillSound_Fireball_Shot], fOri, iFrostball, SNDLEVEL_RAIDSIREN, _, 1.0);
	
	for(new i = 1 ; i<=MaxClients ;i++){
		if(IsClientInGame(i)){
			if(IsPlayerAlive(i) && GetClientTeam(i) != GetClientTeam(GetEntPropEnt(iFrostball, Prop_Send, "m_hOwnerEntity"))){
				new Float:fCur[3], Float:fDist;
				GetClientEyePosition(i, fCur);
				fDist = GetVectorDistance(fOri, fCur);
				
				if(fDist <= 120.0){
					ScreenFade(i, FFADE_PURGE, {255, 255, 255, 255}, GetRandomFloat(2.0, 3.5), 2);
				}
			}
		}
	}
	
	AcceptEntityInput(g_iChild[iFrostball], "kill");
	g_iChild[iFrostball]=-1;
	AcceptEntityInput(iFrostball, "kill");
}

public Action:taskKillSprite(Handle:hTimer, any:iSprite){
	if(g_iSprite[iSprite] != -1 && IsValidEntity(iSprite)) AcceptEntityInput(iSprite, "kill");
}

public _skillOnFireballTouch(iFireball, iClient) {
	if(!IsValidEntity(iFireball))
		return;

	SDKUnhook(iFireball, SDKHook_StartTouch, _skillOnFireballTouch);
	
	new iAttacker = GetEntPropEnt(iFireball, Prop_Send, "m_hOwnerEntity");
	
	new Float:fOri[3], Float:fTr[3], Float:fAng[3], Float:fExplo[3], Float:fSub=0.0;
	GetEntPropVector(iFireball, Prop_Data, "m_vecOrigin", fOri);
	GetEntPropVector(iFireball, Prop_Send, "m_angRotation", fAng);
	AddBehindOf(fOri, fAng, 50.0, fExplo);
	AddVectors(fOri, fTr, fTr);
	
	fTr[2]-=FIREBALL_EXPLO_OFFSET;
	TR_TraceRayFilter(fOri, fTr, MASK_SHOT, RayType_EndPoint, trExplosion, iFireball);
	if(TR_DidHit()){
		TR_GetEndPosition(fTr);
		fSub = FloatAbs(FIREBALL_EXPLO_OFFSET-(fOri[2]-fTr[2]));
	}

	fExplo[2] += fSub;
	_skillCreateExplo(skillShot_Fireball, fExplo);
	EmitAmbientSound(g_sSkillSound_Fireball[skillSound_Fireball_Shot], fOri, iFireball, SNDLEVEL_RAIDSIREN, _, 1.0);
	
	for(new i = 1 ; i<=MaxClients ;i++){
		if(IsClientInGame(i)){
			if(IsPlayerAlive(i) && GetClientTeam(i) != GetClientTeam(iAttacker)){
				new Float:fCur[3], Float:fDist, Float:fDmg;
				GetClientEyePosition(i, fCur);
				fDist = GetVectorDistance(fOri, fCur);
				
				if(fDist <= 140.0){
					new Float:fMaxDmg = g_fActivesData[activeFireball][g_ePlayerData[iAttacker][dataActives][activeFireball]];
					fDmg = fMaxDmg  - (0.75 * fDist);
					_headsplashDamage(i, iAttacker, fDmg, "weapon_hegrenade", DMG_SHOCK);
				}
			}
		}
	}
	
	AcceptEntityInput(g_iChild[iFireball], "kill");
	g_iChild[iFireball]=-1;
	AcceptEntityInput(iFireball, "kill");
}

public _skillOnGrabTouch(iGrab, iClient) {
	if(!IsValidEntity(iGrab))
		return;

	SDKUnhook(iGrab, SDKHook_StartTouch, _skillOnGrabTouch);
	
	new Float:fOri[3];
	
	if(IsPlayer(iClient) && IsClientInGame(iClient) && IsPlayerAlive(iClient)){
		new iAttacker = GetEntPropEnt(iGrab, Prop_Send, "m_hOwnerEntity");
		if(GetClientTeam(iAttacker) != GetClientTeam(iClient)){

			Entity_GetAbsOrigin(iClient, fOri);
			EmitAmbientSound(g_sSkillSound_Grab[skillSound_Grab_Pull], fOri, iClient, SNDLEVEL_RAIDSIREN, _, 1.0);
			
			g_iGrabbed[iAttacker] = iClient;
			g_fGrabTime[iAttacker] = GetGameTime();
			
			SetEntityRenderMode(iClient, RENDER_TRANSADD);
			SetEntityRenderColor(iClient, 255, 192, 0, 255);
			
			if(g_hTask[iClient][TASK_GRAB] != INVALID_HANDLE){ 
				KillTimer(g_hTask[iClient][TASK_GRAB]);
									
				g_hTask[iClient][TASK_GRAB] = INVALID_HANDLE;
			}
			g_hTask[iClient][TASK_GRAB] = CreateTimer(0.4, taskGrabOff, iClient);
		}
	}
	
	GetEntPropVector(iGrab, Prop_Data, "m_vecOrigin", fOri);
	for(new i = 0 ; i < 10 ; i++){
		fOri[0]+=GetRandomFloat(-1.2, 1.2);
		fOri[1]+=GetRandomFloat(-1.2, 1.2);
		TE_SetupEnergySplash(fOri, Float:{0.0, 0.0, 0.0}, true);
		TE_SendToAll();
	}
	
	AcceptEntityInput(g_iChild[iGrab], "kill");
	g_iChild[iGrab]=-1;
	AcceptEntityInput(iGrab, "kill");
}

public Action:taskGrabOff(Handle:hTimer, any:iClient){
	g_hTask[iClient][TASK_GRAB] = INVALID_HANDLE;
	
	if(IsClientInGame(iClient) && IsPlayerAlive(iClient)){
		SetEntityRenderMode(iClient, RENDER_NORMAL);
		SetEntityRenderFx(iClient, RENDERFX_NONE);
		SetEntityRenderColor(iClient, 255, 255, 255, 255);
	}
}

Float:_skillGrab_PullToAnother(iWho, iToWho){
	new Float:fPos[2][3], Float:fAng[3], Float:fDir[3];
	GetClientAbsOrigin(iWho, fPos[0]);
	GetClientAbsOrigin(iToWho, fPos[1]);

	GetClientEyeAngles(iToWho, fAng);
			
	AddInFrontOf(fPos[1], fAng, 10.0, fPos[1]);

	SubtractVectors(fPos[1], fPos[0], fDir);
	NormalizeVector(fDir, fDir);
	
	new Float:fDist = GetVectorDistance(fPos[0], fPos[1]);
	
	ScaleVector(fDir, fDist*10.0);
	TeleportEntity(iWho, NULL_VECTOR, NULL_VECTOR, fDir);
	
	return fDist;
}

_skillsUsePush(iClient){
    new Float:fPos[3];
    GetClientAbsOrigin(iClient, fPos);

    _skillsPushNearbyPlayers(fPos, GetClientTeam(iClient), g_fActivesData[activePush][g_ePlayerData[iClient][dataActives][activePush]]);
    //EmitAmbientSound(SOUND_PUSH, fPos);
    CreateParticleWithParent("explosion_basic_water", fPos, iClient);
}
 
_skillsPushNearbyPlayers(const Float:fPos[3], iIgnoreTeam, Float:PUSH_RADIUS){
    for(int i = 1; i <= MaxClients; ++i) {
        if(IsClientConnected(i) && IsPlayerAlive(i) && GetClientTeam(i) != iIgnoreTeam){
            new Float:fPlayerPos[3];
            GetClientAbsOrigin(i, fPlayerPos);
           	
            new Float:fDist = GetVectorDistance(fPos, fPlayerPos);
            if(fDist < PUSH_RADIUS) {
                new Float:fVelocity = 500.0 + 500.0 * (PUSH_RADIUS - fDist)/PUSH_RADIUS;
               
                new Float:fVel[3];
                if(GetEntityFlags(i) & FL_ONGROUND){
                    fVelocity += 1200.0;
                    fVel[2] = 0.0;
                }
                else{
                    new Float:victimVelo[3];
                    GetEntPropVector(i, Prop_Data, "m_vecVelocity", victimVelo);
                   
                    fVel[2] = FloatAbs(victimVelo[2]) + 25.0;
                }
                   
                fVel[0] = (fPlayerPos[0] - fPos[0])/fDist * fVelocity;
                fVel[1] = (fPlayerPos[1] - fPos[1])/fDist * fVelocity;
               
                TeleportEntity(i, NULL_VECTOR, NULL_VECTOR, fVel);
            }
        }
    }
}

_skillsUseStealth(iClient){
	g_iStealthTransparencyTicks[iClient] = 0;
	if(g_hStealthCast[iClient] != INVALID_HANDLE){
		KillTimer(g_hStealthCast[iClient]);
		g_hStealthCast[iClient] = INVALID_HANDLE;
	}
	g_hStealthCast[iClient] = CreateTimer(0.1, _skillsStealthTask, iClient);
   
	new Float:fPos[3];
	GetClientAbsOrigin(iClient, fPos);
    //EmitAmbientSound(SOUND_STEALTH, fPos);
}

public Action _skillsStealthTask(Handle:hTimer, iClient){
	g_hStealthCast[iClient] = INVALID_HANDLE;
  	
	if(!IsClientConnected(iClient) || !IsPlayerAlive(iClient))
		return;
  	
	++g_iStealthTransparencyTicks[iClient];
   
	new iAlpha = RoundToFloor(255.0 * (1.0 - 1.0*g_iStealthTransparencyTicks[iClient]/TICKS_FOR_FULL_STEALTH));
   
	if(iAlpha > 0){
    	//PrintToChatAll("%d", iAlpha);
		new String:sCast[32];
		_formatStealthCast(iAlpha, sCast, sizeof sCast);
			
		PrintHudMessage(iClient, "4", "219 200 255", "32 96 196", "0", "0", "0.3", "0.3", "0.22", sCast, "0.02", "0.33");
		PrintHudMessage(iClient, "4", "219 200 255", "32 96 196", "0", "0", "0.3", "0.3", "0.22", sCast, "0.02", "0.33");
		SetEntityRenderMode(iClient, RENDER_TRANSCOLOR);
		SetEntityRenderColor(iClient, 255, 255, 255, iAlpha);
		g_hStealthCast[iClient] = CreateTimer(0.1, _skillsStealthTask, iClient);
	}
	else{
		SetEntityRenderMode(iClient, RENDER_NONE);
		if(g_hStealthTimer[iClient] != INVALID_HANDLE){
			KillTimer(g_hStealthTimer[iClient]);
			g_hStealthTimer[iClient] = INVALID_HANDLE;
		}
		g_fStealthTime[iClient] = g_fActivesData[activeStealth][g_ePlayerData[iClient][dataActives][activeStealth]];
		g_hStealthTimer[iClient] = CreateTimer(0.1, _skillsStealthTaskHud, iClient);
    	
		new Float:fOri[3];
		Entity_GetAbsOrigin(iClient, fOri);
    	
		EmitAmbientSound("*playcore/playcore_sfx17.mp3", fOri, iClient, SNDLEVEL_RAIDSIREN, _, 0.5);
	}
}

_formatStealthCast(iVal, String:sOutput[], iLen){
	FormatEx(sOutput, iLen, "\n\n\n%s%s%s%s%s%s%s%s%s", iVal==229 ? "C" : "", iVal==204 ? "Ca" : "", iVal==178 ? "Cas" : "", iVal==153 ? "Cast" : "", iVal==127 ? "Casti" : "", iVal==101 ? "Castin" : "", iVal<=76 ? "Casting " : "", iVal <= 50 ? "." : "", iVal <= 25 ? "." : "");
}

public Action _skillsStealthTaskHud(Handle:hTimer, iClient){
	g_hStealthTimer[iClient] = INVALID_HANDLE;
	
	if(!IsClientInGame(iClient) || !IsPlayerAlive(iClient)) return;
	
	new String:sHud[32];
	FormatEx(sHud, sizeof sHud, "\n\n\n%.1f %s- Stealth Skill", g_fStealthTime[iClient], g_fStealthTime[iClient]<10.0 ? "  " : "");
	
	if(g_fStealthTime[iClient]<=4.0) PrintHudMessage(iClient, "4", "204 0 0", "192 96 0", "0", "0", "0.3", "0.3", "0.22", sHud, "0.02", "0.33");//"-1", "-0.05");
	else PrintHudMessage(iClient, "4", "196 196 255", "32 96 196", "0", "0", "0", "0", "0.22", sHud, "0.02", "0.33");//"-1", "-0.05");
	
	//PrintHudMessage(iClient, "0", "204 0 0", "192 96 0", "0", "0", "0.3", "0.3", "0.22", sHud, "0.02", "0.33");//"-1", "-0.05");
	
	if(g_fStealthTime[iClient]>=0.1){
		g_fStealthTime[iClient]-=0.1;
		g_hStealthTimer[iClient] = CreateTimer(0.1, _skillsStealthTaskHud, iClient);
	}
	else
		resetRendering(iClient);
}

_skillsCreateCampfire(iClient){
	if(!(GetEntityFlags(iClient) & FL_ONGROUND)){
		PrintHudMessage(iClient, "4", "255 0 32", "196 128 32", "2", "0.035", "0.5", "1.25", "1.5", "\n\n\n\n\nYou must be on ground", "-1", "-1");
		PrintHudMessage(iClient, "4", "255 0 32", "196 128 32", "2", "0.035", "0.5", "1.25", "1.5", "\n\n\n\n\nYou must be on ground", "-1", "-1");
		return 0;
	}
    
	new iTeam = GetClientTeam(iClient);
	new iTicks = RoundFloat(g_fActivesData[activeFirecamp][g_ePlayerData[iClient][dataActives][activeFirecamp]]);
	
	new Float:fPos[3];
	GetClientAbsOrigin(iClient, fPos);
	fPos[2] += 8.0;
   
	new iEnt = _skillCampfireCreateModel(fPos, Float:{0.0, 0.0, 0.0}, 0);
	_skillCampfireCreateModel(fPos, Float:{0.0, 0.0, -52.0}, iEnt);
	_skillCampfireCreateModel(fPos, Float:{0.0, 120.0, -52.0}, iEnt);
	_skillCampfireCreateModel(fPos, Float:{0.0, 240.0, -52.0}, iEnt);
   
	CreateParticleWithParent("env_fire_small", fPos, iEnt);
	_skillCampfireCreateEffect(iEnt);
	
	_skillCampfireCreateTask(iClient, iTeam, iEnt, iTicks);
    //EmitAmbientSound(SOUND_CAMPFIRE, fPos);

	return 1;
}

_skillCampfireCreateModel(const Float:fPos[3], const Float:fAng[3], iParent){
    new Float:fRealPos[3];
    fRealPos[0] = fPos[0];
    fRealPos[1] = fPos[1];
    fRealPos[2] = fPos[2];

    if(iParent > 0) {
        fRealPos[0] -= Sine(DegToRad(fAng[1]))*CAMP_FIRE_DIFF;
        fRealPos[1] += Cosine(DegToRad(fAng[1]))*CAMP_FIRE_DIFF;
    }
 
    new iEnt = CreateEntityByName("prop_physics_override");
    DispatchKeyValue(iEnt, "model", g_sSkill_Campfire_Assets[skillCampfire_Model]);
    TeleportEntity(iEnt, fRealPos, fAng, NULL_VECTOR);
    DispatchSpawn(iEnt);
    SetEntityMoveType(iEnt, MOVETYPE_NONE);
    SetEntProp(iEnt, Prop_Data, "m_CollisionGroup", 2);
   
    if(iParent > 0){
        SetVariantString("!activator");
        AcceptEntityInput(iEnt, "SetParent", iParent, iEnt, 0);
    }
    else{
        SetEntityRenderMode(iEnt, RENDER_NONE);
    }
   
    return iEnt;
}

_skillCampfireCreateEffect(iEnt){
    float fPos[3];
    GetEntPropVector(iEnt, Prop_Send, "m_vecOrigin", fPos);
   
    new iSprite = CreateEntityByName("env_sprite");
    DispatchKeyValueVector(iSprite, "origin", fPos);
    DispatchKeyValue(iSprite, "spawnflags", "0");
    DispatchKeyValue(iSprite, "rendermode", "5");
    DispatchKeyValue(iSprite, "scale", "0.75"); 
    DispatchKeyValue(iSprite, "model", g_sSkill_Campfire_Assets[skillCampfire_Sprite]);
   
    DispatchSpawn(iSprite);
    ActivateEntity(iSprite);
 
    SetVariantString("!activator");
    AcceptEntityInput(iSprite, "SetParent", iEnt, iSprite, 0);
       
    AcceptEntityInput(iSprite, "ShowSprite");
}

_skillCampfireHealNearbyPlayers(iTeam, iEnt){
    static Float:fCampPos[3], Float:fPos[3];
    GetEntPropVector(iEnt, Prop_Send, "m_vecOrigin", fCampPos);
   
    _skillCampfireAddBeam(fCampPos);
   
    for(new i = 1; i <= MaxClients; ++i) {
        if(IsClientConnected(i) && IsPlayerAlive(i) && GetClientTeam(i) == iTeam) {
            GetClientAbsOrigin(i, fPos);
           
            if(GetVectorDistance(fCampPos, fPos) < CAMP_FIRE_RADIUS) {
                new iMaxHP = _mmGetMaxHealth(i);
                new iHeal = 2; // TODO: skill * base_heal_per_skill
                new iClientHP = GetClientHealth(i);
               
                if(iClientHP < iMaxHP) {
                    new iNewHealth = iClientHP + iHeal;
                    if(iNewHealth > iMaxHP)
                        iNewHealth = iMaxHP;

                    SetEntityHealth(i, iNewHealth);
                }
            }
        }
    }
}

_skillCampfireCreateTask(iClient, iTeam, iEnt, iTicks){
	DataPack hPack;
	g_hCampFireTaskHandle[iClient] = CreateDataTimer(1.0, _skillCampfireTask, hPack);
	hPack.WriteCell(iClient);
	hPack.WriteCell(iTeam);
	hPack.WriteCell(iEnt);
	hPack.WriteCell(iTicks);
}

public Action _skillCampfireTask(Handle hTimer, DataPack hPack){
	hPack.Reset();
   
	new iClient = hPack.ReadCell();
	new iTeam = hPack.ReadCell();
	new iEnt = hPack.ReadCell();
 	new iTicks = hPack.ReadCell();
   	
   	g_hCampFireTaskHandle[iClient] = INVALID_HANDLE;
   	
	_skillCampfireHealNearbyPlayers(iTeam, iEnt);
   
	if(iTicks > 0)
		_skillCampfireCreateTask(iClient, iTeam, iEnt, iTicks-1);
	else AcceptEntityInput(iEnt, "Kill");
}

_skillCampfireAddBeam(const Float:fPos[3]) {  
    new Float:fFixedPos[3];
    fFixedPos[0] = fPos[0];
    fFixedPos[1] = fPos[1];
    fFixedPos[2] = fPos[2] + 16.0;
   
    new model = PrecacheModel("materials/sprites/laserbeam.vmt");

    TE_SetupBeamRingPoint(fFixedPos, 16.0, CAMP_FIRE_RADIUS*2, model, model, 0, 0, 0.5, 1.0, 0.0, {255, 60, 20, 150}, 1, 0);
    TE_SendToAll();
}