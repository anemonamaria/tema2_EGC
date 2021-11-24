#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = true;
    right = 20.f;
    left = .02f;
    bottom = .02f;
    top = 20.f;
    fov = 40.f;
    camera = new implemented::Camera_hw();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    projectile.x = projectile.y = projectile.z = 0;
    projectile.angle = player.angle = 0.0;
    projectile.lenght = 0.0;
    projectile.shot = false;
   /* for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            grid[i][j] = -1;
        }
    }
    grid = createMaze(grid, 10);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }*/
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

}
///////////////////////////////
// 
// -1 - celula nevizitata
// 0 - drum liber 
// 1 - inamic
// 2 - perete
int** Tema2::createMaze(int **grid, int n) {
    int i = 0, j = 0;
    int nrOfEnemies = 5;
    srand(time(NULL));
    do {
        int dir = rand() % 4;  // 0 jos, 1 dreapta, 2 stanga, 3 sus
        if (checkCell(grid, n, i, j, dir) == true)
        {
            if (nrOfEnemies != 0) {
                if (dir == 0)
                {
                    grid[i + 1][j] = rand() % 2;
                    nrOfEnemies -= grid[i + 1][j];
                    i++;
                }
                if (dir == 1)
                {
                    grid[i][j + 1] = rand() % 2;
                    nrOfEnemies -= grid[i][j + 1];
                    j++;
                }
                if (dir == 2)
                {
                    grid[i][j - 1] = rand() % 2;
                    nrOfEnemies -= grid[i][j - 1];
                    j--;
                }
                if (dir == 3)
                {
                    grid[i - 1][j] = rand() % 2;
                    nrOfEnemies -= grid[i - 1][j];
                    i--;
                }
            }
            else {
                if (dir == 0)
                {
                    grid[i + 1][j] = 0;
                    i++;
                }
                if (dir == 1)
                {
                    grid[i][j + 1] = 0;
                    j++;
                }
                if (dir == 2)
                {
                    grid[i][j - 1] = 0;
                    j--;
                }
                if (dir == 3)
                {
                    grid[i - 1][j] = 0;
                    i--;
                }
            }
        }
        else {
            dir = checkFreeDir(grid, i, j, n);
        }
        if (dir == -1)
            exit;
    } while (i < 9 && j < 9);

    return grid;
}

int Tema2::checkFreeDir(int** grid, int i, int j, int n)
{
    int dir = -1;
    if (i < n - 1 && grid[i + 1][j] == -1)
        dir = 0;
    if (j < n - 1 && grid[i][j + 1] == -1)
        dir = 1;
    if (j > 1 && grid[i][j - 1] == -1)
        dir = 2;
    if (i > 1 && grid[i - 1][j] == -1)
        dir = 3;
    return dir;
}

bool Tema2::checkCell(int **grid, int n, int i, int j, int dir) 
{
    if (dir == 0 && i == n - 1)
        return false;
    if (dir == 1 && j == n - 1)
        return false;
    if (dir == 2 && j == 0)
        return false;
    if (dir == 3 && i == 0)
        return false;
    if (dir == 0 && grid[i + 1][j] != -1)
        return false;
    if (dir == 1 && grid[i][j + 1] != -1)
        return false;
    if (dir == 2 && grid[i][j - 1] != -1)
        return false;
    if (dir == 3 && grid[i][j - 1] != -1)
        return false;
    return true;
}
/// <summary>
/// //////////////////////////////////////
/// </summary>

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::Update(float deltaTimeSeconds)
{
    //projectile
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(projectile.x, projectile.y, projectile.z)); //  +glm::vec3(0.5f, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }

    //projectile movement
    if (projectile.shot == true) 
    {
        projectile.x += deltaTimeSeconds * 50 * cos(projectile.angle);
        projectile.y += deltaTimeSeconds * 50 * sin(projectile.angle)/ cos(projectile.angle); //TODO aici e stricat, fa unghiul bun
        projectile.z += deltaTimeSeconds * 50 * sin(projectile.angle);

        projectile.lenght = sqrt((double)(((double)projectile.x) * (projectile.x)) + (double)(((double)projectile.z) * (projectile.z)));

        if (projectile.lenght >= 5)
            ResetProjectile();
    }

    //player
    if (renderCameraTarget)
    {
        //picior drept
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0.12f, -0.36f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0.12f, -0.16, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
        }
        //picior stang
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(-0.12f, -0.36f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(-0.12f, -0.16, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
        }
        //corp
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0, 0.f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0.2f, 0.f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(-0.2f, 0.f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }

            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0, 0.16f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0.2f, 0.16f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(-0.2f, 0.16f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
        }
        // cap
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0, 0.37f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
        }
        //brat drept
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0.415f, 0.16f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);    
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(0.415f, 0.f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
        }
        //brat stang
        {
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(-0.415f, 0.16f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
            {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(-0.415f, 0.f, 0));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
                RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
            }
        }
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema2::ResetProjectile() {
    projectile.x = 0; // camera->GetTargetPosition().x;
    projectile.y = 0; // camera->GetTargetPosition().y;
    projectile.z = 0;
    projectile.angle = 0;
    projectile.lenght = 0;
    projectile.shot = false;
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    float cameraSpeed = 2.0f * deltaTime;

    if (window->KeyHold(GLFW_KEY_W)) {
        camera->TranslateForward(cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        camera->TranslateRight(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        camera->TranslateForward(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        camera->TranslateRight(cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        camera->TranslateUpward(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        camera->TranslateUpward(cameraSpeed);
    }

    // TODO(student): Change projection parameters. Declare any extra
    // variables you might need in the class header. Inspect this file
    // for any hardcoded projection arguments (can you find any?) and
    // replace them with those extra variables.
   /* if (projection) {
        if (window->KeyHold(GLFW_KEY_G)) {
            fov += cameraSpeed;
        }

        if (window->KeyHold(GLFW_KEY_H)) {
            fov -= cameraSpeed;
        }

        if (window->KeyHold(GLFW_KEY_J))
        {
            bottom += deltaTime;
            top += deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_K))
        {
            bottom -= deltaTime;
            top -= deltaTime;
        }
    }*/

}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
   /* if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }

    if (key == GLFW_KEY_O)
    {
        projection = false;
        projectionMatrix = glm::ortho(left, right, bottom, top, 0.01f, 100.f);
    }

    if (key == GLFW_KEY_P)
    {
        projection = true;
        projectionMatrix = glm::perspective(RADIANS(60) + fov, window->props.aspectRatio, 0.01f, 100.f);
    }*/
    // TODO(student): Switch projections

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;
        cursorX = mouseX - deltaX;
        cursorY = mouseY - deltaY;
        setPlayerAngle();

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * -deltaX);
            if (projectile.shot == false && window->KeyHold(GLFW_KEY_SPACE))
            {
                projectile.shot = true;
            }

        }
       renderCameraTarget = true;
}

void Tema2::setPlayerAngle() {

    int ax = camera->GetTargetPosition().x;
    int ay = camera->GetTargetPosition().z;
    ax = cursorX - ax;
    ay = -cursorY + ay;
    glm::vec2 auxvect = glm::normalize(glm::vec2(ax, ay));
    if (auxvect.y < 0) {
        auxvect.x = -auxvect.x;
    }
    if (auxvect.x == 0 && auxvect.y < 0) {
        player.angle = 180.f;
    }
    else if (auxvect.x == 0 && auxvect.y > 0) {
        player.angle = 0.f;
    }
    else
    {
        player.angle = atan(auxvect.x / auxvect.y);
    }
    if (auxvect.y > 0) {
        player.angle = -player.angle + M_PI;
    }
    if (!projectile.shot)
        projectile.angle = player.angle + 3 * M_PI / 2;

}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
