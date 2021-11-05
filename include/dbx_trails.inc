#if defined _dbx_trails_included
 #endinput
#endif
#define _dbx_trails_included

#define MAX_TRAILS 		9
#define MAX_PARTICLES 	8

enum dataParticles{
	PARTICLE_PATH,
	PARTICLE_NAME
};

enum{
	trailFloat_width,
	trailFloat_life,
	trailFloat_velo
};

enum{
	trail_none,
	trail_laser,
	trail_2colors,
	trail_frosty,
	trail_cameleon,
	trail_rainbow,
	trail_stars,
	trail_plazma,
	trail_thunder
};

new const Float:g_fTrailsFloats[MAX_TRAILS][3] = {
/*  width	life	velo				*/	
	{0.0,	0.0,	0.0},
	{1.5,	5.0,	2500.0},
	{2.25,	4.0,	500.0},
	{3.0,	2.5,	1500.0},
	{4.0,	4.0,	500.0},
	{1.5,	5.0,	4500.0},
	{3.25, 	3.0,	10000.0},
	{3.00, 	1.5,	5000.0},
	{5.00,	5.0,	2000.0}
};

new const g_iTrailsRgb[MAX_TRAILS][4] = {
/*  red		green	blu		alpha		*/	
	{0, 	0, 		0, 		0},
	{255, 	0, 		16, 	255},
	{128,	0	, 	16, 	225},
	{8,		16,		255,	200},
	{100, 	196, 	0, 		255},
	{230, 	196, 	230, 	255},
	{200, 	255,	16, 	250},
	{255, 	255,	0,	 	255},
	{255,	255,	255,	225}
};

new String:g_sTrailName[MAX_TRAILS][] = {
	"Default",
	"Laser",
	"2Colors",
	"Frosty",
	"Cameleon",
	"Colorful",
	"StarZ",
	"Dots",
	"Thunder★"
};
new const String:g_sTrailsPaths[MAX_TRAILS][2][] = {
	{"",													""},
	{"materials/supporter_tracers/laser", 					"materials/supporter_tracers/laser_beam"},
	{"materials/trails/beam_01", 							""},
	{"materials/supporter_tracers/plasma_beam", 			""},
	{"materials/supporter_tracers/phys_beam", 				""},
	{"materials/supporter_tracers/rainbow_arrow", 			""},
	{"materials/playcorehns_misc/vFX_0x0L",/*stars*/ 			""},
	{"materials/playcorehns_misc/vFX_0x0M",/*plazma*/ 		""},
	{"materials/playcorehns_misc/vFX_0x0N",/*thunder*/ 		""}
};

new const g_iRandomColors[7][4] = {
	{255, 0, 0, 240},
	{255, 255, 0, 240},
	{64, 255, 255, 240},
	{255, 0, 255, 240},
	{255, 255, 0, 240},
	{0, 255, 0, 240},
	{0, 0, 255, 240}
};

new const String:g_sParticleFile[] = "particles/2j.pcf";
new const String:g_sParticles[MAX_PARTICLES][dataParticles][] = {
	{"new_002", 				"Breath★"}, //28
	{"puzyr", 					"Bluez★"}, //34
	{"puzyr2", 					"Rainbow★"}, //35
	{"smoking", 				"Smokey★"}, //36
	{"trail2_color1", 			"Snowflakes★"}, //46
	{"trail_new_001", 			"Ms.Violet★"}, //47
	{"trail_new_002", 			"Ms.Green★"}, //48
	{"trail_new_003_final", 	"Fire★"} //49
};

new g_iUnlockedTrail[PLAYER_TAB][MAX_TRAILS+MAX_PARTICLES];
new g_iChosenTrail[PLAYER_TAB];
new g_iPrecachedTrail[MAX_TRAILS][2];
new g_iNadeType[2048];

new Handle:g_hTrailCookie;

new g_iTParticle[MAXPLAYERS+1];
public Action:cmdTrailMenu(client){	
	new Handle:hPanel = CreatePanel();
	new String:sTitle[64];
	FormatEx(sTitle, sizeof sTitle, "Current %d\nChoose\n \n", g_iTParticle[client]);
	SetPanelTitle(hPanel, sTitle);

	DrawPanelItem(hPanel, "+");
	DrawPanelItem(hPanel, "-");
	DrawPanelText(hPanel, "\n \n");
	DrawPanelItem(hPanel, "Exit");
	
	SendPanelToClient(hPanel, client, TrailMenu_Cb, 0);
	return Plugin_Handled;
}

public TrailMenu_Cb(Handle:hMenu, MenuAction:action, client, iKey){
	new bool:bRefreshMenu=false;
	switch(iKey){
		case 1,2:{
			g_iTParticle[client] = iKey == 1 ? g_iTParticle[client] < 49 ? g_iTParticle[client]+1 : 0 : g_iTParticle[client]>0 ? g_iTParticle[client]-1 : 49;
			bRefreshMenu=true;
		}
	}
	if(bRefreshMenu) cmdTrailMenu(client);
}

_trailsCreateCookies(){
	g_hTrailCookie = RegClientCookie("DBX_TrailCk", "DBX TrailCk", CookieAccess_Protected);
}

_trailsSaveCookies(client){
	new String:sCookie[3];				
	IntToString(g_iChosenTrail[client], sCookie, sizeof(sCookie));
 
	SetClientCookie(client, g_hTrailCookie, sCookie);
	//PrintToChatAll(sCookie);
}

_trailsLoadCookies(client){
	new String:sCookie[3];
	GetClientCookie(client, g_hTrailCookie, sCookie, sizeof sCookie);
	
	g_iChosenTrail[client] = StringToInt(sCookie);
}

_trailsGetUnlocked(client)
{
	int iSum = 0;
	for(int i = 0 ; i < MAX_TRAILS+MAX_PARTICLES; i++)
	{
		if(g_iUnlockedTrail[client][i])
			iSum++;
	}
	return iSum;
}

_trailsPrecacheTrails(){
	//PrecacheEffect("ParticleEffect");
	PrecacheGeneric(g_sParticleFile, true);
	//pomexa
	new String:sPath[128], String:sTempPath[128];

	for(new i = 1 ; i < MAX_TRAILS ; i++){
		new x;
		for(x = 0 ; x < 2 ; x++)
			g_iPrecachedTrail[i][x] = -1;
		
		FormatEx(sPath, sizeof sPath, "%s", g_sTrailsPaths[i][0]);
		
		for(x = 0 ; x < 2 ; x++){
			FormatEx(sTempPath, sizeof sTempPath, "%s.%s", sPath, x == 0 ? "vmt" : "vtf");
			
			AddFileToDownloadsTable(sTempPath);
			
			if(!x){
				g_iPrecachedTrail[i][0] = PrecacheModel(sTempPath);
				//LogError("i=%d x=%d %s", i, x, sTempPath);
				
			}
		}
		
		new iSecondPathLen = strlen(g_sTrailsPaths[i][1]);
		
		if(iSecondPathLen>1){
			FormatEx(sPath, sizeof sPath, "%s", g_sTrailsPaths[i][1]);
			
			for(x = 0 ; x < 2 ; x++){
				FormatEx(sTempPath, sizeof sTempPath, "%s.%s", sPath, x == 0 ? "vmt" : "vtf");
			
				AddFileToDownloadsTable(sTempPath);
			
				if(!x){
					g_iPrecachedTrail[i][1] = PrecacheModel(sTempPath);
					//LogError("i=%d x=%d %s", i, x, sTempPath);
				}
			}
		}
	}
}

public Handle _trailsList(client)
{
	char sField[128], sData[128];
	
	Menu hMenu = new Menu(_trailsList_Cb);
	
	char sTitle[192], sOption[128];
	FormatEx(sTitle, sizeof sTitle, "%s MoneyMod\nGrenade Trails\n ", COMMUNITYNAME);
	hMenu.SetTitle(sTitle);
	
	int i;
	for(i = 0; i < MAX_TRAILS; i++)
	{
		FormatEx(sField, sizeof sField, "  %s  ⭑%s", _trailGetClientTrail(client) == i ? "‣" : "  ", g_iUnlockedTrail[client][i] ? g_sTrailName[i] : "---LOCKED--- ");
		AddMenuItem(hMenu, sField, sField);
	}

	for(i = 0; i < MAX_PARTICLES; i++)
	{
		FormatEx(sField, sizeof(sField), "  %s  ⭑%s", _trailGetClientTrail(client) == MAX_TRAILS+i ? "‣" : "  ", g_iUnlockedTrail[client][MAX_TRAILS+i] ? g_sParticles[i][PARTICLE_NAME] : "---LOCKED--- ");
		FormatEx(sData, sizeof(sData), g_sParticles[i][PARTICLE_PATH]);
		hMenu.AddItem(sData, sField);
	}
	
	hMenu.ExitButton 				= true;
	hMenu.ExitBackButton 			= true;
	hMenu.Display(client, 			MENU_TIME_FOREVER);

	return hMenu;
}

_trailsList_Cb(Handle hMenu, MenuAction action, int client, int iParam)
{
	if (action == MenuAction_Select)
	{
		if(g_iUnlockedTrail[client][iParam])
		{
			_trailSetClientTrail(client, iParam);
			if(iParam<=MAX_TRAILS-1)
			{
				PrintToChat(client, "\x01\x0B\x01 %s\x0D Grenade trail chosen:\x05 %s", PREFIX, g_sTrailName[iParam]);
			}
			else
			{
				PrintToChat(client, "\x01\x0B\x01 %s\x0D Grenade trail chosen:\x05 %s", PREFIX, g_sParticles[iParam-MAX_TRAILS][PARTICLE_NAME]);
			}
			_trailsSaveCookies(client);
		}
		else
		{
			PrintToChat(client, "\x01\x0B\x01 %s\x0D You need to unlock this one\x0F :'(", PREFIX);
		}
		new iMenuPage;
		iMenuPage = iParam > 5 ? iParam > 11 ? 12 : 6 : 0;
		
		DisplayMenuAtItem(_trailsList(client), client, iMenuPage, 0);
		
		return;
	}  
	else if ((action == MenuAction_Cancel))
	{
		cmdUnlockablesMenu(client);
	}
	else if(action == MenuAction_End)
		delete hMenu;
}


_trailGetClientTrail(client){
	return g_iChosenTrail[client];
}

_trailSetClientTrail(client, iNum){
	g_iChosenTrail[client]=iNum;
}

public OnEntityCreated(iEnt, const String:sClass[]){	
	if(StrEqual(sClass, "hegrenade_projectile") || StrEqual(sClass, "flashbang_projectile") || StrEqual(sClass, "smokegrenade_projectile")){
		g_iNadeType[iEnt] = StrEqual(sClass, "hegrenade_projectile") ? 0 : StrEqual(sClass, "smokegrenade_projectile") ? 1 : 2;
		SDKHook(iEnt, SDKHook_SpawnPost, onGrenadeSpawn);
	}
}

public onGrenadeSpawn(iEnt){
	CreateTimer(0.00, fncTest, iEnt);
   
	SDKUnhook(iEnt, SDKHook_SpawnPost, onGrenadeSpawn);
}

public Action:fncTest(Handle:hTimer, any:iEnt){
	new client = GetEntPropEnt(iEnt, Prop_Send, "m_hThrower");
	
	switch(g_iNadeType[iEnt]){
		case 0:	TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[1][0], g_iPrecachedTrail[2][0], 0.35, 8.0, 12.0, 3500, {255, 0, 0, 255});
		case 1:	TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[1][0], g_iPrecachedTrail[2][0], 0.5, 8.0, 10.0, 3500, {0, 128, 255, 500});
		case 2: TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[1][0], g_iPrecachedTrail[2][0], 0.35, 8.0, 12.0, 3500, {255, 255, 255, 225});
	}
	TE_SendToAll();
	//PrintToChatAll("!");
	
	if(1 <= client <= MaxClients && IsClientInGame(client) && !IsFakeClient(client)){
		new iCur = _trailGetClientTrail(client);
		
		if(trail_none < iCur <=MAX_TRAILS-1){
			new iSecondSprite = g_iPrecachedTrail[iCur][1] != -1? g_iPrecachedTrail[iCur][1] : 0;
			TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, g_fTrailsFloats[iCur][trailFloat_life], g_fTrailsFloats[iCur][trailFloat_width], g_fTrailsFloats[iCur][trailFloat_width], RoundFloat(g_fTrailsFloats[iCur][trailFloat_velo]), g_iTrailsRgb[iCur]);
			TE_SendToAll();
			
			switch(iCur){
				case trail_thunder:{
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[4][0], g_iPrecachedTrail[2][0], 2.0, 6.0, 6.0, 3000, {255, 196, 0, 196});
					TE_SendToAll();	
					
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[4][0], g_iPrecachedTrail[2][0], 1.25, 6.0, 6.0, 3000, {255, 0, 0, 220});
					TE_SendToAll();		
				}
				case trail_2colors:{
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, g_fTrailsFloats[iCur][trailFloat_life]/4.0, g_fTrailsFloats[iCur][trailFloat_width], g_fTrailsFloats[iCur][trailFloat_width], RoundFloat(g_fTrailsFloats[iCur][trailFloat_velo])*4, {0, 0, 255, 255});
					TE_SendToAll();	
				}
				case trail_cameleon:{
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, 0.75, 5.0, 5.0, 2250, {255, 0, 128, 255});
					TE_SendToAll();
					
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, 1.25, 2.0, 2.0, 2500, {255, 0, 96, 245});
					TE_SendToAll();
				}
				case trail_plazma:{
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, 5.75, 4.0, 4.0, 3000, {0, 0, 255, 255});
					TE_SendToAll();
					
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, 5.0, 3.0, 3.0, 5000, {255, 0, 0, 255});
					TE_SendToAll();
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, 5.0, 3.0, 3.0, 2500, {255, 0, 0, 255});
					TE_SendToAll();
				}
				case trail_frosty:{
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, 1.0, 1.0, 1.0, 3700, {0, 255, 255, 196});
					TE_SendToAll();
					TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, 1.45, 2.1, 2.1, 700, {0, 255, 0, 196});
					TE_SendToAll();
				}
				case trail_stars:{
					new iRandom;
					for(new i = 0 ; i < 5 ; i++){
						iRandom = GetRandomInt(0, 6);
						TE_SetupBeamFollow(iEnt, g_iPrecachedTrail[iCur][0], iSecondSprite, GetRandomFloat(0.5, 1.0), 3.0, 3.0, GetRandomInt(2000, 2750), g_iRandomColors[iRandom]);
						TE_SendToAll();
					}
				}
			}
		}
		else if(trail_none < iCur){
			new Float:fVec[3];
			Entity_GetAbsOrigin(iEnt, fVec);
			
			int particle_system = CreateEntityByName("info_particle_system");
			DispatchKeyValue(particle_system, "start_active", "0");
			DispatchKeyValue(particle_system, "effect_name", g_sParticles[iCur-MAX_TRAILS][PARTICLE_PATH]);
			DispatchSpawn(particle_system);
			TeleportEntity(particle_system, fVec, NULL_VECTOR, NULL_VECTOR);
			ActivateEntity(particle_system);
			SetVariantString("!activator");
			AcceptEntityInput(particle_system, "SetParent", iEnt, particle_system, 0);
						
			AcceptEntityInput(particle_system, "Start");
			
			if (GetEdictFlags(particle_system) & FL_EDICT_ALWAYS)
			SetEdictFlags(particle_system, (GetEdictFlags(particle_system) ^ FL_EDICT_ALWAYS));
		}
	}
}