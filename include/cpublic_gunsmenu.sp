#pragma semicolon 1
#pragma newdecls required

#include <sourcemod>
#include <sdktools>
#include <cstrike>

enum eType
{
	Pistol,
	Rifle
};

int g_iChoice[MAXPLAYERS+1][eType];
int g_iMenuType[MAXPLAYERS+1] = Pistol;

char PISTOLS[][][] = {
	{"weapon_hkp2000",			"P2000"},
	{"weapon_usp_silencer",		"USP-S"},
	{"weapon_glock",			"Glock-18"},
	{"weapon_cz75a",			"CZ-75 Auto"},
	{"weapon_tec9",				"Tec-9"},
	{"weapon_deagle",			"Desert Eagle"}
};

char GUNS[][][] = {
	{"weapon_famas",			"Famas"},
	{"weapon_m4a1",				"M4A4"},
	{"weapon_ak47",				"AK-47"},
	{"weapon_m4a1_silencer",	"M4A1-S"},
	{"weapon_aug",				"AUG"},
	{"weapon_awp",				"AWP"},
	{"weapon_ssg08",			"SSG08"}
};

public Plugin myinfo = 
{
	name = "eFrag [Guns Menu v2]",
	author = "zwolo",
	description = "Custom Guns Menu",
	version = "1.1",
	url = "/id/zwolof"
};

char g_sCommands[][] = {
	"guns",
	"gun",
	"weps",
	"weapons"
};

public void OnPluginStart()
{
    RegConsoleCmd("sm_deathmatch", Command_Deathmatch); 
    HookEvent("round_start", Event_RoundStart);
    
}

public Action Command_Guns(int client, int iArgs)
{
	if(iArgs > 0)
	{
		ReplyToCommand(client, "[SM] Usage /guns");
		return Plugin_Handled;
	}
	CreateGunsMenu(client, 0);
	return Plugin_Handled;
}

public void Event_RoundStart(Event hEvent, const char[] szName, bool dontBroadcast)
{
    for (int i = 1; i < MaxClients; i++)
	{
        if (IsValidClient(i) && IsPlayerAlive(i))
		{
           GiveWeapon(i, PISTOLS[g_iChoice[i]][0]);
		   GiveWeapon(i, GUNS[g_iChoice[i]][0]);
        }
    }
	return Plugin_Continue;
} 

public CreateGunsMenu(int client, int iArgs)
{
	int iType = g_iMenuType[client];
	
	Menu menu = new Menu(GunsMenuCallback);
	char Title[200];
	Format(Title, sizeof(Title), "Choose your %s weapon:\n", iType==Pistol ? "Secondary" : "Primary");
	menu.SetTitle(Title);
	
	for(int i = 0; i < sizeof(iType==Pistol?PISTOLS:GUNS); i++)
		menu.AddItem("xx", iType==Pistol?PISTOLS[i][1]:GUNS[i][1]);
		
	hMenu.ExitButton 		= true;
	menu.Display(client, 	MENU_TIME_FOREVER);
}

stock GiveWeapon(int client, int iWeapon, bool bPistol=false)
{
	if(IsValidClient(client))
	{
		int iWeapon = GetPlayerWeaponSlot(client, CS_SLOT_SECONDARY);
		if(iWeapon != -1) 
		if (IsValidEntity(iWeapon))
			AcceptEntityInput(iWeapon, "kill");
			
		GivePlayerItem(client, bPistol ? PISTOLS[iWeapon][0] : GUNS[iWeapon][0]);
		PrintToChat(client, "%s \x0AYou have been given a \x04%s", PREFIX, bPistol ? PISTOLS[iWeapon][1] : GUNS[iWeapon][1]);
	}
}

public int GunsMenuCallback(Menu menu, MenuAction action, int client, int iOption)
{
	switch(action)
	{
		case MenuAction_Select:
		{
			g_iChoice[client][iType] = iOption;
			GiveWeapon(client, iOption, true);
			g_iMenuType[client] = (g_iMenuType[client] == Rifle) ? Pistol : Rifle;
			
			CreateGunsMenu(client, 0);
		}
		case MenuAction_End:
			delete menu;
	}
}

stock bool IsValidClient(int client)
{
	return view_as<bool>((0 < client <= MaxClients) && IsClientInGame(client) && !IsFakeClient(client));
}