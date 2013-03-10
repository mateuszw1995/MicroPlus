#pragma once
#include <fstream>

namespace db {
	extern void wsa_last_error(char*);
	extern void     last_error(char*);
	extern void  gl_last_error(char*);

	extern int  wsa_last_error();
	extern int      last_error();
	extern int   gl_last_error();

	class error {
		void* dummy, enter(), leave();
		static unsigned long lasterr();
	public:
		int _err(int expression, long lin, const char* fil,  const char* fun) {
			if((!expression || logsucc) && logs.is_open() && enabled) {  
					char er[1000]; 

					enter(); 
				
					if(expression && logsucc) 
						logs << "OPERATION HAS COMPLETED SUCCESSFULLY! This message's been generated only because of \"log_succesfull\" flag set to true." << std::endl;
					if(error_func && errorid_func) {
						error_func(er);  
						logs << "line: " << lin << "\nfile: " << fil << "\nfunc: " << fun << "\nerror " << errorid_func() << ": " <<  er << std::endl; 
					}
					else
						logs << "line: " << lin << "\nfile: " << fil << "\nfunc: " << fun; 
					logs << std::endl;
					
					leave(); 
			} 
			return expression; 
		}
		
		int _errs(int expression, long lin, const char* fil,  const char* fun, const char* strr) {
			if((!expression || logsucc) && logs.is_open() && enabled) {  
					char er[1000]; 
					enter(); 
				
					if(expression && logsucc) 
						logs << "OPERATION HAS COMPLETED SUCCESSFULLY! This message's been generated only because of \"log_succesfull\" flag set to true." << std::endl;
					if(error_func && errorid_func) {
						error_func(er); 
						logs << "line: " << lin << "\nfile: " << fil << "\nfunc: " << fun << "\nerror " << errorid_func() << ": " <<  er << std::endl << "message: " << strr << std::endl; 
					}
					else 
						logs << "line: " << lin << "\nfile: " << fil << "\nfunc: " << fun << "\nmessage: " << strr << std::endl; 
					logs << std::endl;
					leave(); 
			}
			return expression;
		}

		std::ofstream logs;
		error(void (*error_func)(char*), int(*errorid_func)() = last_error);
		~error();
		
		void (*error_func)(char*);
		int (*errorid_func)();
		bool enabled, logsucc;
		void enable(bool = true);
		void log_successful(bool = false);

		void open(const char* fname);
		void close();

		template<typename T>
		std::ofstream& operator<<(const T& t) {
			return logs << t;
		}

	};
}
