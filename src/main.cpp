#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}


GLuint compileShader(const char* path, GLenum type) {
    std::string src = loadShaderSource(path);
    const char* c_src = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &c_src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader error: " << log << std::endl;
    }

    return shader;
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    GLuint vertex = compileShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragment = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return program;
}

std::vector<float> generateGear(float radius, int teeth, float toothDepth) {
    std::vector<float> vertices;

    float angleStep = 2 * M_PI / (teeth * 2); // każda zębatka ma ząb + przerwę

    for (int i = 0; i < teeth * 2; ++i) {
        float angle1 = i * angleStep;
        float angle2 = (i + 1) * angleStep;

        float r1 = (i % 2 == 0) ? radius : radius - toothDepth;
        float r2 = ((i + 1) % 2 == 0) ? radius : radius - toothDepth;

        float x1 = cos(angle1) * r1;
        float y1 = sin(angle1) * r1;
        float x2 = cos(angle2) * r2;
        float y2 = sin(angle2) * r2;

        // Środek + dwa punkty brzegu
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);
        vertices.push_back(0.0f);

        vertices.push_back(x1);
        vertices.push_back(y1);
        vertices.push_back(0.0f);

        vertices.push_back(x2);
        vertices.push_back(y2);
        vertices.push_back(0.0f);
    }

    return vertices;
}



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Zegar", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Load OpenGL functions via glad
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {    
        std::cerr << "GLAD init failed\n";
        return -1;
    }

    auto gear1Vertices = generateGear(0.3f, 20, 0.05f); // większe koło
    auto gear2Vertices = generateGear(0.2f, 15, 0.04f); // mniejsze koło

    GLuint gear1VAO, gear1VBO;
    glGenVertexArrays(1, &gear1VAO);
    glGenBuffers(1, &gear1VBO);
    glBindVertexArray(gear1VAO);
    glBindBuffer(GL_ARRAY_BUFFER, gear1VBO);
    glBufferData(GL_ARRAY_BUFFER, gear1Vertices.size() * sizeof(float), gear1Vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    GLuint gear2VAO, gear2VBO;
    glGenVertexArrays(1, &gear2VAO);
    glGenBuffers(1, &gear2VBO);
    glBindVertexArray(gear2VAO);
    glBindBuffer(GL_ARRAY_BUFFER, gear2VBO);
    glBufferData(GL_ARRAY_BUFFER, gear2Vertices.size() * sizeof(float), gear2Vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);



    // Wskazówka sekundowa (dłuższa)
    float secVertices[] = {
        0.0f,  0.2f, 0.0f,
        -0.02f, -0.2f, 0.0f,
        0.02f, -0.2f, 0.0f
    };

    // Wskazówka minutowa (krótsza)
    float minVertices[] = {
        0.0f,  0.15f, 0.0f,
        -0.015f, -0.15f, 0.0f,
        0.015f, -0.15f, 0.0f
    };

    float hourVertices[] = {
     0.0f,  0.12f, 0.0f,
    -0.025f, -0.1f, 0.0f,
     0.025f, -0.1f, 0.0f
    };

    GLuint secVBO, secVAO;
    GLuint minVBO, minVAO;
    GLuint hourVAO, hourVBO;

    // Godziny
    glGenVertexArrays(1, &hourVAO);
    glGenBuffers(1, &hourVBO);

    glBindVertexArray(hourVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hourVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hourVertices), hourVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Sekundnik
    glGenVertexArrays(1, &secVAO);
    glGenBuffers(1, &secVBO);

    glBindVertexArray(secVAO);
    glBindBuffer(GL_ARRAY_BUFFER, secVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(secVertices), secVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Minutnik
    glGenVertexArrays(1, &minVAO);
    glGenBuffers(1, &minVBO);

    glBindVertexArray(minVAO);
    glBindBuffer(GL_ARRAY_BUFFER, minVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(minVertices), minVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);




GLuint shaderProgram = createShaderProgram("/Users/szymon/Documents/zegar/shaders/vertex.glsl", "/Users/szymon/Documents/zegar/shaders/fragment.glsl");
GLint colorLoc = glGetUniformLocation(shaderProgram, "color");

// Matryce
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1.0f));
glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);

while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    float currentTime = (float)glfwGetTime();

    glm::mat4 secModel = glm::rotate(glm::mat4(1.0f), currentTime, glm::vec3(0, 0, -1));
    glm::mat4 minModel = glm::rotate(glm::mat4(1.0f), currentTime / 60.0f, glm::vec3(0, 0, -1));
    glm::mat4 hourModel = glm::rotate(glm::mat4(1.0f), currentTime / 3600.0f, glm::vec3(0, 0, -1));

    glUseProgram(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    // Obracamy koła – jedno w prawo, drugie w lewo (z różną prędkością)
    glm::mat4 gear1Model = glm::rotate(glm::mat4(1.0f), currentTime, glm::vec3(0, 0, -1));
    glm::mat4 gear2Model = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.0f, 0.0f)); // obok pierwszego
    gear2Model = glm::rotate(gear2Model, -currentTime * 2.0f, glm::vec3(0, 0, -1));

    // Gear 1 (szare)
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gear1Model));
    glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
    glBindVertexArray(gear1VAO);
    glDrawArrays(GL_TRIANGLES, 0, gear1Vertices.size() / 3);

    // Gear 2 (ciemniejsze)
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(gear2Model));
    glUniform3f(colorLoc, 0.3f, 0.3f, 0.3f);
    glBindVertexArray(gear2VAO);
    glDrawArrays(GL_TRIANGLES, 0, gear2Vertices.size() / 3);



    // Rysuj sekundnik
    glBindVertexArray(secVAO);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(secModel));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Rysuj minutnik
    glBindVertexArray(minVAO);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(minModel));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Rysuj godziny
    glBindVertexArray(hourVAO);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(hourModel));
    glUniform3f(colorLoc, 0.2f, 0.3f, 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
}


    glfwTerminate();
    return 0;
}
