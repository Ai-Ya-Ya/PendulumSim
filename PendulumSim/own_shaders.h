#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
	public:
		unsigned int ID;
		Shader(const char* vertexPath, const char* fragmentPath);
		void use();
		void setBool(const std::string& name, bool value) const;
		void setInt(const std::string& name, int value) const;
		void setFloat(const std::string& name, float value) const;
		void setVector2f(const std::string& name, glm::vec2 value) const;
		void setVector3f(const std::string& name, glm::vec3 value) const;
		void setVector4f(const std::string& name, glm::vec4 value) const;
		void setMatrix4(const std::string& name, const glm::mat4& matrix) const;
	private:
		void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
