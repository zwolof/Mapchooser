#if defined _dbx_stats_included
 #endinput
#endif
#define _dbx_stats_included

new g_eStatistics[PLAYER_TAB][playerStatistics];

_statChange(iClient, dataStatOperations:iOperation, playerStatistics:iType, iAmount){
	g_eStatistics[iClient][iType]=iOperation==STATs_ADD ? g_eStatistics[iClient][iType]+iAmount : g_eStatistics[iClient][iType]-iAmount;
	//PrintToChatAll("Adding: %d x %d box", iAmount, iType);
	
	_mmSqlUpdateStat(iClient, iType);
}

_statCountKDA(iClient){
	if(g_eStatistics[iClient][statsDeaths])
		g_fPlayerKDA[iClient] = (float(g_eStatistics[iClient][statsKilled]+g_eStatistics[iClient][statsAssists])) / float(g_eStatistics[iClient][statsDeaths]);
	else g_fPlayerKDA[iClient] = float(g_eStatistics[iClient][statsKilled]+g_eStatistics[iClient][statsAssists]);
}

_statUpdateScoreboard(iClient){
	SetEntProp(iClient, Prop_Data, "m_iFrags", g_eStatistics[iClient][statsKilled]);
	CS_SetClientContributionScore(iClient, g_eStatistics[iClient][statsKilled]);
	CS_SetClientAssists(iClient, g_eStatistics[iClient][statsAssists]);
}