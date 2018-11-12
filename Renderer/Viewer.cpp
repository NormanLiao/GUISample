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


	nanogui::Window *window = new nanogui::Window(this, "Basic widgets");
    window->setPosition(nanogui::Vector2i(15, 15));
    window->setLayout(new nanogui::GroupLayout());

	nanogui::Widget *tools = new Widget(window);
	tools->setLayout(new nanogui::BoxLayout(nanogui::Orientation::Horizontal, nanogui::Alignment::Middle, 0, 6));

	nanogui::Button *b = new nanogui::Button(tools, "Open");
	b->setCallback([&] {
		std::cout << "File dialog result: " << nanogui::file_dialog( { { "jpg", "JPEG" }, { "png", "Portable Network Graphics" },{ "txt", "Text file" } }, false) << std::endl;
	});
	performLayout();
	
	Prefab* prefab = new Prefab();
	prefab->compileAndLinkShader("../resources/shaders/texture.vs", "../resources/shaders/texture.fs");
	prefab->loadObj("../resources/eos_result.obj");
	prefab->loadTexture("../resources/images/test.jpg");
	prefab->setScene();
	m_prefabs.push_back(prefab);	

}

RenderView::~RenderView() {
	
}
