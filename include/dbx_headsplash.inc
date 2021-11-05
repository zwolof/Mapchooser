#if defined _dbx_headsplash_included
 #endinput
#endif
#define _dbx_headsplash_included

#define HEADSPLASH_BASE_DAMAGE 17.5

enum headsplashData {
	Float:headsplash_SoundTime,
	Float:headsplash_TouchTime,
	headsplash_NextSound
};

enum dataHeadsplashSound {
	dataHSound0,
	dataHSound1,
	dataHSound2
};

new const String:g_sHeadsplashSound[dataHeadsplashSound][] = { 
	"*playcore/playcore_sfx29.mp3",
	"*playcore/playcore_sfx30.mp3",
	"*playcore/playcore_sfx31.mp3"
};

new g_eHeadSplashData[MAXPLAYERS+1][headsplashData];

_headsplashPrecacheSounds(){
	new String:sPath[64];
	for(new x = 0 ; x < _:dataHeadsplashSound ; x++){
		FormatEx(sPath, sizeof sPath, g_sHeadsplashSound[x]);
		AddToStringTable(FindStringTable("soundprecache"), sPath);
		
		ReplaceString(sPath, sizeof sPath, "*", "sound/", false);	
		PrecacheSound(sPath, true);	
		AddFileToDownloadsTable(sPath);	
	}
}

_headsplashDamage(iVictim, iAttacker, Float:fDamage, String:sWeapon[], eDamage=DMG_GENERIC){
	
	new iHurt = CreateEntityByName("point_hurt");
	new String:sDamage[5], String:sDamageType[5];
	IntToString(RoundFloat(fDamage), sDamage, sizeof(sDamage));
	IntToString(eDamage, sDamageType, sizeof(sDamageType));
            
	DispatchKeyValue(iVictim, "targetname", "hurtme");
	DispatchKeyValue(iHurt, "Damagetarget", "hurtme");
	DispatchKeyValue(iHurt, "Damage", sDamage);
	DispatchKeyValue(iHurt, "DamageType", sDamageType);
	DispatchKeyValue(iHurt, "classname", sWeapon);
	DispatchSpawn(iHurt);
	AcceptEntityInput(iHurt, "Hurt", iAttacker);
	DispatchKeyValue(iVictim, "targetname","donthurtme"); //unset the victim as target for damage
	RemoveEdict(iHurt);
}

public Action:_headsplashOnTouch(iClient, iVictim){ 
    if((1<=iVictim<=MaxClients) && IsPlayerAlive(iVictim) && GetEntPropEnt(iClient, Prop_Send, "m_hGroundEntity") == iVictim && GetClientTeam(iVictim) != GetClientTeam(iClient)){
		if(GetEntProp(iVictim, Prop_Data, "m_takedamage") != 2) return;
		
		new Float:fTime = GetGameTime();
		if(fTime-g_eHeadSplashData[iClient][headsplash_TouchTime] >= 0.2){
			g_eHeadSplashData[iClient][headsplash_TouchTime] = GetGameTime();
						
			new Float:fFinalDmg, Float:fVel[3];
			GetEntPropVector(iClient, Prop_Data, "m_vecVelocity", fVel);
			
			if(fTime-g_eHeadSplashData[iClient][headsplash_SoundTime] >= 0.4){
				if(fTime-g_eHeadSplashData[iClient][headsplash_SoundTime] >= 3.0){
					g_eHeadSplashData[iClient][headsplash_NextSound] = 0;
				}
				
				new Float:fPos[3];
				GetClientEyePosition(iVictim, fPos);
				
				if(fVel[2] <= -350.0)
					EmitAmbientSound(g_sHeadsplashSound[2], fPos, iVictim, SNDLEVEL_RAIDSIREN, _, 1.25);
				else{
					EmitAmbientSound(g_sHeadsplashSound[g_eHeadSplashData[iClient][headsplash_NextSound]], fPos, iVictim, SNDLEVEL_RAIDSIREN, _, 1.25);
					g_eHeadSplashData[iClient][headsplash_NextSound] = g_eHeadSplashData[iClient][headsplash_NextSound] < 2 ? g_eHeadSplashData[iClient][headsplash_NextSound]+1 : 0;
				}
				g_eHeadSplashData[iClient][headsplash_SoundTime] = GetGameTime();
			}
			
			
			new iVictimHp = GetClientHealth(iVictim);
			fFinalDmg = fVel[2] <= -350.0 ? FloatMin(float(iVictimHp), HEADSPLASH_BASE_DAMAGE+(fVel[2]*-1.0) * 0.2) : HEADSPLASH_BASE_DAMAGE;
			
			new iHeadSplash = _mmSkillGetLevel(iClient, PASSIVE, passiveHeadsplashReduction);
			if(iHeadSplash)
				fFinalDmg -= fFinalDmg * (iHeadSplash*g_iPassivesConstants[passiveHeadsplashReduction][INCBYVALUE])/100.0;
			if(hnsGetBandana(iVictim)==1)
				fFinalDmg -= fFinalDmg * 0.25;
				
			if(fFinalDmg>0.0){
				_headsplashDamage(iVictim, iClient, fFinalDmg, "weapon_healthshot", DMG_SHOCK);
				
				if(iVictimHp-RoundFloat(fFinalDmg)<1){
					_statChange(iClient, STATs_ADD, statsKilledSplash, 1);
				}
			}
		}
	}
}