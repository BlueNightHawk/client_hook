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
#include "pmtrace.h"
#include "event_api.h"
#include "pm_defs.h"

#include <algorithm>
#include <cmath>

cl_enginefunc_t gEngfuncs;
cl_enginefunc_t gOriginalEngfuncs;
dllfuncs_t gClientFuncs;
