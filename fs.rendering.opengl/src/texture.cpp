#include "firesteel/rendering/renderer.hpp"
#ifdef OPENGL_RENDERING

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.hpp>
#include <assimp/material.h>

#include "../include/common.hpp"
#include "firesteel/rendering/texture.hpp"
#include <firesteel/utils.hpp>

namespace firesteel {

	Texture::Texture() : id(-1), type(aiTextureType_NONE) { }

	void Texture::generate() {
		glGenTextures(1, &id);
	}

	void Texture::load(bool t_flip) {
		//Load texture.
		int width, height, channels;
		char full_path[100];

		strcpy_s(full_path, path);
		if(!ends_with(path, "\\"))
		strcat_s(full_path, "\\");
		strcat_s(full_path, file);
		stbi_set_flip_vertically_on_load(t_flip);
		unsigned char* data = stbi_load(full_path, &width, &height, &channels, 4);
		if(!data) LOG_ERRR("stbi (Texture::load()): Couldn't load '", full_path, "'.");
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
			color_m_sup = GL_RGBA;
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

	void Texture::remove() const {
		glDeleteTextures(1, &id);
	}

}
#endif //OPENGL_RENDERING