#pragma once
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/nanogui.h>
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

	virtual bool mouseButtonEvent(const nanogui::Vector2i &p, int button, bool down, int modifiers) override {
		if (button == GLFW_MOUSE_BUTTON_1) {
			return true;
		}
		return false;
	}

	virtual bool mouseMotionEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) override {
		if (button == 1) {	
			float angle = rel.x() / 10.0f;
			for (unsigned int idx = 0; idx < m_prefabs.size(); idx++)
			{
				m_prefabs[idx]->m_prog.use();
				m_prefabs[idx]->rotateModel(-rel.x() / 10.0f, -rel.y() / 100.0f);
			}
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
			m_prefabs[idx]->m_prog.use();
			glBindTexture(GL_TEXTURE_2D, m_prefabs[idx]->m_mat.m_texID);
			m_prefabs[idx]->m_geo.m_mesh->render();
		}	
	}
	
private:
	std::vector<Prefab*> m_prefabs;
	nanogui::GLShader mShader;
};


class Viewer
{
	public:
		Viewer();
		~Viewer();
		bool initRenderView();
};