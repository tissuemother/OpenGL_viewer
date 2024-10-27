#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

std::vector<glm::vec3> points;
glm::vec3 boundingBoxMin, boundingBoxMax;
int selectedPointIndex = -1;
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
float cameraYaw = 0.0f, cameraPitch = 0.0f;
float cameraDistance = 5.0f;
float cameraXY = 0.0f;
std::vector<glm::vec3> loadOBJ(const std::string& filename) {
    std::vector<glm::vec3> loadedPoints;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filename << "\n";
        return loadedPoints;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;
        if (type == "v") {  // vertex position
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            loadedPoints.push_back(vertex);
        }
    }

    file.close();
    return loadedPoints;
}

// 바운딩 박스를 계산하는 함수
void calculateBoundingBox() {
    boundingBoxMin = glm::vec3(std::numeric_limits<float>::max());
    boundingBoxMax = glm::vec3(std::numeric_limits<float>::lowest());

    for (const auto& point : points) {
        boundingBoxMin = glm::min(boundingBoxMin, point);
        boundingBoxMax = glm::max(boundingBoxMax, point);
    }
}

// 바운딩 박스를 렌더링하는 함수
void renderBoundingBox() {
    glColor3f(0.0f, 1.0f, 0.0f); // Color
    glBegin(GL_LINES);
    // Front face
    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z); glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z); glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z); glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z); glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z);

    // Back face
    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z); glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z); glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z); glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z); glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z);

    // Connect front and back faces
    glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z); glVertex3f(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z); glVertex3f(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z); glVertex3f(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z);
    glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z); glVertex3f(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z);
    glEnd();
}

// rendering function
void renderPointCloud() {
    glBegin(GL_POINTS);
    for (size_t i = 0; i < points.size(); ++i) {
        if (i == selectedPointIndex) glColor3f(1.0f, 0.0f, 0.0f);
        else glColor3f(1.0f, 1.0f, 1.0f); // white color
        glVertex3f(points[i].x, points[i].y, points[i].z);
    }
    glEnd();
}

// 조명 설정
void setupLighting() {
    GLfloat lightPos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int width, height;
        glfwGetWindowSize(window, &width, &height);

        float mouseX = (2.0f * xpos) / width - 1.0f;
        float mouseY = 1.0f - (2.0f * ypos) / height;
        std::cout<<mouseX<<"\t"<<mouseY<<std::endl;
        selectedPointIndex = -1;
        for (size_t i = 0; i < points.size(); ++i) {
            if (mouseX>0 && mouseY>0) {
                selectedPointIndex = i;
                break;
            }
        }
    }
}
// moving function
void Keyboardinput(GLFWwindow* window) {

    // point move
    if (selectedPointIndex != -1) {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) points[selectedPointIndex].y += 0.05f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) points[selectedPointIndex].y -= 0.05f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) points[selectedPointIndex].x += 0.05f;
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) points[selectedPointIndex].x -= 0.05f;

        calculateBoundingBox();
    }

    // 카메라 조정
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraDistance -= 0.1f; // 줌 인
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraDistance += 0.1f; // 줌 아웃
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraYaw -= 2.0f; // 왼쪽으로 회전
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraYaw += 2.0f; // 오른쪽으로 회전
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) cameraPitch += 2.0f; // 위로 회전
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) cameraPitch -= 2.0f; // 아래로 회전
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) cameraXY -= 0.2f;
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) cameraXY += 0.2f;
    
    
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Point Cloud Viewer with Bounding Box", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW\n";
        return -1;
    }

    points = loadOBJ("/Users/younf/Downloads/stanford-bunny.obj");
    calculateBoundingBox();
    
    glEnable(GL_DEPTH_TEST);
    glPointSize(5.0f);

    setupLighting();

    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 카메라 설정
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        cameraPosition = glm::vec3(cameraDistance * sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)),
                                    cameraDistance * sin(glm::radians(cameraPitch)),
                                    cameraDistance * cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch)));
        gluLookAt(cameraPosition.x, cameraPosition.y, cameraPosition.z, cameraXY, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        
        Keyboardinput(window);
        renderPointCloud();
        renderBoundingBox();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
