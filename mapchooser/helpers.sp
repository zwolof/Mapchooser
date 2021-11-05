void Chat(int client, char[] message, any ...) {
    if(client && IsClientInGame(client) && !IsFakeClient(client)) {
        char szBuffer[PLATFORM_MAX_PATH], szNewMessage[PLATFORM_MAX_PATH];
        Format(szBuffer, sizeof(szBuffer), PLUGIN_PREFIX..." \x08%s", message);
        VFormat(szNewMessage, sizeof(szNewMessage), szBuffer, 3);

        Handle hBf = StartMessageOne("SayText2", client, USERMSG_RELIABLE | USERMSG_BLOCKHOOKS);
		if(hBf != null) {
			if(GetUserMessageType() == UM_Protobuf) {
				Protobuf hProtoBuffer = UserMessageToProtobuf(hBf);
				hProtoBuffer.SetInt("ent_idx", client);
				hProtoBuffer.SetBool("chat", true);
				hProtoBuffer.SetString("msg_name", szNewMessage);
				hProtoBuffer.AddString("params", "");
				hProtoBuffer.AddString("params", "");
				hProtoBuffer.AddString("params", "");
				hProtoBuffer.AddString("params", "");
			}
			else {
				BfWrite hBfBuffer = UserMessageToBfWrite(hBf);
				hBfBuffer.WriteByte(client);
				hBfBuffer.WriteByte(true);
				hBfBuffer.WriteString(szNewMessage);
			}
		}
		EndMessage();
    }
}

void ChatAll(char[] message, any ...) {
    char szBuffer[PLATFORM_MAX_PATH], szNewMessage[PLATFORM_MAX_PATH];
    Format(szBuffer, sizeof(szBuffer), " \x08%s", message);
    VFormat(szNewMessage, sizeof(szNewMessage), szBuffer, 2);

    for(int i = 1; i <= MaxClients; i++) {
        if(IsClientInGame(i) && !IsFakeClient(i)) {
            Chat(i, "%s", szNewMessage);
        }
    }
}


int GetPlayerCount() {
    int count = 0;
    for(int i = 1; i <= MaxClients; i++) {
        if(IsClientInGame(i) && !IsFakeClient(i)) {
            count++;
        }
    }
    return count;
}