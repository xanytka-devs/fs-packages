#include "firesteel/rendering/renderer.hpp"
#ifdef OPENGL_RENDERING

#include "firesteel/common.hpp"
#include "../include/common.hpp"
#include "firesteel/utils.hpp"
#include "firesteel/rendering/cubemap.hpp"
#include <stb/stb_image.hpp>

namespace firesteel {

    Cubemap::Cubemap()
        : m_has_textures(false), m_id(0), m_skybox_vao(0), m_skybox_vbo(0) {}

    void Cubemap::load_m(const char* t_dir,
        const char* t_right, const char* t_left,
        const char* t_top, const char* t_bottom,
        const char* t_front, const char* t_back) {
#ifdef XE_NTVX3
        nvtx3::scoped_range sky_load{ "cubemap_load" };
#endif // XE_NTVX3
        //Setup.
        m_dir = t_dir;
        m_has_textures = true;
        //m_faces = { t_left, t_right, t_top, t_bottom, t_front, t_back };
        m_faces = { t_front, t_back, t_top, t_bottom, t_right, t_left };
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
        //Load faces.
        int w, h, channels;
        for(unsigned int i = 0; i < 6; i++) {
            unsigned char* data = stbi_load((m_dir + "/" + m_faces[i]).c_str(),
                &w, &h, &channels, 0);
            //Get color mode.
            GLenum color_mode = GL_RED;
            switch (channels) {
            case 3:
                color_mode = GL_RGB;
                break;
            case 4:
                color_mode = GL_RGBA;
                break;
            }
            //Bind data.
            if(data)
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, color_mode, w, h, 0, color_mode, GL_UNSIGNED_BYTE, data);
            else
                LOG_ERRR("Failed to load texture at \"", (const char*)m_faces[i], "\".");
            //Free data.
            stbi_image_free(data);
        }
        //Communicate texture properties to OpenGL.
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void Cubemap::load(const char* t_cb_file_path) {
        std::vector<std::string> files = split_str(&read_from_file(t_cb_file_path), '\n');
        if(files.size() != 7) return;
        //Readress function.
        load_m(files[0].c_str(), files[1].c_str(), files[2].c_str(),
            files[3].c_str(), files[4].c_str(), files[5].c_str(), files[6].c_str());
    }

    void Cubemap::initialize(float t_size) {
        // set up vertices
        float skybox_vert[] = {
            // positions          
            -1.0f*t_size,  1.0f*t_size, -1.0f*t_size,
            -1.0f*t_size, -1.0f*t_size, -1.0f*t_size,
             1.0f*t_size, -1.0f*t_size, -1.0f*t_size,
             1.0f*t_size, -1.0f*t_size, -1.0f*t_size,
             1.0f*t_size,  1.0f*t_size, -1.0f*t_size,
            -1.0f*t_size,  1.0f*t_size, -1.0f*t_size,

            -1.0f*t_size, -1.0f*t_size,  1.0f*t_size,
            -1.0f*t_size, -1.0f*t_size, -1.0f*t_size,
            -1.0f*t_size,  1.0f*t_size, -1.0f*t_size,
            -1.0f*t_size,  1.0f*t_size, -1.0f*t_size,
            -1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
            -1.0f*t_size, -1.0f*t_size,  1.0f*t_size,

             1.0f*t_size, -1.0f*t_size, -1.0f*t_size,
             1.0f*t_size, -1.0f*t_size,  1.0f*t_size,
             1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
             1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
             1.0f*t_size,  1.0f*t_size, -1.0f*t_size,
             1.0f*t_size, -1.0f*t_size, -1.0f*t_size,

            -1.0f*t_size, -1.0f*t_size,  1.0f*t_size,
            -1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
             1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
             1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
             1.0f*t_size, -1.0f*t_size,  1.0f*t_size,
            -1.0f*t_size, -1.0f*t_size,  1.0f*t_size,

            -1.0f*t_size,  1.0f*t_size, -1.0f*t_size,
             1.0f*t_size,  1.0f*t_size, -1.0f*t_size,
             1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
             1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
            -1.0f*t_size,  1.0f*t_size,  1.0f*t_size,
            -1.0f*t_size,  1.0f*t_size, -1.0f*t_size,

            -1.0f*t_size, -1.0f*t_size, -1.0f*t_size,
            -1.0f*t_size, -1.0f*t_size,  1.0f*t_size,
             1.0f*t_size, -1.0f*t_size, -1.0f*t_size,
             1.0f*t_size, -1.0f*t_size, -1.0f*t_size,
            -1.0f*t_size, -1.0f*t_size,  1.0f*t_size,
             1.0f*t_size, -1.0f*t_size,  1.0f*t_size
        };
        //Create buffers and arrays.
        glGenVertexArrays(1, &m_skybox_vao);
        glGenBuffers(1, &m_skybox_vbo);
        glBindVertexArray(m_skybox_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_skybox_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vert), &skybox_vert, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    void Cubemap::render(Shader t_shader) {
        glDepthFunc(GL_LEQUAL);
        t_shader.enable();
        t_shader.set_mat4("view", Enviroment::get_current_scene()->get_camera()->get_view_matrix());
        t_shader.set_mat4("projection", Enviroment::get_current_scene()->get_camera()->get_projection_matrix());
        // skybox cube
        glBindVertexArray(m_skybox_vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

    void Cubemap::clear() {
        glDeleteTextures(1, &m_id);
        m_faces.clear();
    }

    void Cubemap::remove() {
        glDeleteVertexArrays(1, &m_skybox_vao);
        glDeleteBuffers(1, &m_skybox_vbo);
        clear();
    }
}
#endif //OPENGL_RENDERING