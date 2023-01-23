#include "defs.h"

#include "studio.h"
#include "com_model.h"
#include "r_studioint.h"

#include "pmtrace.h"
#include "pm_defs.h"
#include "event_api.h"

extern engine_studio_api_s IOriginalEngineStudio;

int g_iRenderMode = 0;

cl_entity_s* m_pCurrentEntity;
mstudiomodel_t* m_pSubModel;
mstudiobodyparts_t* m_pBodyPart;
studiohdr_t* m_pStudioHeader;

Vector verts[MAXSTUDIOVERTS];

float (*m_pbonetransform)[MAXSTUDIOBONES][3][4];
typedef vec_t matrix3x4[3][4];

float flShadowZ = 0.0f;

cvar_t* r_shadows;

/*
===============
GL_SetDefaults
===============
*/
static void GL_SetDefaults(void)
{
	glFinish();

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glDepthFunc(GL_LEQUAL);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glDisable(GL_STENCIL_TEST);
	glStencilMask(0xFF);
	glStencilFunc(GL_EQUAL, 0, ~0);
	glStencilOp(GL_KEEP, GL_INCR, GL_INCR);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPolygonOffset(-1.0f, -2.0f);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glAlphaFunc(GL_GREATER, 0.0f);
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glFrontFace(GL_CCW);

	glPointSize(1.2f);
	glLineWidth(1.2f);

	glDisable(GL_CULL_FACE);
}


void SetupMatrix()
{
	r_shadows = gEngfuncs.pfnRegisterVariable("r_engine_shadows", "1", FCVAR_ARCHIVE);

	m_pbonetransform = (float(*)[MAXSTUDIOBONES][3][4])IOriginalEngineStudio.StudioGetBoneTransform();

	GL_SetDefaults();
}

void HOOKED_StudioSetHeader(void* header)
{
	m_pCurrentEntity = IOriginalEngineStudio.GetCurrentEntity();
	m_pStudioHeader = (studiohdr_t*)header;

	pmtrace_t tr;
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);

	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(m_pCurrentEntity->origin, m_pCurrentEntity->origin - Vector(0, 0, 8192), PM_WORLD_ONLY | PM_STUDIO_BOX, -1, &tr);

	flShadowZ = tr.endpos[2];

	gEngfuncs.pEventAPI->EV_PopPMStates();

	IOriginalEngineStudio.StudioSetHeader(header);
}

/*
===============
R_StudioDrawPointsShadow

===============
*/
static void R_StudioDrawPointsShadow(void)
{
	float *av, height;
	float vec_x, vec_y;
	mstudiomesh_t* pmesh;
	Vector point;
	int i, k;



	glEnable(GL_STENCIL_TEST);

	// magic nipples - no more shadows from lightsources because it looks bad
	height = 0.1f;
	vec_y = 0.75f;
	vec_x = 0.0f;

	for (k = 0; k < m_pSubModel->nummesh; k++)
	{
		short* ptricmds;

		pmesh = (mstudiomesh_t*)((byte*)m_pStudioHeader + m_pSubModel->meshindex) + k;
		ptricmds = (short*)((byte*)m_pStudioHeader + pmesh->triindex);

		while (i = *(ptricmds++))
		{
			if (i < 0)
			{
				glBegin(GL_TRIANGLE_FAN);
				i = -i;
			}
			else
			{
				glBegin(GL_TRIANGLE_STRIP);
			}


			for (; i > 0; i--, ptricmds += 4)
			{
				av = verts[ptricmds[0]];
				point[0] = av[0] - (vec_x * (av[2] - flShadowZ));
				point[1] = av[1] - (vec_y * (av[2] - flShadowZ));
				point[2] = flShadowZ + height + 0.15f;

				glVertex3fv(point);
			}

			glEnd();
		}
	}

	glDisable(GL_STENCIL_TEST);
}

void SetupModel()
{
	glDepthMask(GL_TRUE);

	if (g_iRenderMode != kRenderTransAdd)
	{
		float color = 1.0 - 0.5;

		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f - color);

		glDepthFunc(GL_LESS);
		R_StudioDrawPointsShadow();
		glDepthFunc(GL_LEQUAL);

		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glShadeModel(GL_SMOOTH);
	}
}

void HOOKED_GL_SetRenderMode(int mode)
{
	g_iRenderMode = mode;
	IOriginalEngineStudio.GL_SetRenderMode(mode);
}

void Matrix3x4_VectorTransform(const matrix3x4 in, const float v[3], float out[3])
{
	out[0] = v[0] * in[0][0] + v[1] * in[0][1] + v[2] * in[0][2] + in[0][3];
	out[1] = v[0] * in[1][0] + v[1] * in[1][1] + v[2] * in[1][2] + in[1][3];
	out[2] = v[0] * in[2][0] + v[1] * in[2][1] + v[2] * in[2][2] + in[2][3];
}

void R_StudioDrawPoints(void)
{
	byte* pvertbone;
	Vector* pstudioverts;

	if (!m_pStudioHeader)
		return;

	pvertbone = ((byte*)m_pStudioHeader + m_pSubModel->vertinfoindex);
	pstudioverts = (Vector*)((byte*)m_pStudioHeader + m_pSubModel->vertindex);

	for (int i = 0; i < m_pSubModel->numverts; i++)
	{
		Matrix3x4_VectorTransform((*m_pbonetransform)[pvertbone[i]], pstudioverts[i], verts[i]);
	}
}

void HOOKED_StudioSetupModel(int bodypart, void** ppbodypart, void** ppsubmodel)
{
	IOriginalEngineStudio.StudioSetupModel(bodypart, ppbodypart, ppsubmodel);

	int index;

	if (bodypart > m_pStudioHeader->numbodyparts)
		bodypart = 0;

	m_pBodyPart = (mstudiobodyparts_t*)((byte*)m_pStudioHeader + m_pStudioHeader->bodypartindex) + bodypart;

	index = m_pCurrentEntity->curstate.body / m_pBodyPart->base;
	index = index % m_pBodyPart->nummodels;

	m_pSubModel = (mstudiomodel_t*)((byte*)m_pStudioHeader + m_pBodyPart->modelindex) + index;

	if (ppbodypart)
		*ppbodypart = m_pBodyPart;
	if (ppsubmodel)
		*ppsubmodel = m_pSubModel;

	R_StudioDrawPoints();
}

void HOOKED_GL_StudioDrawShadow()
{
	if (m_pCurrentEntity == gEngfuncs.GetViewModel() || r_shadows->value == 0.0f)
		return;

	SetupModel();
}