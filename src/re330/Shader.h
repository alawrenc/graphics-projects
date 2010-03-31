#ifndef __Shader_h__
#define __Shader_h__

#include "RE330_global.h"

namespace RE330 {

	class RE330_EXPORT Shader
	{
	public:
		/** Load a vertex and fragment shader from file.
		*/
		Shader(char *vertexFileName, char *fragmentFileName);

		/** Activate the shader.
		*/
		void use();

		/** Disable the shader.
		*/
		void disable();

	private:
		char* readSource(char *fn);
		unsigned int v,f,p;
	};

}

#endif