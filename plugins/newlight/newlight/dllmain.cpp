// dllmain.cpp : Defines the entry point for the DLL application.
#define NOHOOK
#define NOGL

#include "defs.h"

#include "event_api.h"
#include "pmtrace.h"
#include "pm_defs.h"

DEFINE_MAIN;

cl_enginefunc_t gOriginalEngfuncs;

cvar_t *cl_bobcycle, *cl_bob, *cl_bobup;
cvar_t* cl_bobnew;
cvar_t* cl_bobmode;

double bobTimes[3]{0.0f};
float lastTimes[3]{0.0f};
Vector m_vecLastFacing;

Vector cl_jumpangle;
Vector cl_jumppunch;

cvar_t* cl_rollangle = nullptr;
cvar_t* cl_rollspeed = nullptr;

cvar_t* cl_viewroll = nullptr;
cvar_t* cl_weaponbob = nullptr;
cvar_t* cl_jumpanim = nullptr;
cvar_t* cl_weaponlag = nullptr;
cvar_t* cl_retractweapon = nullptr;

static bool g_bJumpState = false;

#define InvPitch(a) Vector(-a[0], a[1], a[2])

void (*pIN_JumpDown)();

PLUGINFUNCS
{
	void DLLEXPORT InitializePlugin(cl_enginefunc_t * pEnginefuncs, struct dllfuncs_s * clfuncs, int iVersion);
	void DLLEXPORT V_CalcRefdef_Pre(struct ref_params_s * pparams);
	void DLLEXPORT V_CalcRefdef_Post(struct ref_params_s * pparams);
}
	

void IN_JumpDown()
{
	if (!g_bJumpState)
	{
		cl_jumppunch = Vector(-2.5, 2.5, 0) * 20;
		g_bJumpState = true;
	}
	pIN_JumpDown();
}

int AddCommand(const char* cmd_name, void (*pfnEngSrc_function)(void))
{
	if (!stricmp(cmd_name, "+jump"))
	{
		pIN_JumpDown = pfnEngSrc_function;
		return gOriginalEngfuncs.pfnAddCommand(cmd_name, &IN_JumpDown);
	}
	return gOriginalEngfuncs.pfnAddCommand(cmd_name, pfnEngSrc_function);
}

void DLLEXPORT InitializePlugin(cl_enginefunc_t* pEnginefuncs, struct dllfuncs_s* clfuncs, int iVersion)
{
	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));
	memcpy(&gOriginalEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	pEnginefuncs->pfnAddCommand = &AddCommand;
}

void V_Jump(ref_params_s* pparams, cl_entity_t* view)
{
	static float flFallVel = 0.0f;
	static float l_FallVel = 0.0f;

	if (pparams->waterlevel != 0)
	{
		flFallVel = 0.0f;
		l_FallVel = std::lerp(l_FallVel, 0.0f, pparams->frametime * 25.0f);
		g_bJumpState = false;
	}
	else if (pparams->onground <= 0)
	{
		flFallVel = -pparams->simvel[2];

		l_FallVel = std::lerp(l_FallVel, flFallVel, pparams->frametime * 5.0f);
		l_FallVel = std::clamp(l_FallVel, -50.0f, 500.0f);
	}
	else
	{
		l_FallVel = std::lerp(l_FallVel, 0, pparams->frametime * 25.0f);
	}

	if (g_bJumpState && pparams->onground != 0 && pparams->waterlevel == 0)
	{
		cl_jumppunch = Vector(flFallVel * -0.01f, flFallVel * 0.01f, 0) * 20.0f;

		flFallVel = 0;
		g_bJumpState = false;
	}
	else if (!g_bJumpState && pparams->onground <= 0)
	{
		g_bJumpState = true;
	}

	float angle = V_min(l_FallVel * 0.05f, 20);
	view->curstate.angles[0] += angle;
	for (int i = 0; i < 3; i++)
	{
		pparams->viewangles[i] += gEngfuncs.pfnRandomFloat(-0.0055, 0.0055) * flFallVel * 0.05f;
		view->curstate.angles[i] += gEngfuncs.pfnRandomFloat(-0.0055, 0.0055) * flFallVel * 0.05f;
	}
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
		origin = origin + (vDifference * -1.0f) * cl_weaponlag->value;
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

		if (!(cl_rollangle = gEngfuncs.pfnGetCvarPointer("cl_rollangle")))
			cl_rollangle = gEngfuncs.pfnRegisterVariable("cl_rollangle", "2.0", FCVAR_ARCHIVE);

		if (!(cl_rollspeed = gEngfuncs.pfnGetCvarPointer("cl_rollspeed")))
			cl_rollspeed = gEngfuncs.pfnRegisterVariable("cl_rollspeed", "200", FCVAR_ARCHIVE);

		cl_bobnew = gEngfuncs.pfnRegisterVariable("cl_bobnew", "0.01", FCVAR_ARCHIVE);
		cl_bobmode = gEngfuncs.pfnRegisterVariable("cl_bobmode", "0", FCVAR_ARCHIVE);

		cl_viewroll = gEngfuncs.pfnRegisterVariable("cl_viewroll", "1", FCVAR_ARCHIVE);
		cl_weaponbob = gEngfuncs.pfnRegisterVariable("cl_weaponbob", "1", FCVAR_ARCHIVE);
		cl_jumpanim = gEngfuncs.pfnRegisterVariable("cl_jumpanim", "1", FCVAR_ARCHIVE);
		cl_weaponlag = gEngfuncs.pfnRegisterVariable("cl_weaponlag", "5", FCVAR_ARCHIVE);

		cl_retractweapon = gEngfuncs.pfnRegisterVariable("cl_retractweapon", "1", FCVAR_ARCHIVE);
	}
}

/*
=============
PLut Client Punch From HL2
=============
*/

#define PUNCH_DAMPING 6.0f // bigger number makes the response more damped, smaller is less damped

// currently the system will overshoot, with larger damping values it won't
#define PUNCH_SPRING_CONSTANT 65.0f // bigger number increases the speed at which the view corrects


void V_PunchAngle(float* ev_punchangle, float frametime, float* punch)
{
	float damping;
	float springForceMagnitude;

	if (Length(ev_punchangle) > 0.001 || Length(punch) > 0.001)
	{
		VectorMA(ev_punchangle, frametime, punch, ev_punchangle);
		damping = 1 - (PUNCH_DAMPING * frametime);

		if (damping < 0)
		{
			damping = 0;
		}

		VectorScale(punch, damping, punch);

		// torsional spring

		// UNDONE: Per-axis spring constant?

		springForceMagnitude = PUNCH_SPRING_CONSTANT * frametime;

		springForceMagnitude = std::clamp(springForceMagnitude, 0.0f, 2.0f);
		VectorMA(punch, -springForceMagnitude, ev_punchangle, punch);

		// don't wrap around
		ev_punchangle[0] = std::clamp(ev_punchangle[0], -7.0f, 7.0f);
		ev_punchangle[1] = std::clamp(ev_punchangle[1], -179.0f, 179.0f);
		ev_punchangle[2] = std::clamp(ev_punchangle[2], -7.0f, 7.0f);
	}
}

/*
===============
V_CalcAngle
Used by view and sv_user
===============
*/
float V_CalcAngle(Vector angles, Vector velocity, float rollangle, float rollspeed, int dir)
{
	float sign;
	float side;
	float value;
	Vector forward, right, up;

	AngleVectors(angles, &forward, &right, &up);

	switch (dir)
	{
	default:
	case 0:
		side = DotProduct(velocity, right);
		break;
	case 1:
		side = DotProduct(velocity, forward);
		break;
	case 2:
		side = DotProduct(velocity, up);
		break;
	}

	sign = side < 0 ? -1 : 1;
	side = fabs(side);

	value = rollangle;
	if (side < rollspeed)
	{
		side = side * value / rollspeed;
	}
	else
	{
		side = value;
	}
	return side * sign;
}

/*
==============
V_CalcViewRoll
Roll is induced by movement and damage
==============
*/
void V_CalcViewRoll(struct ref_params_s* pparams)
{
	float side;
	cl_entity_t* viewentity;

	viewentity = gEngfuncs.GetEntityByIndex(pparams->viewentity);
	if (!viewentity)
		return;

	side = V_CalcAngle(viewentity->angles, pparams->simvel, cl_rollangle->value, cl_rollspeed->value, 0);

	pparams->viewangles[2] += side;
}

void V_RetractWeapon(struct ref_params_s* pparams, cl_entity_s* view)
{
	static float lerpedfraction = 0.0f;

	pmtrace_t tr;
	Vector vecSrc, vecEnd;
	vecSrc = pparams->vieworg;
	vecEnd = vecSrc + Vector(pparams->forward) * 50;

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(0, 1);

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(pparams->viewentity - 1);

	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_NORMAL, -1, &tr);

	gEngfuncs.pEventAPI->EV_PopPMStates();

	lerpedfraction = std::lerp(lerpedfraction, (1 - tr.fraction) * 12.0f, pparams->frametime * 15.0f);

	view->curstate.angles[0] -= lerpedfraction;
	view->curstate.angles[1] += lerpedfraction;
}

extern Vector nl_CamPos, nl_CamAngles;

void V_PerformFuncs(struct ref_params_s* pparams)
{
	if (pparams->paused != 0)
		return;

	float bobRight = 0, bobUp = 0, bobForward = 0;

	cl_entity_t* view = gEngfuncs.GetViewModel();

	// transform the view offset by the model's matrix to get the offset from
	// model origin for the view
	Vector angles;
	gEngfuncs.GetViewAngles((float*)&angles);

	AngleVectors(angles, (Vector*)&pparams->forward, (Vector*)&pparams->right, (Vector*)&pparams->up);

	if (cl_viewroll->value != 0.0f)
		V_CalcViewRoll(pparams);

	// transform the view offset by the model's matrix to get the offset from
	// model origin for the view
	if (cl_weaponbob->value != 0.0f)
	{
		V_CalcBob(pparams, 0.75f, CalcBobMode::VB_SIN, bobTimes[0], bobRight, lastTimes[0]);
		V_CalcBob(pparams, 1.50f, CalcBobMode::VB_SIN, bobTimes[1], bobUp, lastTimes[1]);
		V_CalcBob(pparams, 1.00f, CalcBobMode::VB_SIN, bobTimes[2], bobForward, lastTimes[2]);
	}
	if (cl_weaponlag->value != 0.0f)
		V_CalcViewModelLag(pparams, view->origin, angles, angles);

	V_PunchAngle(cl_jumpangle, pparams->frametime, cl_jumppunch);

	if (cl_jumpanim->value != 0.0f)
		V_Jump(pparams, view);

	if (cl_retractweapon->value != 0.0f)
		V_RetractWeapon(pparams, view);

	VectorAdd(pparams->viewangles, InvPitch(cl_jumpangle) / 3.0f, pparams->viewangles);
	VectorAdd(view->curstate.angles, cl_jumpangle, view->curstate.angles);

	VectorAdd(pparams->viewangles, nl_CamAngles, pparams->viewangles);

	for (int i = 0; i < 3; i++)
	{
		view->origin[i] += bobRight * 0.23 * pparams->right[i];
		view->origin[i] += bobUp * 0.17 * pparams->up[i];
	}

	cl_bob->string = "0.0";
	cl_bob->value = 0.0f;
}

void DLLEXPORT V_CalcRefdef_Post(struct ref_params_s* pparams)
{
	V_PerformFuncs(pparams);
}