public Action MC_Timer_DisplayVoteResults(Handle timer, any data) {
    int client = GetClientOfUserId(data);

    if(!((0 < client <= MaxClients) && IsClientInGame(client) && !IsFakeClient(client))) {
        return Plugin_Continue;
    }

    if(g_currentState == EState_PostVote) {
        g_hMapvoteResultsTimer[client] = null;
        return Plugin_Stop;
    }

    MC_ShowMapvoteResults(client, 0);
    return Plugin_Continue;
}

public Action MC_Timer_CalculateVoteCount(Handle timer, any data) {

    g_nextMapIdx = MC_GetMapvoteWinnerIndex();

    char mapName[256];
    MC_GetMapCleanNameByIndex(g_alMaps_Random, g_nextMapIdx, mapName, sizeof(mapName));

    g_currentState = EState_PostVote;
    g_bIsVoteDone = true;

    for(int i = 1; i <= MaxClients; i++) {
        if(IsClientInGame(i) && !IsFakeClient(i)) {
            MC_ShowMapvoteResults(i, MENU_TIME_FOREVER);
            Chat(i, "Displaying results, vote is done..");
        }
    }
    // ChatAll("MC_ShowMapvoteResults()");
    ChatAll("The next map will be \x10%s", mapName);
    
    return Plugin_Stop;
}

public Action Timer_EnableRTV(Handle tmr, any data) {
    g_hAllowRTVTimer = null;
    g_bIsRTVAllowed = true;

    ChatAll("RTV Is now allowed!");
    return Plugin_Stop
}