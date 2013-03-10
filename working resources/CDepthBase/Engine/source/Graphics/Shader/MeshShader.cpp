#pragma once

#include "../../../Graphics/Shader/MeshShader.h"
#include "../../../Graphics/Light.h"

Registry<__MeshShader>		__MeshShader::g_MeshShaders = Registry<__MeshShader>();
/***********************************************************************************************************************************************************************/
/***********************************************************************************************************************************************************************/
void __MeshShader::ComponentSet::Enable(__MeshShader::util_flag option){
	_flags |= option;
}
/***********************************************************************************************************************************************************************/
void __MeshShader::ComponentSet::Disable(__MeshShader::util_flag option){
	_flags &= ~option;
}
/***********************************************************************************************************************************************************************/
bool __MeshShader::ComponentSet::IsEnabled(__MeshShader::util_flag option){
	return _flags & option;
}
__MeshShader::ComponentSet::ComponentSet() : _flags(0), fog_fx(0), material(0), Lights(_list<CLight*>()), parallax(0) {

}
bool __MeshShader::Use() {
	return __Shader::Use() ? __Shader::active_mesh_sp = this : false;
}

void __MeshShader::Import(){
	unsigned __int64 _flags = compo_set._flags;

	bool lights_on = compo_set.Lights.size();
	_list<CLight*>::Node* iter = compo_set.Lights.first;
	Fog* fog_fx = compo_set.fog_fx;
	Material* material = compo_set.material;
	ParallaxMap* parallax = compo_set.parallax;
	 
	 /*
											  static bool unit_slots[4];
											  unit_slots[0] = flags & DIFFUSE_MAPPING;
											  unit_slots[1] = flags & NORMAL_MAPPING;
											  unit_slots[2] = flags & PARALLAX_MAPPING;
											  unit_slots[3] = flags & SPECULAR_MAPPING; */

	fsrc.clear();
	vsrc.clear();

	_string lightVCalc,
	        lightFCalc,
	        Varying,
	        FUni,
	        VUni;
	
	clr_use<CLight::Effect>();
	clr_use<CLight::Switch>();
	clr_use<Fog>();
	clr_use<Material::Effect>();
	clr_use<Material::Switch>();
	clr_use<ParallaxMap>();
	
	if(fog_fx)
		fog_fx->set_names();

	if(material) 
		material->set_names();

	if(parallax)
		parallax->set_names();


	int prefCount[3] =  { 0, 0, 0 } ;
	int shadowCount = 0;

#define UNI(u) (*CLight::u.act_name)

	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/
	int last_slot = 4;
	while(iter) {
		CLight* l = iter->_v;  
		l->set_names();

		bool doshadows = (_flags & SHADOW_MAPPING && l->shadows);

		static _string lightShadowSuffix; 
		static _string lightShadowUniSuffix; 

		_string lightDir = "lightDir";
		lightDir += dosuffix(prefCount[CLight::__dir] + prefCount[CLight::__point] + prefCount[CLight::__spot]);

		if(l->_type == CLight::__dir) 
			lightVCalc("\n ")(lightDir)("= normalize(")UNI(POSITION)(_string(");"));
		else {
			lightVCalc("\n ")(lightDir)("= (")UNI(POSITION)(" - vertexPos) *")UNI(RADIUS)(";");
			if(l->_type == CLight::__spot)
				lightVCalc("\n spotVec")(dosuffix(prefCount[CLight::__spot]))(" =")UNI(SPOTDIR)(";");

		}

		if(_flags & NORMAL_MAPPING) {
			lightVCalc("\n ")(lightDir)(" = tbnMatrix * ")(lightDir)(";");
			if(l->_type == CLight::__spot)
				lightVCalc("\n spotVec")(dosuffix(prefCount[CLight::__spot]))(" = tbnMatrix * spotVec")(dosuffix(prefCount[CLight::__spot]))(";"); 
		}

		lightFCalc("\n l = normalize(")(lightDir)(");")
		          ("\n h = normalize(l+v);");

		if(l->_type > 0) {
			lightFCalc("\n atten = max(0.0, 1.0 - dot(")(lightDir)(", ")(lightDir)("))"); 
			if(l->_type == CLight::__spot) 
			    lightFCalc("* smoothstep(")UNI(COS_ANGLES)(".x, ")UNI(COS_ANGLES)(".y, dot(-l, normalize(spotVec")(dosuffix(prefCount[CLight::__spot]))(")))");
		
			lightFCalc += ";";
		}

		if(doshadows) { 

			lightShadowSuffix = dosuffix(shadowCount);
			lightShadowUniSuffix = _string::i2s(shadowCount++);
			FUni("\n uniform sampler2DShadow shadowMap")(lightShadowUniSuffix)(";");

			lightVCalc("\n ShadowCoord")(lightShadowSuffix)(" = ")UNI(TEXTURE_MATRIX)(" * vec4(at_Vertex, 1.0);");
			if(l->PCFmode == CLight::NONE) {
				lightFCalc("\n shadowCoordWdiv = ShadowCoord")(lightShadowSuffix)(" / ShadowCoord")(lightShadowSuffix)(".w;");
			}
			//lightFCalc("\n shadowCoordWdiv.z += 0.0005;";
			/*int slot = -1;
			if(!unit_slots[0]) { slot = 0; unit_slots[0] = true; }
			else if(!unit_slots[1]) { slot = 1; unit_slots[1] = true; }
			else if(!unit_slots[2]) { slot = 2; unit_slots[2] = true; }
			else if(!unit_slots[3]) { slot = 3; unit_slots[3] = true; }
			else slot = last_slot++;*/
			DepthMap newMap = { last_slot++, l->depthTextureId }; 
			ShadowMaps.push_back(newMap);

			float sqmult;
			switch(l->PCFmode) {
			case CLight::NONE: 
				lightFCalc("\n if(ShadowCoord")(lightShadowSuffix)(".w > 0.0) ")
						  ("\n shadow = shadow2DProj( shadowMap")(lightShadowUniSuffix)(",ShadowCoord")(lightShadowSuffix)(" + vec4(0.0, 0.0, 0.05, 0.0) ).w;")
						  ("\n else shadow = 1.0;");
				break;

			case CLight::STATIC_STEP:
				if(l->_type != CLight::__dir){
					lightFCalc("\n if(ShadowCoord")(lightShadowSuffix)(".w > 0.0) {");
				}

		  lightFCalc("\n shadow = 0.0;")
		            ("\n for (y=-")(_string::d2s(l->max_delta, 5))(";y<")(_string::d2s(l->max_delta, 5))(";y+=")(_string::d2s(l->blurStep, 5))(")")
		            ("\n for (x=-")(_string::d2s(l->max_delta, 5))(";x<")(_string::d2s(l->max_delta, 5))(";x+=")(_string::d2s(l->blurStep, 5))(")")
		            ("\n shadow += shadow2DProj(shadowMap")
					(lightShadowUniSuffix)(", ShadowCoord")
					(lightShadowSuffix)(" + vec4(x * ")(_string::d2s(1.0/l->sMapW, 5))(" * ShadowCoord")
					(lightShadowSuffix)(".w, y * ")(_string::d2s(1.0/l->sMapH, 5))(" * ShadowCoord")
					(lightShadowSuffix)(".w, 0.0, 0.0) ).w;");

				sqmult = 2 * (l->max_delta/l->blurStep);

				lightFCalc("\n shadow *= ")(_string::d2s ( 1.0/(sqmult*sqmult), 8))(";");

				if(l->_type != CLight::__dir)
					lightFCalc("\n } else shadow = 1.0;"); 

				break;

			default: break;

			}
		}
		unsigned __int64 m_f = material ? material->use_flags : 0;
		if(l->use_flags & CLight::DIFFUSE) {
		lightFCalc("\n color += ")UNI(DIFFUSE)(m_f & Material::DIFFUSE ? _string(" *")(*material->DIFFUSE.act_name) : " ")
				  ("* max(0.0, dot(n, l))"); if(doshadows) lightFCalc("* shadow"); 
		lightFCalc(l->_type > 0 ? "* atten;" : ";"); 
		}
		
		if(l->use_flags & CLight::SPECULAR) {
		lightFCalc("\n color += ")UNI(SPECULAR)(m_f & Material::SPECULAR ? _string(" *")(*material->SPECULAR.act_name) : " "); if(doshadows) lightFCalc("* shadow");  
				  lightFCalc("* pow(max(0.0, dot(n, h)),")(material ? (*material->SHININESS.act_name) : "10.0");  
		if(_flags & SPECULAR_MAPPING) lightFCalc("* speccol.r"); 
		lightFCalc(")")( l->_type > 0 ? "* atten;" : ";");
		}

		if(l->use_flags & CLight::AMBIENT) {
		lightFCalc("\n color += ")UNI(AMBIENT)(m_f & Material::AMBIENT ? _string(" *")(*material->AMBIENT.act_name) : " ")( l->_type > 0 ? "* atten;" : ";");
		}

		++prefCount[l->_type];

		iter = iter->next;
	}
#undef UNI

	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/
	if(compo_set.Lights.size()) {
		write_uni<CLight::Effect>(VUni, FUni, true);
		write_uni<CLight::Switch>(VUni, FUni, true);
	}
	if(fog_fx)
		write_uni<Fog>(VUni, FUni);
	if(material) {
		write_uni<Material::Effect>(VUni, FUni);
		write_uni<Material::Switch>(VUni, FUni);
	}
	if(parallax)
		write_uni<ParallaxMap>(VUni, FUni);

			  
	if(prefCount[CLight::__spot] > 0) 
		Varying("\n varying vec3 spotVec[")(_string::i2s(prefCount[CLight::__spot]))("];");

	if(prefCount[CLight::__spot] || prefCount[CLight::__dir] || prefCount[CLight::__point])
	Varying("\n varying vec3 lightDir[")(_string::i2s(prefCount[CLight::__dir] + prefCount[CLight::__point] + prefCount[CLight::__spot]))("];");

	if(shadowCount) 
		Varying("\n varying vec4 ShadowCoord[")(_string::i2s(shadowCount))("];");

	VUni("\n uniform mat4 ModelViewMatrix;")
		("\n uniform mat4 ModelViewProjectionMatrix;");

	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/

	fsrc(FUni);

	if(_flags &  DIFFUSE_MAPPING)	fsrc("\n uniform sampler2D diffuseMap;");
	if(_flags &   NORMAL_MAPPING)   fsrc("\n uniform sampler2D normalMap;");
	if(parallax)					fsrc("\n uniform sampler2D heightMap;");
	if(_flags & SPECULAR_MAPPING)	fsrc("\n uniform sampler2D specularMap;");

	fsrc("\n varying vec2 texCoordinate;");
	fsrc(Varying);
	fsrc("\n varying vec3 viewDir;");
	if(!(_flags & NORMAL_MAPPING) && lights_on)	fsrc("\n varying vec3 normal;");

	fsrc("\n void main() ")
		("\n { ")
		("\n vec3 v = normalize(viewDir);")
		("\n vec2 TexCoord = texCoordinate;");
	
#define UNI(u) (*ParallaxMap::u.act_name)
	if(parallax && parallax->relief){    

		fsrc("\n float _height = 1.0; ")
		    ("\n float numSteps = mix(")UNI(STEPS)("*2.0, ")UNI(STEPS)(", v.z); \n")
		    ("\n float step = 1.0 / numSteps;")
		    ("\n vec2 delta = vec2(-viewDir.x, -viewDir.y) * ")UNI(SCALE)(" / (viewDir.z * numSteps); \n")
		    ("\n float height = texture2D(heightMap, TexCoord).r;")
			("\n while (height < _height) {")
		    ("\n 	_height -= step;")
		    ("\n 	TexCoord += delta;")
		    ("\n 	height = texture2D(heightMap, TexCoord).r;")
		    ("\n }");
	}
	else if(parallax && !parallax->relief){    
		fsrc("\n\n float height = texture2D(heightMap, texCoordinate).r;")
		    ("\n height = height * ")UNI(SCALE)(" + ")UNI(BIAS)(";")
		    ("\n TexCoord = texCoordinate + (height * v.xy); \n");
	}
#undef UNI

	if(_flags & NORMAL_MAPPING && lights_on){
		fsrc("\n vec3 n = normalize(texture2D(normalMap, TexCoord).rgb * 2.0 - 1.0);");}
	else if (lights_on)
		fsrc("\n vec3 n = normalize(normal);");
	
	if(lights_on)
	fsrc("\n vec3 l;")
		("\n vec3 h;")
	    ("\n float atten;")
		("\n float shadow = 0.0;")
	    ("\n float x,y;");
	fsrc("\n vec4 color = vec4(")(lights_on ? "0.0, 0.0, 0.0, 0.0" : "1.0, 1.0, 1.0, 1.0" )(");");


	if(_flags & SPECULAR_MAPPING) fsrc("\n vec4 speccol = texture2D(specularMap, TexCoord);");

	fsrc(lightFCalc);

	if(_flags & DIFFUSE_MAPPING){
		fsrc("\n color *= texture2D(diffuseMap,TexCoord);");
	}
	
	
#define UNI(u) (*Fog::u.act_name)
	if(fog_fx) { 
	fsrc("\n float z = gl_FragCoord.z / gl_FragCoord.w;")
		("\n color = mix(")UNI(COLOR)(", color, clamp(exp2( -")UNI(DENSITY)("*")UNI(DENSITY)("*z*z*1.442695)")(", 0.0, 1.0)")(");");
	}
#undef UNI

	fsrc("\n gl_FragColor = color;")
	    ("\n } ");


	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/
	/*****************************************************************************************************************************************/

	if(!(_flags & NORMAL_MAPPING) && lights_on)	vsrc("\n varying vec3 normal;");
	else if(lights_on)						vsrc("\n attribute vec4 at_Tangent;");

	vsrc(VUni)
	    ("\n attribute vec3 at_Vertex;")
	    ("\n attribute vec2 at_Texcoord;")
	    ("\n attribute vec3 at_Normal;")
		("\n varying vec2 texCoordinate;")
	    (Varying)
	    ("\n varying vec3 viewDir;")
	    ("\n void main() ")
	    ("\n { ")
	    ("\n gl_Position = ModelViewProjectionMatrix * vec4(at_Vertex, 1.0);")
	    ("\n texCoordinate = at_Texcoord;")
	    ("\n vec3 vertexPos = vec3(ModelViewMatrix * vec4(at_Vertex, 1.0));");

	if(_flags & NORMAL_MAPPING)
		vsrc("\n vec3 n = normalize(ModelViewMatrix * vec4(at_Normal, 0.0)).xyz;")
		    ("\n vec3 t = normalize(ModelViewMatrix * vec4(at_Tangent.xyz, 0.0)).xyz;")
		    ("\n vec3 b = cross(t, n) * at_Tangent.w;")
		    
		    ("\n mat3 tbnMatrix = mat3(t.x, b.x, n.x,")
		    ("\n                       t.y, b.y, n.y,")
		    ("\n                       t.z, b.z, n.z);")
		    
		    ("\n viewDir = tbnMatrix * -vertexPos;");
	else {
		vsrc("\n viewDir = -vertexPos;");
	if(lights_on)
		vsrc("\n normal = normalize(ModelViewMatrix * vec4(at_Normal, 0.0)).xyz;");
	}
	

	vsrc(lightVCalc)
		("\n } ");

}
void __MeshShader::Build(){
	if(imported) __Shader::~__Shader();
	compile();

	glBindAttribLocation(p, ATTRIB_VERTEX, "at_Vertex");
	glBindAttribLocation(p, ATTRIB_TEXCOORD, "at_Texcoord");
	glBindAttribLocation(p, ATTRIB_NORMAL, "at_Normal");
	glBindAttribLocation(p, ATTRIB_COLOR, "at_Color");

	
	unsigned __int64 _flags = compo_set._flags;
	if(_flags & NORMAL_MAPPING) 
		glBindAttribLocation(p, ATTRIB_TANGENT, "at_Tangent");
	
	link();
	Use();

	if(_flags & DIFFUSE_MAPPING) {
		diffuseUni.loc = glGetUniformLocation(p, "diffuseMap");
		diffuseUni.set(0);
	}
	if(_flags & NORMAL_MAPPING) {
		normalUni.loc = glGetUniformLocation(p, "normalMap");
		normalUni.set(1);
	}
	if(_flags & SPECULAR_MAPPING) {
		specularUni.loc = glGetUniformLocation(p, "specularMap");
		specularUni.set(3);
	}

	modelView.loc = glGetUniformLocation(p, "ModelViewMatrix");
	modelViewProjection.loc = glGetUniformLocation(p, "ModelViewProjectionMatrix");

	if(compo_set.material) {
	   compo_set.material->save_set<Material>(this);
		get_uni<Material::Effect>(p);
		get_uni<Material::Switch>(p);
	}

	static _list<__MeshShader::DepthMap>::Node* d_iter;
	d_iter = __Shader::active_mesh_sp->ShadowMaps.first;
	int shaduni = 0;
	while(d_iter) {
		glUniform1i(glGetUniformLocation(p, (_string("shadowMap") + _string::i2s(shaduni++)).cstr), d_iter->_v.texUnit);
		d_iter = d_iter->next;
	}

	/*****************************************************************************************************************************/
	if(compo_set.Lights.size()) {
		_list<CLight*>::Node* iter = compo_set.Lights.first;
		
		clr_use<CLight::Effect>();
		clr_use<CLight::Switch>();

		while(iter){
			if(iter->_v) 
				iter->_v->save_set<CLight, CLight::uniform_set>(this);
				get_arr_uni<CLight::Effect>(p);
				get_arr_uni<CLight::Switch>(p);
			iter = iter->next;
		}
	}
		/*****************************************************************************************************************************/
	if(compo_set.fog_fx) {
		compo_set.fog_fx->save_set<Fog, Fog::uniform_set>(this);
		get_uni<Fog>(p);
	}
	/*****************************************************************************************************************************/
	if(compo_set.parallax) {
		heightUni.loc = glGetUniformLocation(p, "heightMap");
		heightUni.set(2);
		
		compo_set.parallax->save_set<ParallaxMap, ParallaxMap::uniform_set>(this);
		get_uni<ParallaxMap>(p);
	}
	/*****************************************************************************************************************************/
	imported = true;
}


int __MeshShader::operator==(const __MeshShader& b){
	return (vsrc == b.vsrc && fsrc == b.fsrc && compo_set == b.compo_set);
}
int __MeshShader::ComponentSet::operator==(const ComponentSet& b){
	return (fog_fx == b.fog_fx && material == b.material && Lights == b.Lights && parallax == b.parallax);
}

__MeshShader::__MeshShader() : 
__Shader(), modelView(1), modelViewProjection(1), ShadowMaps(_list<DepthMap>()) {
}
__MeshShader::__MeshShader(ComponentSet& compos) : 
__Shader(), modelView(1), modelViewProjection(1), compo_set(compos), ShadowMaps(_list<DepthMap>()) {
	Import();
}
__MeshShader::__MeshShader(_string v, _string f, ComponentSet& compos) : 
__Shader(v, f), modelView(1), modelViewProjection(1), compo_set(compos), ShadowMaps(_list<DepthMap>()) {
}