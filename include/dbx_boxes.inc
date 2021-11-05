#if defined _dbx_boxes_included
 #endinput
#endif
#define _dbx_boxes_included

#define BOXES_RESOURCES_COUNT			6
#define RARE_BOX_MAGICPOINTS_CHANCE 	7
#define EPIC_BOX_TRAILSCROLL_CHANCE 	5
native speedoDisable(iClient, Float:fTime);
native speedoEnable(iClient);
enum dataBoxSlotOperations{
	BOXSLOT_ADD,
	BOXSLOT_REMOVE
};

enum dataBoxSprites{
	BOXSPRITE_LASER,
	BOXSPRITE_HALO
};

enum dataBoxTypes {
	dataBoxType_Common,
	dataBoxType_Rare,
	dataBoxType_Epic,
	dataBoxType_Starter,
	dataBoxType_BeamScroll,
	dataBoxType_TrailScroll,
	dataBoxType_LaserTrail,
	dataBoxType_2ColorsTrail,
	dataBoxType_FrostyTrail,
	dataBoxType_CameleonTrail,
	dataBoxType_ColorfulTrail,
	dataBoxType_StarzTrail,
	dataBoxType_DotsTrail,
	dataBoxType_ThunderTrail,
	dataBoxType_BreathTrail,
	dataBoxType_BluezTrail,
	dataBoxType_RainbowTrail,
	dataBoxType_SmokeyTrail,
	dataBoxType_SnowflakesTrail,
	dataBoxType_MsVioletTrail,
	dataBoxType_MsGreenTrail,
	dataBoxType_FireTrail
};

new String:g_sLootBags[dataBoxTypes][] = {
	"LootBox (Common)",
	"LootBox (Rare)",
	"LootBox (EPIC)",
	"Starter Box",
	"Magic points",
	"Trail Rune",
	"Laser Trail",
	"2Colors Trail",
	"Frosty Trail",
	"Cameleon Trail",
	"Colorful Trail",
	"StarZ Trail",
	"Dots Trail",
	"Thunder★ Trail",
	"Breath★ Trail",
	"Bluez★ Trail",
	"Rainbow★ Trail",
	"Smokey★ Trail",
	"Snowflakes★ Trail",
	"Ms.Violet★ Trail",
	"Ms.Green★ Trail",
	"Fire★ Trail"
};

new String:g_sLootBagDesc[dataBoxTypes][] = {
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Contains 10-20$     \n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Contains 20-40$ and \n   has a chance to win\n   Model Rune\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Contains 40-80$ and \n   has a chance to win\n   Trail Rune\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Contains 500-800$   \n   and a random Model\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Unlocks random      \n   rarity Player Model\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Unlocks random      \n   rarity Grenade Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Laser Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   2Colors Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Frosty Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Cameleon Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Colorful Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   StarZ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Dots Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Thunder★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Breath★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Bluez★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Rainbow★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Smokey★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Snowflakes★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Ms.Violet★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Ms.Green★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n",
	"\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n   Permanently unlocks \n   Fire★ Trail\n  ▬▬▬▬▬▬▬▬▬▬▬▬\n \n"
};

new const String:g_sBoxNamesAnim[6][] = {
	"LOOTBOX(Common)",
	"  LOOTBOX(Rare)",
	"  LOOTBOX(Epic)",
	"BEGINNERS BOX",
	"BEAM Rune",
	"TRAIL Rune"
};

new const String:g_sBoxModels[6][] = {
	"models/playcorehns_misc/lootboxes/lootbox_common.mdl",
	"models/playcorehns_misc/lootboxes/lootbox_rare.mdl",
	"models/playcorehns_misc/lootboxes/lootbox_epic.mdl",
	"",
	"",
	""
};

new const String:g_sBoxHexClr[6][] = {
	"009933",
	"4d8dee",
	"be06d2",
	"ffffff",
	"fae456",
	"4ADAAE"
};

new const String:g_sBoxAnimHex[26][] = {
	"cc0000",
	"ff5050",
	"fd6453",
	"fd7553",
	"fc9754",
	"fdb553",
	"fdc153",
	"fdd253",
	"fed852",
	"fddb53",
	"fde853",
	"fafe52",
	"f0fd53",
	"d8fe52",
	"c0fb55",
	"acfc54",
	"9bfd53",
	"7efc54",
	"0afa10",
	"04bf09",
	"03b407",
	"029305",
	"018704",
	"017604",
	"016503",
	"015403"
};

new const String:g_sBoxResources[BOXES_RESOURCES_COUNT][] = {
	".mdl",
	".vvd",
	".phy",
	".dx90.vtx",
	".vmt",
	".vtf"
};

new const String:g_sBoxSprites[dataBoxSprites][] = {
	"materials/sprites/laserbeam.vmt",
	"materials/sprites/halo.vmt"
};

new const String:g_sBoxSound[] = "sound/playcore/playcore_sfx00.mp3";

new g_iBoxRGB[dataBoxTypes][3];
new g_iBoxSprites[dataBoxSprites];
new g_iPlayerBoxes[PLAYER_TAB][dataBoxTypes];
new g_iOpeningBox[PLAYER_TAB];
new g_iOpeningProgress[PLAYER_TAB];
new g_iSelectedBag[PLAYER_TAB];
new g_iBoxTrigered[2048];
new g_iBoxType[2048];
new g_iBox[2048];

new String:g_sBoxSoundCached[64];

new Handle:g_hOpeningBox[MAXPLAYERS+1];

_boxesChangeAmount(iClient, dataBoxSlotOperations:iOperation, iType, iAmount){
	g_iPlayerBoxes[iClient][iType]=iOperation==BOXSLOT_ADD ? g_iPlayerBoxes[iClient][iType]+iAmount : g_iPlayerBoxes[iClient][iType]-iAmount;
	//PrintToChatAll("Adding: %d x %d box", iAmount, iType);
	_mmSqlUpdateLootbagSlot(iClient, iType);
}

_boxesLootItemsCount(iClient){
	new iSum = 0;
	for(new i = 0 ; i < _:dataBoxTypes; i++){
		if(g_iPlayerBoxes[iClient][i]) iSum += g_iPlayerBoxes[iClient][i];
	}
	
	return iSum;
}

_boxesLootGetFirstItem(iClient){
	for(new i = 0 ; i < _:dataBoxTypes ; i++){
		if(g_iPlayerBoxes[iClient][i]) return i;
	}
	return -1;
}

_boxesLootGetSlottedItems(iClient){
	new iSlots = 0;
	for(new i = 0 ; i < _:dataBoxTypes ; i++){
		if(g_iPlayerBoxes[iClient][i]) iSlots++;
	}
	return iSlots;
}

_boxesLootItemCount(iClient, iType){
	return g_iPlayerBoxes[iClient][iType] > 0 ? g_iPlayerBoxes[iClient][iType] : -1;
}

_boxesLootNextItem(iClient, iCur){
	if(!_boxesLootHasItems(iClient)) return -1;
	
	new iLastSlot = _:dataBoxTypes-1;
	if(iCur < iLastSlot){
		iCur++;
		
		if(g_iPlayerBoxes[iClient][iCur])
			return iCur;
		else
			return _boxesLootNextItem(iClient, iCur);
	}
	else{
		iCur = 0;
		
		if(g_iPlayerBoxes[iClient][iCur])
			return iCur;
		else
			return _boxesLootNextItem(iClient, iCur);
	}
}

_boxesLootHasItems(iClient){
	for(new i = 0 ; i < _:dataBoxTypes ; i++){
		if(_boxesLootItemCount(iClient, i) > 0) return 1;
	}
	return 0;
}


_boxesFormatOpeningBar(String:sBar[], iLen, iProgress){
	const iMax = 25;
	FormatEx(sBar, iLen, "[", g_sBoxAnimHex[25-iProgress]);
	
	for(new i = 0 ; i < iMax ; i++){
		Format(sBar, iLen, "%s|%s", sBar, i == iMax-1 ? "]" : "");
	}

	for(new i = 0 ; i < iProgress ; i++)
		sBar[1+i] = ')';
}

_boxesSpawnBoxAtOrigin(iFor, Float:fOrigin[3], iType=dataBoxType_Common){
	new iEnt = CreateEntityByName("prop_physics_override");
	DispatchKeyValue(iEnt, "model", g_sBoxModels[iType]);
	TeleportEntity(iEnt, fOrigin, NULL_VECTOR, NULL_VECTOR);
	DispatchSpawn(iEnt);
	
	EmitAmbientSound(g_sBoxSoundCached, fOrigin, iEnt, SNDLEVEL_RAIDSIREN, SND_NOFLAGS, 0.15);	 
	
	SetEntityMoveType(iEnt, MOVETYPE_VPHYSICS);
	SetEntProp(iEnt, Prop_Send, "m_usSolidFlags", 0x0008); 
	SetEntProp(iEnt, Prop_Data, "m_nSolidType", 6);
	SetEntProp(iEnt, Prop_Send, "m_CollisionGroup", 1);
	
	TeleportEntity(iEnt, NULL_VECTOR, NULL_VECTOR, Float:{0.0, 0.0, 300.0});
	
	SDKHook(iEnt, SDKHook_Touch, _boxesPickedUp);
		
	SetEntityRenderColor(iEnt, g_iBoxRGB[iType][0], g_iBoxRGB[iType][1], g_iBoxRGB[iType][2], 255);
	
	g_iBoxType[iEnt] = iType;
	g_iBox[iEnt] = 1337;
		
	new iRgbA[4]={0, 0, 0, 196};
	for(new i = 0 ; i < 3 ; i++) iRgbA[i] = g_iBoxRGB[iType][i];
		
	fOrigin[2]-= 5.0
	for(new i = 0 ; i < 3 ; i++){
		TE_SetupBeamRingPoint(fOrigin, 4.0, 45.0, g_iBoxSprites[BOXSPRITE_LASER], g_iBoxSprites[BOXSPRITE_HALO],  1, 30, 0.4, 1.0, 35.0, iRgbA, 10, 0);
		TE_SendToAll();
			
		fOrigin[2]+=5.0;
	}
		
	fOrigin[2]-=40.0
	for(new i = 0 ; i < 50 ; i++){
		fOrigin[2]+=1.0;
		fOrigin[0] += GetRandomFloat(-1.0, 1.0);
		fOrigin[1] += GetRandomFloat(-1.0, 1.0);
		TE_SetupEnergySplash(fOrigin, Float:{0.0, 0.0, 0.0}, true);
		TE_SendToAll();
	}
	
	iRgbA[3]=255;
	TE_SetupBeamFollow(iEnt, g_iBoxSprites[BOXSPRITE_LASER], 0, 1.5, 4.0, 4.0, 50000, iRgbA);
	TE_SendToAll();
	g_iBoxTrigered[iEnt]=-1;
	CreateTimer(0.6, taskEffectOnBox, iEnt);
	
	SetEntPropEnt(iEnt, Prop_Send, "m_hOwnerEntity", iFor);
}
_boxesSpawnBox(iOwner, iVictim, iType=dataBoxType_Common){	
#pragma unused iVictim
	if(IsClientInGame(iOwner)){
		new iEnt = CreateEntityByName("prop_physics_override");
		DispatchKeyValue(iEnt, "model", g_sBoxModels[iType]);
		new Float:fOrigin[3];
		GetAimOrigin(iOwner, fOrigin);
		TeleportEntity(iEnt, fOrigin, NULL_VECTOR, NULL_VECTOR);
		DispatchSpawn(iEnt);
	
		EmitAmbientSound(g_sBoxSoundCached, fOrigin, iEnt, SNDLEVEL_RAIDSIREN, SND_NOFLAGS, 0.15);	 
	
		SetEntityMoveType(iEnt, MOVETYPE_VPHYSICS);
		SetEntProp(iEnt, Prop_Send, "m_usSolidFlags", 0x0008); 
		SetEntProp(iEnt, Prop_Data, "m_nSolidType", 6);
		SetEntProp(iEnt, Prop_Send, "m_CollisionGroup", 1);
	
		TeleportEntity(iEnt, NULL_VECTOR, NULL_VECTOR, Float:{0.0, 0.0, 340.0});
	
		SDKHook(iEnt, SDKHook_Touch, _boxesPickedUp);
		
		SetEntityRenderColor(iEnt, g_iBoxRGB[iType][0], g_iBoxRGB[iType][1], g_iBoxRGB[iType][2], 255);
	
		g_iBoxType[iEnt] = iType;
		g_iBox[iEnt] = 1337;
		
		new iRgbA[4]={0, 0, 0, 196};
		for(new i = 0 ; i < 3 ; i++) iRgbA[i] = g_iBoxRGB[iType][i];
		
		fOrigin[2]-= 5.0
		for(new i = 0 ; i < 3 ; i++){
			TE_SetupBeamRingPoint(fOrigin, 4.0, 45.0, g_iBoxSprites[BOXSPRITE_LASER], g_iBoxSprites[BOXSPRITE_HALO],  1, 30, 0.4, 1.0, 35.0, iRgbA, 10, 0);
			TE_SendToAll();
			
			fOrigin[2]+=5.0;
		}
		
		fOrigin[2]-=40.0
		for(new i = 0 ; i < 50 ; i++){
			fOrigin[2]+=1.0;
			fOrigin[0] += GetRandomFloat(-1.0, 1.0);
			fOrigin[1] += GetRandomFloat(-1.0, 1.0);
			TE_SetupEnergySplash(fOrigin, Float:{0.0, 0.0, 0.0}, true);
			TE_SendToAll();
		}
		
		iRgbA[3]=255;
		TE_SetupBeamFollow(iEnt, g_iBoxSprites[BOXSPRITE_LASER], 0, 1.5, 4.0, 4.0, 50000, iRgbA);
		TE_SendToAll();
		g_iBoxTrigered[iEnt]=-1;
		SetEntPropEnt(iEnt, Prop_Send, "m_hOwnerEntity", iOwner);
		CreateTimer(0.6, taskEffectOnBox, iEnt);
	}
}

_boxesStartOpeningBox(iClient, iType){
	if(g_hOpeningBox[iClient]==INVALID_HANDLE){
		_boxesChangeAmount(iClient, BOXSLOT_REMOVE, iType, 1);
		_statChange(iClient, STATs_ADD, statsBoxesOpened, 1);
		
		g_iOpeningBox[iClient]=iType;
		g_iOpeningProgress[iClient]=25;
		
		g_hOpeningBox[iClient] = CreateTimer(0.1, taskOpening, iClient);
	}
}

public _boxesPickedUp(iEnt, iClient){
	if(!(IsPlayer(iClient) && IsPlayerAlive(iClient)) || g_iBoxType[iEnt] == -1)
		return; 

	if(GetEntPropEnt(iEnt, Prop_Send, "m_hOwnerEntity") != iClient){
		speedoDisable(iClient, 3.0);
		PrintHintText(iClient, "<span class='fontSize-m'>Box belongs to</span>\n<font color='#FFAA00'>%N", GetEntPropEnt(iEnt, Prop_Send, "m_hOwnerEntity"));
		return;
	}
	_boxesChangeAmount(iClient, BOXSLOT_ADD, g_iBoxType[iEnt], 1);
	_statChange(iClient, STATs_ADD, statsBoxes, 1);
	speedoDisable(iClient, 3.0);
	PrintHintText(iClient, "<span class='fontSize-m'>Picked up</span>\n<span class='fontSize-xl'><font color='#%s'>%s<span class='fontSize-m'></font>\nTotal items:<font color='#FFFFFF'><b> %d</b>", g_sBoxHexClr[g_iBoxType[iEnt]], g_sLootBags[g_iBoxType[iEnt]], _boxesLootItemsCount(iClient));
	
	new Float:fOrigin[3], iRgbA[4]={0, 0, 0, 192};
	GetEntPropVector(iEnt, Prop_Send,"m_vecOrigin", fOrigin);
	
	for(new i = 0 ; i < 3 ; i++) iRgbA[i] = g_iBoxRGB[g_iBoxType[iEnt]][i];
	
	fOrigin[2]-=10.0;
	for(new i = 0 ; i < 10 ; i++){
		fOrigin[2]+=1.0;
		TE_SetupEnergySplash(fOrigin, Float:{0.0, 0.0, 0.0}, true);
		TE_SendToAll();
	}

	for(new i = 0 ; i < 3 ; i++){
		TE_SetupBeamRingPoint(fOrigin, 55.0, 1.0, g_iBoxSprites[BOXSPRITE_LASER], g_iBoxSprites[BOXSPRITE_HALO],  1, 30, 0.4, 1.75, 35.0, iRgbA, 10, 0);
		TE_SendToAll();
		
		fOrigin[2]+=12.0;
	}
	
	iRgbA[3] = 110;
	ScreenFade(iClient, FFADE_IN|FFADE_PURGE|FFADE_MODULATE, iRgbA, 0.4, 0);
	
	g_iBox[iEnt]=-1;
	g_iBoxTrigered[iEnt]=-1;
	g_iBoxType[iEnt] = -1;
	
	SDKUnhook(iEnt, SDKHook_Touch, _boxesPickedUp);
	AcceptEntityInput(iEnt, "kill");
	RemoveEdict(iEnt);
}

public Action:taskEffectOnBox(Handle:hTimer, any:iBox){
	if(IsValidEntity(iBox) && g_iBox[iBox]==1337){
		AcceptEntityInput(iBox, "disablemotion");
		
		new Float:fOrigin[3];
		GetEntPropVector(iBox, Prop_Send,"m_vecOrigin", fOrigin);
		new iRgbA[4]={0, 0, 0, 16};
		for(new i = 0 ; i < 3 ; i++) iRgbA[i] = g_iBoxRGB[g_iBoxType[iBox]][i];
		
		fOrigin[2]-=0.5;
		for(new i = 0 ; i < 3 ; i++){
			TE_SetupBeamRingPoint(fOrigin, 1.0, i==2? 45.0 : 30.0, g_iBoxSprites[BOXSPRITE_LASER], g_iBoxSprites[BOXSPRITE_HALO],  1, 30, 0.2, 10.0, 0.1, iRgbA, 10, 0);
			TE_SendToAll();
		}
		
		g_iBoxTrigered[iBox]=1;
	}
}