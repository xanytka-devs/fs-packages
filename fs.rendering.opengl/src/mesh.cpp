#include "firesteel/rendering/renderer.hpp"
#ifdef OPENGL_RENDERING

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../include/common.hpp"
#include "firesteel/rendering/mesh.hpp"

namespace firesteel {

	std::vector<Vertex> Vertex::generate_list(float* t_vertices, int t_vert_num) {
		//Setup stride.
		std::vector<Vertex> output(t_vert_num);
		int stride = sizeof(Vertex) / sizeof(float);
		//Iterate throught vertices.
		for (int i = 0; i < t_vert_num; i++) {
			//Parse positions.
			output[i].pos = glm::vec3(
				t_vertices[i * stride + 0],
				t_vertices[i * stride + 1],
				t_vertices[i * stride + 2]
			);
			//Parse normal.
			output[i].normal = glm::vec3(
				t_vertices[i * stride + 3],
				t_vertices[i * stride + 4],
				t_vertices[i * stride + 5]
			);
			//Parse UVs.
			output[i].uv = glm::vec2(
				t_vertices[i * stride + 6],
				t_vertices[i * stride + 7]
			);
		}
		return output;
	}

	static void average_vectors(glm::vec3& t_base, glm::vec3 t_addition, unsigned char t_existing) {
		if (!t_existing) t_base = t_addition;
		else {
			float f = 1 / ((float)t_existing + 1);

			t_base *= (float)(t_existing)*f;

			t_base += t_addition * f;
		}
	}

	void Vertex::calc_tan_vectors(std::vector<Vertex>& t_list, std::vector<unsigned int>& t_indices) {
		unsigned char* counts = (unsigned char*)malloc(t_list.size() * sizeof(unsigned char));
		for(unsigned int i = 0, len = t_list.size(); i < len; i++) counts[i] = 0;
		//Iterate through indices and calculate vectors for each face.
		for (unsigned int i = 0, len = t_indices.size(); i < len; i += 3) {
			//3 vertices corresponding to the face
			Vertex v1 = t_list[t_indices[i + 0]];
			Vertex v2 = t_list[t_indices[i + 1]];
			Vertex v3 = t_list[t_indices[i + 2]];
			//Calculate edges.
			glm::vec3 edge1 = v2.pos - v1.pos;
			glm::vec3 edge2 = v3.pos - v1.pos;
			//Calculate dUVs.
			glm::vec2 deltaUV1 = v2.uv - v1.uv;
			glm::vec2 deltaUV2 = v3.uv - v1.uv;
			//Use inverse of the UV matrix to determine tangent.
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			glm::vec3 tangent = {
				f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
				f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
				f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
			};
			//Average in the new tangent vector.
			average_vectors(t_list[t_indices[i + 0]].tangent, tangent, counts[t_indices[i + 0]]++);
			average_vectors(t_list[t_indices[i + 1]].tangent, tangent, counts[t_indices[i + 1]]++);
			average_vectors(t_list[t_indices[i + 2]].tangent, tangent, counts[t_indices[i + 2]]++);
		}
	}

	Mesh::Mesh(std::vector<Vertex> t_vertices, std::vector<unsigned int> t_indices,
		std::vector<Texture> t_textures)
		: vertices(t_vertices), indices(t_indices), textures(t_textures) {
		m_no_textures = (textures.size() == 0);
		setup();
	}

	Mesh::Mesh(std::vector<Vertex> t_vertices, std::vector<unsigned int> t_indices, 
	glm::vec4 t_diffuse, glm::vec4 t_specular, glm::vec4 t_emis)
		: vertices(t_vertices), indices(t_indices),
		diffuse(t_diffuse), specular(t_specular), emission(t_emis) {
		m_no_textures = true;
		setup();
	}

	void Mesh::setup() {
		//Generate buffers.
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		//Bind buffers.
		// VAO //
		glBindVertexArray(vao);
		// VBO //
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		// EBO //
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		//Attribute pointers.
		// POSITIONS //
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// Normals //
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// UVs //
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
		// Tangets //
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
		//Unbind vertex array.
		glBindVertexArray(0);
	}

	void Mesh::render(Shader t_shader) {
		t_shader.set_4_floats("material.diffuse", diffuse.r, diffuse.g, diffuse.b, diffuse.a);
		t_shader.set_4_floats("material.specular", specular.r, specular.g, specular.b, specular.a);
		t_shader.set_4_floats("material.emission", emission.r, emission.g, emission.b, emission.a);
		t_shader.set_int("no_textures", 1);
		if(!m_no_textures) {
			t_shader.set_int("no_textures", 0);
			//Load textures.
			unsigned int diffuse_idx = 0;
			unsigned int specular_idx = 0;
			unsigned int emission_idx = 0;
			for(unsigned int i = 0; i < textures.size(); i++) {
				//Activate texture.
				textures[i].enable();
				//Retrieve texture info.
				std::string name;
				switch(textures[i].type) {
				case aiTextureType_DIFFUSE:
					name = "diffuse" + std::to_string(diffuse_idx);
					diffuse_idx++;
					break;
				case aiTextureType_SPECULAR:
					name = "specular" + std::to_string(specular_idx);
					specular_idx++;
					break;
				case aiTextureType_EMISSIVE:
					name = "emission" + std::to_string(emission_idx);
					emission_idx++;
					break;
				}
				//Set shader value.
				t_shader.set_int(name.c_str(), textures[i].get_id());
				glActiveTexture(GL_TEXTURE0);
			}
		}
		//Enable buffers.
		glBindVertexArray(vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap_id);
		glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
		//Unbind active variables.
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Mesh::set_cubemap(unsigned int t_id) {
		m_cubemap_id = t_id;
	}

	void Mesh::remove() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);
		//Clear arrays.
		vertices.clear();
		indices.clear();
		for(unsigned int i = 0; i < textures.size(); i++)
			textures[i].remove();
		textures.clear();
	}

}
#endif //OPENGL_RENDERING