#pragma once

#include "Shader.h"

class CLight;
class __MeshShader : public __Shader {
public:
	static Registry<__MeshShader> g_MeshShaders;
	
	enum {
		ATTRIB_VERTEX = 0, 
		ATTRIB_TEXCOORD, 
		ATTRIB_NORMAL, 
		ATTRIB_TANGENT,
		ATTRIB_COLOR
	};

	struct DepthMap {
		GLuint texUnit, depthTexId;
	};

	_list<DepthMap> ShadowMaps;

	UniformMatrix4fv modelView,
					 modelViewProjection;


	/************************************************************/
	struct Fog : public Effect {
		static Component term[];
		static const int comps;

		float color[4], density;

		struct uniform_set { 
			Uniform4f u_color;
			Uniform1f u_density;
			__Shader* program;
		};

		_list<uniform_set> Updaters;

		static Component &COLOR, &DENSITY;
		
		void set_uni(uniform_set&);
		void set_names();

		void Update(unsigned __int64);
		Fog(RGBA_1, float);
	};
	/************************************************************/
	struct ParallaxMap : public Effect {
		static Component term[];
		static const int comps;

		float scale, bias, steps;
		bool relief;
	
		static Component &SCALE, &BIAS, &STEPS;
		
		struct uniform_set { 
			Uniform1f u_scale, u_bias, u_steps;
			__Shader* program;
		};

		void set_uni(uniform_set&);
		void set_names();

		_list<uniform_set> Updaters;
		
		void Update(unsigned __int64);

		void Relief(bool, float);
		ParallaxMap(float = 0.04, float = -0.03);

	};
	/************************************************************/
	struct Material : public Effect, public SwitchableEffect {
		struct Effect { 
			static Component term[];
			static const int comps;
		};
		struct Switch { 
			static SwitchComponent term[];
			static const int comps;
		};

		static Component &SHININESS;
		static SwitchComponent &DIFFUSE, &AMBIENT, &SPECULAR;
		
		float diffuse[4], ambient[4], specular[4], shininess;

		Uniform4f u_diffuse, u_ambient, u_specular;
		Uniform1f u_shininess;
		__Shader* program;
		
		void set_uni();
		void set_names();

		Material(RGBA_1 = RGBA_1(1,1,1,1), RGBA_1 = RGBA_1(1,1,1,1), RGBA_1 = RGBA_1(1,1,1,1), float = 10);
	};
	
	enum util_flag{
		NORMAL_MAPPING		= 1,
		SPECULAR_MAPPING	= 2,
		DIFFUSE_MAPPING		= 4,
		SHADOW_MAPPING		= 8
	};
	/************************************************************/
	struct ComponentSet {
		_list<CLight*> Lights;
		Fog* fog_fx;
		Material* material;
		ParallaxMap* parallax;
		unsigned __int64 _flags;

		int operator==(const ComponentSet&);
		ComponentSet();

		void Enable (util_flag option);
		void Disable (util_flag option);
		bool IsEnabled (util_flag option);

	} compo_set;
	

	__MeshShader();
	__MeshShader(ComponentSet&);
	__MeshShader(_string, _string, ComponentSet&);
	int operator==(const __MeshShader&); 
	
	void Import();
	void Build();
	bool Use();
	
	
	// maps, samplers

	

	Uniform1i diffuseUni,
			  normalUni,
			  specularUni,
			  heightUni;

};

typedef Registry<__MeshShader>::Link MeshShader;


template <>
Registry<__MeshShader>::Link::Link(const __MeshShader& val){
			node = Registry<__MeshShader>::MakeResource(val); 
			++(node->_v.use);
			if(node->_v.univalent()) node->_v._v.Build();
}

