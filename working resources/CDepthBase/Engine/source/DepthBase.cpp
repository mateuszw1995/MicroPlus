#pragma once

#include "../CDepthBase.h"
#include "../Graphics/Scene.h"
#include "../Graphics/vertex.h"
#include "../Container/string.h"
#include "../Math/Camera.h"

vec3::vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) { }

vec3::operator Vec3D<float>() { 
return Vec3D<float> (x,y,z);
};
bool DB::VBO_supported;

/***********************************************************************************************************************************************************************/

int DB::OpenGLSet(ShaderType shadertype) {

	glShadeModel( GL_SMOOTH );
	glClearDepth( 1.0f );
	glPixelStorei(GL_UNPACK_ALIGNMENT,4);
	glDepthFunc( GL_LEQUAL );
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, DB::Window::getWidth(), DB::Window::getHeight());

	if (GLEW_OK != glewInit())
		DB::Window::MBX("Failed to init GLEW.", "Error");
	
	VBO_supported = (bool)glewIsSupported("GL_ARB_vertex_buffer_object");

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	FX::Rect::FULLSCREEN = FX::Rect(0,0,(float)DB::Window::getWidth(), (float)DB::Window::getHeight());

	
	bool a = (shadertype == SHADER_2_0);

	glCreateShader   = a ? glCreateShader   :  glCreateShaderObjectARB;
	glShaderSource	 = a ? glShaderSource   :  glShaderSourceARB;
	glCompileShader	 = a ? glCompileShader	:  glCompileShaderARB;
	glCreateProgram	 = a ? glCreateProgram	:  glCreateProgramObjectARB;
	glAttachShader	 = a ? glAttachShader	:  glAttachObjectARB;
	glLinkProgram	 = a ? glLinkProgram	:  glLinkProgramARB;
	glUseProgram	 = a ? glUseProgram		:  glUseProgramObjectARB;
	glDetachShader   = a ? glDetachShader   :  glDetachObjectARB;
	glDeleteShader   = a ? glDeleteShader   :  glDeleteObjectARB; 
	glDeleteProgram  = a ? glDeleteProgram  :  glDeleteObjectARB; 
	glGetShaderiv        = a ? glGetShaderiv        : glGetObjectParameterivARB;
	glGetProgramiv		 = a ? glGetProgramiv		: glGetObjectParameterivARB;
	glGetShaderInfoLog	 = a ? glGetShaderInfoLog	: glGetInfoLogARB;
	glGetProgramInfoLog	 = a ? glGetProgramInfoLog	: glGetInfoLogARB;
	glGetUniformLocation = a ? glGetUniformLocation : glGetUniformLocationARB;
	glGetAttribLocation  = a ? glGetAttribLocation  : glGetAttribLocationARB;
	glVertexAttrib1f = a ? glVertexAttrib1f : glVertexAttrib1fARB;
	glVertexAttrib2f = a ? glVertexAttrib2f : glVertexAttrib2fARB;
	glVertexAttrib3f = a ? glVertexAttrib3f : glVertexAttrib3fARB;
	glVertexAttrib4f = a ? glVertexAttrib4f : glVertexAttrib4fARB;
	glEnableVertexAttribArray = a ? glEnableVertexAttribArray : glEnableVertexAttribArrayARB;
	glDisableVertexAttribArray = a ? glDisableVertexAttribArray : glDisableVertexAttribArrayARB;
	glVertexAttribPointer = a ? glVertexAttribPointer : glVertexAttribPointerARB;
	glUniform1i		  = a ? glUniform1i		  : glUniform1iARB		  ;
	glUniform2i		  = a ? glUniform2i		  : glUniform2iARB		  ;
	glUniform3i		  = a ? glUniform3i		  : glUniform3iARB		  ;
	glUniform4i		  = a ? glUniform4i		  : glUniform4iARB		  ;
	glUniform1f		  = a ? glUniform1f		  : glUniform1fARB		  ;
	glUniform2f		  = a ? glUniform2f		  : glUniform2fARB		  ;
	glUniform3f		  = a ? glUniform3f		  : glUniform3fARB		  ;
	glUniform4f		  = a ? glUniform4f		  : glUniform4fARB		  ;
	glUniform1iv	  = a ? glUniform1iv	  : glUniform1ivARB	      ;
	glUniform2iv	  = a ? glUniform2iv	  : glUniform2ivARB	      ;
	glUniform3iv	  = a ? glUniform3iv	  : glUniform3ivARB	      ;
	glUniform4iv	  = a ? glUniform4iv	  : glUniform4ivARB	      ;
	glUniform1fv	  = a ? glUniform1fv	  : glUniform1fvARB	      ;
	glUniform2fv	  = a ? glUniform2fv	  : glUniform2fvARB	      ;
	glUniform3fv	  = a ? glUniform3fv	  : glUniform3fvARB	      ;
	glUniform4fv	  = a ? glUniform4fv	  : glUniform4fvARB	      ;
	glUniformMatrix2fv= a ? glUniformMatrix2fv: glUniformMatrix2fvARB ;
	glUniformMatrix3fv= a ? glUniformMatrix3fv: glUniformMatrix3fvARB ;
	glUniformMatrix4fv= a ? glUniformMatrix4fv: glUniformMatrix4fvARB ;

	glEnableVertexAttribArray(__MeshShader::ATTRIB_VERTEX);
	glEnableVertexAttribArray(__MeshShader::ATTRIB_TEXCOORD);
	glEnableVertexAttribArray(__MeshShader::ATTRIB_NORMAL);
	
	CScene::default_camera.Perspective(45.0f,  (float)DB::Window::getWidth() / (float)DB::Window::getHeight(), 0.1f, 9000.0f);
		CScene::depthbuf_shader = 
		MeshShader(__MeshShader("attribute vec3 at_Vertex;\n uniform mat4 ModelViewProjectionMatrix;\n void main(void){gl_Position= ModelViewProjectionMatrix*vec4(at_Vertex, 1.0);}", "void main(){gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);}", __MeshShader::ComponentSet()));

	return 0;
}

