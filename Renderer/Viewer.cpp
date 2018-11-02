#include "Viewer.h"



Viewer::Viewer()
{
}


Viewer::~Viewer()
{
}


bool Viewer::initRenderView()
{
	
	try {
		nanogui::init();

		/* scoped variables */ {
			nanogui::ref<RenderView> view = new RenderView();
			view->drawAll();
			view->setVisible(true);
			nanogui::mainloop();
		}
		nanogui::shutdown();
	}
	catch (const std::runtime_error &e) {
		std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
		return false;
	}
	return true;
}



RenderView::RenderView() : nanogui::Screen(Eigen::Vector2i(1024, 768), "Render Viewer") {
	
	loadObject("../resources/suzanne.obj");

	Prefab* prefab = m_prefabs[0];

	m_shader.init(
		/* An identifying name */
		"Basis Shader",
		prefab->mat.m_shader.m_vertex_shader ,
		prefab->mat.m_shader.m_fragment_shader
	);

	nanogui::MatrixXu indices(3, prefab->geo.m_faces.m_vertex_index.size());
	for (unsigned int idx=0; idx< prefab->geo.m_faces.m_vertex_index.size(); idx= idx+3)
	{
		indices.col(idx) << prefab->geo.m_faces.m_vertex_index[idx], prefab->geo.m_faces.m_vertex_index[idx+1], prefab->geo.m_faces.m_vertex_index[idx+2];
	}
	
	nanogui::MatrixXf positions(3, prefab->geo.m_pos.size());
	for (unsigned int idx=0; idx< prefab->geo.m_pos.size(); idx++) 
	{
		positions.col(idx) << prefab->geo.m_pos[idx].x, prefab->geo.m_pos[idx].y, prefab->geo.m_pos[idx].z;
	}
	
	nanogui::MatrixXf normals(3, prefab->geo.m_normals.size());
	for (unsigned int idx = 0; idx< prefab->geo.m_normals.size(); idx++)
	{
		normals.col(idx) << prefab->geo.m_normals[idx].x, prefab->geo.m_normals[idx].y, prefab->geo.m_normals[idx].z;
	}
	m_shader.bind();
	m_shader.uploadIndices(indices);
	m_shader.uploadAttrib("VertexPosition", positions);
	m_shader.uploadAttrib("VertexNormal", normals);
	setScene();
}

RenderView::~RenderView() {
	m_shader.free();
}

void RenderView::setScene()
{
	Eigen::Matrix4f model;
	model.setIdentity();

	Eigen::Matrix4f view = transferGLMtoEigen(glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	Eigen::Matrix4f projection = transferGLMtoEigen(glm::perspective(45.0f, (float)4 / 3, 0.01f, 12.0f));
	Eigen::Matrix4f mvp = projection * view * model;
	Eigen::Matrix4f mv = view * model;
	

	m_shader.setUniform("MVP", mvp);
	m_shader.setUniform("ModelViewMatrix", mv);
	m_shader.setUniform("NormalMatrix", Eigen::Matrix3f(mv.block(0,0,3,3)));

	Eigen::Matrix4f light_pos;
	light_pos.setIdentity();
	light_pos = view * light_pos;

	m_shader.setUniform("Material.Kd", Eigen::Vector3f(0.9f, 0.5f, 0.3f));
	m_shader.setUniform("Light.Ld", Eigen::Vector3f(1.0f, 1.0f, 1.0f));
	m_shader.setUniform("Light.Position", light_pos);
	m_shader.setUniform("Material.Ka", Eigen::Vector3f(0.9f, 0.5f, 0.3f));
	m_shader.setUniform("Light.La", Eigen::Vector3f(0.8f, 0.8f, 0.8f));
	m_shader.setUniform("Material.Ks", Eigen::Vector3f(0.9f, 0.5f, 0.3f));
	m_shader.setUniform("Light.Ls", Eigen::Vector3f(1.0f, 1.0f, 1.0f));
	m_shader.setUniform("Material.Shininess", 100.0f);
	
}

void RenderView::setObject()
{
}

bool RenderView::loadObject(std::string filepath)
{
	Prefab *prefab = new Prefab();
	prefab->loadObj(filepath);
	prefab->mat.m_shader.m_vertex_shader = prefab->loadShaderFile("../resources/shaders/phong.vert");
	prefab->mat.m_shader.m_fragment_shader = prefab->loadShaderFile("../resources/shaders/phong.frag");
	m_prefabs.push_back(prefab);
	return true;
}

Eigen::Matrix4f RenderView::transferGLMtoEigen(glm::mat4 intput)
{
	Eigen::Matrix4f output;
	output.setZero();
	for (unsigned int idx1=0 ; idx1 < 4 ; idx1++) 
	{
		for (unsigned int idx2 = 0; idx2 < 4; idx2++)
		{
			output(idx1, idx2) = intput[idx2][idx1];
		}
	}
	return output;
}
