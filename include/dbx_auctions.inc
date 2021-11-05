#if defined _dbx_auctions_included
 #endinput
#endif
#define _dbx_auctions_included

#include <dbx_sql>

#define AUCTION_CODE_LENGTH 	512
#define AUCTION_CODE_HASH		"!@$$@*"
#define AUCTION_COUNT_LIMIT		200

enum auctionStrings {
	auctionStringSid,
	auctionStringName,
	auctionStringDate
};

enum auctionIntegers {
	auctionIntegerItem,
	auctionIntegerItemCount,
	auctionIntegerItemPrize
};

enum marketData {
	String:market_PlayerSid[64],
	String:market_PlayerName[64],
	String:market_ItemDateEnd[64],
	market_Item,
	market_ItemCount,
	market_ItemPrice,
	market_AuctionId
};

enum dateStuff {
	String:date_fullDate[12],
	String:date_Day[3],
	String:date_Month[3],
	date_DayInt,
	date_MonthInt
};

new String:g_sAuctionCode[MAXPLAYERS+1][AUCTION_CODE_LENGTH];
new String:g_sAuctionStringData[PLAYER_TAB][auctionStrings][64];
new String:g_sDateData[dateStuff];

new g_iCurrentAuction[PLAYER_TAB];
new g_iAuctionData[PLAYER_TAB][auctionIntegers];
new g_iAuctionBuyQty[PLAYER_TAB];
new g_eMarketData[AUCTION_COUNT_LIMIT][marketData];
new g_iCurrentAuctions[PLAYER_TAB];
new g_iTempSellCount[PLAYER_TAB];
new g_iTempSellPrice[PLAYER_TAB];
new g_iTempSellDays[PLAYER_TAB];

#pragma unused _marketAuctionCodeToData
#pragma unused g_sAuctionCode
_marketAuctionCodeToData(iClient){
	new String:sToData[AUCTION_CODE_LENGTH];	
	new String:sData[6][64];
	
	FormatEx(sToData, AUCTION_CODE_LENGTH, g_sAuctionCode[iClient]);
	ExplodeString(sToData, "@#$$#$$#@", sData, sizeof sData, sizeof sData[]);

	if(strlen(sToData)){
		if(StringToInt(sData[0])>0){
			g_iAuctionData[iClient][auctionIntegerItemCount] = StringToInt(sData[0]);
			g_iAuctionData[iClient][auctionIntegerItem] = StringToInt(sData[1]);
			g_iAuctionData[iClient][auctionIntegerItemPrize] = StringToInt(sData[2]);
			FormatEx(g_sAuctionStringData[iClient][auctionStringSid], sizeof g_sAuctionStringData[][], sData[3]);
			FormatEx(g_sAuctionStringData[iClient][auctionStringName], sizeof g_sAuctionStringData[][], sData[4]);
			FormatEx(g_sAuctionStringData[iClient][auctionStringDate], sizeof g_sAuctionStringData[][], sData[5]);
		}
		else{
			g_iAuctionData[iClient][auctionIntegerItemCount] = -1;
			g_iAuctionData[iClient][auctionIntegerItem] = -1;
			g_iAuctionData[iClient][auctionIntegerItemPrize] = -1;
			g_sAuctionStringData[iClient][auctionStringSid][0] = EOS;
			g_sAuctionStringData[iClient][auctionStringName][0] = EOS;
			g_sAuctionStringData[iClient][auctionStringDate][0] = EOS;
		}
	}
}

_marketAuctionShortCodeToData(iClient, String:sToHash[]){
	new String:sData[2][64];
	ExplodeString(sToHash, AUCTION_CODE_HASH, sData, sizeof sData, sizeof sData[]);
	
	//PrintToChatAll("%s(%d) | %s(%d)", sData[0], strlen(sData[0]), sData[1], strlen(sData[1]));
	
	if(strlen(sData[0]) && strlen(sData[1])){
		g_iCurrentAuction[iClient] = StringToInt(sData[0]);
		g_iAuctionBuyQty[iClient] = 1;
	}
	else{
		g_iCurrentAuction[iClient] = -1;
		g_iAuctionBuyQty[iClient] = -1;
	}
}

_marketInitializeAuctions(){
	for(new i = 0 ; i < AUCTION_COUNT_LIMIT ; i++){
		_marketResetAuction(i);
	}
	_mmSqlQueryAllAuctions();
}

_marketResetAuction(iWhich){
	g_eMarketData[iWhich][market_PlayerSid][0] = 	EOS;
	g_eMarketData[iWhich][market_PlayerName][0] = 	EOS;
	g_eMarketData[iWhich][market_ItemDateEnd][0] = 	EOS;

	g_eMarketData[iWhich][market_Item] = 			-1;
	g_eMarketData[iWhich][market_ItemCount] = 		-1;
	g_eMarketData[iWhich][market_ItemPrice] = 		-1;
	g_eMarketData[iWhich][market_AuctionId] = 		-1;
}

_marketItemSold(iBuyer, iItem, iQuantity, iMoney, String:sSid[]){
	PrintToChatAll("\x01\x0B\x01 %s\x05 %N\x0D bought\x05 %d\x0Dx\x05 %s\x0D for\x05 %d$", PREFIX, iBuyer, iQuantity, g_sLootBags[iItem], iMoney);
	
	new String:sSteam[32], iFound=-1;
	for(new i = 1 ; i <= MaxClients ; i++){
		if(IsClientConnected(i) && IsClientInGame(i)){
			GetClientAuthId(i, AuthId_Steam2, sSteam, sizeof sSteam);
			
			if(StrEqual(sSteam, sSid)){
				_mmSetMoney(i, MONEY_ADD, iMoney);

				PrintToChat(i, "\x01\x0B\x01 %s\x05 %N\x0D bought your\x05 %d\x0Dx\x05 %s\x0D for\x05 %d$", PREFIX, iBuyer, iQuantity, g_sLootBags[iItem], iMoney);
				iFound=i;
			}
		}
	}
	if(iFound==-1){
		_mmSqlPrePendingMoney(sSid, iMoney, iQuantity);
		PrintToChat(iBuyer, "\x01\x0B\x01 %s\x0D Seller offline. He will get his money on\x05 log-in", PREFIX);
		//_mmSqlSendPendingMoney(sSid, iMoney);
		//_mmSqlSendPendingMoney...
	}
}

_marketListAuctions(iClient){
	new Handle:hMenu = CreateMenu(_marketListAuctions_Cb);
	new iAuctions=0, String:sAuctions[64], String:sAuctionCode[128], String:sAuction[256];
	for(new x = 0 ; x < AUCTION_COUNT_LIMIT ; x++){
		if(g_eMarketData[x][market_ItemCount] > 0){
			iAuctions++;
		}
	}
	
		/*SetMenuTitle(menu, "Top 20 Longjump\n    Rank    Distance           Strafes      Player");  
				Format(szValue, 128, "[0%i.]    %.3f units      %s      » %s", i, ljrecord,szStrafes, szName);
			else
				Format(szValue, 128, "[%i.]    %.3f units      %s      » %s", i, ljrecord,szStrafes, szName);*/
	
	if(iAuctions>0)
		FormatEx(sAuctions, sizeof sAuctions, "Listing %d auction%s\n \n     Price      Item", iAuctions, iAuctions > 1 ? "s" : "");
	else
		FormatEx(sAuctions, sizeof sAuctions, "There are no auctions");

	SetMenuTitle(hMenu, sAuctions);

	for (new a=0; a < AUCTION_COUNT_LIMIT ; a++){
		if(g_eMarketData[a][market_ItemCount] < 1)
			continue;
		
		FormatEx(sAuction, sizeof sAuction, "[%s%d$]    %s      ",  g_eMarketData[a][market_ItemPrice]<100? "  " : "", g_eMarketData[a][market_ItemPrice], g_sLootBags[g_eMarketData[a][market_Item]]);
		//FormatEx(sAuctionCode, sizeof sAuctionCode, "%d@#$%%s@#$$#$$#@%d@#$$#$$#@%s@#$$#$$#@%s@#$$#$$#@%s", g_eMarketData[a][market_ItemCount], g_sLootBags[g_eMarketData[a][market_Item]], g_eMarketData[a][market_ItemPrice], g_eMarketData[a][market_PlayerSid], g_eMarketData[a][market_PlayerName], g_eMarketData[a][market_ItemDateEnd]);
		//new String:sAuctionId[32];
		//IntToString(g_eMarketData[a][market_AuctionId], sAuctionId, sizeof sAuctionId);
		
		FormatEx(sAuctionCode, sizeof sAuctionCode, "%d%s%d", g_eMarketData[a][market_AuctionId], AUCTION_CODE_HASH, g_eMarketData[a][market_ItemCount]);
		
		AddMenuItem(hMenu, sAuctionCode, sAuction);
	}

	if(iAuctions<1){
		AddMenuItem(hMenu, "", "", ITEMDRAW_SPACER);
		AddMenuItem(hMenu, "#no", "Back");
	}
	
	SetMenuExitButton(hMenu, iAuctions ? true : false);
		
	DisplayMenu(hMenu, iClient, 99999);
}

_marketListAuctions_Cb(Handle:hMenu, MenuAction:action, iClient, iParam){
	if (action == MenuAction_Select){
		new String:sChosenCode[64]
		GetMenuItem(hMenu, iParam, sChosenCode, sizeof(sChosenCode));
		
		if(StrEqual(sChosenCode, "#no")){
			cmdMainMenu(iClient, 0);
			return;
		}
		
		//GetMenuItem(hMenu, param2, g_sAuctionCode[param1], sizeof(g_sAuctionCode[]));
		//_marketAuctionCodeToData(iClient);

		_marketAuctionShortCodeToData(iClient, sChosenCode);
		
		if(g_iCurrentAuction[iClient] != -1 && g_iAuctionBuyQty[iClient]>0)
			_marketShowAuction(iClient);
		else{
			PrintToChat(iClient, "\x01\x0B\x01 %s\x0F Error:\x0D Auction no longer exists", PREFIX);
			g_iAuctionBuyQty[iClient]=-1;
			g_iCurrentAuction[iClient]=-1;
			_marketListAuctions(iClient);
			return;
		}
	}  
	if ((action == MenuAction_Cancel))
		cmdMainMenu(iClient, 0);
}

_marketAddApostrof(String:lastString[], String:sOut[], iLen){
	new iAmt = strlen(lastString) + 10;

	for(new i = 0 ; i < iAmt ;  i++){
		Format(sOut, iLen, "%s_", sOut);
	}
}

_marketShowAuction(iClient){
	new String:sTitle[128], String:sText[512], String:sBuy[64], String:sApostrof[64];
	FormatEx(sTitle, sizeof sTitle, "");

	new Handle:hPanel = CreatePanel();
	SetPanelTitle(hPanel, sTitle);
	_marketAddApostrof(g_sLootBags[g_eMarketData[g_iCurrentAuction[iClient]][market_Item]], sApostrof, sizeof sApostrof);
	
	new iStamp = StringToInt(g_eMarketData[g_iCurrentAuction[iClient]][market_ItemDateEnd]);
	new String:sDay[32];
	FormatTime(sDay, sizeof sDay, "%A", iStamp);
	
	new iCur = GetTime();
	new iDays = iStamp-iCur;
	new iDaysLeft=0, iHoursLeft=0, iMinutesLeft=0;
	for(new i = 0 ; i < 3 ; i++){
		if(iDays-86400>0){
			iDays-=86400;
			iDaysLeft++;
		}
	}

	if(iDaysLeft<1){
		for(new i = 0 ; i < 24 ; i++){
			if(iDays>=3600){
				iDays-=3600;
				iHoursLeft++;
			}
		}
		if(iHoursLeft<1){
			for(new i = 0 ; i < 60 ; i++){
				if(iDays>=60){
					iDays-=60;
					iMinutesLeft++;
				}
			}
			
			new iMapLeft;
			GetMapTimeLeft(iMapLeft);

			if(iMinutesLeft <= iMapLeft){
				FormatEx(sDay, sizeof sDay, "After this map!");
			}
			else{
				FormatEx(sDay, sizeof sDay, "Today (%d minute%s left)", iMinutesLeft, iMinutesLeft==1 ? "" : "s");
			}
		}
		else{
			FormatEx(sDay, sizeof sDay, "Today (%d hour%s left)", iHoursLeft, iHoursLeft==1 ? "" : "s");
		}
	}
	else{
		Format(sDay, sizeof sDay, "%s (%d day%s left)", sDay, iDaysLeft, iDaysLeft>1 ? "s" : "");
	}
	
	FormatEx(sText, sizeof sText, "\n \n   %s » %d$ [%dx]\n   %s\n   Ends:  %s\n   By:     %s\n \n", g_sLootBags[g_eMarketData[g_iCurrentAuction[iClient]][market_Item]], g_eMarketData[g_iCurrentAuction[iClient]][market_ItemPrice], g_eMarketData[g_iCurrentAuction[iClient]][market_ItemCount], sApostrof, sDay, g_eMarketData[g_iCurrentAuction[iClient]][market_PlayerName]);
	DrawPanelText(hPanel, sText);
	
	FormatEx(sBuy, sizeof sBuy, "Buy %dx\n%s (%d$)\n \n", g_iAuctionBuyQty[iClient], g_sLootBags[g_eMarketData[g_iCurrentAuction[iClient]][market_Item]], g_iAuctionBuyQty[iClient]*g_eMarketData[g_iCurrentAuction[iClient]][market_ItemPrice]);
	DrawPanelItem(hPanel, sBuy);
	
	if(g_eMarketData[g_iCurrentAuction[iClient]][market_ItemCount]>1){
		DrawPanelItem(hPanel, "[-]");
		DrawPanelItem(hPanel, "[+]");
	}
	
	DrawPanelItem(hPanel, "Back");
	
	SendPanelToClient(hPanel, iClient, _marketShowAuction_Cb, 0);
}

public _marketShowAuction_Cb(Handle:hMenu, MenuAction:action, iClient, iKey){
	new bool:bRefresh=false;

	switch(iKey){
		case 1:{
			new iCur = g_iCurrentAuction[iClient];
			if(g_eMarketData[iCur][market_ItemCount]>=1){
				if(g_eMarketData[iCur][market_ItemCount] >= g_iAuctionBuyQty[iClient]){
					if(g_ePlayerData[iClient][dataMoney] >= g_eMarketData[iCur][market_ItemPrice]*g_iAuctionBuyQty[iClient]){
						g_eMarketData[iCur][market_ItemCount]-=g_iAuctionBuyQty[iClient];
						
						_mmSetMoney(iClient, MONEY_REMOVE, g_eMarketData[iCur][market_ItemPrice]*g_iAuctionBuyQty[iClient]);
						_boxesChangeAmount(iClient, BOXSLOT_ADD, g_eMarketData[iCur][market_Item], g_iAuctionBuyQty[iClient]);
						
						ScreenFade(iClient, FFADE_IN|FFADE_PURGE|FFADE_MODULATE, {0, 196, 32, 255}, 0.4, 0);
						_marketItemSold(iClient, g_eMarketData[iCur][market_Item], g_iAuctionBuyQty[iClient], g_eMarketData[iCur][market_ItemPrice]*g_iAuctionBuyQty[iClient], g_eMarketData[iCur][market_PlayerSid]/*, g_eMarketData[iCur][market_PlayerName]*/);
						
						if(g_eMarketData[iCur][market_ItemCount]>0){
							_mmSqlUpdateMarketAuctionQuantity(iCur); 
							g_iAuctionBuyQty[iClient]=1;//g_eMarketData[iCur][market_ItemCount]; for max available
							 
							_marketShowAuction(iClient);
							return;
						}
						else{
							_marketResetAuction(iCur);
							_mmSqlUpdateMarketResetAuction(iCur);
							
							_marketListAuctions(iClient);
							return;
						}
					}
					else{
						PrintToChat(iClient, "\x01\x0B\x01 %s\x0F Error:\x0D Not enough money", PREFIX);
						_marketShowAuction(iClient);
						return;
					}
				}
				else{
					PrintToChat(iClient, "\x01\x0B\x01 %s\x0F Error:\x0D There are no longer\x05 %d\x0D items to buy", PREFIX, g_iAuctionBuyQty[iClient]);
					g_iAuctionBuyQty[iClient] = g_eMarketData[iCur][market_ItemCount];
					_marketShowAuction(iClient);
					return;
				}
			}
			else{
				PrintToChat(iClient, "\x01\x0B\x01 %s\x0F Error:\x0D Auction no longer exists", PREFIX);
				_marketListAuctions(iClient);
				return;
			}
		}
		case 2,3:{
			if(g_eMarketData[g_iCurrentAuction[iClient]][market_ItemCount]>=1){
				if(g_eMarketData[g_iCurrentAuction[iClient]][market_ItemCount]>1){
					g_iAuctionBuyQty[iClient] = iKey==3? g_iAuctionBuyQty[iClient] >= g_eMarketData[g_iCurrentAuction[iClient]][market_ItemCount] ? 1 : g_iAuctionBuyQty[iClient]+1 :  g_iAuctionBuyQty[iClient] == 1 ? g_eMarketData[g_iCurrentAuction[iClient]][market_ItemCount] : g_iAuctionBuyQty[iClient]-1;
					bRefresh=true;
				}
				else{
					if(iKey==2){
						_marketListAuctions(iClient);
						return;
					}
				}
			}
			else{
				_marketListAuctions(iClient);
				return;
			}
		}
	}
	if(bRefresh)
		_marketShowAuction(iClient);
	else
		_marketListAuctions(iClient);
}

_marketSellItemPanel(iClient){
	new String:sTitle[192], /*String:sItem[128],*/ String:sCount[64], String:sPrice[64], String:sDate[32];
	new Handle:hPanel = CreatePanel();
	
	FormatEx(sTitle, sizeof sTitle, "Item: %s\n%s", g_sLootBags[g_iSelectedBag[iClient]], g_sLootBagDesc[g_iSelectedBag[iClient]]);
	SetPanelTitle(hPanel, sTitle);
	
	FormatEx(sCount, sizeof sCount, "Amount:       %d", g_iTempSellCount[iClient]);
	DrawPanelItem(hPanel, sCount);
	
	FormatEx(sPrice, sizeof sPrice, "Price each:   %d$", g_iTempSellPrice[iClient]);
	DrawPanelItem(hPanel, sPrice);
	
	FormatEx(sDate, sizeof sDate, "Duration:     %d day%s\n \n", g_iTempSellDays[iClient], g_iTempSellDays[iClient]>1 ? "s" : "");
	
	DrawPanelItem(hPanel, sDate);
	
	DrawPanelItem(hPanel, "ADD!");
	DrawPanelItem(hPanel, "Back");
	SendPanelToClient(hPanel, iClient, _marketSellItemPanel_Cb, 0);
	
	CloseHandle(hPanel);
}

public _marketSellItemPanel_Cb(Handle:hMenu, MenuAction:action, iClient, iOpcja){
	switch(iOpcja){
		case 1:{
			g_iTempSellCount[iClient] = g_iTempSellCount[iClient] == g_iPlayerBoxes[iClient][g_iSelectedBag[iClient]] ? 1 : g_iTempSellCount[iClient] + 1;
			_marketSellItemPanel(iClient);
		}
		case 2:{
			g_iTempSellPrice[iClient] = g_iTempSellPrice[iClient] == 200 ? 10 : g_iTempSellPrice[iClient] + 10;
			_marketSellItemPanel(iClient);
		}
		case 3:{
			g_iTempSellDays[iClient] = g_iTempSellDays[iClient] < 3 ? g_iTempSellDays[iClient]+1 : 1;
			_marketSellItemPanel(iClient);
		}
		case 4:{
			new iFree = -1;
			for(new i = 0 ; i < AUCTION_COUNT_LIMIT ; i++){
				if(strlen(g_eMarketData[i][market_PlayerSid]) < 1){
					iFree = i;
					break;
				}
			}
			if(iFree!=-1){
				if(g_iPlayerBoxes[iClient][g_iSelectedBag[iClient]]>0 && g_iTempSellCount[iClient] <= g_iPlayerBoxes[iClient][g_iSelectedBag[iClient]]){
					new String:sSid[64], String:sName[64];
					GetClientAuthId(iClient, AuthId_Steam2, sSid, sizeof(sSid), false);
					FormatEx(sName, sizeof sName, "%N", iClient);
					
					FormatEx(g_eMarketData[iFree][market_PlayerSid], 64, "%s", sSid);
					FormatEx(g_eMarketData[iFree][market_PlayerName], 64, "%s", sName);
					
					new String:sStamp[16], iTimeStamp;
					FormatTime(sStamp, sizeof sStamp, "%s", GetTime());
					
					iTimeStamp = StringToInt(sStamp);
					iTimeStamp += ((24*60*60)*g_iTempSellDays[iClient]) + 3600;
					
					FormatEx(sStamp, sizeof sStamp, "%d", iTimeStamp);
					
					FormatEx(g_eMarketData[iFree][market_ItemDateEnd], 64, sStamp);
					g_eMarketData[iFree][market_AuctionId] = iFree;
					g_eMarketData[iFree][market_Item] = g_iSelectedBag[iClient];
					g_eMarketData[iFree][market_ItemCount] = g_iTempSellCount[iClient];
					g_eMarketData[iFree][market_ItemPrice] = g_iTempSellPrice[iClient];
					
					_mmSqlUpdateMarketAddAuction(iFree);
					_boxesChangeAmount(iClient, BOXSLOT_REMOVE, g_iSelectedBag[iClient], g_iTempSellCount[iClient]);
					g_iCurrentAuctions[iClient]++;
					PrintToChat(iClient, "\x01\x0B\x01 %s\x0D Added\x05 %d\x0Dx\x05 %s\x0D with a price:\x05 %d\x0D$", PREFIX, g_iTempSellCount[iClient], g_sLootBags[g_iSelectedBag[iClient]], g_iTempSellPrice[iClient]);
					
					g_iSelectedBag[iClient] = _boxesLootGetFirstItem(iClient);
					cmdLootBag(iClient, 0);
				}
				else{
					PrintToChat(iClient, "\x01\x0B\x01 %s\x0F Error:\x0D didn't add\x05 %s\x0D to the market!", PREFIX, g_sLootBags[g_iSelectedBag[iClient]]);
					g_iSelectedBag[iClient] = _boxesLootGetFirstItem(iClient);
					cmdLootBag(iClient, 0);
					return;
				}
			}
			else{
				PrintToChat(iClient, "\x01\x0B\x01 %s\x0F Error:\x0D Auction limit of 200 exceed.\x05 Try again later.", PREFIX);
				_marketSellItemPanel(iClient);
			}
		}
		case 5:{
			cmdLootBag(iClient, 0);
		}
	}
}