#include <sdkhooks>
#include <sdktools>
#include <cstrike>

bool g_DEBUG = true;

#include "mapchooser/structs/SMap.sp"
#include "mapchooser/structs/SNomination.sp"
#include "mapchooser/structs/SMapGroup.sp"
#include "mapchooser/structs/SPlayer.sp"
// #include "mapchooser/structs/SSpawns.sp" 
#include "mapchooser/structs/SVote.sp"

#include "mapchooser/globals.sp"
#include "mapchooser/database.sp"
#include "mapchooser/helpers.sp"
#include "mapchooser/lib.sp"

#include "mapchooser/menus.sp"
#include "mapchooser/menus_handlers.sp"
#include "mapchooser/timers.sp"

public void OnPluginStart() {
    g_alMaps = new ArrayList(sizeof(SMap));
    g_alMapGroups = new ArrayList(sizeof(SMapGroup));
    g_alVotes = new ArrayList(sizeof(SVote));
    g_alRecentlyPlayed = new ArrayList(sizeof(RecentlyPlayed));

    // Listen for <say> and <say_team>
    AddCommandListener(MC_CommandListener, "say");
    AddCommandListener(MC_CommandListener, "say_team");

    // Events
    HookEvent("round_start", Event_OnRoundStart, EventHookMode_Post);
    
    // Initialize menus that wont ever change
    g_mMenuMaps = new Menu(MC_Menu_Maplist_Handler);
    g_MapvoteMenu = new Menu(MC_Menu_Mapvote_Handler);
    g_mVoteMenu = new Menu(MC_Menu_Mapvote_Handler);
    g_smVotes = new StringMap();

    // Create the static maplist, this wont change since
    // this is just the list of all the available maps
    MC_CreateMaplistMenu();

    // Set Vote In Progress to false, cause we have no vote at the moment
    g_bVoteInProgress = false;

    // Commands
    RegConsoleCmd("sm_nominate", Command_Nominate);
    RegConsoleCmd("sm_maps", Command_DisplayMaplistMenu);

    Database_OnPluginStart();

    for(int i = 0; i < sizeof(g_iVoteCount)-1; i++) {
        g_iVoteCount[i] = 0;
    }
    // g_iVoteCount = {0, ...};
    g_currentState = EState_MapStart;
}

public void OnPluginEnd() {
    delete g_mMenuMaps;
    delete g_smVotes;
    delete g_MapvoteMenu;
    delete g_mVoteMenu;
}

public Action Command_DisplayMaplistMenu(int client, int args) {
    g_mMenuMaps.Display(client, 30);
    return Plugin_Handled;
}

public void Event_OnRoundStart(Event event, const char[] name, bool bDontBroadcast) {
    if(g_currentState == EState_PostVote) {
        // Change the map if vote is done
        SMap map; g_alMaps.GetArray(g_nextMapIdx, map, sizeof(SMap));
        ChatAll("Chaning map to \x10%s", map.filename);

        RequestFrame(Task_ChangeMap);
    }
}

public void Task_ChangeMap(any data) {
    SMap map; g_alMaps.GetArray(g_nextMapIdx, map, sizeof(SMap));
    ChatAll("Chaning map to \x10%s", map.filename);

    ServerCommand("changelevel %s", map.filename);
}

public Action Command_Nominate(int client, int args) {
    char sMap[256], buffer[512];
    
    Chat(client, "Current State: %d", view_as<int>(g_currentState));

    if(g_currentState == EState_Vote) {
        Chat(client, "A vote is in progress, you cannot nominate right now.");
        return Plugin_Handled;
    }

    if(args > 0) {
        GetCmdArg(1, sMap, sizeof(sMap));
        int idx = MC_FindMapByPartionalString(sMap, buffer, sizeof(buffer));

        if(idx > -1) {
            SMap map;g_alMaps.GetArray(idx, map, sizeof(SMap));
            g_player[client].nominationIdx = idx;
            Chat(client, "Nominated map \x10%s", map.cleanname);
        }
        else {
            Chat(client, "Could not find map \x10%s", sMap);
        }
    }
    else {
        if(g_DEBUG) {
            Chat(client, "Opening maplist menu");
        }
        MC_OpenNominationMenu(client, MENU_TIME_FOREVER);
    }
    return Plugin_Handled;
}

public Action MC_CommandListener(int client, const char[] command, int argc) {
    char rtvCommands[][] = { "!rtv", "/rtv", "rtv" };

    char sCommandArgs[256];
    GetCmdArgString(sCommandArgs, sizeof(sCommandArgs));
    StripQuotes(sCommandArgs);

    if(IsRTVCommand(sCommandArgs) && g_currentState == EState_PostVote) {
        Chat(client, "Current State: \x10EState_PostVote");
        return Plugin_Handled;
    }

    if(g_player[client].hasVoted && IsRTVCommand(sCommandArgs)) {
        Chat(client, "You have already voted!");
        return Plugin_Handled;
    }

    // if(IsRTVCommand(sCommandArgs)) {
    //     MC_OpenMapvoteMenuToEveryone();
    //     return Plugin_Handled;
    // }

    if(!g_bVoteInProgress) {
        // Chat(client, "!g_bVoteInProgress");

        if(IsRTVCommand(sCommandArgs)) {
            // Chat(client, "!g_bVoteInProgress && IsRTVCommand(sCommandArgs)");

            if(g_player[client].hasVoted) {
                Chat(client, "You have already voted!");
                return Plugin_Handled;
            }

            if(!g_bIsRTVAllowed) {
                Chat(client, "RTV is not allowed yet!");
                return Plugin_Handled;
            }

            float percent = ++g_iRockTheVotes / GetPlayerCount();

            if(g_iRockTheVotes == GetPlayerCount()) {
                percent = 100.0;
                g_bVoteInProgress = true;
                MC_OpenMapvoteMenuToEveryone();
            }
            if(percent > 0.6) {
                g_bVoteInProgress = true;
            }
            Chat(client, "\x08You rocked the vote! (\x04%.1f\x08)", percent);
            return Plugin_Handled;
        }
    }
    else {
        if(IsRTVCommand(sCommandArgs)) {
            int playerCount = GetPlayerCount();
            if(playerCount > 0) {
                if((g_iRockTheVotes / playerCount) > MC_MIN_ROCKTHEVOTE_PERCENTAGE) {
                    g_bVoteInProgress = true;

                    MC_OpenMapvoteMenuToEveryone();
                }
            }
        }
    }
}

bool IsRTVCommand(char[] chatMessage) {
    char rtvCommands[][] = { "!rtv", "/rtv", "rtv" };

    for(int i = 0; i < sizeof(rtvCommands); i++) {
        if(StrEqual(chatMessage, rtvCommands[i], false)) {
            return true;
        }
    }
    return false
}

public void OnClientPutInServer(int client) {
    if(g_player[client].init(client)) {
        Chat(client, "Initialized Player");
        g_hMapvoteResultsTimer[client] = null;
    }
}

public void OnMapStart() {
    g_currentState = EState_MapStart;
    g_hAllowRTVTimer = CreateTimer(30.0, Timer_EnableRTV, _, TIMER_FLAG_NO_MAPCHANGE);
}