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

		/* Vertex shader */
		prefab->mat.m_shader.m_vertex_shader
		,
		/* Fragment shader */
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
	
	m_shader.bind();
	m_shader.uploadIndices(indices);
	m_shader.uploadAttrib("position", positions);
	m_shader.setUniform("intensity", 1.0f);
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

	m_shader.setUniform("modelViewProj", mvp);
}

void RenderView::setObject()
{
}

bool RenderView::loadObject(std::string filepath)
{
	Prefab *prefab = new Prefab();
	prefab->loadObj(filepath);
	prefab->mat.m_shader.m_vertex_shader = prefab->loadShaderFile("../resources/shaders/basis.vert");
	prefab->mat.m_shader.m_fragment_shader = prefab->loadShaderFile("../resources/shaders/basis.frag");
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
