#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glad.h"
#include "glfw3.h"
#include "stb_easy_font.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cmath>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

const char* const BG_IMAGE_PATH = "background.jpg";

struct Ball{
    glm::vec2 pos;
    glm::vec2 size;
    glm::vec2 velocity;
};

struct Paddle{
    glm::vec2 pos;
    glm::vec2 size;
};

struct Brick{
    glm::vec2 pos;
    glm::vec2 size;
    int health;
    bool active;
};

void drawText(float x, float y, const char* text)
{
    char buffer[99999];

    int num_quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, sizeof(buffer));

    GLuint textVAO, textVBO;

    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);

    glBindVertexArray(textVAO);

    glBindBuffer(GL_ARRAY_BUFFER, textVBO);

    glBufferData(GL_ARRAY_BUFFER, num_quads * 4 * 16, buffer, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void*)0);

    glEnableVertexAttribArray(0);

    glBindVertexArray(textVAO);

    glDrawArrays(GL_QUADS, 0, num_quads * 4);

    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "Bouncing Ball", NULL, NULL);

    if(!win){
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(win);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, WIDTH, HEIGHT);

    unsigned int bgTex = 0;

    {
        int w, h, ch;

        stbi_set_flip_vertically_on_load(1);

        unsigned char* data = stbi_load(BG_IMAGE_PATH, &w, &h, &ch, 3);

        if(data)
        {
            glGenTextures(1, &bgTex);

            glBindTexture(GL_TEXTURE_2D, bgTex);

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGB,
                w,
                h,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );

            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MIN_FILTER,
                GL_LINEAR_MIPMAP_LINEAR
            );

            glTexParameteri(
                GL_TEXTURE_2D,
                GL_TEXTURE_MAG_FILTER,
                GL_LINEAR
            );

            stbi_image_free(data);
        }
    }

    srand(time(0));

    const char* vertexShaderSource = R"(

    #version 330 core

    layout (location = 0) in vec2 aPos;

    uniform mat4 model;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
    }

    )";

    const char* textVertexShader = R"(

    #version 330 core

    layout(location = 0) in vec2 aPos;

    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * vec4(aPos.xy, 0.0, 1.0);
    }

    )";

    const char* fragmentShaderSource = R"(

    #version 330 core

    out vec4 FragColor;

    uniform vec3 color;

    void main()
    {
        FragColor = vec4(color, 1.0);
    }

    )";

    const char* textFragmentShader = R"(

    #version 330 core

    out vec4 FragColor;

    uniform vec3 textColor;

    void main()
    {
        FragColor = vec4(textColor, 1.0);
    }

    )";

    const char* bgVertexShader = R"(

    #version 330 core

    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aUV;

    out vec2 vUV;

    void main()
    {
        vUV = aUV;

        gl_Position = vec4(aPos, 0.0, 1.0);
    }

    )";

    const char* bgFragmentShader = R"(

    #version 330 core

    in vec2 vUV;

    out vec4 FragColor;

    uniform sampler2D bgTex;

    void main()
    {
        FragColor = texture(bgTex, vUV);
    }

    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);

    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);

    glDeleteShader(fragmentShader);

    unsigned int tvs = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(tvs, 1, &textVertexShader, NULL);

    glCompileShader(tvs);

    unsigned int tfs = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(tfs, 1, &textFragmentShader, NULL);

    glCompileShader(tfs);

    unsigned int textProgram = glCreateProgram();

    glAttachShader(textProgram, tvs);

    glAttachShader(textProgram, tfs);

    glLinkProgram(textProgram);

    glDeleteShader(tvs);

    glDeleteShader(tfs);

    unsigned int bvs = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(bvs, 1, &bgVertexShader, NULL);

    glCompileShader(bvs);

    unsigned int bfs = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(bfs, 1, &bgFragmentShader, NULL);

    glCompileShader(bfs);

    unsigned int bgProgram = glCreateProgram();

    glAttachShader(bgProgram, bvs);

    glAttachShader(bgProgram, bfs);

    glLinkProgram(bgProgram);

    glDeleteShader(bvs);

    glDeleteShader(bfs);

    std::vector<float> circleVertices;

    circleVertices.push_back(0.0f);
    circleVertices.push_back(0.0f);

    const int segments = 100;

    for(int i = 0; i <= segments; i++)
    {
        float angle = 2.0f * 3.1415926f * i / segments;

        float x = cos(angle) * 0.5f;
        float y = sin(angle) * 0.5f;

        circleVertices.push_back(x);
        circleVertices.push_back(y);
    }

    unsigned int VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    float rectVertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,

        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    unsigned int rectVAO, rectVBO;

    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);

    glBindVertexArray(rectVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    float bgVertices[] = {

        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f
    };

    unsigned int bgVAO, bgVBO;

    glGenVertexArrays(1, &bgVAO);

    glGenBuffers(1, &bgVBO);

    glBindVertexArray(bgVAO);

    glBindBuffer(GL_ARRAY_BUFFER, bgVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(bgVertices),
        bgVertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4 * sizeof(float),
        (void*)(2 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glm::mat4 projection = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT, -1.0f, 1.0f);

    glm::mat4 textProjection = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);

    Paddle basket;

    basket.pos = {WIDTH / 2.0f, 40.0f};
    basket.size = {120.0f, 20.0f};

    Ball ball;

    ball.pos = {WIDTH / 2.0f, HEIGHT / 2.0f};

    ball.size = {15.0f, 15.0f};

    float dir = (rand() % 2 == 0) ? -1.0f : 1.0f;

    ball.velocity = {300.0f * dir, -300.0f};

    std::vector<Brick> bricks;

    int rows = 4;
    int cols = 8;

    float brickWidth = 80.0f;
    float brickHeight = 25.0f;
    float spacing = 10.0f;

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            Brick brick;

            brick.pos = {
                80.0f + c * (brickWidth + spacing),
                HEIGHT - 80.0f - r * (brickHeight + spacing)
            };

            brick.size = {brickWidth, brickHeight};

            brick.health = (r % 2 == 0) ? 1 : 2;

            brick.active = true;

            bricks.push_back(brick);
        }
    }

    int score = 0;

    bool gameOver = false;

    float finalTime = 0.0f;

    float startTime = glfwGetTime();

    float last = glfwGetTime();

    float basketSpeed = 500.0f;

    while(!glfwWindowShouldClose(win))
    {
        float current = glfwGetTime();

        float dt = current - last;

        last = current;

        glfwPollEvents();

        if(!gameOver)
        {
            if(glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS)
                basket.pos.x -= basketSpeed * dt;

            if(glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS)
                basket.pos.x += basketSpeed * dt;

            if(basket.pos.x - basket.size.x / 2 < 0)
                basket.pos.x = basket.size.x / 2;

            if(basket.pos.x + basket.size.x / 2 > WIDTH)
                basket.pos.x = WIDTH - basket.size.x / 2;

            float speedIncrease = 1.0f + (glfwGetTime() - startTime) * 0.05f;

            ball.pos += ball.velocity * speedIncrease * dt;

            if(ball.pos.x - ball.size.x / 2 <= 0){
                ball.pos.x = ball.size.x / 2;
                ball.velocity.x *= -1;
            }

            if(ball.pos.x + ball.size.x / 2 >= WIDTH){
                ball.pos.x = WIDTH - ball.size.x / 2;
                ball.velocity.x *= -1;
            }

            if(ball.pos.y + ball.size.y / 2 >= HEIGHT){
                ball.pos.y = HEIGHT - ball.size.y / 2;
                ball.velocity.y *= -1;
            }

            if(ball.pos.y - ball.size.y / 2 <= 0){
                gameOver = true;
                finalTime = glfwGetTime() - startTime;
            }

            float bx1 = ball.pos.x - ball.size.x / 2;
            float bx2 = ball.pos.x + ball.size.x / 2;
            float by1 = ball.pos.y - ball.size.y / 2;
            float by2 = ball.pos.y + ball.size.y / 2;

            float px1 = basket.pos.x - basket.size.x / 2;
            float px2 = basket.pos.x + basket.size.x / 2;
            float py1 = basket.pos.y - basket.size.y / 2;
            float py2 = basket.pos.y + basket.size.y / 2;

            bool paddleHit = bx1 < px2 && bx2 > px1 && by1 < py2 && by2 > py1;

            if(paddleHit)
            {
                ball.pos.y = py2 + ball.size.y / 2;

                ball.velocity.y = abs(ball.velocity.y);

                float hitPos = (ball.pos.x - basket.pos.x) / (basket.size.x / 2);

                ball.velocity.x = hitPos * 300.0f;
            }

            for(auto& brick : bricks)
            {
                if(!brick.active)
                    continue;

                float rx1 = brick.pos.x - brick.size.x / 2;
                float rx2 = brick.pos.x + brick.size.x / 2;
                float ry1 = brick.pos.y - brick.size.y / 2;
                float ry2 = brick.pos.y + brick.size.y / 2;

                bool hit = bx1 < rx2 && bx2 > rx1 && by1 < ry2 && by2 > ry1;

                if(hit)
                {
                    ball.pos.y += ball.velocity.y > 0 ? -5.0f : 5.0f;

                    ball.velocity.y *= -1;

                    brick.health--;

                    if(brick.health <= 0)
                    {
                        brick.active = false;
                        score += 10;
                    }

                    break;
                }
            }

            bool allDestroyed = true;

            for(auto& brick : bricks)
            {
                if(brick.active)
                {
                    allDestroyed = false;
                    break;
                }
            }

            if(allDestroyed)
            {
                gameOver = true;
                finalTime = glfwGetTime() - startTime;
            }
        }

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        if(bgTex)
        {
            glUseProgram(bgProgram);

            glActiveTexture(GL_TEXTURE0);

            glBindTexture(GL_TEXTURE_2D, bgTex);

            glUniform1i(
                glGetUniformLocation(bgProgram, "bgTex"),
                0
            );

            glBindVertexArray(bgVAO);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glUseProgram(shaderProgram);

        int locModel = glGetUniformLocation(shaderProgram, "model");
        int locProj = glGetUniformLocation(shaderProgram, "projection");
        int locColor = glGetUniformLocation(shaderProgram, "color");

        glUniformMatrix4fv(locProj, 1, GL_FALSE, &projection[0][0]);

        glBindVertexArray(rectVAO);

        for(auto& brick : bricks)
        {
            if(!brick.active)
                continue;

            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, glm::vec3(brick.pos, 0.0f));

            model = glm::scale(model, glm::vec3(brick.size, 1.0f));

            glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);

            if(brick.health == 2)
                glUniform3f(locColor, 0.1f, 0.2f, 1.0f);
            else
                glUniform3f(locColor, 1.0f, 1.0f, 1.0f);

            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(VAO);

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(ball.pos, 0.0f));

        model = glm::scale(model, glm::vec3(ball.size, 1.0f));

        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);

        glUniform3f(locColor, 0.0f, 1.0f, 1.0f);

        glDrawArrays(GL_TRIANGLE_FAN, 0, segments + 2);

        glBindVertexArray(rectVAO);

        model = glm::mat4(1.0f);

        model = glm::translate(model, glm::vec3(basket.pos, 0.0f));

        model = glm::scale(model, glm::vec3(basket.size, 1.0f));

        glUniformMatrix4fv(locModel, 1, GL_FALSE, &model[0][0]);

        glUniform3f(locColor, 0.2f, 0.8f, 1.0f);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        float elapsed;

        if(gameOver)
            elapsed = finalTime;
        else
            elapsed = glfwGetTime() - startTime;

        glUseProgram(textProgram);

        int textProj = glGetUniformLocation(textProgram, "projection");

        glUniformMatrix4fv(textProj, 1, GL_FALSE, &textProjection[0][0]);

        int textColorLoc = glGetUniformLocation(textProgram, "textColor");

        glUniform3f(textColorLoc, 1.0f, 1.0f, 1.0f);

        std::stringstream scoreText;

        scoreText << "SCORE: " << score;

        std::string scoreStr = scoreText.str();

        drawText(20.0f, 30.0f, scoreStr.c_str());

        std::stringstream timeText;

        timeText << "TIME: " << std::fixed << std::setprecision(1) << elapsed;

        std::string timeStr = timeText.str();

        drawText(620.0f, 30.0f, timeStr.c_str());

        if(gameOver)
        {
            glUniform3f(textColorLoc, 1.0f, 1.2f, 0.2f);

            drawText(WIDTH / 2.0f - 60.0f, HEIGHT / 2.0f, "    GAME OVER");

            std::stringstream finalScore;

            finalScore << "       FINAL SCORE: " << score;

            std::string finalStr = finalScore.str();

            drawText(WIDTH / 2.0f - 90.0f, HEIGHT / 2.0f - 40.0f, finalStr.c_str());

            glUniform3f(textColorLoc, 1.0f, 1.0f, 1.0f);

            drawText(WIDTH / 2.0f - 120.0f, HEIGHT / 2.0f + 50.0f, "         PRESS R TO RESTART");

            drawText(WIDTH / 2.0f - 120.0f, HEIGHT / 2.0f + 80.0f, "          PRESS C TO CLOSE");

            if(glfwGetKey(win, GLFW_KEY_R) == GLFW_PRESS)
            {
                basket.pos = {WIDTH / 2.0f, 40.0f};

                ball.pos = {WIDTH / 2.0f, HEIGHT / 2.0f};

                float dir = (rand() % 2 == 0) ? -1.0f : 1.0f;

                ball.velocity = {200.0f * dir, -200.0f};

                bricks.clear();

                for(int r = 0; r < rows; r++)
                {
                    for(int c = 0; c < cols; c++)
                    {
                        Brick brick;

                        brick.pos = {
                            80.0f + c * (brickWidth + spacing),
                            HEIGHT - 80.0f - r * (brickHeight + spacing)
                        };

                        brick.size = {brickWidth, brickHeight};

                        brick.health = (r % 2 == 0) ? 1 : 2;

                        brick.active = true;

                        bricks.push_back(brick);
                    }
                }

                score = 0;

                gameOver = false;

                startTime = glfwGetTime();
            }

            if(glfwGetKey(win, GLFW_KEY_C) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(win, true);
            }
        }

        glfwSwapBuffers(win);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteVertexArrays(1, &rectVAO);
    glDeleteBuffers(1, &rectVBO);

    glDeleteProgram(shaderProgram);

    if(bgTex)
    glDeleteTextures(1, &bgTex);

    glDeleteVertexArrays(1, &bgVAO);

    glDeleteBuffers(1, &bgVBO);

    glDeleteProgram(bgProgram);

    glfwTerminate();

    return 0;
}