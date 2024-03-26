#include "xengine/rendering/renderer.hpp"
#ifdef OPENGL_RENDERING

#include "../include/common.hpp"
#include "xengine/rendering/framebuffer.hpp"

namespace XEngine {

	FrameBuffer::FrameBuffer() { }

	FrameBuffer::FrameBuffer(glm::vec2 t_scale, bool t_drawer) {
		//Generate framebuffer.
		glGenFramebuffers(1, &m_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		//Generate color buffer.
		glGenTextures(1, &m_texture_color_buffer);
		glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(t_scale.x), static_cast<GLsizei>(t_scale.y),
			0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_color_buffer, 0);
		//Generate RBO.
		glGenRenderbuffers(1, &m_rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(t_scale.x),
			static_cast<GLsizei>(t_scale.y));
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
		//Check status.
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			LOG_WARN("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
		//Unbind framebuffer.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		//Create VAO and VBO.
		if(!t_drawer) return;
		//Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		float vertices[] = {
		//		 POS		   UV
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void FrameBuffer::scale(glm::vec2 t_scale) {
		//Resize FBO.
		glBindTexture(GL_TEXTURE_2D, m_fbo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(t_scale.x), static_cast<GLsizei>(t_scale.y),
			0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo, 0);
		//Resize RBO.
		glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
			static_cast<GLsizei>(t_scale.x), static_cast<GLsizei>(t_scale.y));
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
	}

	void FrameBuffer::enable() {
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glEnable(GL_DEPTH_TEST);
	}

	void FrameBuffer::disable() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
	}

	void FrameBuffer::remove() const {
		glDeleteFramebuffers(1, &m_fbo);
		glDeleteTextures(1, &m_texture_color_buffer);
		glDeleteRenderbuffers(1, &m_rbo);
	}

	void FrameBuffer::draw_to(unsigned int t_object) {
		glBindVertexArray(t_object);
		glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
	}

	void FrameBuffer::draw(Shader t_screen_shader) {
		t_screen_shader.enable();
		glBindVertexArray(m_vao);
		glBindTexture(GL_TEXTURE_2D, m_texture_color_buffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

}

#endif // OPENGL_RENDERING