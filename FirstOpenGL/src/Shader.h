#pragma once
#include <string>
#include <unordered_map>

struct ProgramShaderSource {
	std::string vertex;
	std::string fragment;
};

class Shader
{
private:
	std::string filepath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, unsigned int> m_UniformLocationCache;


	int GetUniformLocation(const std::string& name);

	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	unsigned int CompileShader(const std::string& source, unsigned int type);

	ProgramShaderSource ParseProgramShader(const std::string& file);
public:
	Shader(const std::string& path);
	~Shader();

	void Bind() const;
	void Unbind() const;

	//Set uniforms
	void SetUniform4f(const std::string& name, float v0, float v1, float f2, float f3);


};