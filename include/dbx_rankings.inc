#if defined _dbx_rankings_included
 #endinput
#endif
#define _dbx_rankins_included
bool g_bShowRankPanel;

enum rankTypes {
	rankType_KDA,
	rankType_Money,
	rankType_Deaths,
	rankType_Headshots
};

enum rankPersonalData{
	rankPosition,
	rankValue
};

new g_iChosenRanking[PLAYER_TAB];
new g_ePlayerRankData[PLAYER_TAB][rankTypes][rankPersonalData];

new String:g_sAbove[PLAYER_TAB][10][32];
new String:g_sUnder[PLAYER_TAB][10][32];
new g_iAbove[PLAYER_TAB][2][10];
new g_iUnder[PLAYER_TAB][2][10];

new const String:g_sRankTypes[rankTypes][] = {
	"Kills",
	"Money",
	"Deaths",
	"Headshots"
};

public int _createRankPanel(int client)
{
	//_mmSqlGetRankingPosition(client, rankTypes:g_iChosenRanking[client]);
	 
	//new Handle:hPanel = CreatePanel();
	Menu hMenu = new Menu(_HandlerRankMenu);
	hMenu.SetTitle("Your ranking");
	
	char sTitle[192], sOption[128], sType[64];
	FormatEx(sType, sizeof sType, "%s", g_sRankTypes[g_iChosenRanking[client]]);
	hMenu.AddItem("Type", sType);
	
	FormatEx(sTitle, sizeof sTitle, "%s MoneyMod\nTop 20: %s\n ", COMMUNITYNAME, sType);
	hMenu.SetTitle(sTitle);

	FormatEx(sType, sizeof sType, "Sort by:\n    %s", g_sRankTypes[g_iChosenRanking[client]]);
	hMenu.AddItem("Type", sType);

	char sFullInfos[2048], sAmount[16], sPlace[16];


	int i;
	int iStopped=0;
	int iSuccess=0;
	Format(sFullInfos, sizeof sFullInfos, "Statistics\n ");
	
	for(i = 0 ; i < 10 ; i++)
	{
		if(g_iAbove[client][0][i] != 0){
			FormatEx(sAmount, sizeof sAmount, "%s%s%s%s%s%d", g_iAbove[client][1][i] < 100000 ? "  " : "", g_iAbove[client][1][i] < 10000 ? "  " : "", g_iAbove[client][1][i] < 1000 ? "  " : "", g_iAbove[client][1][i] < 100 ? "  " : "", g_iAbove[client][1][i] < 10 ? "  " : "", g_iAbove[client][1][i]);
			FormatEx(sPlace, sizeof sPlace, "%d〕%s%s%s%s", g_iAbove[client][0][i], g_iAbove[client][0][i] < 10000 ? "  " : "", g_iAbove[client][0][i] < 1000 ? "  " : "", g_iAbove[client][0][i] < 100 ? "  " : "", g_iAbove[client][0][i] < 10 ? "  " : "");
			Format(sFullInfos, sizeof sFullInfos, "%s\n   〔#%s%s     %s", sFullInfos, sPlace, sAmount, g_sAbove[client][i]);
			iSuccess++;
		}
		else
		{
			FormatEx(sAmount, sizeof sAmount, "%s%s%s%s%s%d", g_iAbove[client][1][i] < 100000 ? "  " : "", g_ePlayerRankData[client][g_iChosenRanking[client]][rankValue] < 10000 ? "  " : "", g_ePlayerRankData[client][g_iChosenRanking[client]][rankValue] < 1000 ? "  " : "", g_ePlayerRankData[client][g_iChosenRanking[client]][rankValue] < 100 ? "  " : "", g_ePlayerRankData[client][g_iChosenRanking[client]][rankValue] < 10 ? "  " : "", g_ePlayerRankData[client][g_iChosenRanking[client]][rankValue]);
			
			FormatEx(sPlace, sizeof sPlace, "%d〕%s%s%s%s", g_ePlayerRankData[client][g_iChosenRanking[client]][rankPosition], g_ePlayerRankData[client][g_iChosenRanking[client]][rankPosition] < 10000 ? "  " : "", g_ePlayerRankData[client][g_iChosenRanking[client]][rankPosition] < 1000 ? "  " : "", g_ePlayerRankData[client][g_iChosenRanking[client]][rankPosition] < 100 ? "  " : "", g_ePlayerRankData[client][g_iChosenRanking[client]][rankPosition] < 10 ? "  " : "");
			
			Format(sFullInfos, sizeof sFullInfos, "%s\n〔#%s%s  ⮞  %N", sFullInfos, sPlace, sAmount, client);
			
			iStopped=i+1;
			iSuccess++;
			break;
		}
	}

	int iMax = (10-iStopped);
	
	for(i = 0 ; i < iMax ; i++)
	{
		if(iSuccess >= 9) break;
		if(g_iUnder[client][0][i] != 0)
		{
			FormatEx(sAmount, sizeof sAmount, "%s%s%s%s%s%d", g_iUnder[client][1][i] < 100000 ? "  " : "", g_iUnder[client][1][i] < 10000 ? "  " : "", g_iUnder[client][1][i] < 1000 ? "  " : "", g_iUnder[client][1][i] < 100 ? "  " : "", g_iUnder[client][1][i] < 10 ? "  " : "", g_iUnder[client][1][i]);
			
			FormatEx(sPlace, sizeof sPlace, "%d〕%s%s%s%s", g_iUnder[client][0][i], g_iUnder[client][0][i] < 10000 ? "  " : "", g_iUnder[client][0][i] < 1000 ? "  " : "", g_iUnder[client][0][i] < 100 ? "  " : "", g_iUnder[client][0][i] < 10 ? "  " : "");
			Format(sFullInfos, sizeof sFullInfos, "%s〔#%s%s     %s", sFullInfos, sPlace, sAmount, g_sUnder[client][i]);
			iSuccess++;
		}
	}
	
	hMenu.AddItem("#xd", sFullInfos);
	
	hMenu.ExitButton 				= true;
	hMenu.ExitBackButton 			= true;
	hMenu.Display(client, MENU_TIME_FOREVER);
}

public _HandlerRankMenu(Handle hMenu, MenuAction eAction, int client, int iOption)
{
	if(eAction == MenuAction_Select)
	{
		switch(iOption)
		{
			case 1:
			{
				g_iChosenRanking[client] = g_iChosenRanking[client] == _:rankTypes-1 ?  0 : g_iChosenRanking[client]++;
				//PrintToChat(client, "\x01\x0B\x01 %s\x0D Sorting by: \x05%s", PREFIX, g_sRankTypes[g_iChosenRanking[client]]);
				_mmSqlGetRankingPosition(client, rankTypes:g_iChosenRanking[client]);
				
				return;
			}
			default:
			{
				cmdMainMenu(client,0);
			}
		}
	}
	else if(eAction == MenuAction_End)
		delete hMenu;
}