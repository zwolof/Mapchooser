// Maplist


void MC_ShuffleMaps() {
    if(g_alMaps == null) return;

    g_alMaps_Random = g_alMaps.Clone();

    for(int i = g_alMaps_Random.Length-1; i >= 0; i--) {
        int k = GetRandomInt(0, i);
        g_alMaps_Random.SwapAt(k, i);

        // Fix Nominations
        for(int j = 1; j <= MaxClients; j++) {
            int temp = g_iNomination[i];

            g_iNomination[i] = g_iNomination[k];
            g_iNomination[k] = temp;
        }

        // for(int m = 0; m < g_alMaps_Random.Length; m++) {
        //     int tempVotes = g_iVoteCount[m];
        //     g_iVoteCount[m] = g_iVoteCount[k];
        //     g_iVoteCount[k] = tempVotes;
        // }
    }
}

void MC_CreateMaplistMenu() {
    g_mMenuMaps.SetTitle("Maps");

    int len = g_alMaps.Length;

    g_mMenuMaps.RemoveAllItems();
    if(len == 0) {
        g_mMenuMaps.AddItem("no_maps_found", "No Maps Available", ITEMDRAW_DISABLED);
    }
    else {
        SMap map;
        for(int i = 0; i < g_alMaps.Length; i++) {
            g_alMaps.GetArray(i, map, sizeof(SMap));

            g_mMenuMaps.AddItem(map.filename, map.cleanname);
        }
    }
    g_mMenuMaps.ExitButton = true;
}

// Open Mapvote for everyone
void MC_OpenMapvoteMenuToEveryone() {

    g_currentState = EState_Vote;
    PrepareVote();

    ChatAll("Randomized Maps...");
    ChatAll("Choose wisely!");
    
    // Loop clients and open the menu for everyone
    for(int i = 1; i <= MaxClients; i++) {
        if(IsClientInGame(i) && !IsFakeClient(i)) {
            MC_Menu_Mapvote(i, MENU_TIME_FOREVER);
        }
    }
    CreateTimer(30.0, MC_Timer_CalculateVoteCount, _, TIMER_FLAG_NO_MAPCHANGE);
}

// Nominations

enum struct VoteResult {
    SMap map;
    int votes;
}

public Action MC_ShowMapvoteResults(int client, int args) {
    Panel panel = new Panel();
    
    char TitleBuffer[2048];
    FormatEx(TitleBuffer, sizeof(TitleBuffer), "Mapvote Results");

    ArrayList sortedArray = new ArrayList(sizeof(VoteResult));

    for(int i = 0; i < g_alMaps_Random.Length; i++) {
        SMap map; g_alMaps_Random.GetArray(i, map, sizeof(SMap));
        int value = g_iVoteCount[i];

        VoteResult result;
        result.map = map;
        result.votes = value;

        sortedArray.PushArray(result);
    }

    sortedArray.SortCustom(ArrayADTCustomCallback);

    for(int i = 0; i < sortedArray.Length; i++) {
        VoteResult voteresult;
        sortedArray.GetArray(i, voteresult, sizeof(VoteResult));

        // Variables
        SMap map;
        map = voteresult.map;
        
        int votes = 0;
        votes = voteresult.votes;

        if(votes <= 0) continue;
        if(MC_IsMapRecentlyPlayed(map.filename)) continue;
        
        FormatEx(TitleBuffer, sizeof(TitleBuffer), "%s\n ▸ %s [%d vote%s]", TitleBuffer, map.cleanname, votes, (votes > 1 || votes == 0) ? "s" : "");
        // menu.AddItem(map.filename, mapItemBuffer, ITEMDRAW_DISABLED);
    }
    panel.SetTitle(TitleBuffer);
    panel.Send(client, MC_MapvoteResultsMenu_Handler, 1);

    return Plugin_Handled;
}

public ArrayADTCustomCallback(int index1, int index2, Handle array, Handle hndl) {
    VoteResult result1; VoteResult result2;

    GetArrayArray(array, index1, result1, sizeof(VoteResult));
    GetArrayArray(array, index2, result2, sizeof(VoteResult));

	return (result2.votes > result1.votes);
}

public Action MC_OpenNominationMenu(int client, int args) {
    Menu menu = new Menu(MC_Menu_Nominations_Handler);
    menu.SetTitle("Nominate map");

    int len = g_alMaps.Length;

    if(len == 0) {
        menu.AddItem("no_maps_found", "No Maps Available", ITEMDRAW_DISABLED);
    }
    else {
        SMap map; char buffer[512];
        for(int i = 0; i < g_alMaps.Length; i++) {
            g_alMaps.GetArray(i, map, sizeof(SMap));
            bool nominated = (g_player[client].nominationIdx == i);

            FormatEx(buffer, sizeof(buffer), "%s%s", nominated ? "*" : "", map.cleanname);

            char sIndex[32];
            IntToString(i, sIndex, sizeof(sIndex));

            if(!MC_IsMapRecentlyPlayed(map.filename)) {
                menu.AddItem(sIndex, buffer, nominated ? ITEMDRAW_DISABLED : ITEMDRAW_DEFAULT);
            }
        }
    }
    menu.ExitButton = true;
    menu.Display(client, MENU_TIME_FOREVER);
 
    return Plugin_Handled;
}

void MC_ResetVotes() {
    for(int i = 0; i < g_alMaps_Random.Length; i++) {
        g_iVoteCount[i] = (i % 2 == 0) ? GetRandomInt(1, 2) : 0;
    }
}

void PrepareVote() {
    g_mVoteMenu.RemoveAllItems();
    MC_ShuffleMaps();
    MC_ResetVotes();

    int len = g_alMaps.Length;

    if(len == 0) {
        g_mVoteMenu.AddItem("no_maps_found", "No Maps Available", ITEMDRAW_DISABLED);
    }
    else {
        SMap map; char buffer[512];

        int mapCount = 0;
        for(int i = 0; i < len; i++) {

            char key[32];IntToString(i, key, sizeof(key));
            g_alMaps_Random.GetArray(i, map, sizeof(SMap));

            if(MC_IsMapNominated(i)) {
                PrintToChatAll("Is Nominated: \x10", map.cleanname);

                // if(!MC_IsMapRecentlyPlayed(map.filename)) {
                    g_mVoteMenu.AddItem(key, map.cleanname);
                    mapCount++;
                // }
            }
        }

        if(mapCount == 0) {
            PrintToChatAll("No Maps Nominated, randomizing...");

            SMap randomMap;
            for(int j = 0; j < 6; j++) {
                g_alMaps_Random.GetArray(j, randomMap, sizeof(SMap));

                char idx[16]; IntToString(j, idx, sizeof(idx));
                g_mVoteMenu.AddItem(idx, randomMap.cleanname);
            }
        }
    }
    g_mVoteMenu.ExitButton = false;
}

public Action MC_Menu_Mapvote(int client, int args) {
    g_mVoteMenu.Display(client, 29);
 
    return Plugin_Handled;
}

// void MC_BuildMapvoteMenu() {

//     char sMapTitle[256];
//     g_MapvoteMenu.SetTitle("Choose the next map");

//     SMap map;
//     for(int i = 0; i < g_alMaps.Length; i++) {
//         g_alMaps.GetArray(i, map, sizeof(SMap));

//         FormatEx(sMapTitle, sizeof(sMapTitle), "%s (%d/%d)", map.cleanname, map.votes, GetPlayerCount());
//         g_MapvoteMenu.AddItem(map.filename, sMapTitle);
//     }
// }

// void MC_OpenRTVMenuForAll() {
//     g_smVotes = new StringMap();
//     float fTimer = 30.0;
//     for(int i = 1; i <= MaxClients; i++) {
//         if(IsClientInGame(i) && !IsFakeClient(i)) {
//             g_MapvoteMenu.Display(i, view_as<int>(FloatAbs(fTimer)))
//         }
//     }
//     CreateTimer(fTime, MC_Timer_CalculateVoteCount, _, TIMER_FLAG_NO_MAPCHANGE);
// }

