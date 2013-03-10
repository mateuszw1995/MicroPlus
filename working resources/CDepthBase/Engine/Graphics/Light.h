#pragma once
#include "../Defines.h"
#include "../Math/Transformation.h"
#include "../Graphics/Scene.h"
#include "../Graphics/vertex.h"
#include "../Graphics/Color.h"
#include "../Graphics/Shader/Shader.h"
#include "../Graphics/Shader/uniform_overloads.h"
class CLight : public __Shader::Effect,  public __Shader::SwitchableEffect {
	enum { __dir = 0, __point, __spot } _type;
	void __base(RGBA_1, RGBA_1, RGBA_1, Vec3D<>);
public:

	struct Effect {
		static __Shader::Component term[];
		static const int comps;
	};
	struct Switch {
		static __Shader::SwitchComponent term[];
		static const int comps;
	};

	struct uniform_set {
		Uniform4f diffuse, 
				  ambient, 
				  specular; 
		Uniform3f position;
		// spot/point
		Uniform1f radius;
		// spot
		Uniform2f cos_angles;
		Uniform3f spotdir;

		UniformMatrix4fv texmat;

		uniform_set();

		__Shader* program;
	};

	_list<uniform_set> Updaters;

	void set_uni(uniform_set&);
	void set_names();

	friend struct CScene;
	friend class __Shader;
	friend class __MeshShader;

	GLuint depthTextureId;

	GLuint fboId, color_rb;
	int sMapW,sMapH;
	GLfloat blurStep;
	GLuint max_delta;
	
	Matrix<GLfloat> shadow_proj;

	void set_texture_matrix();

	bool shadows;
	void Update(unsigned __int64);

	RGBA_1 ambient, diffuse, specular;
	float radius, cos_angles[2];
	Vec3D<> position, lookat;
	Matrix<> tex_mat;

	static  __Shader::Component
		&POSITION,
		&RADIUS,
		&COS_ANGLES,
		&SPOTDIR,
		&TEXTURE_MATRIX;

	static  __Shader::SwitchComponent
		&DIFFUSE,
		&AMBIENT,
		&SPECULAR;

	enum PCFMode {
		NONE,
		STATIC_STEP
	} PCFmode;
	
	enum DIRECTIONAL {};
	enum POINT {};
	enum SPOT {};
	
	CLight(CLight::DIRECTIONAL, RGBA_1 ambient, RGBA_1 diffuse, RGBA_1 specular, Vec3D<> position);
	CLight(CLight::POINT,		RGBA_1 ambient, RGBA_1 diffuse, RGBA_1 specular, Vec3D<> position, float radius);
	CLight(CLight::SPOT,		RGBA_1 ambient, RGBA_1 diffuse, RGBA_1 specular, Vec3D<> position, Vec3D<> sdir, float inner_angle, float outer_angle, float radius);

	~CLight();

	void Init_Shadows(int, int);
	void Init_PCF(GLuint, GLfloat, PCFMode pcfmode = STATIC_STEP);
};
