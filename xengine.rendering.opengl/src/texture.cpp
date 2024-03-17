#include "xengine/rendering/renderer.hpp"
#ifdef OPENGL_RENDERING

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.hpp>
#include <assimp/material.h>

#include "../include/common.hpp"
#include "xengine/rendering/texture.hpp"

namespace XEngine {

	Texture::Texture() : id(-1), type(aiTextureType_NONE) { }

	void Texture::generate() {
		glGenTextures(1, &id);
	}

	void Texture::load(bool t_flip) {
		//Load texture.
		int width, height, channels;
		std::string full_path = path + "/" + file;
		stbi_set_flip_vertically_on_load(t_flip);
		unsigned char* data = stbi_load(full_path.c_str(), &width, &height, &channels, 3);
		if (!data) LOG_ERRR("stbi (Texture::load()): '" + full_path + "' not loaded.");
		//Set texture mode.
		GLenum color_m = GL_RGB;
		GLenum color_m_sup = GL_RGB;
		switch (channels) {
		case 1:
			color_m = GL_RED;
			color_m_sup = GL_RED;
			break;
		case 4:
			color_m = GL_RGBA;
			color_m_sup = GL_RGB;
			break;
		}
		//Generate mipmaps.
		if(data) {
			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, color_m, width, height, 0, color_m_sup, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		//Free data.
		stbi_image_free(data);
	}

	void Texture::enable() {
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void Texture::remove() {
		glDeleteTextures(1, &id);
	}

}
#endif //OPENGL_RENDERING