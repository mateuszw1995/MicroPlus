#pragma once

#include "../../Container/list.h"
#include "../../Container/registry.h"
#include "../../Container/string.h"
#include "../../Graphics/Shader/uniform_overloads.h"
#include "../../Graphics/Color.h"
struct CScene;
class __MeshShader;
class __PostFXShader;
class __Shader {
protected:
	static char* file2text(char*);
	void compile();
	void link();
public:
	static void DumpAllLogsToFolder(_string folder);
	static const unsigned __int64 EVERYTHING;
	GLuint v, f, p;
	void Build();
	void exportDebug(_string);
	int operator==(const __Shader&); 
	__Shader();
	__Shader(_string, _string);
	~__Shader();

	struct Component { 
		int flag_2, in;
		Uniform* uni;
		_string name;
		_string* act_name;
		_string type;
		operator int();
		operator _string();
		Component(int, int, _string, _string, _string _init = "" );
		_string init;
		~Component();
		int arr;
	};
	struct SwitchComponent : public Component {
		SwitchComponent(int, int, _string, _string,  _string _init = "");
	};

	template<class T>
	static void clr_names(unsigned __int64 flags){
		for(int a = 0; a < T::comps; ++a) 
			if(T::term[a].act_name && T::term[a] & flags) { 
				delete [] T::term[a].act_name; T::term[a].act_name = 0; 
			}  
	}

	template<class T>
	static void clr_use() {
		for(int a = 0; a < T::comps; T::term[a++].arr = 0);
	}

	template<class T>
	static void write_uni(_string& V, _string& F, bool arr = false) {
		for(Component* t = T::term; t < T::term+T::comps; ++t)
			if(t->arr > 0)
				(t->type.cstr[0] == 'v' ? V : F)("\n ")(t->type.cstr+1)(" ")(t->name)( (t->arr > 1 || arr) ? dosuffix(t->arr) : "")(t->init)(";");
	}

	template<class T>
	static void get_uni (int p) {
		for(Component* t = T::term; t < T::term+T::comps; ++t)
			if(t->uni)
				t->uni->loc = glGetUniformLocation(p, (t->name).c_str());
	}
	template<class T>
	static void get_arr_uni (int p) {
		for(Component* t = T::term; t < T::term+T::comps; ++t)
			if(t->uni)
				t->uni->loc = glGetUniformLocation(p, (t->name + dosuffix(t->arr++)).c_str() );
	}

	struct _eff {

		template<class T, class U>
		void save_set (__Shader* s) {
			T* p = reinterpret_cast<T*>(this);
			U& u = (p->Updaters.push_back(U()))->_v;
			p->set_uni(u);
			u.program = s;
		}
		template<class T>
		void save_set (__Shader* s) {
			T* p = reinterpret_cast<T*>(this);
			p->set_uni(); 
			p->program = s;
		}

	};

	struct Effect : public _eff {
		unsigned __int64 uni_flags;

		void  enableUniform(const Component&);
		void disableUniform(const Component&);
		void  enableUniform(const SwitchComponent&);
		void disableUniform(const SwitchComponent&);

		Effect();
	};

	struct SwitchableEffect : public _eff {

		unsigned __int64 use_flags;
		void  enableComponent(const SwitchComponent&);
		void disableComponent(const SwitchComponent&);

		SwitchableEffect();
	};

	static bool   switch_flag;
	static GLuint active_shader;
	static bool   depth_buffer_mode;
	static __Shader*		active_sp;
	static __MeshShader*	active_mesh_sp;
	static __PostFXShader*	active_post_sp;

	virtual bool Use();
	static void UseFixed();
	static void SwitchProgram(bool);

	bool imported;
	_string vsrc;
	_string fsrc;
	static Registry<__Shader> g_Shaders;
};

typedef Registry<__Shader>::Link Shader;

template <>
Registry<__Shader>::Link::Link(const __Shader& val);