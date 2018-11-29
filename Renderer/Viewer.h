#pragma once
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/nanogui.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include "Prefab.h"
#include "VBOMesh.h"
#include "GLSLProgram.h"

using std::string;
using std::vector;
using std::pair;
using std::to_string;


class RenderView : public nanogui::GLCanvas
{
public:
	RenderView(nanogui::Widget *parent);
	~RenderView();

	virtual bool mouseMotionEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) override {
		if (button == GLFW_MOUSE_BUTTON_2) {
			float angle = rel.x() / 100.0f;
			for (unsigned int idx = 0; idx < m_prefabs.size(); idx++)
			{
				m_prefabs[idx]->m_prog.use();
				m_prefabs[idx]->rotateModel(-rel.x() / 500.0f, -rel.y() / 5000.0f);
			}
			return true;
		}
		return false;
	}

	virtual void drawGL() override {
		/* Draw the window contents using OpenGL */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		for (unsigned int idx = 0; idx<m_prefabs.size(); idx++)
		{
			m_prefabs[idx]->m_prog.use();
			glBindTexture(GL_TEXTURE_2D, m_prefabs[idx]->m_mat.m_texID);
			m_prefabs[idx]->m_geo.m_mesh->render();
		}
		glDisable(GL_DEPTH_TEST);
	}

	bool loadPrefab(std::string filename);
	bool loadDefaultAvatar();
	void deletePrefabs();
private:
	std::vector<Prefab*> m_prefabs;
};

class ScreenView : public nanogui::Screen {
public:
	ScreenView();
	~ScreenView();

	virtual void draw(NVGcontext *ctx) {
		/* Draw the user interface */
		Screen::draw(ctx);
	}

	virtual void drawContents() {
	}
	
private:
	RenderView *m_render;
	void setRenderWidget(nanogui::Window *window);
	void setBasicWidget(nanogui::Window *window);
	void setLightWidget(nanogui::Window *window);
};



class Viewer
{
	public:
		Viewer();
		~Viewer();
		bool initRenderView();
};