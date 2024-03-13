#include "xengine/rendering/renderer.hpp"
#ifdef OPENGL_RENDERING

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "xengine/log.hpp"

namespace XEngine {
	glm::vec4 Renderer::m_color(0.15f, 0.15f, 0.15f, 1);

	bool Renderer::is_available() {
		if(glfwInit()!=GLFW_TRUE) return false;
		glfwTerminate();
		return true;
	}

	void Renderer::initialize() {
		glfwInit();
		//Set version to 3.3.
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		//Set profile.
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	}

	bool Renderer::initialize_libs() {
		//Load GLAD.
		return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	}

	void Renderer::terminate() {
		//Terminate GLFW.
		glfwTerminate();
	}

	void Renderer::switch_mode(RenderMode t_mode) {
		switch (t_mode) {
		case DEFAULT:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
		case WIREFRAME:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;
		}
	}

	double Renderer::get_time() {
		return glfwGetTime();
	}

	void Renderer::print_host_info() {
		printf("Rendering | OpenGL (GLAD & GLFW)\n");
		printf(("	Vendor: " + get_vendor() + "\n").c_str());
		printf(("	Renderer: " + get_renderer() + "\n").c_str());
		printf(("	Version: " + get_version() + "\n").c_str());
	}

	std::string Renderer::get_vendor() {
		return std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	}

	std::string Renderer::get_renderer() {
		return std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	}

	std::string Renderer::get_version() {
		return std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	}
}
#endif //OPENGL_RENDERING