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
		{
			nanogui::ref<ScreenView> view = new ScreenView();
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

ScreenView::ScreenView() : nanogui::Screen(Eigen::Vector2i(1100, 900), "Render Viewer") 
{	
	nanogui::Window *window = new nanogui::Window(this, "Basic widgets");
	window->setPosition(nanogui::Vector2i(0, 0));
	window->setLayout(new nanogui::GroupLayout());

	m_render = new RenderView(window);
	m_render->setSize({ 1024, 768 });
	
	setBasicWidget(window);
	performLayout();
}

ScreenView::~ScreenView() {
	
}

void ScreenView::setBasicWidget(nanogui::Window *window)
{
	nanogui::Widget *tools = new nanogui::Widget(window);
	tools->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Middle, 0, 6));

	nanogui::Button *b = new nanogui::Button(tools, "Load Object");
	b->setCallback([&] {
		std::string obj_path = nanogui::file_dialog({ { "obj", "Object Mesh" }}, false);
		m_render->loadPrefab(obj_path);
	});
}

RenderView::RenderView(nanogui::Widget *parent) : nanogui::GLCanvas(parent)
{

}

RenderView::~RenderView()
{
}

bool RenderView::loadPrefab(std::string filename)
{
	Prefab* prefab = new Prefab();
	prefab->compileAndLinkShader("../resources/shaders/texture.vs", "../resources/shaders/texture.fs");
	prefab->loadObj(filename);
	prefab->loadTexture("../resources/images/test.jpg");
	prefab->setScene();
	m_prefabs.push_back(prefab);
	return true;
}
