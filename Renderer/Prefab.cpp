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
	m_geo.m_mesh = new VBOMesh(filepath.c_str(), false, true, false);
	return true;
}


GLuint Prefab::loadTexture(std::string filepath)
{
	glActiveTexture(GL_TEXTURE0);
	m_mat.m_tex = cv::imread(filepath, CV_LOAD_IMAGE_COLOR);

	if (!m_mat.m_tex.data) {
		std::cout << "Could not open or find the image: " << filepath << std::endl;
		return 0;
	}
	glGenTextures(1, &m_mat.m_texID);
	glBindTexture(GL_TEXTURE_2D, m_mat.m_texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_mat.m_tex.size().width, m_mat.m_tex.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, m_mat.m_tex.ptr());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_mat.m_tex.release();
	m_prog.setUniform("tDiffuse", 0);
	return m_mat.m_texID;
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


void PhongPrefab::setScene()
{
	m_scene.model = glm::mat4(1.0f);
	m_scene.view = glm::lookAt(glm::vec3(0.0f, 150.0f, 250.0f), glm::vec3(0.0f, 100.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	m_scene.projection = glm::perspective(45.0f, (float)4 / 3, 1.0f, 1000.0f);

	m_prog.setUniform("Light.Position", glm::vec4(0.0f, 500.0f, 500.0f, 1.0f));
	m_prog.setUniform("Light.Intensity", glm::vec3(1.0f, 1.0f, 1.0f));
	m_prog.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
	m_prog.setUniform("Material.Ks", 0.1f, 0.1f, 0.1f);
	m_prog.setUniform("Material.Ka", 0.4f, 0.4f, 0.4f);
	m_prog.setUniform("Material.Shininess", 10.0f);

	glm::mat4 mv = m_scene.view * m_scene.model;
	m_prog.setUniform("ModelViewMatrix", mv);
	m_prog.setUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	m_prog.setUniform("MVP", m_scene.projection * mv);
}

bool PhongPrefab::rotateModel(float angle_x, float angle_y)
{
	m_scene.model *= glm::rotate(angle_x, glm::vec3(0.0f, 1.0f, 0.0f));
	m_scene.model *= glm::rotate(angle_y, glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 mv = m_scene.view * m_scene.model;
	m_prog.setUniform("ModelViewMatrix", mv);
	m_prog.setUniform("NormalMatrix", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	m_prog.setUniform("MVP", m_scene.projection * mv);
	return false;
}
