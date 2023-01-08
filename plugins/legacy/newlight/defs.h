#pragma once

extern "C" {
void DLLEXPORT InitializePlugin(cl_enginefunc_t* pEnginefuncs, dllfuncs_t* clfuncs, int iVersion);

void DLLEXPORT V_CalcRefdef_Post(struct ref_params_s* pparams);
void DLLEXPORT V_CalcRefdef_Pre(struct ref_params_s* pparams);
};