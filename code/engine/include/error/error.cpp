#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "error.h"
#include <Windows.h>
#undef min
#undef max
#include <gl/glew.h>
namespace db {
	void error::enter() {
		EnterCriticalSection((LPCRITICAL_SECTION)dummy);
	}
	void error::leave() {
		LeaveCriticalSection((LPCRITICAL_SECTION)dummy);
	}

	void last_error(char* msgbuf) {
		DWORD dw = GetLastError(); 
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			msgbuf,
			1000, NULL );
	}

	void wsa_last_error(char* msgbuf) {
		DWORD dw = WSAGetLastError(); 
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			msgbuf,
			1000, NULL );
	}

	void gl_last_error(char* msgbuf) {
		strcpy(msgbuf, (char*)gluErrorString(glGetError()));
	}

	int wsa_last_error() {
		return WSAGetLastError();
	}

	int last_error() {
		return GetLastError();
	}

	int gl_last_error() {
		return glGetError();
	}

	error::error(void (*errf)(char*), int(*errid)()) : error_func(errf), errorid_func(errid), enabled(true), logsucc(false) { dummy = malloc(sizeof(CRITICAL_SECTION)); InitializeCriticalSection((LPCRITICAL_SECTION)dummy); }
	
	void error::enable(bool f) {
		enabled = f;
	}

	void error::log_successful(bool f) {
		logsucc = f;
	}

	void error::open(const char* fname) {
		logs.open(fname);
	}

	void error::close() {
		if(dummy) {
			DeleteCriticalSection((LPCRITICAL_SECTION)dummy);
			free(dummy);
			dummy = 0;
		}
		if(logs.is_open()) logs.close();
	}

	error::~error() {
		close();
	}

}
