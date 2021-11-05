#if defined _eranks_included
#endinput
#endif
#define _eranks_included

Database g_Database = null;

#define COMMUNITY_NAME 	"eFrag"
#define SQL_CONNECTION 	"ebans"
#define DB_NAME 		"ebans"
#define PREFIX	 		" \x01\x04\x01[\x10eRank\x01] "
#define STOREURL		"www.store.efrag.eu"
#define BANSURL			"www.bans.efrag.eu"
#define SITEURL			"www.efrag.eu"
#define MS 				MAXPLAYERS+1

#define SERVER			"retakes"
#define SEASON			1

// ================================= //
enum struct Ranking
{
	int id;
	int iRank;
	
	// Stats
	int iPoints;
	int iKills;
	int iDeaths;
	int iAssists;
	int iHeadshot;
	
	// Methods
	int __rankUp() {
		return ++this.iRank;
	}
	
	int __rankDown() {
		return --this.iRank;
	}
	
	int __addRank(int iRank2) {
		return (this.iRank += iRank2);
	}
	
	int __remRank(int iRank2) {
		return (this.iRank -= iRank2);
	}
}
Ranking g_Rank[MS];
// ================================= //

enum Ranks
{ 
	UNRANKED = 0,	
	NEW,
	BEGINNER,
	SCRUB,
	CASUAL,
	POTENTIAL,
	REGULAR,
	GOOD,
	AVERAGE,
	SKILLED,
	EXPERT,
	SEMI_PRO,
	PRO,
	GODLIKE,
	GOD,
	VETERAN,
	MASTER,
	BOSS,
	SPECIALIST,
	IMPRESSIVE,
	SURREAL
};

char g_sRanks[Ranks][32] = {
	"Unranked",
	"New",
	"Beginner",
	"Scrub",
	"Casual",
	"Potential",
	"Regular",
	"Good",
	"Average",
	"Skilled",
	"Expert",
	"Semi Pro",
	"Pro",
	"Godlike",
	"God",
	"Veteran",
	"Master",
	"Boss",
	"Specialist",
	"Impressive",
	"Surreal"
};

public void SQL_ConnectCallback(Database db, const char[] error, any data)
{
	if(db == null)
	{
		LogError("T_Connect returned invalid Database Handle");
		return;
	}
	g_Database = db;
}

void SQL_FetchUserData(int client)
{
	if(IsValidClient(client)) 
	{
		char szQuery[256], szSteamId[64];
		GetClientAuthId(client, AuthId_SteamID64, szSteamId, sizeof(szSteamId));
		
		// We need to get all of their data from the eFrag Databases `efrag_users` and `efrag_ranking`.
		g_Database.Format(szQuery, sizeof(szQuery), "SELECT * FROM `%s_ranking` WHERE `steamid`='%s' AND `server`='%s' AND `season`='%d' LIMIT 1;",
			DB_NAME, szSteamId, SERVER, SEASON);
		
		PrintToChat(client, "%s Fetching UserData..", PREFIX);
		
		g_Database.Query(SQL_FetchUserData_Callback, szQuery, GetClientUserId(client));
	}
}

void SQL_UpdateStats(int client)
{
	if(IsValidClient(client)) 
	{
		char szQuery[256], szSteamId[64];
		GetClientAuthId(client, AuthId_SteamID64, szSteamId, sizeof(szSteamId));
		
		// We need to get all of their data from the eFrag Databases `efrag_users` and `efrag_ranking`.
		g_Database.Format(szQuery, sizeof(szQuery), "UPDATE `%s_ranking` SET points='%d', kills='%d', deaths='%d', assists='%d', headshot='%d' WHERE steamid='%s' AND server='%s' AND season='%d';",
													DB_NAME,
													g_Rank[client].iPoints,
													g_Rank[client].iKills,
													g_Rank[client].iDeaths,
													g_Rank[client].iAssists,
													g_Rank[client].iHeadshot,
													szSteamId,
													SERVER,
													SEASON);
		
		PrintToChat(client, "%s Updating UserData..", PREFIX);
		
		g_Database.Query(SQL_UpdateStats_Callback, szQuery, GetClientUserId(client));
	}
}

void SQL_InsertUser(int client)
{
	if(IsValidClient(client)) 
	{
		char szQuery[256], szSteamId[64];
		GetClientAuthId(client, AuthId_SteamID64, szSteamId, sizeof(szSteamId));
		
		g_Database.Format(szQuery, sizeof(szQuery), "INSERT INTO `%s_ranking`(`steamid`, `points`, `kills`, `deaths`, `assists`, `headshot`, `server`, `season`) VALUES('%s', '%d', '%d', '%d', '%d', '%d', '0', '%d');",
			DB_NAME,
			szSteamId,
			g_Rank[client].iPoints,
			g_Rank[client].iKills,
			g_Rank[client].iDeaths,
			g_Rank[client].iAssists,
			g_Rank[client].iHeadshot,
			SERVER,
			SEASON);
		
		PrintToChat(client, "%s Inserting UserData..", PREFIX);
		
		g_Database.Query(SQL_InsertUser_Callback, szQuery, GetClientUserId(client));
	}
}

public int SQL_FetchUserData_Callback(Database db, DBResultSet results, const char[] szError, int userid)
{	
	int client = GetClientOfUserId(userid); if(client == 0) return;
	
	if(db == null || results == null)
	{
		PrintToChatAll("[SQL] Fetch Check Query failure: %s", szError);
		return;
	}
	else if (results.RowCount == 0)
	{
		SQL_InsertUser(client);
		return;
	}
	else if (results.RowCount == 1)
	{
		if(IsValidClient(client))
		{
			int points, kills, deaths, assists, headshot;
			
			results.FieldNameToNum("points", points);
			results.FieldNameToNum("kills", kills);
			results.FieldNameToNum("deaths", deaths);
			results.FieldNameToNum("assists", assists);
			results.FieldNameToNum("headshot", headshot);
			
			if(results.FetchRow())
			{
				g_Rank[client].iPoints 		= results.FetchInt(points);
				g_Rank[client].iKills 		= results.FetchInt(kills);
				g_Rank[client].iDeaths 		= results.FetchInt(deaths);
				g_Rank[client].iAssists 	= results.FetchInt(assists);
				g_Rank[client].iHeadshot 	= results.FetchInt(headshot);
			}
		}
		return;
	}
	else PrintToChat(client, "%s You haven't played this season!", PREFIX);
}

public int SQL_InsertUser_Callback(Database db, DBResultSet results, const char[] szError, int userid)
{
	int client = GetClientOfUserId(userid); if(client == 0) return;
	
	if(db == null || results == null)
	{
		PrintToChatAll("[SQL] Insert Query failure: %s", szError);
		return;
	}
	else return;
}

public int SQL_UpdateStats_Callback(Database db, DBResultSet results, const char[] szError, int userid)
{
	int client = GetClientOfUserId(userid); if(client == 0) return;
	
	if(db == null || results == null)
	{
		PrintToChatAll("[SQL] Update Query failure: %s", szError);
		return;
	}
	else return;
}


stock bool IsValidClient(int client)
{
	return view_as<bool>((0 < client <= MaxClients) && IsClientInGame(client) && !IsFakeClient(client));
}