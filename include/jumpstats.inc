stock GetBeamEndOrigin(float fOrigin[3], float vAngles[3], float fDistance, float fResult[3])
{
	float fAngleVector[3];
	GetAngleVectors(vAngles, fAngleVector, NULL_VECTOR, NULL_VECTOR);
	NormalizeVector(fAngleVector, fAngleVector);
	ScaleVector(fAngleVector, fDistance);
	AddVectors(fOrigin, fAngleVector, fResult);
}

stock GetBeamHitOrigin(float fOrigin[3], float vAngles[3], float fResult[3])
{
	Handle hTrace = TR_TraceRayFilterEx(fOrigin, vAngles, MASK_SHOT, RayType_Infinite, TraceEntityFilterPlayer);
	
	if(TR_DidHit(hTrace))
	{
		TR_GetEndPosition(fResult, hTrace);
		CloseHandle(hTrace);
	}
}


stock GetAimOrigin(client, float fOrigin2[3])
{
	float vAngles[3];
	float fOrigin[3];
	GetClientEyePosition(client,fOrigin);
	GetClientEyeAngles(client, vAngles);

	Handle trace = TR_TraceRayFilterEx(fOrigin, vAngles, MASK_SHOT, RayType_Infinite, TraceEntityFilterPlayer);

	if(TR_DidHit(trace))
	{
		TR_GetEndPosition(fOrigin2, trace);
		CloseHandle(trace);
		return 1;
	}
	CloseHandle(trace);
	return 0;
}

stock GetBoxFromPoint(float fOrigin[3], float fResult[2][3])
{
	float fTemp[3];
	fTemp = fOrigin;
	fTemp[2] += 1.0;
	
	float fAng[4][3];
	fAng[1][1] = 90.0;
	fAng[2][1] = 180.0;
	fAng[3][1] = -90.0;
	
	bool bEdgeFound[4];
	float fDist[4];
	float fTempdist[4];
	float fPos[3];
	float fGround[3];
	float fLast[4];
	float fEdge[4][3];
	
	for(int i = 0; i < 4; i++)
	{
		TraceWallOrigin(fTemp, fAng[i], fEdge[i]);
		fTempdist[i] = GetVectorDistance(fTemp, fEdge[i]);
		fLast[i] = fOrigin[2];
		
		while(fDist[i] < fTempdist[i])
		{
			if(bEdgeFound[i])
				break;
				
			GetBeamEndOrigin(fTemp, fAng[i], fDist[i], fPos);
			TraceGroundOrigin(fPos, fGround);
			
			if((fLast[i] != fGround[2])&&(fLast[i] > fGround[2]))
			{
				fEdge[i] = fGround;
				bEdgeFound[i] = true;
			}
			fLast[i] = fGround[2];
			fDist[i] += 10.0;
		}
		if(!bEdgeFound[i])
		{
			TraceGroundOrigin(fEdge[i], fEdge[i]);
			bEdgeFound[i] = true;
		}
		else
		{
			fGround = fEdge[i];
			fGround[2] = fOrigin[2];
			
			MakeVectorFromPoints(fGround, fOrigin, fPos);
			GetVectorAngles(fPos, fAng[i]);
			
			fGround[2] -= 1.0;
			GetBeamHitOrigin(fGround, fAng[i], fEdge[i]);
		}
		fEdge[i][2] = fOrigin[2];
	}
	
	if(bEdgeFound[0] && bEdgeFound[1] && bEdgeFound[2] && bEdgeFound[3])
	{
		fResult[0][2] = fOrigin[2];
		fResult[1][2] = fOrigin[2];
		fResult[0][0] = fEdge[0][0];
		fResult[0][1] = fEdge[1][1];
		fResult[1][0] = fEdge[2][0];
		fResult[1][1] = fEdge[3][1];
	}
}


stock bool IsCoordInBlockPoint(const float origin[3], const float pos[2][3], bool ignorez)
{
	bool bX;
	bool bY;
	bool bZ;
	
	float fTemp[2][3];
	fTemp[0] = pos[0];
	fTemp[1] = pos[1];
	fTemp[0][0] += 16.0;
	fTemp[0][1] += 16.0;
	fTemp[1][0] -= 16.0;
	fTemp[1][1] -= 16.0;
	
	if (ignorez) bZ = true;

	if(fTemp[0][0] > fTemp[1][0])
	{
		if(fTemp[0][0] >= origin[0] >= fTemp[1][0])
			bX = true;
	}
	else
	{
		if(fTemp[1][0] >= origin[0] >= fTemp[0][0])
			bX = true;
	}
	
	if(fTemp[0][1] > fTemp[1][1])
	{
		if(fTemp[0][1] >= origin[1] >= fTemp[1][1])
			bY = true;
	}
	else
	{
		if(fTemp[1][1] >= origin[1] >= fTemp[0][1])
			bY = true;
	}
	if(fTemp[0][2] + 0.002 >= origin[2] >= fTemp[0][2])
		bZ = true;

	return (bX && bY && bZ);
}

public bool TRDontHitSelf(entity, mask, any data)
{
	return (entity == data)
}

stock TraceClientViewEntity(int client)
{
	float m_vecOrigin[3];
	float m_angRotation[3];
	GetClientEyePosition(client, m_vecOrigin);
	GetClientEyeAngles(client, m_angRotation);
	
	Handle tr = TR_TraceRayFilterEx(m_vecOrigin, m_angRotation, MASK_VISIBLE, RayType_Infinite, TRDontHitSelf, client);
	int pEntity = -1;
	
	if (TR_DidHit(tr))
	{
		pEntity = TR_GetEntityIndex(tr);
		CloseHandle(tr);
		return pEntity;
	}
	
	CloseHandle(tr);
	return -1;
}

stock TraceGroundOrigin(float fOrigin[3], float fResult[3])
{
	float vAngles[3] = {90.0, 0.0, 0.0};
	Handle hTrace = TR_TraceRayFilterEx(fOrigin, vAngles, MASK_SHOT, RayType_Infinite, TraceEntityFilterPlayer);
	
	if(TR_DidHit(hTrace))
	{
		TR_GetEndPosition(fResult, hTrace);
		CloseHandle(hTrace);
		
		return 1;
	}
	CloseHandle(hTrace);
	
	return 0;
}

stock TraceWallOrigin(float fOrigin[3], float vAngles[3], float fResult[3])
{
	Handle hTrace = TR_TraceRayFilterEx(fOrigin, vAngles, MASK_SHOT, RayType_Infinite, TraceEntityFilterPlayer);
	
	if(TR_DidHit(hTrace))
	{
		TR_GetEndPosition(fResult, hTrace);
		CloseHandle(hTrace);
		
		return 1;
	}
	CloseHandle(hTrace);
	
	return 0;
}

stock TE_SendBlockPoint(client, const float fPos1[3], const float fPos2[3], model)
{
	float fBuffer[4][3];
	fBuffer[2] 		= fPos1;
	fBuffer[3] 		= fPos2;
	fBuffer[0] 		= fBuffer[2];
	fBuffer[0][1] 	= fBuffer[3][1];
	fBuffer[1] 		= fBuffer[3];
	fBuffer[1][1] 	= fBuffer[2][1];
	
	TE_SetupBeamPoints(fBuffer[3], fBuffer[0], model, 0, 0, 0, 0.13, 2.0, 2.0, 10, 0.0, {0,255,0,255}, 0);
	TE_SendToClient(client);
	
	TE_SetupBeamPoints(fBuffer[0], fBuffer[2], model, 0, 0, 0, 0.13, 2.0, 2.0, 10, 0.0, {0,255,0,255}, 0);
	TE_SendToClient(client);
	
	TE_SetupBeamPoints(fBuffer[2], fBuffer[1], model, 0, 0, 0, 0.13, 2.0, 2.0, 10, 0.0, {0,255,0,255}, 0);
	TE_SendToClient(client);
	
	TE_SetupBeamPoints(fBuffer[1], fBuffer[3], model, 0, 0, 0, 0.13, 2.0, 2.0, 10, 0.0, {0,255,0,255}, 0);
	TE_SendToClient(client);
}

