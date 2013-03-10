#pragma once
//#include "uniform_overloads.h"
/******************************************************************************************************************************/
void Uniform1i::set(int v0){
	if(loc != -1 && (switch_next || v0 != CurrentValue[0])){
		CurrentValue[0] = v0;
		glUniform1i(loc, v0); switch_next = false;
	}
}

/******************************************************************************************************************************/
void Uniform2i::set(int v0, int v1){
	if(loc != -1 && (switch_next || v0 != CurrentValue[0] || v1 != CurrentValue[1])){
		CurrentValue[0] = v0;
		CurrentValue[1] = v1;
		glUniform2i(loc, v0, v1); switch_next = false;
	}
}

/******************************************************************************************************************************/
void Uniform3i::set(int v0, int v1, int v2){
	if(loc != -1 && (switch_next || v0 != CurrentValue[0] || v1 != CurrentValue[1] || v2 != CurrentValue[2])){
		CurrentValue[0] = v0;
		CurrentValue[1] = v1;
		CurrentValue[2] = v2;
		glUniform3i(loc, v0, v1, v2); switch_next = false;
	}
}
/******************************************************************************************************************************/
void Uniform4i::set(int v0, int v1, int v2, int v3){
	if(loc != -1 && (switch_next || v0 != CurrentValue[0] || v1 != CurrentValue[1] || v2 != CurrentValue[2]  || v3 != CurrentValue[3])){
		CurrentValue[0] = v0;
		CurrentValue[1] = v1;
		CurrentValue[2] = v2;
		CurrentValue[3] = v3;
		glUniform4i(loc, v0, v1, v2, v3); switch_next = false;
	}
}

/******************************************************************************************************************************/
void Uniform1f::set(float v0){
	if(loc != -1 && (switch_next || v0 != CurrentValue[0])){
		CurrentValue[0] = v0;
		glUniform1f(loc, v0); switch_next = false;
	}
}

/******************************************************************************************************************************/
void Uniform2f::set(float v0, float v1){
	if(loc != -1 && (switch_next || v0 != CurrentValue[0] || v1 != CurrentValue[1])){
		CurrentValue[0] = v0;
		CurrentValue[1] = v1;
		glUniform2f(loc, v0, v1); switch_next = false;
	}
}

/******************************************************************************************************************************/
void Uniform3f::set(float v0, float v1, float v2){
	if(loc != -1 && (switch_next || v0 != CurrentValue[0] || v1 != CurrentValue[1] || v2 != CurrentValue[2])){
		CurrentValue[0] = v0;
		CurrentValue[1] = v1;
		CurrentValue[2] = v2;
		glUniform3f(loc, v0, v1, v2); switch_next = false;
	}
}
/******************************************************************************************************************************/
void Uniform4f::set(float v0, float v1, float v2, float v3){
	if(loc != -1 && (switch_next || v0 != CurrentValue[0] || v1 != CurrentValue[1] || v2 != CurrentValue[2] || v3 != CurrentValue[3])){
		CurrentValue[0] = v0;
		CurrentValue[1] = v1;
		CurrentValue[2] = v2;
		CurrentValue[3] = v3;
		glUniform4f(loc, v0, v1, v2, v3); switch_next = false;
	}
}
/******************************************************************************************************************************/
void Uniform1iv::set(int* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(int)))){
		memcpy(CurrentValue, v, count*sizeof(int));
		glUniform1iv(loc, count, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void Uniform2iv::set(int* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(int)))){
		memcpy(CurrentValue, v, count*sizeof(int));
		glUniform2iv(loc, count/2, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void Uniform3iv::set(int* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(int)))){
		memcpy(CurrentValue, v, count*sizeof(int));
		glUniform3iv(loc, count/3, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void Uniform4iv::set(int* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(int)))){
		memcpy(CurrentValue, v, count*sizeof(int));
		glUniform4iv(loc, count/4, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void Uniform1fv::set(float* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(float)))){
		memcpy(CurrentValue, v, count*sizeof(float));
		glUniform1fv(loc, count, v); switch_next = false;
	}
}
/******************************************************************************************************************************/

void Uniform2fv::set(float* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(float)))){
		memcpy(CurrentValue, v, count*sizeof(float));
		glUniform2fv(loc, count/2, v); switch_next = false;
	}
}
/******************************************************************************************************************************/

void Uniform3fv::set(float* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(float)))){
		memcpy(CurrentValue, v, count*sizeof(float));
		glUniform3fv(loc, count/3, v); switch_next = false;
	}
}
/******************************************************************************************************************************/

void Uniform4fv::set(float* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(float)))){
		memcpy(CurrentValue, v, count*sizeof(float));
		glUniform4fv(loc, count/4, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void UniformMatrix2fv::set(float* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(*CurrentValue)))){
		memcpy(CurrentValue, v, count*sizeof(*CurrentValue));
		glUniformMatrix2fv(loc, count/4, false, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void UniformMatrix3fv::set(float* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(*CurrentValue)))){
		memcpy(CurrentValue, v, count*sizeof(*CurrentValue));
		glUniformMatrix3fv(loc, count/9, false, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void UniformMatrix4fv::set(float* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(*CurrentValue)))){
		memcpy(CurrentValue, v, count*sizeof(*CurrentValue));
		glUniformMatrix4fv(loc, count/16, false, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void UniformMatrix2dv::set(double* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(*CurrentValue)))){
		memcpy(CurrentValue, v, count*sizeof(*CurrentValue));
		glUniformMatrix2dv(loc, count/4, false, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void UniformMatrix3dv::set(double* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(*CurrentValue)))){
		memcpy(CurrentValue, v, count*sizeof(*CurrentValue));
		glUniformMatrix3dv(loc, count/9, false, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
void UniformMatrix4dv::set(double* v){
	if(loc != -1 && (switch_next || memcmp(CurrentValue, v, count*sizeof(*CurrentValue)))){
		memcpy(CurrentValue, v, count*sizeof(*CurrentValue));
		glUniformMatrix4dv(loc, count/16, false, v); switch_next = false;
	}
}
/******************************************************************************************************************************/
template<class T>
Uniformv<T>::~Uniformv() {
	delete [] CurrentValue ;
}
template<class T>
Uniformv<T>::Uniformv(int cnt) : count(cnt) {
	CurrentValue = new T[cnt];
}
template<class T>
Uniformv<T>& Uniformv<T>::operator=(const Uniformv<T>& a) {
	count = a.count; delete [] CurrentValue; CurrentValue = new T[count]; memcpy(CurrentValue, a.CurrentValue, count*sizeof(T)); return *this;
}
template<class T>
Uniformv<T>::Uniformv(const Uniformv<T>& a) {
	count = a.count; CurrentValue = new T[count]; memcpy(CurrentValue, a.CurrentValue, count*sizeof(T));
}
Uniform1iv::Uniform1iv(int cnt) : Uniformv(cnt  ) {}
Uniform2iv::Uniform2iv(int cnt) : Uniformv(cnt*2) {}
Uniform3iv::Uniform3iv(int cnt) : Uniformv(cnt*3) {}
Uniform4iv::Uniform4iv(int cnt) : Uniformv(cnt*4) {}
Uniform1fv::Uniform1fv(int cnt) : Uniformv(cnt  ) {}
Uniform2fv::Uniform2fv(int cnt) : Uniformv(cnt*2) {}
Uniform3fv::Uniform3fv(int cnt) : Uniformv(cnt*3) {}
Uniform4fv::Uniform4fv(int cnt) : Uniformv(cnt*4) {}
UniformMatrix2fv::UniformMatrix2fv(int cnt) : Uniformv(cnt*4) {}
UniformMatrix3fv::UniformMatrix3fv(int cnt) : Uniformv(cnt*9)  {}
UniformMatrix4fv::UniformMatrix4fv(int cnt) : Uniformv(cnt*16)  {}
UniformMatrix2dv::UniformMatrix2dv(int cnt) : Uniformv(cnt*4) {}
UniformMatrix3dv::UniformMatrix3dv(int cnt) : Uniformv(cnt*9)  {}
UniformMatrix4dv::UniformMatrix4dv(int cnt) : Uniformv(cnt*16)  {}


