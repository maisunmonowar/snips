//#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#* DtRecord.h *#*#*#*#*#*#*#*#* (C) 2000-2015 DekTec
//

#ifndef __MAINFILE_H__
#define __MAINFILE_H__

//.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.- Include files -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-#include "DTAPI.h"
#include <stdarg.h>
#include "const_def.h"
#include <DTAPI.h>
#include "cmd_parsing.h"
#include "support_functions.h"
#include "bbProcess.h"
// Windows Microsoft uses with _ => Linux uses without
#ifndef WIN32
#define _vsnprintf vsnprintf
#endif



#endif // #ifndef __MAINFILE_H__
