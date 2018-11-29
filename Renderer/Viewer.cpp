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

ScreenView::ScreenView() : nanogui::Screen(Eigen::Vector2i(1300, 900), "Render Viewer") 
{	
	nanogui::Window * window = nullptr;
	
	setRenderWidget(window);
	setBasicWidget(window);
	
	performLayout();

	m_render->loadDefaultAvatar();
}

ScreenView::~ScreenView() {
	
}

void ScreenView::setRenderWidget(nanogui::Window *window)
{
	window = new nanogui::Window(this, "Basic widgets");
	window->setPosition(nanogui::Vector2i(0, 0));
	window->setLayout(new nanogui::GroupLayout());

	m_render = new RenderView(window);
	m_render->setSize({ 1024, 768 });

	nanogui::Widget *tools = new nanogui::Widget(window);
	tools->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Middle, 0, 6));

	nanogui::Button *b = new nanogui::Button(tools, "Load Object");
	b->setCallback([&] {
		std::string obj_path = nanogui::file_dialog({ { "obj", "Object Mesh" }}, true);
		m_render->loadPrefab(obj_path);
	});

	b = new nanogui::Button(tools, "Clear");
	b->setCallback([&] {
		m_render->deletePrefabs();
		std::cout << "Clear" << std::endl;
	});
}

void ScreenView::setBasicWidget(nanogui::Window * window)
{
	window = new nanogui::Window(this, "Camera Setting");
	window->setPosition(nanogui::Vector2i(1060, 0));
	window->setLayout(new nanogui::GroupLayout());
	new nanogui::Label(window, "Camera", "sans-bold");

	Widget *panel = new Widget(window);
	panel->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Middle, 0, 20));

	nanogui::Slider *slider = new nanogui::Slider(panel);
	slider->setValue(0.5f);
	slider->setFixedWidth(80);
	nanogui::TextBox *textBox = new nanogui::TextBox(panel);
	textBox->setFixedSize(nanogui::Vector2i(60, 25));
	textBox->setValue("50");
	textBox->setUnits("%");
	slider->setCallback([textBox](float value) {
		textBox->setValue(std::to_string((int)(value * 100)));
	});
	slider->setFinalCallback([&](float value) {
		std::cout << "Final slider value: " << (int)(value * 100) << std::endl;
	});
	textBox->setFixedSize(nanogui::Vector2i(60, 25));
	textBox->setFontSize(20);
	textBox->setAlignment(nanogui::TextBox::Alignment::Right);
}

void ScreenView::setLightWidget(nanogui::Window * window)
{
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
	std::string vert_shader = "../resources/shaders/disney_brdf.vs";
	std::string frag_shader = "../resources/shaders/disney_brdf.fs";

	BRDFPrefab* prefab_face = new BRDFPrefab();
	prefab_face->compileAndLinkShader(vert_shader, frag_shader);
	prefab_face->loadObj("../resources/avatar/face.obj");
	prefab_face->loadTexture("../resources/avatar/face.jpg");
	prefab_face->setScene();
	m_prefabs.push_back(prefab_face);

	BRDFPrefab* prefab_body = new BRDFPrefab();
	prefab_body->compileAndLinkShader(vert_shader, frag_shader);
	prefab_body->loadObj("../resources/avatar/body_part.obj");
	prefab_body->loadTexture("../resources/avatar/body.jpg");
	prefab_body->setScene();
	m_prefabs.push_back(prefab_body);

	BRDFPrefab* prefab_cloth = new BRDFPrefab();
	prefab_cloth->compileAndLinkShader(vert_shader, frag_shader);
	prefab_cloth->loadObj("../resources/avatar/cloth/cloth.obj");
	prefab_cloth->loadTexture("../resources/avatar/cloth/texture.png");
	prefab_cloth->setScene();
	m_prefabs.push_back(prefab_cloth);

	return true;
}

void RenderView::deletePrefabs()
{
	m_prefabs.clear();
}
