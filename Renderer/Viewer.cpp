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

RenderView::RenderView() : nanogui::Screen(Eigen::Vector2i(1024, 768), "Render Viewer") 
{	
	Prefab* prefab = new Prefab();
	prefab->compileAndLinkShader("../resources/shaders/texture.vs", "../resources/shaders/texture.fs");
	prefab->loadObj("../resources/eos_result.obj");
	prefab->loadTexture("../resources/images/test.jpg");
	prefab->setScene();
	m_prefabs.push_back(prefab);	
}

RenderView::~RenderView() {
	
}