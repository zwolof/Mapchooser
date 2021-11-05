#if defined _dbx_sql_included
#endinput
#endif
#define _dbx_sql_included

#include "dbx_core.inc"
#include "dbx_auctions.inc"
//pobieranie nie na id tylko globalne..
#define SQL_KEY_LENGTH 			16000
#define SQL_TABLE_NAME 			"dmg2money_core"
#define SQL_MARKET_TABLE_NAME	"dmg2money_market"
#define SQL_MARKET_R_TABLE_NAME	"dmg2money_market_returns"
#define SQL_LOOTBAG_TABLE_NAME	"dmg2money_lootbag"
#define SQL_STATS_TABLE_NAME	"dmg2money_stats"
#define SQL_MSG_PREFIX			"[PlayCore-MM]"

enum sqlTableTypes{
	String:sqlTable_Core[SQL_KEY_LENGTH],
	String:sqlTable_Market[SQL_KEY_LENGTH],
	String:sqlTable_Lootbag[SQL_KEY_LENGTH],
	String:sqlTable_Stats[SQL_KEY_LENGTH],
	String:sqlTable_MarketReturns[SQL_KEY_LENGTH]
};

enum sqlImportantes{
	String:sql_TableKey[sqlTableTypes],
	String:sql_InsertKey[sqlTableTypes],
	bool:sql_Loaded[PLAYER_TAB]
};

enum sqlPersonal{
	String:sql_PlayerName[MAX_NAME_LENGTH],
	String:sql_PlayerSid[MAX_NAME_LENGTH]
};

new g_eSqlData[sqlImportantes];
new g_eSqlPersonalData[PLAYER_TAB][sqlPersonal];
int g_iPosition[PLAYER_TAB];

new g_iExecutedMarketInsertions;
new String:g_sExecuteMarketInsertion[5][SQL_KEY_LENGTH];

new Handle:g_hSqlDb;

new const String:g_sSqlToInsertion[2][] = { "SID", "NAME" };

_mmSqlFormatTable(){
	FormatEx(g_eSqlData[sql_TableKey][sqlTable_Core], SQL_KEY_LENGTH, "CREATE TABLE IF NOT EXISTS %s (steamid VARCHAR(32), money INT NOT NULL DEFAULT '-1', totalmoney INT NOT NULL DEFAULT '-1', totaldamage INT NOT NULL DEFAULT '-1'", SQL_TABLE_NAME);
	
	new i;
	for(i = 0 ; i < _:playerChancesTypes ; i++){
		Format(g_eSqlData[sql_TableKey][sqlTable_Core], SQL_KEY_LENGTH, "%s, chance_%d INT NOT NULL DEFAULT '0'", g_eSqlData[sql_TableKey][sqlTable_Core], i);
	}
	for(i = 0 ; i < _:playerActivesTypes ; i++){
		Format(g_eSqlData[sql_TableKey][sqlTable_Core], SQL_KEY_LENGTH, "%s, active_%d INT NOT NULL DEFAULT '0'", g_eSqlData[sql_TableKey][sqlTable_Core], i);
	}
	for(i = 0 ; i < _:playerPassiveTypes ; i++){
		Format(g_eSqlData[sql_TableKey][sqlTable_Core], SQL_KEY_LENGTH, "%s, passive_%d INT NOT NULL DEFAULT '0'%s", g_eSqlData[sql_TableKey][sqlTable_Core], i, i == _:playerPassiveTypes-1 ? ",  PRIMARY KEY(steamid));" : "");
	}
	
	//market
	FormatEx(g_eSqlData[sql_TableKey][sqlTable_Market], SQL_KEY_LENGTH, "CREATE TABLE IF NOT EXISTS %s (auction_number INT NOT NULL DEFAULT '0', auction_item INT NOT NULL DEFAULT '0', auction_itemcount INT NOT NULL DEFAULT '0', auction_itemprice INT NOT NULL DEFAULT '0', auction_steamid VARCHAR(32), auction_name VARCHAR(32), auction_end VARCHAR(32), PRIMARY KEY(auction_number));", SQL_MARKET_TABLE_NAME);
	//lootbag
	FormatEx(g_eSqlData[sql_TableKey][sqlTable_Lootbag], SQL_KEY_LENGTH, "CREATE TABLE IF NOT EXISTS %s (steamid VARCHAR(32)", SQL_LOOTBAG_TABLE_NAME);
	for(i = 0 ; i < _:dataBoxTypes ; i++){
		Format(g_eSqlData[sql_TableKey][sqlTable_Lootbag], SQL_KEY_LENGTH, "%s, inv_slot%d INT NOT NULL DEFAULT '0'%s", g_eSqlData[sql_TableKey][sqlTable_Lootbag], i, i == _:dataBoxTypes-1 ? ",  PRIMARY KEY(steamid));" : "");
	}
	//stats
	FormatEx(g_eSqlData[sql_TableKey][sqlTable_Stats], SQL_KEY_LENGTH, "CREATE TABLE IF NOT EXISTS %s (steamid VARCHAR(32), name VARCHAR(32)", SQL_STATS_TABLE_NAME);
	for(i = 0 ; i < _:playerStatistics ; i++){
		Format(g_eSqlData[sql_TableKey][sqlTable_Stats], SQL_KEY_LENGTH, "%s, stat_%d INT NOT NULL DEFAULT '0'%s", g_eSqlData[sql_TableKey][sqlTable_Stats], i, i == _:playerStatistics-1 ? ",  PRIMARY KEY(steamid));" : "");
	}
	//LogMessage("%s", g_eSqlData[sql_TableKey]); 		//DEBUG
	
	FormatEx(g_eSqlData[sql_TableKey][sqlTable_MarketReturns], SQL_KEY_LENGTH, "CREATE TABLE IF NOT EXISTS %s (return_steamid VARCHAR(32), return_item_id INT NOT NULL DEFAULT '0', return_item_count INT NOT NULL DEFAULT '0', return_money INT NOT NULL DEFAULT '0');", SQL_MARKET_R_TABLE_NAME);
}

_mmSqlFormatInsertion(){
	//CORE
	FormatEx(g_eSqlData[sql_InsertKey][sqlTable_Core], SQL_KEY_LENGTH, "INSERT INTO %s (steamid, money, totalmoney, totaldamage", SQL_TABLE_NAME);
	
	new i;
	new String:sValues[1024];
	FormatEx(sValues, sizeof sValues, " VALUES('%s', '0', '0', '0'", g_sSqlToInsertion[0]);
	
	for(i = 0 ; i < _:playerChancesTypes ; i++){
		Format(g_eSqlData[sql_InsertKey][sqlTable_Core], SQL_KEY_LENGTH, "%s, chance_%d", g_eSqlData[sql_InsertKey][sqlTable_Core], i);
		Format(sValues, sizeof sValues, "%s, '0'", sValues);
	}
	for(i = 0 ; i < _:playerActivesTypes ; i++){
		Format(g_eSqlData[sql_InsertKey][sqlTable_Core], SQL_KEY_LENGTH, "%s, active_%d", g_eSqlData[sql_InsertKey][sqlTable_Core], i);
		Format(sValues, sizeof sValues, "%s, '0'", sValues);
	}
	for(i = 0 ; i < _:playerPassiveTypes ; i++){
		Format(g_eSqlData[sql_InsertKey][sqlTable_Core], SQL_KEY_LENGTH, "%s, passive_%d%s", g_eSqlData[sql_InsertKey][sqlTable_Core], i, i == _:playerPassiveTypes-1 ? ")" : "");
		Format(sValues, sizeof sValues, "%s, '0'%s", sValues, i == _:playerPassiveTypes-1 ? ")" : "");
	}
	Format(g_eSqlData[sql_InsertKey][sqlTable_Core], SQL_KEY_LENGTH, "%s%s", g_eSqlData[sql_InsertKey], sValues);
	
	//MARKET
	new iCur=0;
	new iMax=4
	for(new x = 0 ; x < iMax+1 ; x++){
		FormatEx(g_eSqlData[sql_InsertKey][sqlTable_Market], SQL_KEY_LENGTH, "INSERT INTO %s (auction_number, auction_item, auction_itemcount, auction_itemprice, auction_steamid, auction_name, auction_end) VALUES ", SQL_MARKET_TABLE_NAME);
		
		new iX = x == iMax ? ((41-x)-1) : 41;
		for(i = 0 ; i < iX ; i++){
			Format(g_eSqlData[sql_InsertKey][sqlTable_Market], SQL_KEY_LENGTH, "%s (%d,0,0,0,0,0,0)%s", g_eSqlData[sql_InsertKey][sqlTable_Market], iCur+i, i==iX-1 ? "" : ",");
		}
		iCur+=41;
		
		FormatEx(g_sExecuteMarketInsertion[x], sizeof g_sExecuteMarketInsertion[], g_eSqlData[sql_InsertKey][sqlTable_Market]);
	}
	
	//LOOTBAG
	FormatEx(g_eSqlData[sql_InsertKey][sqlTable_Lootbag], SQL_KEY_LENGTH, "INSERT INTO %s (steamid", SQL_LOOTBAG_TABLE_NAME);
	FormatEx(sValues, sizeof sValues, " VALUES('%s'", g_sSqlToInsertion[0]);
	
	for(i = 0 ; i < _:dataBoxTypes ; i++){
		Format(g_eSqlData[sql_InsertKey][sqlTable_Lootbag], SQL_KEY_LENGTH, "%s, inv_slot%d%s", g_eSqlData[sql_InsertKey][sqlTable_Lootbag], i, i == _:dataBoxTypes-1 ? ")" : "")
		
		Format(sValues, sizeof sValues, "%s, '0'%s", sValues, i == _:dataBoxTypes-1 ? ")" : "");
	}
	Format(g_eSqlData[sql_InsertKey][sqlTable_Lootbag], SQL_KEY_LENGTH, "%s%s", g_eSqlData[sql_InsertKey][sqlTable_Lootbag], sValues);
	
	//STATS
	FormatEx(g_eSqlData[sql_InsertKey][sqlTable_Stats], SQL_KEY_LENGTH, "INSERT INTO %s (steamid, name", SQL_STATS_TABLE_NAME);
	FormatEx(sValues, sizeof sValues, " VALUES('%s','%s'", g_sSqlToInsertion[0], g_sSqlToInsertion[1]);
	
	for(i = 0 ; i < _:playerStatistics ; i++){
		Format(g_eSqlData[sql_InsertKey][sqlTable_Stats], SQL_KEY_LENGTH, "%s, stat_%d%s", g_eSqlData[sql_InsertKey][sqlTable_Stats], i, i == _:playerStatistics-1 ? ")" : "")
		
		Format(sValues, sizeof sValues, "%s, '0.0'%s", sValues, i == _:playerStatistics-1 ? ")" : "");
	}
	Format(g_eSqlData[sql_InsertKey][sqlTable_Stats], SQL_KEY_LENGTH, "%s%s", g_eSqlData[sql_InsertKey][sqlTable_Stats], sValues);
}

_mmSqlConnect(){
	g_iExecutedMarketInsertions=0;
	
	_mmSqlFormatTable();
	
	new String:sError[256], String:sIdent[8];
	g_hSqlDb = SQL_Connect("diablix_dmgmoney", false, sError, sizeof sError);
	
	if(g_hSqlDb == INVALID_HANDLE){
		SetFailState("%s Database error (%s)", SQL_MSG_PREFIX, sError);
		return;
	}
	
	SQL_ReadDriver(g_hSqlDb, sIdent, sizeof sIdent)
	LogMessage("%s Database is using: %s", SQL_MSG_PREFIX, sIdent);
	
	SQL_FastQuery(g_hSqlDb, "SET NAMES 'utf8mb4'");
	
	SQL_LockDatabase(g_hSqlDb);        
	SQL_FastQuery(g_hSqlDb, g_eSqlData[sql_TableKey][sqlTable_Core]);
	SQL_UnlockDatabase(g_hSqlDb);
	
	SQL_LockDatabase(g_hSqlDb);    
	SQL_FastQuery(g_hSqlDb, g_eSqlData[sql_TableKey][sqlTable_Market]);
	SQL_UnlockDatabase(g_hSqlDb);
	
	SQL_LockDatabase(g_hSqlDb);    
	SQL_FastQuery(g_hSqlDb,  g_eSqlData[sql_TableKey][sqlTable_Lootbag]);
	SQL_UnlockDatabase(g_hSqlDb);
	
	SQL_LockDatabase(g_hSqlDb);    
	SQL_FastQuery(g_hSqlDb,  g_eSqlData[sql_TableKey][sqlTable_Stats]);
	SQL_UnlockDatabase(g_hSqlDb);
	
	SQL_LockDatabase(g_hSqlDb);
	SQL_FastQuery(g_hSqlDb, g_eSqlData[sql_TableKey][sqlTable_MarketReturns]);
	SQL_UnlockDatabase(g_hSqlDb);
	
	_mmSqlFormatInsertion();
	//_mmSqlInsertAuctions();
}

public _mmSqlInsertPendingItem(String:sSid[], iItem, iQuant){
	new String:sQuery[1024];
	FormatEx(sQuery, sizeof sQuery, "INSERT INTO %s (return_steamid, return_item_id, return_item_count) VALUES('%s', '%d', '%d')", SQL_MARKET_R_TABLE_NAME, sSid, iItem, iQuant);
	
	SQL_TQuery(g_hSqlDb, _mmSqlInsertPendingItem_Cb, sQuery, 0, DBPrio_Low);
}

public _mmSqlInsertPendingMoney(String:sSid[], iQuant, iMoney){
	new String:sQuery[1024];
	FormatEx(sQuery, sizeof sQuery, "INSERT INTO %s (return_steamid, return_item_id, return_item_count, return_money) VALUES('%s', '-1', '%d', '%d')", SQL_MARKET_R_TABLE_NAME, sSid, iQuant, iMoney);
	
	SQL_TQuery(g_hSqlDb, _mmSqlInsertPendingMoney_Cb, sQuery, 0, DBPrio_Low);
}

public _mmSqlUpdatePendingMoney(String:sSid[], iQuant, iMoney){
	new String:sQuery[512];
	
	FormatEx(sQuery, sizeof(sQuery), "UPDATE `%s` SET `return_item_count` = '%d', `return_money` = '%d' WHERE `return_steamid` = '%s'", SQL_MARKET_R_TABLE_NAME, iQuant, iMoney, sSid);	
	SQL_TQuery(g_hSqlDb, _mmSqlUpdatePendingMoney_Cb, sQuery, 0, DBPrio_Low);
}

public _mmSqlPrePendingMoney(String:sSid[], iMoney, iAmountSold){
	new Handle:hData = CreateDataPack();
	WritePackString(hData, sSid);
	WritePackCell(hData, iAmountSold);
	WritePackCell(hData, iMoney);
	
	new String:sQuery[256];
	FormatEx(sQuery, sizeof(sQuery), "SELECT * FROM %s WHERE `return_steamid` = '%s'", SQL_MARKET_R_TABLE_NAME, sSid); 
	
	SQL_TQuery(g_hSqlDb, _mmSqlPrePendingMoney_Cb, sQuery, hData, DBPrio_Low);
}

public _mmSqlGetTopRecords(iClient, iPosition, rankTypes:iType)
{
	new String:sQuery[512];
	FormatEx(sQuery, sizeof sQuery,"SELECT stat_0 FROM `%s` ORDER BY `stat_0` DESC LIMIT 100;", SQL_STATS_TABLE_NAME, iType==rankType_KDA ? "stat_0" : iType==rankType_Money ? "stat_6" : iType==rankType_Deaths ? "stat_4" : "stat_2");
	
	new Handle:hData = CreateDataPack();
	WritePackCell(hData, iClient);
	WritePackCell(hData, iPosition);
	WritePackCell(hData, iType);
	
	SQL_TQuery(g_hSqlDb, _mmSqlGetTopRecords_Cb, sQuery, hData);
}

public _mmSqlGetRankingPosition(iClient, rankTypes:iType){
	new String:sQuery[512];
	FormatEx(sQuery, sizeof sQuery, "SELECT * FROM `%s` ORDER BY `%s` DESC LIMIT 1000", SQL_STATS_TABLE_NAME, iType==rankType_KDA ? "stat_0" : iType==rankType_Money ? "stat_6" : iType==rankType_Deaths ? "stat_4" : "stat_2");
	
	new Handle:hData = CreateDataPack();
	WritePackCell(hData, iClient);
	WritePackCell(hData, iType);
	
	SQL_TQuery(g_hSqlDb, _mmSqlGetRankingPosition_Cb, sQuery, hData);
}

public _mmSqlUpdateBasic(iClient){
	if(!IsValidClient(iClient) || !g_eSqlData[sql_Loaded][iClient]) return;
	
	new String:sQuery[512], String:sSteamId[32];
	
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "UPDATE `%s` SET `money` = '%d', `totalmoney` = '%d', `totaldamage` = '%d' WHERE `steamid` = '%s';", SQL_TABLE_NAME, g_ePlayerData[iClient][dataMoney], g_ePlayerData[iClient][dataTotalMoney], g_ePlayerData[iClient][dataTotalDamage], sSteamId); 			
	SQL_TQuery(g_hSqlDb, _mmSqlUpdateBasic_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlUpdateSkill(iClient, iType, iNumber){
	if(!IsValidClient(iClient) || !g_eSqlData[sql_Loaded][iClient]) return;
	
	new String:sQuery[256], String:sSteamId[32];
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "UPDATE `%s` SET `%s_%d` = '%d', `money` = '%d' WHERE `steamid` = '%s';", SQL_TABLE_NAME, iType == CHANCE ? "chance" : iType == ACTIVE ? "active" : "passive", iNumber, iType == CHANCE ? g_ePlayerData[iClient][dataChances][iNumber] : iType == ACTIVE ? g_ePlayerData[iClient][dataActives][iNumber] : g_ePlayerData[iClient][dataPassives][iNumber], g_ePlayerData[iClient][dataMoney], sSteamId); 			
	SQL_TQuery(g_hSqlDb, _mmSqlUpdateSkills_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlUpdateLootbagSlot(iClient, iType){
	if(!IsValidClient(iClient) || !g_eSqlData[sql_Loaded][iClient]) return;
	
	new String:sQuery[256], String:sSteamId[32];
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "UPDATE `%s` SET `inv_slot%d` = '%d' WHERE `steamid` = '%s';", SQL_LOOTBAG_TABLE_NAME, iType, g_iPlayerBoxes[iClient][iType], sSteamId); 			
	SQL_TQuery(g_hSqlDb, _mmSqlUpdateLootbagSlot_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlUpdateStat(iClient, iType){
	if(!IsValidClient(iClient) || !g_eSqlData[sql_Loaded][iClient]) return;
	
	new String:sQuery[256], String:sSteamId[32], String:sName[MAX_NAME_LENGTH], String:sEscapedName[MAX_NAME_LENGTH*2+1];
	
	GetClientName(iClient, sName, sizeof(sName));	
	SQL_EscapeString(g_hSqlDb, sName, sEscapedName, sizeof(sEscapedName))
	
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "UPDATE `%s` SET `stat_%d` = '%d', `name` = '%s' WHERE `steamid` = '%s';", SQL_STATS_TABLE_NAME, iType, g_eStatistics[iClient][iType], sEscapedName, sSteamId); 			
	SQL_TQuery(g_hSqlDb, _mmSqlUpdateStat_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlLoadSkills(iClient){
	if(!IsValidClient(iClient)) return;
	
	new String:sQuery[128], String:sSteamId[32];
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "SELECT * FROM %s WHERE `steamid` = '%s';", SQL_TABLE_NAME, sSteamId); 			
	SQL_TQuery(g_hSqlDb, _mmSqlLoadSkills_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlLoadLootbag(iClient){
	if(!IsValidClient(iClient)) return;
	
	new String:sQuery[128], String:sSteamId[32];
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "SELECT * FROM %s WHERE `steamid` = '%s';", SQL_LOOTBAG_TABLE_NAME, sSteamId); 			
	SQL_TQuery(g_hSqlDb, _mmSqlLoadLootbag_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlLoadPendingItems(iClient){
	if(!IsValidClient(iClient)) return;
	
	new String:sQuery[256], String:sSteamId[32];
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "SELECT * FROM %s WHERE `return_steamid` = '%s' AND `return_item_count` > '0';", SQL_MARKET_R_TABLE_NAME, sSteamId);
	SQL_TQuery(g_hSqlDb, _mmSqlLoadPendingItems_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlLoadPendingMoney(iClient){
	if(!IsValidClient(iClient)) return;
	
	new String:sQuery[256], String:sSteamId[32];
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "SELECT * FROM %s WHERE `return_steamid` = '%s' AND `return_item_id` = '-1';", SQL_MARKET_R_TABLE_NAME, sSteamId); 			
	SQL_TQuery(g_hSqlDb, _mmSqlLoadPendingMoney_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlLoadStats(iClient){
	if(!IsValidClient(iClient)) return;
	
	new String:sQuery[128], String:sSteamId[32];
	GetClientAuthId(iClient, AuthId_Steam2, sSteamId, sizeof(sSteamId), false);
	
	FormatEx(sQuery, sizeof(sQuery), "SELECT * FROM %s WHERE `steamid` = '%s';", SQL_STATS_TABLE_NAME, sSteamId); 			
	SQL_TQuery(g_hSqlDb, _mmSqlLoadStats_Cb, sQuery, iClient, DBPrio_Low);
}

public _mmSqlFinishedLoading(iClient){
	g_eSqlData[sql_Loaded][iClient] = true;
	LogMessage("Client %N fully loaded", iClient);
}

public _mmSqlGetTopRecords_Cb(Handle hOwner, Handle hDb, const char[] sError, any hData)
{
	if (hDb == INVALID_HANDLE)
	{
		LogError("%s Get-Top-Error (%s)", SQL_MSG_PREFIX, sError);
		return;
	}
	
	int iDataPack[3];
	Handle hPack = hData;
	ResetPack(hPack);
	
	iDataPack[0] = ReadPackCell(hPack); //client
	iDataPack[1] = ReadPackCell(hPack); //position
	iDataPack[2] = ReadPackCell(hPack);	//type
	CloseHandle(hPack);

	int iKills;
	int i = 0;
	char szItem[64];
	char sNum[64];
	
	if(SQL_GetRowCount(hDb))
	{
		//PrintToChatAll("%d", SQL_GetRowCount(hndl));
		char szTitle[256];
		Menu hMenu = new Menu(RankingMenuHandler);
			Format(szTitle, sizeof(szTitle), "TOP 20");
			hMenu.SetTitle(szTitle);

		while(SQL_HasResultSet(hDb) && SQL_FetchRow(hDb))
		{
			iKills = SQL_FetchInt(hDb, 1); 
			
			if(iKills > 0)
			{
				Format(szItem, 255, "{ #%d }   Kills: > %d <", i, iKills);
				IntToString(i, sNum, sizeof(sNum));
				hMenu.AddItem(sNum, szItem);
				i++;
			}
		}
		hMenu.ExitButton = true;
		hMenu.ExitBackButton = true;
		hMenu.Display(iDataPack[0], MENU_TIME_FOREVER);
	}
	
	//if(g_bShowRankPanel)
	//	_createRankPanel(iDataPack[0]);
}

public RankingMenuHandler(Menu hMenu, MenuAction iAction, int client, int iOption)
{
	if(iAction ==  MenuAction_Select)
	{
		PrintToChat(client, "123");
	}
	else if(iAction == MenuAction_End)
	{
		delete hMenu;
	}
}

public _mmSqlGetRankingPosition_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:hData){
	if (hDb == INVALID_HANDLE){
		LogError("%s Get-Ranking-Error (%s)", SQL_MSG_PREFIX, sError);
		return;
	}
	
	new iDataPack[2];
	new Handle:hPack = hData;
	ResetPack(hPack);
	
	iDataPack[0] = ReadPackCell(hPack); //client
	iDataPack[1] = ReadPackCell(hPack);	//type
	CloseHandle(hPack);
	
	new iDataType = !iDataPack[1] ? (_:rankType_KDA) : iDataPack[1] == 1 ? (_:rankType_Money) : iDataPack[1] == 2 ? (_:rankType_Deaths) : (_:rankType_Headshots);
	new iColumn = !iDataPack[1] ? 2 : iDataPack[1] == 1 ? 8 : iDataPack[1] == 2 ? 6 : 4;
	
	if(SQL_HasResultSet(hDb)){
		new iPlace=0;
		new String:sSid[2][64];
		GetClientAuthId(iDataPack[0], AuthId_Steam2, sSid[0], sizeof sSid[], false);
		
		while(SQL_FetchRow(hDb)){
			iPlace++;
			
			SQL_FetchString(hDb, 0, sSid[1], sizeof sSid[]);
			if(StrEqual(sSid[0], sSid[1])){
				g_ePlayerRankData[iDataPack[0]][iDataType][rankPosition] = iPlace;
				g_ePlayerRankData[iDataPack[0]][iDataType][rankValue] = SQL_FetchInt(hDb, iColumn);
				//PrintToChatAll("LOL: %d", iKtory);
				//g_iPozycja[id] = iPlace;
				g_iPosition[iDataPack[0]] = iPlace;
				
				PrintToChatAll("#%d - %d[%s] (you)", iPlace, g_ePlayerRankData[iDataPack[0]][iDataType][rankValue], g_sRankTypes[iDataType]);
				//PrintToChatAll("#%d - %d[%s] (you)", iPlace, g_ePlayerRankData[iDataPack[0]][iDataType][rankValue], g_sRankTypes[iDataType]);
				break;
			}
		}
	}
	_mmSqlGetTopRecords(iDataPack[0], g_ePlayerRankData[iDataPack[0]][iDataType][rankPosition], rankTypes:iDataType);
}

public _mmSqlLoadStats_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE){
		LogError("%s Load-stats error (%s)", SQL_MSG_PREFIX, sError);
		KickClient(iClient, "SQL DATABASE ERROR");
		return;
	} 
	if(SQL_HasResultSet(hDb) && SQL_FetchRow(hDb)){
		//SQL_FetchString(hDb, 0, sTempSid[1], sizeof(sTempSid[]));
		
		SQL_FetchString(hDb, 1, g_eSqlPersonalData[iClient][sql_PlayerName], 32);
		
		//if(StrEqual(sTempSid[0], sTempSid[1])){
		new i;
		for(i = 0 ; i < _:playerStatistics ; i++)
			g_eStatistics[iClient][i] = SQL_FetchInt(hDb, i+2);
		
		_mmSqlFinishedLoading(iClient);
		//}
		//else{
		//	KickClient(iClient, "SQL DATABASE ERROR");
		//	LogError("[INSPECTOR_dbx] Prevented record swap [stats] (old id-user disconnected) - executing again..");
		//	return;
		//}
	}
	else{
		new String:sTempSid[32];
		GetClientAuthId(iClient, AuthId_Steam2, sTempSid, sizeof(sTempSid), false);
		
		new String:sQuery[1024];
		FormatEx(sQuery, sizeof sQuery, g_eSqlData[sql_InsertKey][sqlTable_Stats]);
		
		ReplaceString(sQuery, sizeof sQuery, g_sSqlToInsertion[0], sTempSid);
		
		SQL_TQuery(g_hSqlDb, _mmSqlInsertStats_Cb, sQuery, iClient, DBPrio_Low);
	}
}


//_mmSqlLoadStats(iClient);
public _mmSqlLoadLootbag_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE){
		LogError("%s Load-lootbag error (%s)", SQL_MSG_PREFIX, sError);
		KickClient(iClient, "SQL DATABASE ERROR");
		return;
	} 
	
	if(SQL_HasResultSet(hDb) && SQL_FetchRow(hDb)){
		//SQL_FetchString(hDb, 0, sTempSid[1], sizeof(sTempSid[]));
		
		//if(StrEqual(sTempSid[0], sTempSid[1])){
		new i;
		for(i = 0 ; i < _:dataBoxTypes ; i++)
		g_iPlayerBoxes[iClient][i] = SQL_FetchInt(hDb, i+1);
		
		_mmSqlLoadPendingMoney(iClient);
			//_mmSqlLoadStats(iClient);
		//}
		//else{
		//	KickClient(iClient, "SQL DATABASE ERROR");
		//	LogError("[INSPECTOR_dbx] Prevented record swap [lootbag] (old id-user disconnected) - executing again..");
		//	return;
		//}
	}
	else{
		new String:sTempSid[32];
		GetClientAuthId(iClient, AuthId_Steam2, sTempSid, sizeof(sTempSid), false);
		
		new String:sQuery[1024];
		FormatEx(sQuery, sizeof sQuery, g_eSqlData[sql_InsertKey][sqlTable_Lootbag]);
		
		ReplaceString(sQuery, sizeof sQuery, g_sSqlToInsertion[0], sTempSid);
		
		SQL_TQuery(g_hSqlDb, _mmSqlInsertLootbag_Cb, sQuery, iClient, DBPrio_Low);
	}
}

public _mmSqlLoadPendingMoney_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE){
		LogError("%s Load-pending-money error (%s)", SQL_MSG_PREFIX, sError);
		KickClient(iClient, "SQL DATABASE ERROR");
		return;
	} 
	
	new String:sSid[32];
	GetClientAuthId(iClient, AuthId_Steam2, sSid, sizeof(sSid), false);
	
	if(SQL_HasResultSet(hDb) && SQL_FetchRow(hDb)){
		g_iPendingQuant[iClient] = SQL_FetchInt(hDb, 2); 	//quant
		g_iPendingMoney[iClient] = SQL_FetchInt(hDb, 3);	//money
		
		new String:sQuery[512];
		FormatEx(sQuery, sizeof sQuery, "DELETE FROM %s WHERE `return_steamid` = '%s' AND `return_item_id` = '-1'", SQL_MARKET_R_TABLE_NAME, sSid);
		SQL_TQuery(g_hSqlDb, _mmSqlRemovePendingCash_Cb, sQuery, 0, DBPrio_Low);
		
	}
	_mmSqlLoadStats(iClient);
}

public _mmSqlLoadPendingItems_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE){
		LogError("%s Load-pending-item error (%s)", SQL_MSG_PREFIX, sError);
		KickClient(iClient, "SQL DATABASE ERROR");
		return;
	} 
	
	new String:sSid[32], iAuctionSlot=0;
	GetClientAuthId(iClient, AuthId_Steam2, sSid, sizeof(sSid), false);
	
	for(new i = 0 ; i < AUCTIONS_SLOTS ; i++){
		g_iPendingItem[iClient][i]=-1;
		g_iPendingItemQuant[iClient][i]=-1;
	}
	
	if(SQL_HasResultSet(hDb)){
		while(SQL_FetchRow(hDb)){			
			new iItemId =  SQL_FetchInt(hDb, 1);
			new iItemQuant = SQL_FetchInt(hDb, 2);
			
			g_iPendingItem[iClient][iAuctionSlot]=iItemId;
			g_iPendingItemQuant[iClient][iAuctionSlot]=iItemQuant;
			iAuctionSlot++;
		}
	}
	
	new bool:bNotif=false;
	for(new x = 0;  x < AUCTIONS_SLOTS ; x++){
		if(g_iPendingItem[iClient][x]>=0 && g_iPendingItemQuant[iClient][x]>0){
			if(!bNotif){
				PrintToChat(iClient, " ");
			}
			new String:sQuery[512];
			FormatEx(sQuery, sizeof sQuery, "DELETE FROM %s WHERE `return_steamid` = '%s' AND `return_item_id` = '%d' AND `return_item_count` = '%d'", SQL_MARKET_R_TABLE_NAME, sSid, g_iPendingItem[iClient][x], g_iPendingItemQuant[iClient][x]);
			SQL_TQuery(g_hSqlDb, _mmSqlRemovePendingItem_Cb, sQuery, 0, DBPrio_Low);
			
			if(!bNotif){
				bNotif=true;
				PrintToChat(iClient, "\x01\x0B\x01 %s\x05 %N\x0D, your auction ended!", PREFIX, iClient);
			}

			PrintToChat(iClient, "\x01\x0B\x01 %s\x0D Item%s not solid:\x05 %d\x0Dx\x05 %s", PREFIX, g_iPendingItemQuant[iClient][x]>1 ? "s" : "", g_iPendingItemQuant[iClient][x], g_sLootBags[g_iPendingItem[iClient][x]]);
			
			g_iPlayerBoxes[iClient][g_iPendingItem[iClient][x]] += g_iPendingItemQuant[iClient][x];
			
			g_iPendingItemQuant[iClient][x]=0;
			g_iPendingItem[iClient][x]=0;
		}
	}
}

public _mmSqlResetData(iClient){
	g_ePlayerData[iClient][dataKills]=0;
	g_ePlayerData[iClient][dataMoney]=0;
	g_ePlayerData[iClient][dataTotalMoney]=0;
	g_ePlayerData[iClient][dataTotalDamage]=0;
	new i;
	for(i = 0 ; i < _:playerChancesTypes ; i++)
	g_ePlayerData[iClient][dataChances][i] = 0;
	
	for(i = 0 ; i < _:playerActivesTypes ; i++)
	g_ePlayerData[iClient][dataActives][i] = 0;
	
	for(i = 0 ; i < _:playerPassiveTypes ; i++)
	g_ePlayerData[iClient][dataPassives][i] = 0;
}

public _mmSqlLoadSkills_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE){
		LogError("%s Load-skills error (%s)", SQL_MSG_PREFIX, sError);
		KickClient(iClient, "SQL DATABASE ERROR");
		return;
	} 
	
	if(SQL_HasResultSet(hDb) && SQL_FetchRow(hDb)){
		new String:sTempSid[32];
		SQL_FetchString(hDb, 0, sTempSid, sizeof(sTempSid));
		
		//if(StrEqual(sTempSid[0], sTempSid[1])){
		FormatEx(g_eSqlPersonalData[iClient][sql_PlayerSid], 32, sTempSid);

		g_ePlayerData[iClient][dataMoney] = SQL_FetchInt(hDb, 1);
		g_ePlayerData[iClient][dataTotalMoney] = SQL_FetchInt(hDb, 2);
		g_ePlayerData[iClient][dataTotalDamage] = SQL_FetchInt(hDb, 3);
		
		new i, iStartFrom;
		for(i = 0 ; i < _:playerChancesTypes ; i++)
		g_ePlayerData[iClient][dataChances][i] = SQL_FetchInt(hDb, 4+i);
		
		iStartFrom = _:playerChancesTypes + 4;
		
		for(i = 0 ; i < _:playerActivesTypes ; i++)
			g_ePlayerData[iClient][dataActives][i] = SQL_FetchInt(hDb, iStartFrom+i);
		
		iStartFrom += _:playerActivesTypes;
		
		for(i = 0 ; i < _:playerPassiveTypes ; i++)
			g_ePlayerData[iClient][dataPassives][i] = SQL_FetchInt(hDb, iStartFrom+i);
		
		iStartFrom += _:playerPassiveTypes;
		
		_mmSqlLoadLootbag(iClient);
			//LogMessage("Got %s=%s | %d",  g_eSqlPersonalData[iClient][sql_PlayerSid], g_eSqlPersonalData[iClient][sql_PlayerName], iStartFrom);
		//}
		//else{
		//	KickClient(iClient, "SQL DATABASE ERROR");
		//	LogError("[INSPECTOR_dbx] Prevented record swap (old id-user disconnected) - executing again..");
		//	return;
		//}
	}
	else{
		new String:sTempSid[32];
		GetClientAuthId(iClient, AuthId_Steam2, sTempSid, sizeof(sTempSid), false);
		new String:sQuery[1024], String:sName[MAX_NAME_LENGTH], String:sEscapedName[MAX_NAME_LENGTH*2+1];
		
		FormatEx(sQuery, sizeof sQuery, g_eSqlData[sql_InsertKey]);
		GetClientName(iClient, sName, sizeof(sName));	
		SQL_EscapeString(g_hSqlDb, sName, sEscapedName, sizeof(sEscapedName));
		
		for(new i = 0 ; i < 2 ; i++)
		ReplaceString(sQuery, sizeof sQuery, g_sSqlToInsertion[i], i==0 ? sTempSid : sEscapedName);
		
		SQL_TQuery(g_hSqlDb, _mmSqlInsertSkills_Cb, sQuery, iClient, DBPrio_Low);
	}
}

public _mmSqlQueryAllAuctions(){
	new String:sQuery[128];
	FormatEx(sQuery, sizeof(sQuery), "SELECT * FROM %s ORDER BY `auction_number` ASC LIMIT %d", SQL_MARKET_TABLE_NAME, AUCTION_COUNT_LIMIT); 			
	SQL_TQuery(g_hSqlDb, _mmSqlQueryAllAuctions_Cb, sQuery, 0, DBPrio_Low);
}

public _mmSqlUpdateMarketAuctionQuantity(iAuction){
	new String:sQuery[256];
	
	FormatEx(sQuery, sizeof(sQuery), "UPDATE `%s` SET `auction_itemcount` = '%d' WHERE `auction_number` = '%d'", SQL_MARKET_TABLE_NAME, g_eMarketData[iAuction][market_ItemCount], iAuction);	
	SQL_TQuery(g_hSqlDb, _mmSqlUpdateMarketAuctionQuantity_Cb, sQuery, 0, DBPrio_Low);
}

public _mmSqlUpdateMarketResetAuction(iAuction){
	new String:sQuery[512];
	
	FormatEx(sQuery, sizeof(sQuery), "UPDATE `%s` SET `auction_item` = '-1', `auction_itemcount` = '-1', `auction_itemprice` = '-1', `auction_steamid` = '-1',  `auction_name` = '-1',  `auction_end` = '-1' WHERE `auction_number` = '%d'", SQL_MARKET_TABLE_NAME, iAuction);	
	SQL_TQuery(g_hSqlDb, _mmSqlUpdateMarketResetAuction_Cb, sQuery, 0, DBPrio_Low);
}

public _mmSqlUpdateMarketAddAuction(iAuction){
	new String:sQuery[512];
	
	FormatEx(sQuery, sizeof(sQuery), "UPDATE `%s` SET `auction_item` = '%d', `auction_itemcount` = '%d', `auction_itemprice` = '%d', `auction_steamid` = '%s',  `auction_name` = '%s',  `auction_end` = '%s' WHERE `auction_number` = '%d'", SQL_MARKET_TABLE_NAME, g_eMarketData[iAuction][market_Item], g_eMarketData[iAuction][market_ItemCount], g_eMarketData[iAuction][market_ItemPrice], g_eMarketData[iAuction][market_PlayerSid], g_eMarketData[iAuction][market_PlayerName], g_eMarketData[iAuction][market_ItemDateEnd], iAuction);	
	SQL_TQuery(g_hSqlDb, _mmSqlUpdateMarketAddAuction_Cb, sQuery, 0, DBPrio_Low);
}

public _mmSqlInsertAuctions(){
	SQL_TQuery(g_hSqlDb, _mmSqlInsertAuctions_Cb, g_sExecuteMarketInsertion[g_iExecutedMarketInsertions], 0, DBPrio_Low);
}

public _mmSqlInsertAuctions_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Insert-market error (%s)", SQL_MSG_PREFIX, sError);
	else{
		g_iExecutedMarketInsertions++;
		if(g_iExecutedMarketInsertions<5){
			_mmSqlInsertAuctions();
		}
	}
}

public _mmSqlPrePendingMoney_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:hData){
	if (hDb == INVALID_HANDLE) LogError("%s Select-pre-send-money error (%s)", SQL_MSG_PREFIX, sError);
	
	new iDataPack[2];
	new String:sSid[32];
	
	new iFromDb[2]={0, 0};
	
	if(SQL_HasResultSet(hDb) && SQL_FetchRow(hDb)){
		iFromDb[0] = SQL_FetchInt(hDb, 2); 	//quant
		iFromDb[1] = SQL_FetchInt(hDb, 3);	//money
	}
	
	new Handle:hPack = hData;
	ResetPack(hPack);
	
	ReadPackString(hPack, sSid, sizeof sSid);
	iDataPack[0] = ReadPackCell(hPack); //quant
	iDataPack[1] = ReadPackCell(hPack);	//money
	CloseHandle(hPack);
	
	if(iFromDb[0] != 0 && iFromDb[1] != 0){ //update
		iDataPack[0]+=iFromDb[0];
		iDataPack[1]+=iFromDb[1];
		
		_mmSqlUpdatePendingMoney(sSid, iDataPack[0], iDataPack[1]);
	}
	else{ //insert
		_mmSqlInsertPendingMoney(sSid, iDataPack[0], iDataPack[1]);
	}
	
	//PrintToChatAll("DATAPACK: iMoney=%d iAmountSold=%d | Sid=%s", iMoney, iAmountSold, sSid);
}

_sqlAuctionExpired(String:sDbSid[], iAuctionId, iItemId, iItemCount){	
	new String:sQuery[512];
	FormatEx(sQuery, sizeof sQuery, "DELETE FROM %s WHERE `auction_number` = '%d' AND `auction_steamid` = '%s'", SQL_MARKET_TABLE_NAME, iAuctionId, sDbSid);
	
	SQL_TQuery(g_hSqlDb, _sqlAuctionExpiredDelete_Cb, sQuery, 0, DBPrio_Low);
	
	_mmSqlInsertPendingItem(sDbSid, iItemId, iItemCount);
}

_sqlAuctionExpiredDelete_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iNone){
	if (hDb == INVALID_HANDLE) LogError("%sDelete-expired-auction error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlQueryAllAuctions_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iNone){
	if (hDb == INVALID_HANDLE) LogError("%s Select-market error (%s)", SQL_MSG_PREFIX, sError);
	
	if(SQL_GetRowCount(hDb)<AUCTION_COUNT_LIMIT){
		new String:sQuery[128];
		FormatEx(sQuery, sizeof sQuery, "TRUNCATE TABLE %s", SQL_MARKET_TABLE_NAME);
		SQL_LockDatabase(g_hSqlDb); 
		SQL_FastQuery(g_hSqlDb, sQuery);
		SQL_UnlockDatabase(g_hSqlDb);
		
		_mmSqlInsertAuctions();
	}
	else{
		int iTime = GetTime();
		
		new iCur=0;
		//LogError("Found Row");
		while(SQL_FetchRow(hDb)){
			new iItemCount = SQL_FetchInt(hDb, 2);
			//LogError("%d", iItemCount);
			
			if(iItemCount>0){
				new String:sDbTime[64], String:sDbSid[64];
				SQL_FetchString(hDb, 6, sDbTime, 64);
				new iAuctionId = SQL_FetchInt(hDb, 0);
				new iItemId = SQL_FetchInt(hDb, 1);
				
				new iItemPrice = SQL_FetchInt(hDb, 3);
				SQL_FetchString(hDb, 4, sDbSid, 64);
				
				new iCompare;
				iCompare = StringToInt(sDbTime);
				//LogError("%d>=%d", iTime, iCompare);
				
				if(iTime>=iCompare){
					//LogError("Expiring one..");
					_sqlAuctionExpired(sDbSid, iAuctionId, iItemId, iItemCount);
					continue;
				}
				else{
					FormatEx(g_eMarketData[iCur][market_ItemDateEnd], 64, sDbTime);
					
					g_eMarketData[iCur][market_AuctionId] = iAuctionId;
					g_eMarketData[iCur][market_Item] = iItemId;
					g_eMarketData[iCur][market_ItemCount] = iItemCount;
					g_eMarketData[iCur][market_ItemPrice] = iItemPrice;
					FormatEx(g_eMarketData[iCur][market_PlayerSid], 64, sDbSid);
					SQL_FetchString(hDb, 5, g_eMarketData[iCur][market_PlayerName], 64);
					//LogError("Added: %dx %d = %d$",  iItemCount, iItemId, iItemPrice);
				}
			}
			
			iCur++;
		}
	}
}

public _mmSqlRemovePendingItem_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iNone){
	if (hDb == INVALID_HANDLE) LogError("%s Remove-pending-item error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlRemovePendingCash_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iNone){
	if (hDb == INVALID_HANDLE) LogError("%s Remove-pending-money error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlUpdatePendingMoney_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iNone){
	if (hDb == INVALID_HANDLE) LogError("%s Update-pending-money error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlUpdateMarketAddAuction_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iNone){
	if (hDb == INVALID_HANDLE) LogError("%s Update-auction-addauction error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlUpdateMarketResetAuction_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iNone){
	if (hDb == INVALID_HANDLE) LogError("%s Update-auction-reset error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlUpdateMarketAuctionQuantity_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iNone){
	if (hDb == INVALID_HANDLE) LogError("%s Update-auction-quantity error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlUpdateSkills_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Update-skill error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlUpdateLootbagSlot_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Update-lootbag-slot error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlUpdateStat_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Update-stats error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlUpdateBasic_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Update-basic error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlInsertPendingMoney_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Insert-pending-money error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlInsertPendingItem_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Insert-pending-item error (%s)", SQL_MSG_PREFIX, sError);
}

public _mmSqlInsertStats_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Insert-stats error (%s)", SQL_MSG_PREFIX, sError);
	
	_mmSqlFinishedLoading(iClient);
}

public _mmSqlInsertLootbag_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Insert-lootbag error (%s)", SQL_MSG_PREFIX, sError);
	
	_mmSqlLoadPendingMoney(iClient);
}

public _mmSqlInsertSkills_Cb(Handle:hOwner, Handle:hDb, const String:sError[], any:iClient){
	if (hDb == INVALID_HANDLE) LogError("%s Insert-all error (%s)", SQL_MSG_PREFIX, sError);
	
	_mmSqlLoadLootbag(iClient);
}