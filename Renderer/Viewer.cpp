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

	m_render->loadDefaultAvatar();
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
	PhongPrefab* prefab = new PhongPrefab();
	prefab->compileAndLinkShader("../resources/shaders/texture.vs", "../resources/shaders/texture.fs");
	prefab->loadObj(filename);
	prefab->loadTexture("../resources/images/test.jpg");
	prefab->setScene();
	m_prefabs.push_back(prefab);
	return true;
}

bool RenderView::loadDefaultAvatar()
{
	std::string vert_shader = "../resources/shaders/texture.vs";
	std::string frag_shader = "../resources/shaders/texture.fs";

	PhongPrefab* prefab_face = new PhongPrefab();
	prefab_face->compileAndLinkShader(vert_shader, frag_shader);
	prefab_face->loadObj("../resources/avatar/face.obj");
	prefab_face->loadTexture("../resources/avatar/face.jpg");
	prefab_face->setScene();
	m_prefabs.push_back(prefab_face);

	PhongPrefab* prefab_body = new PhongPrefab();
	prefab_body->compileAndLinkShader(vert_shader, frag_shader);
	prefab_body->loadObj("../resources/avatar/body_part.obj");
	prefab_body->loadTexture("../resources/avatar/body.jpg");
	prefab_body->setScene();
	m_prefabs.push_back(prefab_body);

	PhongPrefab* prefab_cloth = new PhongPrefab();
	prefab_cloth->compileAndLinkShader(vert_shader, frag_shader);
	prefab_cloth->loadObj("../resources/avatar/cloth/cloth.obj");
	prefab_cloth->loadTexture("../resources/avatar/cloth/texture.png");
	prefab_cloth->setScene();
	m_prefabs.push_back(prefab_cloth);

	return true;
}
