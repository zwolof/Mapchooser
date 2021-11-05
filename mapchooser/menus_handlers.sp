
// public int MC_Menu_Nominations_Handler(Menu menu, MenuAction action, int client, int choice) {

//     char sItem[128];
//     menu.GetItem(choice, sItem, sizeof(sItem));

//     // Switch the actions boys
//     switch(action) {
//         case MenuAction_Select: {

//             // int id = MC_FindMapIdByFilename(sItem);

//             // if(id == -1) {
//             //     Chat(client, "\x08Could not nominate map, map was not found.");
//             // }
//             // else {
//             //     // g_player[client].nominations.mapid = id;

//             //     char name[128]; MC_GetMapCleanName(sItem, name, sizeof(name));
//             //     Chat(client, "\x08Successfully nominated \x10%s", name);
//             // }
//         }
//         case MenuAction_End: {
//             delete menu;
//         }
//     }
// }
// enum struct SMap {
//     int id;
//     int votes;
    
//     char filename[128];
//     char cleanname[128];
// }
public int MC_Menu_Nominations_Handler(Menu menu, MenuAction action, int client, int choice) {
    char sItem[128]; menu.GetItem(choice, sItem, sizeof(sItem));

    int index = StringToInt(sItem);

    switch(action) {
        case MenuAction_Select: {
            
            int lastNomination = g_player[client].nominationIdx;
            g_player[client].nominationIdx = index;

            SMap map;
            g_alMaps.GetArray(index, map, sizeof(SMap));

            bool changedNomination = !(g_player[client].nominationIdx == lastNomination);
            Chat(client, "You %s\x10%s", changedNomination ? "changed your nomination to " : "nominated map ", map.cleanname);

            MC_OpenNominationMenu(client, MENU_TIME_FOREVER);
        }
        case MenuAction_End: {
            delete menu;
        }
    }
}

public int MC_Menu_Maplist_Handler(Menu menu, MenuAction action, int client, int choice) {
    char sItem[128]; menu.GetItem(choice, sItem, sizeof(sItem));

    switch(action) {
        case MenuAction_Select: {
            Chat(client, "Map: \x10%s", sItem);
        }
    }
}

public int MC_MapvoteResultsMenu_Handler(Menu menu, MenuAction action, int client, int choice) {
    switch(action) {
        case MenuAction_Select: {
            if(choice == 0) {
                Chat(client, "Deleting Menu Handle Here")
                delete menu;
            }
        }
        case MenuAction_End: {
            delete menu;
        }
    }
}

public int MC_Menu_Mapvote_Handler(Menu menu, MenuAction action, int client, int choice) {

    char sItem[128];
    menu.GetItem(choice, sItem, sizeof(sItem));

    // Switch the actions boys
    switch(action) {
        case MenuAction_Select: {
            
            // Menu Item key is the array index of g_alMaps
            int mapIndex = StringToInt(sItem);

            // Get map object from array by index supplied
            SMap map; g_alMaps_Random.GetArray(mapIndex, map, sizeof(SMap));

            // Increment votes for that map
            g_iVoteCount[mapIndex]++;

            // Set Voted to true cuz they just voted right?
            g_player[client].hasVoted = true;

            // Print Vote confirmation to chat
            Chat(client, "\x08You voted for \x10%s \x08(\x0F%d\x08 vote%s)", map.cleanname, g_iVoteCount[mapIndex], g_iVoteCount[mapIndex] > 1 ? "s" : "");
            
            // Create Timer to reopen Mapvote Results menu every 1.0s
            g_hMapvoteResultsTimer[client] = CreateTimer(1.0, MC_Timer_DisplayVoteResults, GetClientUserId(client), TIMER_FLAG_NO_MAPCHANGE|TIMER_REPEAT);
        }
    }
}