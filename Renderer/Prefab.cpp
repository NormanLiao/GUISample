#include "Prefab.h"
#include <fstream>
#include <algorithm>

Prefab::Prefab()
{
}

Prefab::~Prefab()
{
}

bool Prefab::loadObj(std::string filepath)
{
	m_geo.m_mesh = new VBOMesh(filepath.c_str());
	return true;
}


bool Prefab::loadTexture(std::string filepath)
{
	m_mat.m_tex = cv::imread(filepath, CV_LOAD_IMAGE_COLOR);

	if (!m_mat.m_tex.data) {
		std::cout << "Could not open or find the image: " << filepath << std::endl;
		return false;
	}
	
	return true;
	
}

bool Prefab::compileAndLinkShader(std::string vert_path, std::string frag_path)
{
	if (!m_prog.compileShaderFromFile(vert_path.c_str(), GLSLShader::VERTEX))
	{
		printf("Vertex shader failed to compile!\n%s", m_prog.log().c_str());
		return false;
	}
	if (!m_prog.compileShaderFromFile(frag_path.c_str(), GLSLShader::FRAGMENT))
	{
		printf("Fragment shader failed to compile!\n%s", m_prog.log().c_str());
		return false;
	}
	if (!m_prog.link())
	{
		printf("Shader program failed to link!\n%s", m_prog.log().c_str());
		return false;
	}
	m_prog.use();

	return true;
}

void Prefab::setScene()
{
	m_scene.model = glm::mat4(1.0f);
	m_scene.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 200.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_scene.projection = glm::perspective(45.0f, (float)4 / 3, 0.01f, 250.0f);

	m_prog.setUniform("Kd", 0.9f, 0.5f, 0.3f);
	m_prog.setUniform("Ld", 1.0f, 1.0f, 1.0f);
	m_prog.setUniform("LightPosition", m_scene.view * glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));

	glm::mat4 mv = m_scene.view * m_scene.model;
	m_prog.setUniform("ModelViewMatrix", mv);
	m_prog.setUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	m_prog.setUniform("MVP", m_scene.projection * mv);
}
