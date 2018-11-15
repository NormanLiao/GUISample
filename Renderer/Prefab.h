#pragma once
#include <string>
#include <vector>
#include <map>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform2.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include "GLSLProgram.h"
#include "VBOMesh.h"


struct Geometry {
	VBOMesh* m_mesh;
};


struct Material {
	cv::Mat m_tex;
	GLuint m_texID;
};

struct Scene {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;
};

class Prefab
{
	public:
		Prefab();
		~Prefab();
		bool loadObj(std::string filename);
		GLuint loadTexture(std::string filepath);
		bool compileAndLinkShader(std::string vert_path, std::string frag_path);
		virtual void setScene() = 0;
		virtual bool rotateModel(float angle_x, float angle_y) = 0;
		GLSLProgram m_prog;
		Geometry m_geo;
		Material m_mat;
		Scene m_scene;
};

class PhongPrefab : public Prefab {
	public:
		void setScene();
		bool rotateModel(float angle_x, float angle_y);
		glm::vec3 mat_ka;
		glm::vec3 mat_kd;
		glm::vec3 mat_ks;
		float shininess;
};