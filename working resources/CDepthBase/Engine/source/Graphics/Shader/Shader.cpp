#pragma once

#include "../../../Graphics/Shader/Shader.h"
#include "../../../Graphics/Light.h"
#include <ctime>
#include "../../../Container/registry.h"

template <>
Registry<__Shader>::Link::Link(const __Shader& val) {
	node = Registry<__Shader>::MakeResource(val); 
	++(node->_v.use);
	if(node->_v.univalent()) node->_v._v.Build();
}

const unsigned __int64 __Shader::EVERYTHING = 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128 | 256 | 512 | 1024 | 2048 | 4096 | 8192 | 16384 | 32768 | 65536 | 131072;
Registry<__Shader>			__Shader::g_Shaders = Registry<__Shader>();

__Shader*  __Shader::active_sp = 0;
__MeshShader*  __Shader::active_mesh_sp = 0;
__PostFXShader*  __Shader::active_post_sp = 0;
bool		__Shader::switch_flag = true;
GLuint		__Shader::active_shader = 0;
bool __Shader::depth_buffer_mode = false;

/***********************************************************************************************************************************************************************/
__Shader::Component::operator int() {
	return flag_2;
}
__Shader::Component::operator _string() {
	return name;
}
__Shader::Component::Component(int ind, int fl, _string typ, _string nam, _string _init) : init(_init), in(ind), flag_2(fl), type(typ), name(nam), act_name(new _string[1]), uni(0), arr(0) {
}
__Shader::SwitchComponent::SwitchComponent(int ind, int fl, _string typ, _string nam, _string _init) : Component(ind, fl, typ, nam, _init) {
}
__Shader::Component::~Component()  {
	//if(act_name) delete [] act_name;
}

void  __Shader::Effect::enableUniform(const Component& uni) {uni_flags |=  uni.flag_2;}
void __Shader::Effect::disableUniform(const Component& uni) {uni_flags &=~ uni.flag_2;}
void  __Shader::Effect::enableUniform(const SwitchComponent& uni) {uni_flags |=  uni.flag_2;}
void __Shader::Effect::disableUniform(const SwitchComponent& uni) {uni_flags &=~ uni.flag_2;}
void  __Shader::SwitchableEffect::enableComponent(const SwitchComponent& uni) {use_flags |=  uni.flag_2;}
void __Shader::SwitchableEffect::disableComponent(const SwitchComponent& uni) {use_flags &=~ uni.flag_2;}
__Shader::Effect::Effect() { uni_flags = 0; } 
__Shader::SwitchableEffect::SwitchableEffect() { use_flags = 0; } 


/********************************************************************************************************/
/********************************************************************************************************/

void __Shader::compile(){
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);	

	glShaderSource(v, 1, (const GLchar **)&vsrc.cstr,NULL);
	glShaderSource(f, 1, (const GLchar **)&fsrc.cstr,NULL);

	glCompileShader(v);
	glCompileShader(f);

	p = glCreateProgram();

	glAttachShader(p,v);
	glAttachShader(p,f);
}

void __Shader::link() {
	glLinkProgram(p);
}
void __Shader::Build(){
	if(imported) __Shader::~__Shader();
	compile();
	link();
	imported = true;
}

/********************************************************************************************************/
/********************************************************************************************************/
__Shader::__Shader() : imported(false), vsrc(_string(" ")), fsrc(_string(" ")), v(0), f(0), p(0) {
}

__Shader::__Shader(_string vvv, _string fff) : imported(false), vsrc(vvv), fsrc(fff), v(0), f(0), p(0) {
}

__Shader::~__Shader(){
	if(!imported) return;
	glDetachShader(p, v);
	glDetachShader(p, f);
	glDeleteShader(v);
	glDeleteShader(f);
	glDeleteProgram(p);
	imported = false;
}

int __Shader::operator==(const __Shader& b){
	return (vsrc == b.vsrc && fsrc == b.fsrc);
}

/********************************************************************************************************/
/********************************************************************************************************/
char* __Shader::file2text(char *fn) {

	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = new char[count+1];
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}


/********************************************************************************************************/
/********************************************************************************************************/
void __Shader::exportDebug(_string filename){
	FILE* report = fopen(filename.cstr, "w");
	time_t czas;
	struct tm *czas_strukt;
	czas = time(0);
	czas_strukt = localtime(&czas);


	fprintf(report, "DepthBase __Shader Debug Log. For programmers use.\n------------------------------------------------------------------------------------------------------------------\n---|DATE: %d.%d.%d|\n---|TIME: %d:%s%d:%s%d|\n------------------------------------------------------------------------------------------------------------------\n\n\n\n", czas_strukt->tm_mday, czas_strukt->tm_mon, 1900+czas_strukt->tm_year, czas_strukt->tm_hour, (czas_strukt->tm_min < 10 ? "0" : " " ),  czas_strukt->tm_min,(czas_strukt->tm_sec < 10 ? "0" : " " ), czas_strukt->tm_sec);
	fprintf(report, "addr:%p\nv: %d\nf: %d\np: %d\n\n", this, v, f, p);

	int infologLength = 0;
	int charsWritten  = 0;
	char *infoLog;

	glGetShaderiv(v, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0){
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(v, infologLength, &charsWritten, infoLog);
		fprintf(report, "%s\n\n",infoLog);
		free(infoLog);
	}

	glGetShaderiv(f, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0){
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(f, infologLength, &charsWritten, infoLog);
		fprintf(report, "%s\n\n",infoLog);
		free(infoLog);
	}

	glGetProgramiv(p, GL_INFO_LOG_LENGTH,&infologLength);

	if (infologLength > 0){
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(p, infologLength, &charsWritten, infoLog);
		fprintf(report, "%s\n\n",infoLog);
		free(infoLog);
	}

	fprintf(report, "------------------------------------------------------------------------------------------\n\n%s\n\n-----------------------------------------------------------------------------------------%s", vsrc.cstr, fsrc.cstr);

	fclose(report);
}

/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
bool __Shader::Use(){
	if(memcmp(&p, &__Shader::active_shader, sizeof(GLuint)) || __Shader::switch_flag){
		glUseProgram(p);
		__Shader::active_shader = p;
		__Shader::active_sp = this;
		__Shader::active_mesh_sp = 0;
		__Shader::active_post_sp = 0;
		__Shader::switch_flag = false;
		return true;
	}
	return false;
}
void __Shader::UseFixed(){
	if(__Shader::active_shader || __Shader::switch_flag){
		glUseProgram(0);
		__Shader::active_shader = 0;
		__Shader::active_sp = 0;
		__Shader::active_mesh_sp = 0;
		__Shader::active_post_sp = 0;
		__Shader::switch_flag = false;
	}
}

void __Shader::SwitchProgram(bool s){
	__Shader::switch_flag = s;
}

#include "../../../Graphics/Shader/MeshShader.h"
#include "../../../Graphics/Shader/PostFXShader.h"

void __Shader::DumpAllLogsToFolder(_string folder) {
	_list<Registry<__MeshShader>::entry>::Node* iter = Registry<__MeshShader>::content.first;
	int cntt = 0;
	while(iter) {
		iter->_v._v.exportDebug(folder+_string::i2s(cntt++)+_string("_MeshShader.txt"));
		iter = iter->next;
	}
	_list<Registry<__PostFXShader>::entry>::Node* piter = Registry<__PostFXShader>::content.first;
	cntt = 0;
	while(piter) {
		piter->_v._v.exportDebug(folder+_string::i2s(cntt++)+_string("_PostFXShader.txt"));
		piter = piter->next;
	}
}


/******************************************************************************************************************************/

/*
if(flags & RELIEF_MAPPING){    
fsrc("\n float selfShadow = 1.0;";
fsrc("\n 	if (nDotL > 0.0) {";

fsrc("\n 	float numShadowSteps = mix(60,5,l.z;";
fsrc("\n 	step = 1.0 / numShadowSteps;";
fsrc("\n 	delta = vec2(-l.x, -l.y) * scale / (numShadowSteps * l.z;";

fsrc("\n 	_height = height + step * 0.1;";

fsrc("\n 	while ((height < _height) && (_height < 1)) {";
fsrc("\n 		_height += step;";
fsrc("\n 		TexCoord += delta;";
fsrc("\n 		height = texture2D(heightMap, TexCoord).r;";
fsrc("\n 	}";

fsrc("\n 	if(height < _height) selfShadow = 1.0; else selfShadow = 0.0;";

fsrc("\n }";

}
*/