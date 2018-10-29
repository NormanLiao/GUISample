/*
src/example1.cpp -- C++ version of an example application that shows
how to use the various widget classes. For a Python implementation, see
'../python/example1.py'.

NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
The widget drawing code is based on the NanoVG demo application
by Mikko Mononen.

All rights reserved. Use of this source code is governed by a
BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <iostream>
#include <string>

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using std::string;
using std::vector;
using std::pair;
using std::to_string;




class GLTexture {
	public:
		using handleType = std::unique_ptr<uint8_t[], void(*)(void*)>;
		GLTexture() = default;
		GLTexture(const std::string& textureName) : mTextureName(textureName), mTextureId(0) {}

		GLTexture(const std::string& textureName, GLint textureId) : mTextureName(textureName), mTextureId(textureId) {}

		GLTexture(const GLTexture& other) = delete;
		GLTexture(GLTexture&& other) noexcept
			: mTextureName(std::move(other.mTextureName)),
			mTextureId(other.mTextureId) {
			other.mTextureId = 0;
		}
		GLTexture& operator=(const GLTexture& other) = delete;
		GLTexture& operator=(GLTexture&& other) noexcept {
			mTextureName = std::move(other.mTextureName);
			std::swap(mTextureId, other.mTextureId);
			return *this;
		}
		~GLTexture() noexcept {
			if (mTextureId)
				glDeleteTextures(1, &mTextureId);
		}

		GLuint texture() const { return mTextureId; }
		const std::string& textureName() const { return mTextureName; }

		/**
		*  Load a file in memory and create an OpenGL texture.
		*  Returns a handle type (an std::unique_ptr) to the loaded pixels.
		*/
		handleType load(const std::string& fileName) {
			if (mTextureId) {
				glDeleteTextures(1, &mTextureId);
				mTextureId = 0;
			}
			int force_channels = 0;
			int w, h, n;
			handleType textureData(stbi_load(fileName.c_str(), &w, &h, &n, force_channels), stbi_image_free);
			if (!textureData)
				throw std::invalid_argument("Could not load texture data from file " + fileName);
			glGenTextures(1, &mTextureId);
			glBindTexture(GL_TEXTURE_2D, mTextureId);
			GLint internalFormat;
			GLint format;
			switch (n) {
				case 1: internalFormat = GL_R8; format = GL_RED; break;
				case 2: internalFormat = GL_RG8; format = GL_RG; break;
				case 3: internalFormat = GL_RGB8; format = GL_RGB; break;
				case 4: internalFormat = GL_RGBA8; format = GL_RGBA; break;
				default: internalFormat = 0; format = 0; break;
			}
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, textureData.get());
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			return textureData;
		}

	private:
		std::string mTextureName;
		GLuint mTextureId;
};

class ExampleApplication : public nanogui::Screen {
	public:
		ExampleApplication() : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test") {
			using namespace nanogui;

			mShader.init(
				/* An identifying name */
				"a_simple_shader",

				/* Vertex shader */
				"#version 330\n"
				"uniform mat4 modelViewProj;\n"
				"in vec3 position;\n"
				"void main() {\n"
				"    gl_Position = modelViewProj * vec4(position, 1.0);\n"
				"}",

				/* Fragment shader */
				"#version 330\n"
				"out vec4 color;\n"
				"uniform float intensity;\n"
				"void main() {\n"
				"    color = vec4(vec3(intensity), 1.0);\n"
				"}"
			);

			MatrixXu indices(3, 2); /* Draw 2 triangles */
			indices.col(0) << 0, 1, 2;
			indices.col(1) << 2, 3, 0;

			MatrixXf positions(3, 4);
			positions.col(0) << -1, -1, 0;
			positions.col(1) << 1, -1, 0;
			positions.col(2) << 1, 1, 0;
			positions.col(3) << -1, 1, 0;

			mShader.bind();
			mShader.uploadIndices(indices);
			mShader.uploadAttrib("position", positions);
			mShader.setUniform("intensity", 0.5f);
		}

		~ExampleApplication() {
			mShader.free();
		}

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
			using namespace nanogui;

			/* Draw the window contents using OpenGL */
			mShader.bind();

			Matrix4f mvp;
			mvp.setIdentity();
			mvp.topLeftCorner<3, 3>() = Matrix3f(Eigen::AngleAxisf((float)glfwGetTime(), Vector3f::UnitZ())) * 0.25f;

			mvp.row(0) *= (float)mSize.y() / (float)mSize.x();

			mShader.setUniform("modelViewProj", mvp);

			/* Draw 2 triangles starting at index 0 */
			mShader.drawIndexed(GL_TRIANGLES, 0, 2);
		}
	private:
		nanogui::ProgressBar *mProgress;
		nanogui::GLShader mShader;

		using imagesDataType = vector<pair<GLTexture, GLTexture::handleType>>;
		imagesDataType mImagesData;
		int mCurrentImage;
};

int main(int /* argc */, char ** /* argv */) {
	try {
		nanogui::init();

		/* scoped variables */ {
			nanogui::ref<ExampleApplication> app = new ExampleApplication();
			app->drawAll();
			app->setVisible(true);
			nanogui::mainloop();
		}
		nanogui::shutdown();
	}
	catch (const std::runtime_error &e) {
		std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
		return -1;
	}
	return 0;
}
