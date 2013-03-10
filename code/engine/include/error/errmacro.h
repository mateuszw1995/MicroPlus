#pragma once
#include "../options.h"
#include "error.h"

#ifdef enable_errorlogs

#define err(expression) errlog._err(int(expression), __LINE__, __FILE__, __FUNCTION__)
#define errl(expression, errlog) errlog._err(int(expression), __LINE__, __FILE__, __FUNCTION__)
#define errs(expression, str) errlog._errs(int(expression), __LINE__, __FILE__, __FUNCTION__, str)
#define errf(expression, retflag) retflag = retflag ? errlog._err(int(expression), __LINE__, __FILE__, __FUNCTION__) : 0;
#define errsf(expression, str, retflag) retflag = retflag ? errlog._errs(int(expression), __LINE__, __FILE__, __FUNCTION__, str) : 0;

#else 
#define err(expression) expression
#define errf(expression, retflag) retflag = retflag ? int(expression) : 0;
#define errl(expression, errlog) expression
#define errs(expression, str) expression
#define errsf(expression, str, retflag) retflag = retflag ? int(expression) : 0;
#endif