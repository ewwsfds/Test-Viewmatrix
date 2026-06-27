#include <windows.h>
#include <iostream>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


const char* vertexShaderSource = R"(
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, TexCoord);
}
)";


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>



///GLOBALS
///GLOBALS

LPCVOID address = (LPCVOID)0x000000E83891EE30;
DWORD pid = 35096; // example process ID



///END OF GLOBALs





struct FindWindowData
{
    DWORD pid;
    HWND hwnd;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    FindWindowData* data = (FindWindowData*)lParam;

    DWORD windowPid;
    GetWindowThreadProcessId(hwnd, &windowPid);

    if (windowPid == data->pid && IsWindowVisible(hwnd))
    {
        data->hwnd = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND GetWindowFromPID(DWORD pid)
{
    FindWindowData data{};
    data.pid = pid;
    data.hwnd = nullptr;

    EnumWindows(EnumWindowsProc, (LPARAM)&data);

    return data.hwnd;
}




struct Player
{
    glm::vec3 position;
    glm::vec3 rotation; // degrees
    glm::vec3 scale;
};

struct Camera
{
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;

    float yaw;
    float pitch;
};



Player player;


Camera camera;


int windowWidth = 1920;
int windowHeight = 1200;

glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),
    (float)windowWidth / (float)windowHeight,
    0.1f,
    100.0f
);





void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, windowWidth, windowHeight);
}


void UpdateCameraMouse(GLFWwindow* window)
{
    static bool firstMouse = true;
    static double lastX;
    static double lastY;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse)
    {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    float xOffset = (float)(mouseX - lastX);
    float yOffset = (float)(lastY - mouseY);

    lastX = mouseX;
    lastY = mouseY;

    const float sensitivity = 0.1f;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera.yaw += xOffset;
    camera.pitch += yOffset;

    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;

    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    glm::vec3 forward;

    forward.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    forward.y = sin(glm::radians(camera.pitch));
    forward.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));

    camera.forward = glm::normalize(forward);

    glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

    camera.right = glm::normalize(glm::cross(camera.forward, worldUp));
    camera.up = glm::normalize(glm::cross(camera.right, camera.forward));
}

int main()
{



    HANDLE hProcess = OpenProcess(
        PROCESS_ALL_ACCESS, // access rights
        FALSE,
        pid
    );

    if (hProcess == NULL) {
        std::cout << "Failed to open process\n";
        return 0;
    }
    else {
        std::cout << "Process opened!\n";
    }



    HWND gameWindow = GetWindowFromPID(pid);

    if (!gameWindow)
    {
        std::cout << "Failed to find game window\n";
        return 0;
    }



    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE); // optional (no border)

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "First Triangle", nullptr, nullptr);

    if (!window)
    {
        std::cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float vertices[] = {
        // pos            // uv
        -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, 0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, 0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, 0.5f,  0.0f, 1.0f
    };


    unsigned int indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };



    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // UV attribute
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        5 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);



    // Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);



    player.position = glm::vec3(0.0f, 0.0f, 0.0f);
    player.rotation = glm::vec3(0.0f);
    player.scale = glm::vec3(0.2f);





    camera.position = glm::vec3(0.0f, 0.0f, 3.7f);

    camera.yaw = -90.0f;
    camera.pitch = 0.0f;

    camera.forward = glm::vec3(0, 0, -1);
    camera.right = glm::vec3(1, 0, 0);
    camera.up = glm::vec3(0, 1, 0);



    HWND hwnd = glfwGetWin32Window(window);

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

    SetWindowLong(
        hwnd,
        GWL_EXSTYLE,
        exStyle | WS_EX_LAYERED | WS_EX_TRANSPARENT
    );


    SetWindowPos(
        hwnd,
        HWND_TOPMOST,
        0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE
    );



    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load("image.png", &width, &height, &channels, 0);

    if (!data)
    {
        std::cout << "Failed to load texture\n";
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);



        float viewMatrix[16];

        // example memory address (must be valid in target process)

        SIZE_T bytesRead = 0;

        ReadProcessMemory(
            hProcess,
            address,
            viewMatrix,
            sizeof(viewMatrix),
            &bytesRead
        );








        glm::mat4 view = glm::make_mat4(viewMatrix);




        glm::mat4 model(1.0f);

        model = glm::translate(model, player.position);

        model = glm::rotate(model,
            glm::radians(player.rotation.x),
            glm::vec3(1, 0, 0));

        model = glm::rotate(model,
            glm::radians(player.rotation.y),
            glm::vec3(0, 1, 0));

        model = glm::rotate(model,
            glm::radians(player.rotation.z),
            glm::vec3(0, 0, 1));

        model = glm::scale(model, player.scale);



        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "model"),
            1,
            GL_FALSE,
            glm::value_ptr(model));

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "view"),
            1,
            GL_FALSE,
            glm::value_ptr(view));

        glUniformMatrix4fv(
            glGetUniformLocation(shaderProgram, "projection"),
            1,
            GL_FALSE,
            glm::value_ptr(projection));



        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    CloseHandle(hProcess);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
