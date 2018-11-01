#pragma once
#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <iostream>
#include <string>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/transform2.hpp>
#include <Eigen/core>
#include "Prefab.h"

using std::string;
using std::vector;
using std::pair;
using std::to_string;


class RenderView : public nanogui::Screen {
public:
	RenderView();
	~RenderView();

	virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
		if (Screen::keyboardEvent(key, scancode, action, modifiers))
			return true;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			setVisible(false);
			return true;
		}
		return false;
	}

	virtual void draw(NVGcontext *ctx) {
		/* Draw the user interface */
		Screen::draw(ctx);
	}

	virtual void drawContents() {
		/* Draw the window contents using OpenGL */
		m_shader.drawIndexed(GL_TRIANGLES, 0, m_prefabs[0]->geo.m_faces.m_vertex_index.size());
	}
	
private:
	void setScene();
	void setObject();
	bool loadObject(std::string filepath);

	nanogui::GLShader m_shader;
	std::vector<Prefab*> m_prefabs;
};


class Viewer
{
	public:
		Viewer();
		~Viewer();
		bool initRenderView();
};