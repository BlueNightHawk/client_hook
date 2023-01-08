#include "defs.h"

#include "studio.h"
#include "com_model.h"
#include "r_studioint.h"

#include "camanims.h"

// up / down
#define PITCH 0
// left / right
#define YAW 1
// fall over
#define ROLL 2

engine_studio_api_s IEngineStudio;
extern cl_enginefuncs_s gEngfuncs;

PLUGINFUNCS
{
	void DLLEXPORT HUD_Init_Post();
	void DLLEXPORT HUD_Redraw_Post(float flTime, int intermission);
	void DLLEXPORT HUD_GetStudioModelInterface_Pre(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio);
	void DLLEXPORT HUD_GetStudioModelInterface_Post(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio);
}

Vector nl_CamPos, nl_CamAngles;

cvar_t* cl_badcmp = nullptr;

void InitViewmodelInfo()
{
	static char token[64];

	char *pfile, *afile;
	pfile = afile = (char*)gEngfuncs.COM_LoadFile("config/viewmodelinfo.txt", 5, nullptr);
	if (!pfile)
		return;

	while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
	{
		nlviewmodelinfo_s viewinfo;
		static char name[64];

		if (!stricmp(token, "name"))
		{
			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			strcpy_s(name, token);

			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			if (token[0] == '{')
			{
				while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
				{
					if (token[0] == '}')
					{
						break;
					}
					if (!stricmp(token, "camerabone"))
					{
						pfile = gEngfuncs.COM_ParseFile(pfile, token);
						viewinfo.iCameraBone = atoi(token);
					}
					if (!stricmp(token, "animscale"))
					{
						pfile = gEngfuncs.COM_ParseFile(pfile, token);
						viewinfo.flAnimScale = atof(token);
					}
				}
			}
			strcpy_s(viewinfo.name, name);
			nl_viewmodelinfo.insert(std::make_pair(name, viewinfo));
		}
	}

	gEngfuncs.COM_FreeFile(afile);
}

void DLLEXPORT HUD_Init_Post()
{
}

void DLLEXPORT HUD_GetStudioModelInterface_Pre(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{
	InitViewmodelInfo();

	memcpy(&IEngineStudio, pstudio, sizeof(engine_studio_api_s));
	g_Renderer.Init();
}

r_studio_interface_s origStudio;

int R_StudioDrawModel(int flags)
{
	int result = origStudio.StudioDrawModel(flags);

	if (IEngineStudio.GetCurrentEntity() == gEngfuncs.GetViewModel())
	{
		g_Renderer.StudioDrawModel(0);
	}
	return result;
}

void DLLEXPORT HUD_GetStudioModelInterface_Post(int version, struct r_studio_interface_s** ppinterface, struct engine_studio_api_s* pstudio)
{
	memcpy(&origStudio, *ppinterface, sizeof(r_studio_interface_s));

	(*ppinterface)->StudioDrawModel = R_StudioDrawModel;
}

void DLLEXPORT HUD_Redraw_Post(float flTime, int intermission)
{
}

void Matrix3x4_VectorTransform(float in[3][4], float v[3], float out[3])
{
	out[0] = v[0] * in[0][0] + v[1] * in[0][1] + v[2] * in[0][2] + in[0][3];
	out[1] = v[0] * in[1][0] + v[1] * in[1][1] + v[2] * in[1][2] + in[1][3];
	out[2] = v[0] * in[2][0] + v[1] * in[2][1] + v[2] * in[2][2] + in[2][3];
}

#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI))

void Matrix3x4_OriginFromMatrix(float in[3][4], float* out)
{
	out[0] = in[0][3];
	out[1] = in[1][3];
	out[2] = in[2][3];
}

void Matrix3x4_AnglesFromMatrix(float in[3][4], float* out)
{
	float forward[3];
	float left[3];
	float up[3];

	//
	// Extract the basis vectors from the matrix. Since we only need the Z
	// component of the up vector, we don't get X and Y.
	//
	forward[0] = in[0][0];
	forward[1] = in[1][0];
	forward[2] = in[2][0];
	left[0] = in[0][1];
	left[1] = in[1][1];
	left[2] = in[2][1];
	up[2] = in[2][2];

	float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

	// enough here to get angles?
	if (xyDist > 0.001f)
	{
		// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
		out[1] = RAD2DEG(atan2f(forward[1], forward[0]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		out[0] = RAD2DEG(atan2f(-forward[2], xyDist));

		// (roll)	z = ATAN( left.z, up.z );
		out[2] = RAD2DEG(atan2f(left[2], up[2]));
	}
	else // forward is mostly Z, gimbal lock-
	{
		// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
		out[1] = RAD2DEG(atan2f(-left[0], left[1]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		out[0] = RAD2DEG(atan2f(-forward[2], xyDist));

		// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
		out[2] = 0;
	}
}

std::string str_tolower(std::string s)
{
	std::transform(s.begin(), s.end(), s.begin(),
		// static_cast<int(*)(int)>(std::tolower)         // wrong
		// [](int c){ return std::tolower(c); }           // wrong
		// [](char c){ return std::tolower(c); }          // wrong
		[](unsigned char c)
		{ return std::tolower(c); } // correct
	);
	return s;
}

/*
================
ConcatTransforms

================
*/
void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);

/*
====================
AngleQuaternion

====================
*/
void AngleQuaternion(float* angles, vec4_t quaternion)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = angles[2] * 0.5;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * 0.5;
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * 0.5;
	sr = sin(angle);
	cr = cos(angle);

	quaternion[0] = sr * cp * cy - cr * sp * sy; // X
	quaternion[1] = cr * sp * cy + sr * cp * sy; // Y
	quaternion[2] = cr * cp * sy - sr * sp * cy; // Z
	quaternion[3] = cr * cp * cy + sr * sp * sy; // W
}

/*
====================
QuaternionSlerp

====================
*/
void QuaternionSlerp(vec4_t p, vec4_t q, float t, vec4_t qt)
{
	int i;
	float omega, cosom, sinom, sclp, sclq;

	// decide if one of the quaternions is backwards
	float a = 0;
	float b = 0;

	for (i = 0; i < 4; i++)
	{
		a += (p[i] - q[i]) * (p[i] - q[i]);
		b += (p[i] + q[i]) * (p[i] + q[i]);
	}
	if (a > b)
	{
		for (i = 0; i < 4; i++)
		{
			q[i] = -q[i];
		}
	}

	cosom = p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3];

	if ((1.0 + cosom) > 0.000001)
	{
		if ((1.0 - cosom) > 0.000001)
		{
			omega = acos(cosom);
			sinom = sin(omega);
			sclp = sin((1.0 - t) * omega) / sinom;
			sclq = sin(t * omega) / sinom;
		}
		else
		{
			sclp = 1.0 - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++)
		{
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else
	{
		qt[0] = -q[1];
		qt[1] = q[0];
		qt[2] = -q[3];
		qt[3] = q[2];
		sclp = sin((1.0 - t) * (0.5 * M_PI));
		sclq = sin(t * (0.5 * M_PI));
		for (i = 0; i < 3; i++)
		{
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

/*
====================
QuaternionMatrix

====================
*/
void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4])
{
	matrix[0][0] = 1.0 - 2.0 * quaternion[1] * quaternion[1] - 2.0 * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0 * quaternion[0] * quaternion[1] + 2.0 * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0 * quaternion[0] * quaternion[2] - 2.0 * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0 * quaternion[0] * quaternion[1] - 2.0 * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0 * quaternion[1] * quaternion[2] + 2.0 * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0 * quaternion[0] * quaternion[2] + 2.0 * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0 * quaternion[1] * quaternion[2] - 2.0 * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[1] * quaternion[1];
}

/*
====================
MatrixCopy

====================
*/
void MatrixCopy(float in[3][4], float out[3][4])
{
	memcpy(out, in, sizeof(float) * 3 * 4);
}


/*
====================
Init
====================
*/
void CMiniRenderer::Init()
{
	IEngineStudio.GetModelCounters(&m_pStudioModelCount, &m_pModelsDrawn);

	// Get pointers to engine data structures
	m_pbonetransform = (float(*)[MAXSTUDIOBONES][3][4])IEngineStudio.StudioGetBoneTransform();
	m_plighttransform = (float(*)[MAXSTUDIOBONES][3][4])IEngineStudio.StudioGetLightTransform();
	m_paliastransform = (float(*)[3][4])IEngineStudio.StudioGetAliasTransform();
	m_protationmatrix = (float(*)[3][4])IEngineStudio.StudioGetRotationMatrix();
}

/*
====================
CMiniRenderer
====================
*/
CMiniRenderer::CMiniRenderer()
{
	m_fDoInterp = true;
	m_fGaitEstimation = true;
	m_pCurrentEntity = NULL;
	m_pCvarHiModels = NULL;
	m_pCvarDeveloper = NULL;
	m_pCvarDrawEntities = NULL;
	m_pChromeSprite = NULL;
	m_pStudioModelCount = NULL;
	m_pModelsDrawn = NULL;
	m_protationmatrix = NULL;
	m_paliastransform = NULL;
	m_pbonetransform = NULL;
	m_plighttransform = NULL;
	m_pStudioHeader = NULL;
	m_pBodyPart = NULL;
	m_pSubModel = NULL;
	m_pPlayerInfo = NULL;
	m_pRenderModel = NULL;
}

/*
====================
~CMiniRenderer
====================
*/
CMiniRenderer::~CMiniRenderer()
{
}


/*
====================
StudioCalcBoneAdj
====================
*/
void CMiniRenderer::StudioCalcBoneAdj(float dadt, float* adj, const byte* pcontroller1, const byte* pcontroller2, byte mouthopen)
{
	int i, j;
	float value;
	mstudiobonecontroller_t* pbonecontroller;

	pbonecontroller = (mstudiobonecontroller_t*)((byte*)m_pStudioHeader + m_pStudioHeader->bonecontrollerindex);

	for (j = 0; j < m_pStudioHeader->numbonecontrollers; j++)
	{
		i = pbonecontroller[j].index;
		if (i <= 3)
		{
			// check for 360% wrapping
			if ((pbonecontroller[j].type & STUDIO_RLOOP) != 0)
			{
				if (abs(pcontroller1[i] - pcontroller2[i]) > 128)
				{
					int a, b;
					a = (pcontroller1[j] + 128) % 256;
					b = (pcontroller2[j] + 128) % 256;
					value = ((a * dadt) + (b * (1 - dadt)) - 128) * (360.0 / 256.0) + pbonecontroller[j].start;
				}
				else
				{
					value = ((pcontroller1[i] * dadt + (pcontroller2[i]) * (1.0 - dadt))) * (360.0 / 256.0) + pbonecontroller[j].start;
				}
			}
			else
			{
				value = (pcontroller1[i] * dadt + pcontroller2[i] * (1.0 - dadt)) / 255.0;
				if (value < 0)
					value = 0;
				if (value > 1.0)
					value = 1.0;
				value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			}
			// Con_DPrintf( "%d %d %f : %f\n", m_pCurrentEntity->curstate.controller[j], m_pCurrentEntity->latched.prevcontroller[j], value, dadt );
		}
		else
		{
			value = mouthopen / 64.0;
			if (value > 1.0)
				value = 1.0;
			value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
			// Con_DPrintf("%d %f\n", mouthopen, value );
		}
		switch (pbonecontroller[j].type & STUDIO_TYPES)
		{
		case STUDIO_XR:
		case STUDIO_YR:
		case STUDIO_ZR:
			adj[j] = value * (M_PI / 180.0);
			break;
		case STUDIO_X:
		case STUDIO_Y:
		case STUDIO_Z:
			adj[j] = value;
			break;
		}
	}
}


/*
====================
StudioCalcBoneQuaterion
====================
*/
void CMiniRenderer::StudioCalcBoneQuaterion(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* q, int index)
{
	static Vector prevangle;

	int j, k;
	vec4_t q1, q2;
	Vector angle1, angle2;
	mstudioanimvalue_t* panimvalue;

	for (j = 0; j < 3; j++)
	{
		if (panim->offset[j + 3] == 0)
		{
			angle2[j] = angle1[j] = pbone->value[j + 3]; // default;
		}
		else
		{
			panimvalue = (mstudioanimvalue_t*)((byte*)panim + panim->offset[j + 3]);
			k = frame;
			// DEBUG
			if (panimvalue->num.total < panimvalue->num.valid)
				k = 0;
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
				// DEBUG
				if (panimvalue->num.total < panimvalue->num.valid)
					k = 0;
			}
			// Bah, missing blend!
			if (panimvalue->num.valid > k)
			{
				angle1[j] = panimvalue[k + 1].value;

				if (panimvalue->num.valid > k + 1)
				{
					angle2[j] = panimvalue[k + 2].value;
				}
				else
				{
					if (panimvalue->num.total > k + 1)
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;
				if (panimvalue->num.total > k + 1)
				{
					angle2[j] = angle1[j];
				}
				else
				{
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			angle1[j] = pbone->value[j + 3] + angle1[j] * pbone->scale[j + 3];
			angle2[j] = pbone->value[j + 3] + angle2[j] * pbone->scale[j + 3];
		}

		if (pbone->bonecontroller[j + 3] != -1)
		{
			angle1[j] += adj[pbone->bonecontroller[j + 3]];
			angle2[j] += adj[pbone->bonecontroller[j + 3]];
		}
	}

	if (!VectorCompare(angle1, angle2))
	{
		AngleQuaternion(angle1, q1);
		AngleQuaternion(angle2, q2);
		QuaternionSlerp(q1, q2, s, q);
	}
	else
	{
		AngleQuaternion(angle1, q);
	}
}

/*
====================
StudioCalcBonePosition
====================
*/
void CMiniRenderer::StudioCalcBonePosition(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* pos)
{
	int j, k;
	mstudioanimvalue_t* panimvalue;

	for (j = 0; j < 3; j++)
	{
		pos[j] = pbone->value[j]; // default;
		if (panim->offset[j] != 0)
		{
			panimvalue = (mstudioanimvalue_t*)((byte*)panim + panim->offset[j]);
			/*
			if (i == 0 && j == 0)
				Con_DPrintf("%d  %d:%d  %f\n", frame, panimvalue->num.valid, panimvalue->num.total, s );
			*/

			k = frame;
			// DEBUG
			if (panimvalue->num.total < panimvalue->num.valid)
				k = 0;
			// find span of values that includes the frame we want
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
				// DEBUG
				if (panimvalue->num.total < panimvalue->num.valid)
					k = 0;
			}
			// if we're inside the span
			if (panimvalue->num.valid > k)
			{
				// and there's more data in the span
				if (panimvalue->num.valid > k + 1)
				{
					pos[j] += (panimvalue[k + 1].value * (1.0 - s) + s * panimvalue[k + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k + 1].value * pbone->scale[j];
				}
			}
			else
			{
				// are we at the end of the repeating values section and there's another section with data?
				if (panimvalue->num.total <= k + 1)
				{
					pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0 - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
				}
			}
		}
		if (pbone->bonecontroller[j] != -1 && adj)
		{
			pos[j] += adj[pbone->bonecontroller[j]];
		}
	}
}

/*
====================
StudioSlerpBones
====================
*/
void CMiniRenderer::StudioSlerpBones(vec4_t q1[], float pos1[][3], vec4_t q2[], float pos2[][3], float s)
{
	int i;
	vec4_t q3;
	float s1;

	if (s < 0)
		s = 0;
	else if (s > 1.0)
		s = 1.0;

	s1 = 1.0 - s;

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		QuaternionSlerp(q1[i], q2[i], s, q3);
		q1[i][0] = q3[0];
		q1[i][1] = q3[1];
		q1[i][2] = q3[2];
		q1[i][3] = q3[3];
		pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
		pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
		pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
	}
}

/*
====================
StudioGetAnim
====================
*/
mstudioanim_t* CMiniRenderer::StudioGetAnim(model_t* m_pSubModel, mstudioseqdesc_t* pseqdesc)
{
	mstudioseqgroup_t* pseqgroup;
	cache_user_t* paSequences;

	pseqgroup = (mstudioseqgroup_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqgroupindex) + pseqdesc->seqgroup;

	if (pseqdesc->seqgroup == 0)
	{
		return (mstudioanim_t*)((byte*)m_pStudioHeader + pseqdesc->animindex);
	}

	paSequences = (cache_user_t*)m_pSubModel->submodels;

	if (paSequences == NULL)
	{
		paSequences = (cache_user_t*)IEngineStudio.Mem_Calloc(16, sizeof(cache_user_t)); // UNDONE: leak!
		m_pSubModel->submodels = (dmodel_t*)paSequences;
	}

	if (!IEngineStudio.Cache_Check((struct cache_user_s*)&(paSequences[pseqdesc->seqgroup])))
	{
		gEngfuncs.Con_DPrintf("loading %s\n", pseqgroup->name);
		IEngineStudio.LoadCacheFile(pseqgroup->name, (struct cache_user_s*)&paSequences[pseqdesc->seqgroup]);
	}
	return (mstudioanim_t*)((byte*)paSequences[pseqdesc->seqgroup].data + pseqdesc->animindex);
}

/*
====================
StudioPlayerBlend
====================
*/
void CMiniRenderer::StudioPlayerBlend(mstudioseqdesc_t* pseqdesc, int* pBlend, float* pPitch)
{
	// calc up/down pointing
	*pBlend = (*pPitch * 3);
	if (*pBlend < pseqdesc->blendstart[0])
	{
		*pPitch -= pseqdesc->blendstart[0] / 3.0;
		*pBlend = 0;
	}
	else if (*pBlend > pseqdesc->blendend[0])
	{
		*pPitch -= pseqdesc->blendend[0] / 3.0;
		*pBlend = 255;
	}
	else
	{
		if (pseqdesc->blendend[0] - pseqdesc->blendstart[0] < 0.1) // catch qc error
			*pBlend = 127;
		else
			*pBlend = 255 * (*pBlend - pseqdesc->blendstart[0]) / (pseqdesc->blendend[0] - pseqdesc->blendstart[0]);
		*pPitch = 0;
	}
}

/*
====================
StudioSetUpTransform
====================
*/
void CMiniRenderer::StudioSetUpTransform(bool trivial_accept)
{
	int i;
	Vector angles;
	Vector modelpos;

	// tweek model origin
	// for (i = 0; i < 3; i++)
	//	modelpos[i] = m_pCurrentEntity->origin[i];

	VectorCopy(m_pCurrentEntity->origin, modelpos);

	// TODO: should really be stored with the entity instead of being reconstructed
	// TODO: should use a look-up table
	// TODO: could cache lazily, stored in the entity
	angles[ROLL] = m_pCurrentEntity->curstate.angles[ROLL];
	angles[PITCH] = m_pCurrentEntity->curstate.angles[PITCH];
	angles[YAW] = m_pCurrentEntity->curstate.angles[YAW];

	// Con_DPrintf("Angles %4.2f prev %4.2f for %i\n", angles[PITCH], m_pCurrentEntity->index);
	// Con_DPrintf("movetype %d %d\n", m_pCurrentEntity->movetype, m_pCurrentEntity->aiment );
	if (m_pCurrentEntity->curstate.movetype == MOVETYPE_STEP)
	{
		float f = 0;
		float d;

		// don't do it if the goalstarttime hasn't updated in a while.

		// NOTE:  Because we need to interpolate multiplayer characters, the interpolation time limit
		//  was increased to 1.0 s., which is 2x the max lag we are accounting for.

		if ((m_clTime < m_pCurrentEntity->curstate.animtime + 1.0f) &&
			(m_pCurrentEntity->curstate.animtime != m_pCurrentEntity->latched.prevanimtime))
		{
			f = (m_clTime - m_pCurrentEntity->curstate.animtime) / (m_pCurrentEntity->curstate.animtime - m_pCurrentEntity->latched.prevanimtime);
			// Con_DPrintf("%4.2f %.2f %.2f\n", f, m_pCurrentEntity->curstate.animtime, m_clTime);
		}

		if (m_fDoInterp)
		{
			// ugly hack to interpolate angle, position. current is reached 0.1 seconds after being set
			f = f - 1.0;
		}
		else
		{
			f = 0;
		}

		const auto pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;

		if ((pseqdesc->motiontype & STUDIO_LX) != 0 || (m_pCurrentEntity->curstate.eflags & EFLAG_SLERP) != 0)
		{
			for (i = 0; i < 3; i++)
			{
				modelpos[i] += (m_pCurrentEntity->origin[i] - m_pCurrentEntity->latched.prevorigin[i]) * f;
			}
		}

		// NOTE:  Because multiplayer lag can be relatively large, we don't want to cap
		//  f at 1.5 anymore.
		// if (f > -1.0 && f < 1.5) {}

		//			Con_DPrintf("%.0f %.0f\n",m_pCurrentEntity->msg_angles[0][YAW], m_pCurrentEntity->msg_angles[1][YAW] );
		for (i = 0; i < 3; i++)
		{
			float ang1, ang2;

			ang1 = m_pCurrentEntity->angles[i];
			ang2 = m_pCurrentEntity->latched.prevangles[i];

			d = ang1 - ang2;
			if (d > 180)
			{
				d -= 360;
			}
			else if (d < -180)
			{
				d += 360;
			}

			angles[i] += d * f;
		}
		// Con_DPrintf("%.3f \n", f );
	}
	else if (m_pCurrentEntity->curstate.movetype != MOVETYPE_NONE)
	{
		VectorCopy(m_pCurrentEntity->angles, angles);
	}

	// Con_DPrintf("%.0f %0.f %0.f\n", modelpos[0], modelpos[1], modelpos[2] );
	// Con_DPrintf("%.0f %0.f %0.f\n", angles[0], angles[1], angles[2] );

	angles[PITCH] = -angles[PITCH];
	AngleMatrix(angles, (*m_protationmatrix));

	if (0 == IEngineStudio.IsHardware())
	{
		static float viewmatrix[3][4];

		VectorCopy(m_vRight, viewmatrix[0]);
		VectorCopy(m_vUp, viewmatrix[1]);
		VectorInverse(viewmatrix[1]);
		VectorCopy(m_vNormal, viewmatrix[2]);

		(*m_protationmatrix)[0][3] = modelpos[0] - m_vRenderOrigin[0];
		(*m_protationmatrix)[1][3] = modelpos[1] - m_vRenderOrigin[1];
		(*m_protationmatrix)[2][3] = modelpos[2] - m_vRenderOrigin[2];

		ConcatTransforms(viewmatrix, (*m_protationmatrix), (*m_paliastransform));

		// do the scaling up of x and y to screen coordinates as part of the transform
		// for the unclipped case (it would mess up clipping in the clipped case).
		// Also scale down z, so 1/z is scaled 31 bits for free, and scale down x and y
		// correspondingly so the projected x and y come out right
		// FIXME: make this work for clipped case too?
		if (trivial_accept)
		{
			for (i = 0; i < 4; i++)
			{
				(*m_paliastransform)[0][i] *= m_fSoftwareXScale *
											  (1.0 / (ZISCALE * 0x10000));
				(*m_paliastransform)[1][i] *= m_fSoftwareYScale *
											  (1.0 / (ZISCALE * 0x10000));
				(*m_paliastransform)[2][i] *= 1.0 / (ZISCALE * 0x10000);
			}
		}
	}

	(*m_protationmatrix)[0][3] = modelpos[0];
	(*m_protationmatrix)[1][3] = modelpos[1];
	(*m_protationmatrix)[2][3] = modelpos[2];
}


/*
====================
StudioEstimateInterpolant
====================
*/
float CMiniRenderer::StudioEstimateInterpolant()
{
	float dadt = 1.0;

	if (m_fDoInterp && (m_pCurrentEntity->curstate.animtime >= m_pCurrentEntity->latched.prevanimtime + 0.01))
	{
		dadt = (m_clTime - m_pCurrentEntity->curstate.animtime) / 0.1;
		if (dadt > 2.0)
		{
			dadt = 2.0;
		}
	}
	return dadt;
}

/*
====================
StudioCalcRotations
====================
*/
void CMiniRenderer::StudioCalcRotations(float pos[][3], vec4_t* q, mstudioseqdesc_t* pseqdesc, mstudioanim_t* panim, float f)
{
	int i;
	int frame;
	mstudiobone_t* pbone;

	float s;
	float adj[MAXSTUDIOCONTROLLERS];
	float dadt;

	if (f > pseqdesc->numframes - 1)
	{
		f = 0; // bah, fix this bug with changing sequences too fast
	}
	// BUG ( somewhere else ) but this code should validate this data.
	// This could cause a crash if the frame # is negative, so we'll go ahead
	//  and clamp it here
	else if (f < -0.01)
	{
		f = -0.01;
	}

	frame = (int)f;

	// Con_DPrintf("%d %.4f %.4f %.4f %.4f %d\n", m_pCurrentEntity->curstate.sequence, m_clTime, m_pCurrentEntity->animtime, m_pCurrentEntity->frame, f, frame );

	// Con_DPrintf( "%f %f %f\n", m_pCurrentEntity->angles[ROLL], m_pCurrentEntity->angles[PITCH], m_pCurrentEntity->angles[YAW] );

	// Con_DPrintf("frame %d %d\n", frame1, frame2 );


	dadt = StudioEstimateInterpolant();
	s = (f - frame);

	// add in programtic controllers
	pbone = (mstudiobone_t*)((byte*)m_pStudioHeader + m_pStudioHeader->boneindex);

	StudioCalcBoneAdj(dadt, adj, m_pCurrentEntity->curstate.controller, m_pCurrentEntity->latched.prevcontroller, m_pCurrentEntity->mouth.mouthopen);

	for (i = 0; i < m_pStudioHeader->numbones; i++, pbone++, panim++)
	{
		StudioCalcBoneQuaterion(frame, s, pbone, panim, adj, q[i], i);

		StudioCalcBonePosition(frame, s, pbone, panim, adj, pos[i]);
		// if (0 && i == 0)
		//	Con_DPrintf("%d %d %d %d\n", m_pCurrentEntity->curstate.sequence, frame, j, k );
	}

	if ((pseqdesc->motiontype & STUDIO_X) != 0)
	{
		pos[pseqdesc->motionbone][0] = 0.0;
	}
	if ((pseqdesc->motiontype & STUDIO_Y) != 0)
	{
		pos[pseqdesc->motionbone][1] = 0.0;
	}
	if ((pseqdesc->motiontype & STUDIO_Z) != 0)
	{
		pos[pseqdesc->motionbone][2] = 0.0;
	}

	s = 0 * ((1.0 - (f - (int)(f))) / (pseqdesc->numframes)) * m_pCurrentEntity->curstate.framerate;

	if ((pseqdesc->motiontype & STUDIO_LX) != 0)
	{
		pos[pseqdesc->motionbone][0] += s * pseqdesc->linearmovement[0];
	}
	if ((pseqdesc->motiontype & STUDIO_LY) != 0)
	{
		pos[pseqdesc->motionbone][1] += s * pseqdesc->linearmovement[1];
	}
	if ((pseqdesc->motiontype & STUDIO_LZ) != 0)
	{
		pos[pseqdesc->motionbone][2] += s * pseqdesc->linearmovement[2];
	}
}

/*
====================
Studio_FxTransform
====================
*/
void CMiniRenderer::StudioFxTransform(cl_entity_t* ent, float transform[3][4])
{
	switch (ent->curstate.renderfx)
	{
	case kRenderFxDistort:
	case kRenderFxHologram:
		if (gEngfuncs.pfnRandomLong(0, 49) == 0)
		{
			int axis = gEngfuncs.pfnRandomLong(0, 1);
			if (axis == 1) // Choose between x & z
				axis = 2;
			VectorScale(transform[axis], gEngfuncs.pfnRandomFloat(1, 1.484), transform[axis]);
		}
		else if (gEngfuncs.pfnRandomLong(0, 49) == 0)
		{
			float offset;
			int axis = gEngfuncs.pfnRandomLong(0, 1);
			if (axis == 1) // Choose between x & z
				axis = 2;
			offset = gEngfuncs.pfnRandomFloat(-10, 10);
			transform[gEngfuncs.pfnRandomLong(0, 2)][3] += offset;
		}
		break;
	case kRenderFxExplode:
	{
		float scale;

		scale = 1.0 + (m_clTime - ent->curstate.animtime) * 10.0;
		if (scale > 2) // Don't blow up more than 200%
			scale = 2;
		transform[0][1] *= scale;
		transform[1][1] *= scale;
		transform[2][1] *= scale;
	}
	break;
	}
}

/*
====================
StudioEstimateFrame
====================
*/
float CMiniRenderer::StudioEstimateFrame(mstudioseqdesc_t* pseqdesc)
{
	double dfdt, f;
	float flTime = m_clTime;
	float flAnimTime = m_pCurrentEntity->curstate.animtime;
	if (m_fDoInterp)
	{
		if (flTime < m_pCurrentEntity->curstate.animtime)
		{
			dfdt = 0;
		}
		else
		{
			dfdt = (flTime - m_pCurrentEntity->curstate.animtime) * m_pCurrentEntity->curstate.framerate * pseqdesc->fps;
		}
	}
	else
	{
		dfdt = 0;
	}

	if (pseqdesc->numframes <= 1)
	{
		f = 0;
	}
	else
	{
		f = (m_pCurrentEntity->curstate.frame * (pseqdesc->numframes - 1)) / 256.0;
	}

	f += dfdt;

	if ((pseqdesc->flags & STUDIO_LOOPING) != 0)
	{
		if (pseqdesc->numframes > 1)
		{
			f -= (int)(f / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
		}
		if (f < 0)
		{
			f += (pseqdesc->numframes - 1);
		}
	}
	else
	{
		if (f >= pseqdesc->numframes - 1.001)
		{
			f = pseqdesc->numframes - 1.001;
		}
		if (f < 0.0)
		{
			f = 0.0;
		}
	}

	return f;
}

/*
====================
StudioSetupBones
====================
*/
void CMiniRenderer::StudioSetupBones()
{
	int i;
	double f;

	mstudiobone_t* pbones;
	mstudioseqdesc_t* pseqdesc;
	mstudioanim_t* panim;

	static float pos[MAXSTUDIOBONES][3];
	static vec4_t q[MAXSTUDIOBONES];
	float bonematrix[3][4];

	static float pos2[MAXSTUDIOBONES][3];
	static vec4_t q2[MAXSTUDIOBONES];
	static float pos3[MAXSTUDIOBONES][3];
	static vec4_t q3[MAXSTUDIOBONES];
	static float pos4[MAXSTUDIOBONES][3];
	static vec4_t q4[MAXSTUDIOBONES];

	if (m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq)
	{
		m_pCurrentEntity->curstate.sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;

	// always want new gait sequences to start on frame zero
	/*	if ( m_pPlayerInfo )
	{
		int playerNum = m_pCurrentEntity->index - 1;
		// new jump gaitsequence?  start from frame zero
		if ( m_nPlayerGaitSequences[ playerNum ] != m_pPlayerInfo->gaitsequence )
		{
	//		m_pPlayerInfo->gaitframe = 0.0;
			gEngfuncs.Con_Printf( "Setting gaitframe to 0\n" );
		}
		m_nPlayerGaitSequences[ playerNum ] = m_pPlayerInfo->gaitsequence;
//		gEngfuncs.Con_Printf( "index: %d     gaitsequence: %d\n",playerNum, m_pPlayerInfo->gaitsequence);
	}
*/
	f = StudioEstimateFrame(pseqdesc);

	if (m_pCurrentEntity->latched.prevframe > f)
	{
		// Con_DPrintf("%f %f\n", m_pCurrentEntity->prevframe, f );
	}

	panim = StudioGetAnim(m_pRenderModel, pseqdesc);
	StudioCalcRotations(pos, q, pseqdesc, panim, f);

	if (pseqdesc->numblends > 1)
	{
		float s;
		float dadt;

		panim += m_pStudioHeader->numbones;
		StudioCalcRotations(pos2, q2, pseqdesc, panim, f);

		dadt = StudioEstimateInterpolant();
		s = (m_pCurrentEntity->curstate.blending[0] * dadt + m_pCurrentEntity->latched.prevblending[0] * (1.0 - dadt)) / 255.0;

		StudioSlerpBones(q, pos, q2, pos2, s);

		if (pseqdesc->numblends == 4)
		{
			panim += m_pStudioHeader->numbones;
			StudioCalcRotations(pos3, q3, pseqdesc, panim, f);

			panim += m_pStudioHeader->numbones;
			StudioCalcRotations(pos4, q4, pseqdesc, panim, f);

			s = (m_pCurrentEntity->curstate.blending[0] * dadt + m_pCurrentEntity->latched.prevblending[0] * (1.0 - dadt)) / 255.0;
			StudioSlerpBones(q3, pos3, q4, pos4, s);

			s = (m_pCurrentEntity->curstate.blending[1] * dadt + m_pCurrentEntity->latched.prevblending[1] * (1.0 - dadt)) / 255.0;
			StudioSlerpBones(q, pos, q3, pos3, s);
		}
	}

	if (m_fDoInterp &&
		0 != m_pCurrentEntity->latched.sequencetime &&
		(m_pCurrentEntity->latched.sequencetime + 0.2 > m_clTime) &&
		(m_pCurrentEntity->latched.prevsequence < m_pStudioHeader->numseq))
	{
		// blend from last sequence
		static float pos1b[MAXSTUDIOBONES][3];
		static vec4_t q1b[MAXSTUDIOBONES];
		float s;

		if (m_pCurrentEntity->latched.prevsequence >= m_pStudioHeader->numseq)
		{
			m_pCurrentEntity->latched.prevsequence = 0;
		}

		pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->latched.prevsequence;
		panim = StudioGetAnim(m_pRenderModel, pseqdesc);
		// clip prevframe
		StudioCalcRotations(pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);

		if (pseqdesc->numblends > 1)
		{
			panim += m_pStudioHeader->numbones;
			StudioCalcRotations(pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);

			s = (m_pCurrentEntity->latched.prevseqblending[0]) / 255.0;
			StudioSlerpBones(q1b, pos1b, q2, pos2, s);

			if (pseqdesc->numblends == 4)
			{
				panim += m_pStudioHeader->numbones;
				StudioCalcRotations(pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);

				panim += m_pStudioHeader->numbones;
				StudioCalcRotations(pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);

				s = (m_pCurrentEntity->latched.prevseqblending[0]) / 255.0;
				StudioSlerpBones(q3, pos3, q4, pos4, s);

				s = (m_pCurrentEntity->latched.prevseqblending[1]) / 255.0;
				StudioSlerpBones(q1b, pos1b, q3, pos3, s);
			}
		}

		s = 1.0 - (m_clTime - m_pCurrentEntity->latched.sequencetime) / 0.2;
		StudioSlerpBones(q, pos, q1b, pos1b, s);
	}
	else
	{
		// Con_DPrintf("prevframe = %4.2f\n", f);
		m_pCurrentEntity->latched.prevframe = f;
	}

	pbones = (mstudiobone_t*)((byte*)m_pStudioHeader + m_pStudioHeader->boneindex);

	// bounds checking
	if (m_pPlayerInfo)
	{
		if (m_pPlayerInfo->gaitsequence >= m_pStudioHeader->numseq)
		{
			m_pPlayerInfo->gaitsequence = 0;
		}
	}

	// calc gait animation
	if (m_pPlayerInfo && m_pPlayerInfo->gaitsequence != 0)
	{
		if (m_pPlayerInfo->gaitsequence >= m_pStudioHeader->numseq)
		{
			m_pPlayerInfo->gaitsequence = 0;
		}

		bool copy = true;

		pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pPlayerInfo->gaitsequence;

		panim = StudioGetAnim(m_pRenderModel, pseqdesc);
		StudioCalcRotations(pos2, q2, pseqdesc, panim, m_pPlayerInfo->gaitframe);

		for (i = 0; i < m_pStudioHeader->numbones; i++)
		{
			if (0 == strcmp(pbones[i].name, "Bip01 Spine"))
			{
				copy = false;
			}
			else if (0 == strcmp(pbones[pbones[i].parent].name, "Bip01 Pelvis"))
			{
				copy = true;
			}

			if (copy)
			{
				memcpy(pos[i], pos2[i], sizeof(pos[i]));
				memcpy(q[i], q2[i], sizeof(q[i]));
			}
		}
	}

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		QuaternionMatrix(q[i], bonematrix);

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1)
		{
			if (0 != IEngineStudio.IsHardware())
			{
				ConcatTransforms((*m_protationmatrix), bonematrix, (*m_pbonetransform)[i]);

				// MatrixCopy should be faster...
				// ConcatTransforms ((*m_protationmatrix), bonematrix, (*m_plighttransform)[i]);
				MatrixCopy((*m_pbonetransform)[i], (*m_plighttransform)[i]);
			}
			else
			{
				ConcatTransforms((*m_paliastransform), bonematrix, (*m_pbonetransform)[i]);
				ConcatTransforms((*m_protationmatrix), bonematrix, (*m_plighttransform)[i]);
			}

			// Apply client-side effects to the transformation matrix
			StudioFxTransform(m_pCurrentEntity, (*m_pbonetransform)[i]);
		}
		else
		{
			ConcatTransforms((*m_pbonetransform)[pbones[i].parent], bonematrix, (*m_pbonetransform)[i]);
			ConcatTransforms((*m_plighttransform)[pbones[i].parent], bonematrix, (*m_plighttransform)[i]);
		}

		if (pCurrentInfo && (m_pCurrentEntity == gEngfuncs.GetViewModel() || m_pCurrentEntity == &view))
		{
			CalcCamAngles(&bonematrix, i);
		}
	}
}


/*
====================
StudioSaveBones
====================
*/
void CMiniRenderer::StudioSaveBones()
{
	int i;

	mstudiobone_t* pbones;
	pbones = (mstudiobone_t*)((byte*)m_pStudioHeader + m_pStudioHeader->boneindex);

	m_nCachedBones = m_pStudioHeader->numbones;

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		strcpy_s(m_nCachedBoneNames[i], pbones[i].name);
		MatrixCopy((*m_pbonetransform)[i], m_rgCachedBoneTransform[i]);
		MatrixCopy((*m_plighttransform)[i], m_rgCachedLightTransform[i]);
	}
}


/*
====================
StudioMergeBones
====================
*/
void CMiniRenderer::StudioMergeBones(model_t* m_pSubModel)
{
	int i, j;
	double f;

	mstudiobone_t* pbones;
	mstudioseqdesc_t* pseqdesc;
	mstudioanim_t* panim;

	static float pos[MAXSTUDIOBONES][3];
	float bonematrix[3][4];
	static vec4_t q[MAXSTUDIOBONES];

	if (m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq)
	{
		m_pCurrentEntity->curstate.sequence = 0;
	}

	pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;

	f = StudioEstimateFrame(pseqdesc);

	if (m_pCurrentEntity->latched.prevframe > f)
	{
		// Con_DPrintf("%f %f\n", m_pCurrentEntity->prevframe, f );
	}

	panim = StudioGetAnim(m_pSubModel, pseqdesc);
	StudioCalcRotations(pos, q, pseqdesc, panim, f);

	pbones = (mstudiobone_t*)((byte*)m_pStudioHeader + m_pStudioHeader->boneindex);


	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		for (j = 0; j < m_nCachedBones; j++)
		{
			if (stricmp(pbones[i].name, m_nCachedBoneNames[j]) == 0)
			{
				MatrixCopy(m_rgCachedBoneTransform[j], (*m_pbonetransform)[i]);
				MatrixCopy(m_rgCachedLightTransform[j], (*m_plighttransform)[i]);
				break;
			}
		}
		if (j >= m_nCachedBones)
		{
			QuaternionMatrix(q[i], bonematrix);

			bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];

			if (pbones[i].parent == -1)
			{
				if (0 != IEngineStudio.IsHardware())
				{
					ConcatTransforms((*m_protationmatrix), bonematrix, (*m_pbonetransform)[i]);

					// MatrixCopy should be faster...
					// ConcatTransforms ((*m_protationmatrix), bonematrix, (*m_plighttransform)[i]);
					MatrixCopy((*m_pbonetransform)[i], (*m_plighttransform)[i]);
				}
				else
				{
					ConcatTransforms((*m_paliastransform), bonematrix, (*m_pbonetransform)[i]);
					ConcatTransforms((*m_protationmatrix), bonematrix, (*m_plighttransform)[i]);
				}

				// Apply client-side effects to the transformation matrix
				StudioFxTransform(m_pCurrentEntity, (*m_pbonetransform)[i]);
			}
			else
			{
				ConcatTransforms((*m_pbonetransform)[pbones[i].parent], bonematrix, (*m_pbonetransform)[i]);
				ConcatTransforms((*m_plighttransform)[pbones[i].parent], bonematrix, (*m_plighttransform)[i]);
			}
		}
	}
}

/*
====================
StudioDrawModel
====================
*/
bool CMiniRenderer::StudioDrawModel(int flags)
{
	alight_t lighting;
	Vector dir;

	m_pCurrentEntity = gEngfuncs.GetViewModel();
	if (!m_pCurrentEntity || !m_pCurrentEntity->model)
		return false;

	m_pRenderModel = m_pCurrentEntity->model;

	IEngineStudio.GetTimes(&m_nFrameCount, &m_clTime, &m_clOldTime);
	IEngineStudio.GetViewInfo(m_vRenderOrigin, m_vUp, m_vRight, m_vNormal);
	IEngineStudio.GetAliasScale(&m_fSoftwareXScale, &m_fSoftwareYScale);

	CalcBaseAngles();

	m_pStudioHeader = (studiohdr_t*)IEngineStudio.Mod_Extradata(m_pRenderModel);
	IEngineStudio.StudioSetHeader(m_pStudioHeader);
	IEngineStudio.SetRenderModel(m_pRenderModel);

	StudioSetUpTransform(false);

	StudioSetupBones();
	
	StudioSaveBones();

	return true;
}

bool CMiniRenderer::GetCurrentInfo()
{
	if (pCurrentInfo && stricmp(pCurrentInfo->name, m_pRenderModel->name) == 0)
		return false;

	for (auto & x : nl_viewmodelinfo)
	{
		if (stricmp(x.first.c_str() + 7, m_pRenderModel->name + 7) == 0)
		{
			pCurrentInfo = &x.second;
			break;
		}
	}

	if (pCurrentInfo == nullptr)
	{
		return false;
	}

	return true;
}

void CMiniRenderer::CalcBaseAngles()
{
	if (m_pCurrentEntity != gEngfuncs.GetViewModel())
		return;

	if (!GetCurrentInfo())
		return;

	Vector dir;

	m_pCurrentEntity = &view;

	m_pCurrentEntity->curstate.sequence = 0;
	m_pCurrentEntity->curstate.animtime = m_clTime;
	m_pCurrentEntity->curstate.frame = 0;
	m_pCurrentEntity->index = -666;

	m_pStudioHeader = (studiohdr_t*)IEngineStudio.Mod_Extradata(m_pRenderModel);
	IEngineStudio.StudioSetHeader(m_pStudioHeader);
	IEngineStudio.SetRenderModel(m_pRenderModel);

	StudioSetUpTransform(false);
	StudioSetupBones();
	StudioSaveBones();

	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();
	m_pRenderModel = m_pCurrentEntity->model;
}

void CMiniRenderer::CalcCamAngles(float (*transform)[3][4], int index)
{
	if (!gEngfuncs.GetViewModel()->model)
	{
		pCurrentInfo = nullptr;
		nl_CamPos = nl_CamAngles = g_vecZero;
	}

	static Vector baseangle;
	if (!pCurrentInfo)
	{
		return;
	}
	if (index == pCurrentInfo->iCameraBone)
	{
		if (m_pCurrentEntity == &view)
		{
			Matrix3x4_AnglesFromMatrix(*transform, baseangle);
		}
		else if (m_pCurrentEntity == gEngfuncs.GetViewModel())
		{
			Matrix3x4_AnglesFromMatrix(*transform, nl_CamAngles);
			nl_CamAngles = (nl_CamAngles - baseangle) * pCurrentInfo->flAnimScale;
		}
	}
}