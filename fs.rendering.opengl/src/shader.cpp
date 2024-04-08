#include "firesteel/rendering/renderer.hpp"
#ifdef OPENGL_RENDERING

#include "../include/common.hpp"
#include "firesteel/rendering/shader.hpp"
#include "firesteel/utils.hpp"

namespace firesteel {

    Shader::Shader() { id = -1; }

    Shader::Shader(const char* t_vert_shader_path, const char* t_frag_shader_path, bool t_load) : id(-1) {
        if(t_load) generate(t_vert_shader_path, t_frag_shader_path);
    }

    void Shader::generate(const char* t_vertShaderPath, const char* t_frag_shader_path) {
        int success = 0;
        char infoLog[512];
        //Compile vertex shader.
        GLuint vertShader = compile(t_vertShaderPath, GL_VERTEX_SHADER);
        GLuint fragShader = compile(t_frag_shader_path, GL_FRAGMENT_SHADER);
        //Create program.
        id = glCreateProgram();
        glAttachShader(id, vertShader);
        glAttachShader(id, fragShader);
        glLinkProgram(id);
        //Catch errors.
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 512, NULL, infoLog);
            LOG_ERRR(infoLog);
        }
        //Delete shaders.
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
    }

    void Shader::enable() {
        glUseProgram(id);
    }

    void Shader::disable() {
        glUseProgram(0);
    }

    void Shader::remove() {
        glDeleteProgram(id);
    }

    unsigned int Shader::compile(const char* t_path, unsigned int t_type) {
        int success = 0;
        char infoLog[512];
        //Create shader and read file.
        GLuint output = glCreateShader(t_type);
        std::string shaderSrc = read_from_file(t_path);
        const GLchar* shaderGL = shaderSrc.c_str();
        //Compile.
        glShaderSource(output, 1, &shaderGL, NULL);
        glCompileShader(output);
        //Catch errors.
        glGetShaderiv(output, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(output, 512, NULL, infoLog);
            LOG_ERRR(infoLog);
        }
        return output;
    }

    void Shader::set_mat4(std::string t_name, glm::mat4 t_mat) {
        glUniformMatrix4fv(glGetUniformLocation(id, t_name.c_str()), 1, GL_FALSE, glm::value_ptr(t_mat));
    }

    void Shader::set_mat3(std::string t_name, glm::mat3 t_mat) {
        glUniformMatrix3fv(glGetUniformLocation(id, t_name.c_str()), 1, GL_FALSE, glm::value_ptr(t_mat));
    }

    void Shader::set_bool(std::string t_name, bool t_val) {
        glUniform1i(glGetUniformLocation(id, t_name.c_str()), (int)t_val);
    }

    void Shader::set_int(std::string t_name, int t_val) {
        glUniform1i(glGetUniformLocation(id, t_name.c_str()), t_val);
    }

    void Shader::set_float(std::string t_name, float t_val) {
        glUniform1f(glGetUniformLocation(id, t_name.c_str()), t_val);
    }

    void Shader::set_2_floats(std::string t_name, float t_val1, float t_val2) {
        glUniform2f(glGetUniformLocation(id, t_name.c_str()), t_val1, t_val2);
    }

    void Shader::set_2_floats(std::string t_name, glm::vec2 t_vec) {
        glUniform2f(glGetUniformLocation(id, t_name.c_str()), t_vec.x, t_vec.y);
    }

    void Shader::set_3_floats(std::string t_name, float t_val1, float t_val2, float t_val3) {
        glUniform3f(glGetUniformLocation(id, t_name.c_str()), t_val1, t_val2, t_val3);
    }

    void Shader::set_3_floats(std::string t_name, glm::vec3 t_vec) {
        glUniform3f(glGetUniformLocation(id, t_name.c_str()), t_vec.x, t_vec.y, t_vec.z);
    }

    void Shader::set_4_floats(std::string t_name, float t_val1, float t_val2, float t_val3, float t_val4) {
        glUniform4f(glGetUniformLocation(id, t_name.c_str()), t_val1, t_val2, t_val3, t_val4);
    }

    void Shader::set_4_floats(std::string t_name, glm::vec4 t_vec) {
        glUniform4f(glGetUniformLocation(id, t_name.c_str()), t_vec.x, t_vec.y, t_vec.z, t_vec.w);
    }

}
#endif //OPENGL_RENDERING