#include "xengine/rendering/renderer.hpp"
#ifdef OPENGL_RENDERING

#include "../include/common.hpp"

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

	bool Renderer::initialize_libs(GLFWwindow* t_window) {
		nvtx3::mark("opengl_initialize");
		//Load GLAD.
		return gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	}

	void Renderer::terminate() {
		nvtx3::mark("opengl_terminate");
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
		LOG("Rendering | OpenGL (GLAD & GLFW)");
		LOG("	Vendor: ", get_vendor());
		LOG("	Renderer: ", get_renderer());
		LOG("	Version: ", get_version());
	}

	const char* Renderer::get_vendor() {
		return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	}

	const char* Renderer::get_renderer() {
		return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	}

	const char* Renderer::get_version() {
		return reinterpret_cast<const char*>(glGetString(GL_VERSION));
	}
}
#endif // OPENGL_RENDERING