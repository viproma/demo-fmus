/* Copyright 2015-2017, SINTEF Ocean.
   Distributed under the 3-Clause BSD License.
   (See accompanying file LICENCE.txt or copy at
   https://raw.githubusercontent.com/viproma/demo-fmus/master/LICENCE.txt.) */

/*
An FMU with one input and one output of each data type (real, integer, boolean
and string). The value of each output is always identical to the value of the
corresponding input.
*/
#include <math.h>
#include <string.h>
#include <fmiFunctions.h>
#include "identity-fmu-uuid.h"


#ifdef _MSC_VER
#   define FUNCTION_NAME __FUNCTION__
#else
#   define FUNCTION_NAME __func__
#endif

#define FUNCTION_NOT_IMPLEMENTED \
    ((Instance*) c)->functions.logger(c, ((Instance*) c)->instanceName, fmiFatal, "", "Function not implemented: %s", FUNCTION_NAME); \
    return fmiFatal;


enum VarRef
{
    REAL_VAR    = 0,
    INTEGER_VAR = 0,
    BOOLEAN_VAR = 0,
    STRING_VAR  = 0,

    VAR_COUNT
};


typedef struct
{
    // Variables
    fmiReal real;
    fmiInteger integer;
    fmiBoolean boolean;
    char* string;

    // From fmiInstantiateSlave:
    char* instanceName;
    fmiCallbackFunctions functions;
    fmiBoolean loggingOn;

    // From fmiInitializeSlave:
    fmiReal tStart;
    fmiBoolean stopTimeDefined;
    fmiReal tStop;
} Instance;


char* CloneString(const char* source, void* (*allocate)(size_t n, size_t sz))
{
    char* clone = (char*) allocate(strlen(source) + 1, sizeof(char));
    strcpy(clone, source);
    return clone;
}


void ResetVars(Instance* vc)
{
    // Zero all variables
    vc->real = 0.0;
    vc->integer = 0;
    vc->boolean = 0;
    if (vc->string) vc->functions.freeMemory(vc->string);
    vc->string = CloneString(" ", vc->functions.allocateMemory);
}


DllExport const char* fmiGetTypesPlatform() { return fmiPlatform; }


DllExport const char* fmiGetVersion() { return fmiVersion; }


DllExport fmiStatus fmiSetDebugLogging(fmiComponent c, fmiBoolean loggingOn)
{
    ((Instance*) c)->loggingOn = loggingOn;
    return fmiOK;
}


DllExport fmiStatus fmiGetReal(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal    value[])
{
    fmiStatus status = fmiOK;
    size_t i = 0;
    for (i = 0; i < nvr; ++i) {
        if (vr[i] == REAL_VAR) {
            value[i] = ((Instance*) c)->real;
        } else {
            status = fmiWarning;
        }
    }
    return status;
}

DllExport fmiStatus fmiGetInteger(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[])
{
    fmiStatus status = fmiOK;
     size_t i = 0;
   for (i = 0; i < nvr; ++i) {
        if (vr[i] == INTEGER_VAR) {
            value[i] = ((Instance*) c)->integer;
        } else {
            status = fmiWarning;
        }
    }
    return status;
}

DllExport fmiStatus fmiGetBoolean(fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[])
{
    fmiStatus status = fmiOK;
    size_t i = 0;
    for (i = 0; i < nvr; ++i) {
        if (vr[i] == BOOLEAN_VAR) {
            value[i] = ((Instance*) c)->boolean;
        } else {
            status = fmiWarning;
        }
    }
    return status;
}

DllExport fmiStatus fmiGetString (fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString  value[])
{
    fmiStatus status = fmiOK;
    size_t i = 0;
    for (i = 0; i < nvr; ++i) {
        if (vr[i] == STRING_VAR) {
            value[i] = ((Instance*) c)->string;
        } else {
            status = fmiWarning;
        }
    }
    return status;
}


DllExport fmiStatus fmiSetReal    (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal    value[])
{
    fmiStatus status = fmiOK;
    size_t i = 0;
    for (i = 0; i < nvr; ++i) {
        if (vr[i] == REAL_VAR) {
            ((Instance*) c)->real = value[i];
        } else {
            status = fmiWarning;
        }
    }
    return status;
}

DllExport fmiStatus fmiSetInteger (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[])
{
    fmiStatus status = fmiOK;
    size_t i = 0;
    for (i = 0; i < nvr; ++i) {
        if (vr[i] == INTEGER_VAR) {
            ((Instance*) c)->integer = value[i];
        } else {
            status = fmiWarning;
        }
    }
    return status;
}

DllExport fmiStatus fmiSetBoolean (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[])
{
    fmiStatus status = fmiOK;
    size_t i = 0;
    for (i = 0; i < nvr; ++i) {
        if (vr[i] == BOOLEAN_VAR) {
            ((Instance*) c)->boolean = value[i];
        } else {
            status = fmiWarning;
        }
    }
    return status;
}

DllExport fmiStatus fmiSetString  (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString  value[])
{
    Instance* ic = (Instance*) c;
    fmiStatus status = fmiOK;
    size_t i = 0;
    for (i = 0; i < nvr; ++i) {
        if (vr[i] == STRING_VAR) {
            ic->functions.freeMemory(ic->string);
            ic->string = CloneString(value[i], ic->functions.allocateMemory);
        } else {
            status = fmiWarning;
        }
    }
    return status;
}

DllExport fmiComponent fmiInstantiateSlave(fmiString  instanceName,
                                           fmiString  fmuGUID,
                                           fmiString  fmuLocation,
                                           fmiString  mimeType,
                                           fmiReal    timeout,
                                           fmiBoolean visible,
                                           fmiBoolean interactive,
                                           fmiCallbackFunctions functions,
                                           fmiBoolean loggingOn)
{
    Instance* vc = NULL;
    if (strcmp(FMU_UUID, fmuGUID) != 0) {
        functions.logger(NULL, instanceName, fmiFatal, "", "GUID mismatch");
        return NULL;
    }

    vc = (Instance*) functions.allocateMemory(1, sizeof(Instance));
    if (!vc) {
        functions.logger(NULL, instanceName, fmiFatal, "", "Memory allocation failed");
        return NULL;
    }
    vc->string = NULL; // so that the freeMemory() call in ResetVars doesn't fail.

    vc->instanceName = CloneString(instanceName, functions.allocateMemory);
    vc->functions = functions;
    vc->loggingOn = loggingOn;
    ResetVars(vc);
    return vc;
}


DllExport void fmiFreeSlaveInstance(fmiComponent c)
{
    Instance* vc = (Instance*) c;
    vc->functions.freeMemory(vc->instanceName);
    vc->functions.freeMemory(c);
}


DllExport fmiStatus fmiInitializeSlave(fmiComponent c,
                                       fmiReal      tStart,
                                       fmiBoolean   StopTimeDefined,
                                       fmiReal      tStop)
{
    Instance* vc = (Instance*) c;
    vc->tStart = tStart;
    vc->stopTimeDefined = StopTimeDefined;
    vc->tStop = tStop;
    return fmiOK;
}


DllExport fmiStatus fmiResetSlave(fmiComponent c)
{
    ResetVars((Instance*) c);
    return fmiOK;
}


DllExport fmiStatus fmiTerminateSlave(fmiComponent c)
{
    return fmiOK;
}


DllExport fmiStatus fmiSetRealInputDerivatives(
    fmiComponent c,
    const  fmiValueReference vr[],
    size_t nvr,
    const  fmiInteger order[],
    const  fmiReal value[])
{
    FUNCTION_NOT_IMPLEMENTED
}


DllExport fmiStatus fmiGetRealOutputDerivatives(
    fmiComponent c,
    const   fmiValueReference vr[],
    size_t  nvr,
    const   fmiInteger order[],
    fmiReal value[])
{
    FUNCTION_NOT_IMPLEMENTED
}



DllExport fmiStatus fmiCancelStep(fmiComponent c)
{
    return fmiFatal;
}


DllExport fmiStatus fmiDoStep(
    fmiComponent c,
    fmiReal      currentCommunicationPoint,
    fmiReal      communicationStepSize,
    fmiBoolean   newStep)
{
    Instance* ic = (Instance*) c;
    ic->functions.logger(c, ic->instanceName, fmiOK, "info",
        "real = %g,  integer = %d, boolean = %s, \n, string  = %s",
        ic->real, ic->integer, ic->boolean ? "true" : "false", ic->string);
    return fmiOK;
}


DllExport fmiStatus fmiGetStatus       (fmiComponent c, const fmiStatusKind s, fmiStatus*  value)
{
    FUNCTION_NOT_IMPLEMENTED
}



DllExport fmiStatus fmiGetRealStatus   (fmiComponent c, const fmiStatusKind s, fmiReal*    value)
{
    FUNCTION_NOT_IMPLEMENTED
}



DllExport fmiStatus fmiGetIntegerStatus(fmiComponent c, const fmiStatusKind s, fmiInteger* value)
{
    FUNCTION_NOT_IMPLEMENTED
}



DllExport fmiStatus fmiGetBooleanStatus(fmiComponent c, const fmiStatusKind s, fmiBoolean* value)
{
    FUNCTION_NOT_IMPLEMENTED
}



DllExport fmiStatus fmiGetStringStatus (fmiComponent c, const fmiStatusKind s, fmiString*  value)
{
    FUNCTION_NOT_IMPLEMENTED
}
