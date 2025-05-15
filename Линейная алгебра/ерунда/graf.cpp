#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int WIDTH = 800;
const int HEIGHT = 600;

// Параметры поверхности
float a = 1.0f;
float b = 1.1f;
const int N = 50;
const int N2 = 50;

// Параметры камеры и проекции
float cameraPos[3] = { 0.0f, -10.0f, 6.0f };
float cameraTarget[3] = { 0.0f, 0.0f, 0.0f };
float cameraUp[3] = { 0.0f, 0.0f, 1.0f };
float cameraSpeed = 0.5f;
float rotationSpeed = 0.02f;
float zoom = 1.0f;

// Матрица проекции (2x3)
float projectionMatrix[2][3] = {
    {1.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f}
};

struct Point2D {
    float x, y;
    float r, g, b;
};

struct Point3D {
    float x, y, z;
};

Point2D projectPoint(const Point3D& p3d) {
    Point2D p2d;
    p2d.x = projectionMatrix[0][0] * p3d.x + projectionMatrix[0][1] * p3d.y + projectionMatrix[0][2] * p3d.z;
    p2d.y = projectionMatrix[1][0] * p3d.x + projectionMatrix[1][1] * p3d.y + projectionMatrix[1][2] * p3d.z;
    return p2d;
}

Point3D applyCameraTransform(const Point3D& p) {
    Point3D result;

    // Вектор взгляда камеры
    float forward[3] = {
        cameraTarget[0] - cameraPos[0],
        cameraTarget[1] - cameraPos[1],
        cameraTarget[2] - cameraPos[2]
    };

    // Нормализуем
    float len = sqrt(forward[0] * forward[0] + forward[1] * forward[1] + forward[2] * forward[2]);
    if (len > 0) {
        forward[0] /= len;
        forward[1] /= len;
        forward[2] /= len;
    }

    // Вектор вправо (перпендикулярно up и forward)
    float right[3] = {
        forward[1] * cameraUp[2] - forward[2] * cameraUp[1],
        forward[2] * cameraUp[0] - forward[0] * cameraUp[2],
        forward[0] * cameraUp[1] - forward[1] * cameraUp[0]
    };

    // Пересчитываем up, чтобы быть ортогональным
    float up[3] = {
        right[1] * forward[2] - right[2] * forward[1],
        right[2] * forward[0] - right[0] * forward[2],
        right[0] * forward[1] - right[1] * forward[0]
    };

    // Вектор от точки к камере
    float translated[3] = {
        p.x - cameraPos[0],
        p.y - cameraPos[1],
        p.z - cameraPos[2]
    };

    // Проекция на оси камеры
    result.x = right[0] * translated[0] + right[1] * translated[1] + right[2] * translated[2];
    result.y = up[0] * translated[0] + up[1] * translated[1] + up[2] * translated[2];
    result.z = forward[0] * translated[0] + forward[1] * translated[1] + forward[2] * translated[2];

    return result;
}

float calculateBrightness(const Point3D& p1, const Point3D& p2, const Point3D& p3) {
    // Вычисляем нормаль к поверхности
    float edge1[3] = { p2.x - p1.x, p2.y - p1.y, p2.z - p1.z };
    float edge2[3] = { p3.x - p1.x, p3.y - p1.y, p3.z - p1.z };

    float normal[3] = {
        edge1[1] * edge2[2] - edge1[2] * edge2[1],
        edge1[2] * edge2[0] - edge1[0] * edge2[2],
        edge1[0] * edge2[1] - edge1[1] * edge2[0]
    };

    // Нормализуем нормаль
    float len = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
    if (len > 0) {
        normal[0] /= len;
        normal[1] /= len;
        normal[2] /= len;
    }

    // Вектор к источнику света (от точки к камере)
    float lightDir[3] = {
        cameraPos[0] - (p1.x + p2.x + p3.x) / 3,
        cameraPos[1] - (p1.y + p2.y + p3.y) / 3,
        cameraPos[2] - (p1.z + p2.z + p3.z) / 3
    };

    // Нормализуем
    len = sqrt(lightDir[0] * lightDir[0] + lightDir[1] * lightDir[1] + lightDir[2] * lightDir[2]);
    if (len > 0) {
        lightDir[0] /= len;
        lightDir[1] /= len;
        lightDir[2] /= len;
    }

    // Косинус угла между нормалью и направлением света
    float dot = normal[0] * lightDir[0] + normal[1] * lightDir[1] + normal[2] * lightDir[2];
    return 0.2f + 0.8f * std::max(0.0f, dot); // ambient + diffuse
}

void processInput(GLFWwindow* window) {
    static float lastTime = glfwGetTime();
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Движение камеры с учетом deltaTime для плавности
    float moveSpeed = cameraSpeed * deltaTime * 60.0f;
    float rotateSpeed = rotationSpeed * deltaTime * 60.0f;

    // Вектор направления камеры
    float forward[3] = {
        cameraTarget[0] - cameraPos[0],
        cameraTarget[1] - cameraPos[1],
        cameraTarget[2] - cameraPos[2]
    };

    // Нормализуем
    float len = sqrt(forward[0] * forward[0] + forward[1] * forward[1] + forward[2] * forward[2]);
    if (len > 0) {
        forward[0] /= len;
        forward[1] /= len;
        forward[2] /= len;
    }

    // Вектор вправо
    float right[3] = {
        forward[1] * cameraUp[2] - forward[2] * cameraUp[1],
        forward[2] * cameraUp[0] - forward[0] * cameraUp[2],
        forward[0] * cameraUp[1] - forward[1] * cameraUp[0]
    };

    // Нормализуем right
    len = sqrt(right[0] * right[0] + right[1] * right[1] + right[2] * right[2]);
    if (len > 0) {
        right[0] /= len;
        right[1] /= len;
        right[2] /= len;
    }

    // Движение вперед/назад
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos[0] += forward[0] * moveSpeed;
        cameraPos[1] += forward[1] * moveSpeed;
        cameraPos[2] += forward[2] * moveSpeed;
        cameraTarget[0] += forward[0] * moveSpeed;
        cameraTarget[1] += forward[1] * moveSpeed;
        cameraTarget[2] += forward[2] * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos[0] -= forward[0] * moveSpeed;
        cameraPos[1] -= forward[1] * moveSpeed;
        cameraPos[2] -= forward[2] * moveSpeed;
        cameraTarget[0] -= forward[0] * moveSpeed;
        cameraTarget[1] -= forward[1] * moveSpeed;
        cameraTarget[2] -= forward[2] * moveSpeed;
    }

    // Движение влево/вправо
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos[0] -= right[0] * moveSpeed;
        cameraPos[1] -= right[1] * moveSpeed;
        cameraPos[2] -= right[2] * moveSpeed;
        cameraTarget[0] -= right[0] * moveSpeed;
        cameraTarget[1] -= right[1] * moveSpeed;
        cameraTarget[2] -= right[2] * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos[0] += right[0] * moveSpeed;
        cameraPos[1] += right[1] * moveSpeed;
        cameraPos[2] += right[2] * moveSpeed;
        cameraTarget[0] += right[0] * moveSpeed;
        cameraTarget[1] += right[1] * moveSpeed;
        cameraTarget[2] += right[2] * moveSpeed;
    }

    // Вращение камеры вокруг цели
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        // Вращение вокруг вертикальной оси (Y)
        float dx = cameraPos[0] - cameraTarget[0];
        float dz = cameraPos[2] - cameraTarget[2];
        float newDx = dx * cos(rotateSpeed) - dz * sin(rotateSpeed);
        float newDz = dx * sin(rotateSpeed) + dz * cos(rotateSpeed);
        cameraPos[0] = cameraTarget[0] + newDx;
        cameraPos[2] = cameraTarget[2] + newDz;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        // Вращение вокруг вертикальной оси (Y)
        float dx = cameraPos[0] - cameraTarget[0];
        float dz = cameraPos[2] - cameraTarget[2];
        float newDx = dx * cos(-rotateSpeed) - dz * sin(-rotateSpeed);
        float newDz = dx * sin(-rotateSpeed) + dz * cos(-rotateSpeed);
        cameraPos[0] = cameraTarget[0] + newDx;
        cameraPos[2] = cameraTarget[2] + newDz;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        // Вращение вокруг горизонтальной оси (X)
        float dy = cameraPos[1] - cameraTarget[1];
        float dz = cameraPos[2] - cameraTarget[2];
        float newDy = dy * cos(rotateSpeed) - dz * sin(rotateSpeed);
        float newDz = dy * sin(rotateSpeed) + dz * cos(rotateSpeed);
        cameraPos[1] = cameraTarget[1] + newDy;
        cameraPos[2] = cameraTarget[2] + newDz;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        // Вращение вокруг горизонтальной оси (X)
        float dy = cameraPos[1] - cameraTarget[1];
        float dz = cameraPos[2] - cameraTarget[2];
        float newDy = dy * cos(-rotateSpeed) - dz * sin(-rotateSpeed);
        float newDz = dy * sin(-rotateSpeed) + dz * cos(-rotateSpeed);
        cameraPos[1] = cameraTarget[1] + newDy;
        cameraPos[2] = cameraTarget[2] + newDz;
    }

    // Изменение параметров поверхности
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) a += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) a -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) b += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) b -= 0.01f;

    // Изменение проекции
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        // Ортографическая проекция
        projectionMatrix[0][0] = 1; projectionMatrix[0][1] = 0; projectionMatrix[0][2] = 0;
        projectionMatrix[1][0] = 0; projectionMatrix[1][1] = 1; projectionMatrix[1][2] = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        // Изометрическая проекция
        projectionMatrix[0][0] = sqrt(3); projectionMatrix[0][1] = 0; projectionMatrix[0][2] = -sqrt(3);
        projectionMatrix[1][0] = 1; projectionMatrix[1][1] = 2; projectionMatrix[1][2] = 1;
        float norm = sqrt(6);
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                projectionMatrix[i][j] /= norm;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        // Перспективная проекция
        projectionMatrix[0][0] = 1; projectionMatrix[0][1] = 0; projectionMatrix[0][2] = 0;
        projectionMatrix[1][0] = 0; projectionMatrix[1][1] = 1; projectionMatrix[1][2] = 0.2f;
    }
}

std::vector<std::vector<Point2D>> createSurface() {
    std::vector<std::vector<Point2D>> surface;

    for (int i = 0; i <= N; ++i) {
        float u = 4 * M_PI * i / N;
        std::vector<Point2D> row;

        for (int j = 0; j <= N2; ++j) {
            float v = 2 * M_PI * j / N2;

            Point3D p3d;
            float c = a + b * cos(v);
            p3d.x = c * cos(u);
            p3d.y = c * sin(u);
            p3d.z = b * sin(v) + a * u;

            Point3D transformed = applyCameraTransform(p3d);
            Point2D p2d = projectPoint(transformed);

            p2d.x = p2d.x * zoom * 50 + WIDTH / 2;
            p2d.y = p2d.y * zoom * 50 + HEIGHT / 2;

            p2d.r = 0.5f;
            p2d.g = 0.7f;
            p2d.b = 1.0f;

            row.push_back(p2d);
        }

        surface.push_back(row);
    }

    return surface;
}

void drawSurface(GLFWwindow* window, const std::vector<std::vector<Point2D>>& surface) {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N2; ++j) {
            const Point2D& p1 = surface[i][j];
            const Point2D& p2 = surface[i + 1][j];
            const Point2D& p3 = surface[i + 1][j + 1];
            const Point2D& p4 = surface[i][j + 1];

            // Восстанавливаем оригинальные 3D координаты
            Point3D p3d1, p3d2, p3d3, p3d4;
            float u = 4 * M_PI * i / N;
            float v = 2 * M_PI * j / N2;
            float c = a + b * cos(v);
            p3d1.x = c * cos(u);
            p3d1.y = c * sin(u);
            p3d1.z = b * sin(v) + a * u;

            u = 4 * M_PI * (i + 1) / N;
            p3d2.x = (a + b * cos(v)) * cos(u);
            p3d2.y = (a + b * cos(v)) * sin(u);
            p3d2.z = b * sin(v) + a * u;

            v = 2 * M_PI * (j + 1) / N2;
            p3d3.x = (a + b * cos(v)) * cos(u);
            p3d3.y = (a + b * cos(v)) * sin(u);
            p3d3.z = b * sin(v) + a * u;

            u = 4 * M_PI * i / N;
            p3d4.x = (a + b * cos(v)) * cos(u);
            p3d4.y = (a + b * cos(v)) * sin(u);
            p3d4.z = b * sin(v) + a * u;

            // Освещение от камеры
            float brightness1 = calculateBrightness(p3d1, p3d2, p3d4);
            float brightness2 = calculateBrightness(p3d2, p3d3, p3d4);

            // Рисуем первый треугольник
            glBegin(GL_TRIANGLES);
            glColor3f(p1.r * brightness1, p1.g * brightness1, p1.b * brightness1);
            glVertex2f(p1.x, p1.y);
            glColor3f(p2.r * brightness1, p2.g * brightness1, p2.b * brightness1);
            glVertex2f(p2.x, p2.y);
            glColor3f(p4.r * brightness1, p4.g * brightness1, p4.b * brightness1);
            glVertex2f(p4.x, p4.y);
            glEnd();

            // Рисуем второй треугольник
            glBegin(GL_TRIANGLES);
            glColor3f(p2.r * brightness2, p2.g * brightness2, p2.b * brightness2);
            glVertex2f(p2.x, p2.y);
            glColor3f(p3.r * brightness2, p3.g * brightness2, p3.b * brightness2);
            glVertex2f(p3.x, p3.y);
            glColor3f(p4.r * brightness2, p4.g * brightness2, p4.b * brightness2);
            glVertex2f(p4.x, p4.y);
            glEnd();
        }
    }
}

int main() {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "2D Surface Viewer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Настройки OpenGL
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        auto surface = createSurface();
        drawSurface(window, surface);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}