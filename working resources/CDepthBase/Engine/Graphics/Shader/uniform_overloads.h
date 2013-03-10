#pragma once

#include "../../Defines.h"

	struct Uniform {
		bool switch_next;
		GLint loc;
		virtual void chuj() {

		}
		Uniform() : switch_next (true), loc(-1) {}
	};

	struct Uniform1i : public Uniform {
		int CurrentValue[1];
		inline void set(int);
	};
	struct Uniform2i : public Uniform {
		int CurrentValue[2];
		inline void set(int, int);
	};
	struct Uniform3i : public Uniform {
		int CurrentValue[3];
		inline void set(int, int, int);
	};
	struct Uniform4i : public Uniform {
		int CurrentValue[4];
		inline void set(int, int, int, int);
	};	

	struct Uniform1f : public Uniform {
		float CurrentValue[1];
		inline void set(float);
	};
	struct Uniform2f : public Uniform {
		float CurrentValue[2];
		inline void set(float, float);
	};
	struct Uniform3f : public Uniform {
		float CurrentValue[3];
		inline void set(float, float, float);
	};
	struct Uniform4f : public Uniform {
		float CurrentValue[4];
		inline void set(float, float, float, float);
	};

	template<class T>
	struct Uniformv : public Uniform {
		T* CurrentValue;
		int count;
		inline Uniformv(int cnt);
		inline Uniformv(const Uniformv&);
		inline ~Uniformv();
		inline Uniformv& operator=(const Uniformv&);
	};

	struct Uniform1iv : public Uniformv<int> {
		inline void set(int*);
		inline Uniform1iv(int cnt);
	};
	struct Uniform2iv : public Uniformv<int> {
		inline void set(int*);
		inline Uniform2iv(int cnt);
	};
	struct Uniform3iv : public Uniformv<int> {
		inline void set(int*);
		inline Uniform3iv(int cnt);
	};
	struct Uniform4iv : public Uniformv<int> {
		inline void set(int*);
		inline Uniform4iv(int cnt);
	};	
	
	struct Uniform1fv : public Uniformv<float> {
		inline void set(float*);
		inline Uniform1fv(int cnt);
	};
	struct Uniform2fv : public Uniformv<float> {
		inline void set(float*);		
		inline Uniform2fv(int cnt);
	};
	struct Uniform3fv : public Uniformv<float> {
		inline void set(float*);
		inline Uniform3fv(int cnt);
	};
	struct Uniform4fv : public Uniformv<float> {
		inline void set(float*);
		inline Uniform4fv(int cnt);
	};	
	
	struct UniformMatrix2fv : public Uniformv<float> {
		inline void set(float*);		
		inline UniformMatrix2fv(int cnt);
	};
	struct UniformMatrix3fv : public Uniformv<float> {
		inline void set(float*);
		inline UniformMatrix3fv(int cnt);
	};
	struct UniformMatrix4fv : public Uniformv<float> {
		inline void set(float*);
		inline UniformMatrix4fv(int cnt);
	};	
	
	struct UniformMatrix2dv : public Uniformv<double> {
		inline void set(double*);		
		inline UniformMatrix2dv(int cnt);
	};
	struct UniformMatrix3dv : public Uniformv<double> {
		inline void set(double*);
		inline UniformMatrix3dv(int cnt);
	};
	struct UniformMatrix4dv : public Uniformv<double> {
		inline void set(double*);
		inline UniformMatrix4dv(int cnt);
	};	

#include "uniform_overloads.hpp"