// Example plugin : HL2 style viewbob and viewlag

#include "PlatformHeaders.h"
#include "Platform.h"
#include "mathlib.h"
#include "vector.h"
#include "cdll_int.h"
#include "../cl_hook/funcptrs.h"
#include "Exports.h"
#include "defs.h"

#include "cvardef.h"

cl_enginefunc_t gEngfuncs;
dllfuncs_t gClientFuncs;

cvar_t *cl_bobcycle, *cl_bob, *cl_bobup;
cvar_t* cl_bobnew;
cvar_t* cl_bobmode;

double bobTimes[3]{0.0f};
float lastTimes[3]{0.0f};
Vector m_vecLastFacing;

void DLLEXPORT InitializePlugin(cl_enginefunc_t* pEnginefuncs, dllfuncs_t* clfuncs, int iVersion)
{
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));
	memcpy(&gClientFuncs, clfuncs, sizeof(cl_enginefunc_t));
}

float m_flWeaponLag = 1.5f;

void V_CalcViewModelLag(ref_params_t* pparams, Vector& origin, Vector angles, Vector original_angles)
{
	Vector vOriginalOrigin = origin;
	Vector vOriginalAngles = angles;

	// Calculate our drift
	Vector forward, right, up;
	AngleVectors(angles, &forward, &right, &up);

	if (pparams->frametime != 0.0f) // not in paused
	{
		Vector vDifference;

		vDifference = forward - m_vecLastFacing;

		float flSpeed = 5.0f;

		// If we start to lag too far behind, we'll increase the "catch up" speed.
		// Solves the problem with fast cl_yawspeed, m_yaw or joysticks rotating quickly.
		// The old code would slam lastfacing with origin causing the viewmodel to pop to a new position
		float flDiff = vDifference.Length();
		if ((flDiff > m_flWeaponLag) && (m_flWeaponLag > 0.0f))
		{
			float flScale = flDiff / m_flWeaponLag;
			flSpeed *= flScale;
		}

		// FIXME:  Needs to be predictable?
		m_vecLastFacing = m_vecLastFacing + vDifference * (flSpeed * pparams->frametime);
		// Make sure it doesn't grow out of control!!!
		m_vecLastFacing = m_vecLastFacing.Normalize();
		origin = origin + (vDifference * -1.0f) * 5.0f;
	}

	AngleVectors(original_angles, &forward, &right, &up);

	float pitch = original_angles[0];

	if (pitch > 180.0f)
	{
		pitch -= 360.0f;
	}
	else if (pitch < -180.0f)
	{
		pitch += 360.0f;
	}
}

enum class CalcBobMode
{
	VB_COS,
	VB_SIN,
	VB_COS2,
	VB_SIN2
};

// Quakeworld bob code, this fixes jitters in the mutliplayer since the clock (pparams->time) isn't quite linear
void V_CalcBob(struct ref_params_s* pparams, float frequencyMultiplier, const CalcBobMode& mode, double& bobtime, float& bob, float& lasttime)
{
	float cycle;
	Vector vel;

	if (pparams->onground == -1 ||
		pparams->time == lasttime)
	{
		// just use old value
		return;
	}

	lasttime = pparams->time;

	bobtime += pparams->frametime * frequencyMultiplier;
	cycle = bobtime - (int)(bobtime / cl_bobcycle->value) * cl_bobcycle->value;
	cycle /= cl_bobcycle->value;

	if (cycle < cl_bobup->value)
	{
		cycle = M_PI * cycle / cl_bobup->value;
	}
	else
	{
		cycle = M_PI + M_PI * (cycle - cl_bobup->value) / (1.0 - cl_bobup->value);
	}

	// bob is proportional to simulated velocity in the xy plane
	// (don't count Z, or jumping messes it up)
	VectorCopy(pparams->simvel, vel);
	vel[2] = 0;

	bob = sqrt(vel[0] * vel[0] + vel[1] * vel[1]) * 0.01;

	if (mode == CalcBobMode::VB_SIN)
		bob = bob * 0.3 + bob * 0.7 * sin(cycle);
	else if (mode == CalcBobMode::VB_COS)
		bob = bob * 0.3 + bob * 0.7 * cos(cycle);
	else if (mode == CalcBobMode::VB_SIN2)
		bob = bob * 0.3 + bob * 0.7 * sin(cycle) * sin(cycle);
	else if (mode == CalcBobMode::VB_COS2)
		bob = bob * 0.3 + bob * 0.7 * cos(cycle) * cos(cycle);

	bob = V_min(bob, 4);
	bob = V_max(bob, -7);
}


void DLLEXPORT V_CalcRefdef_Pre(struct ref_params_s* pparams)
{
	if (!cl_bobcycle)
	{
		cl_bobcycle = gEngfuncs.pfnGetCvarPointer("cl_bobcycle");
		cl_bob = gEngfuncs.pfnGetCvarPointer("cl_bob");
		cl_bobup = gEngfuncs.pfnGetCvarPointer("cl_bobup");
		cl_bobnew = gEngfuncs.pfnRegisterVariable("cl_bobnew", "0.01", FCVAR_ARCHIVE);
		cl_bobmode = gEngfuncs.pfnRegisterVariable("cl_bobmode", "0", FCVAR_ARCHIVE);
	}
}


void V_PerformFuncs(struct ref_params_s* pparams)
{
	if (pparams->paused != 0)
		return;

	float bobRight = 0, bobUp = 0, bobForward = 0;

	cl_entity_t* view = gEngfuncs.GetViewModel();

	// transform the view offset by the model's matrix to get the offset from
	// model origin for the view

	AngleVectors(pparams->cl_viewangles, (Vector*)&pparams->forward, (Vector*)&pparams->right, (Vector*)&pparams->up);

	// transform the view offset by the model's matrix to get the offset from
	// model origin for the view
	V_CalcBob(pparams, 0.75f, CalcBobMode::VB_SIN, bobTimes[0], bobRight, lastTimes[0]);
	V_CalcBob(pparams, 1.50f, CalcBobMode::VB_SIN, bobTimes[1], bobUp, lastTimes[1]);
	V_CalcBob(pparams, 1.00f, CalcBobMode::VB_SIN, bobTimes[2], bobForward, lastTimes[2]);

	V_CalcViewModelLag(pparams, view->origin, pparams->viewangles, pparams->cl_viewangles);

	for (int i = 0; i < 3; i++)
	{
		view->origin[i] += bobRight * 0.33 * pparams->right[i];
		view->origin[i] += bobUp * 0.17 * pparams->up[i];
	}

	cl_bob->string = "0.0";
	cl_bob->value = 0.0f;
}

void DLLEXPORT V_CalcRefdef_Post(struct ref_params_s* pparams)
{
	V_PerformFuncs(pparams);
}