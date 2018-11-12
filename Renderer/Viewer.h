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
#include "Prefab.h"
#include "VBOMesh.h"
#include "GLSLProgram.h"

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (unsigned int idx=0; idx<m_prefabs.size(); idx++)
		{
			m_prefabs[idx]->m_geo.m_mesh->render();
		}
	}
	
private:
	std::vector<Prefab*> m_prefabs;
};


class Viewer
{
	public:
		Viewer();
		~Viewer();
		bool initRenderView();
};