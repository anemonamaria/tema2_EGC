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

    projectile.x = 0;
    projectile.y = 0;
    projectile.angle = 0;
    projectile.lenght = 0.0;
    projectile.shot = false;

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
   /* {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(45.0f), glm::vec3(0, 1, 0));

        RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0.5f, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
        RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, 0.5f, 0));
        RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
    }*/

    // TODO(student): Draw more objects with different model matrices.
    // Attention! The `RenderMesh()` function overrides the usual
    // `RenderMesh()` that we've been using up until now. This new
    // function uses the view matrix from the camera that you just
    // implemented, and the local projection matrix.

    //{
    //    glm::mat4 modelMatrix = glm::mat4(1);
    //    modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.5f, 1.5f, 1.5));
    //    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
    //    RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    //}

    //projectile
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition() + glm::vec3(projectile.x, projectile.y, 0)); //  +glm::vec3(0.5f, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }

    //projectile movement
    if (projectile.shot == true) 
    {
        projectile.x += deltaTimeSeconds * 50;
        projectile.y += deltaTimeSeconds * 50;
        projectile.lenght = sqrt((double)((projectile.x) * (projectile.x)) + (double)((projectile.y) * (projectile.y)));

        if (projectile.lenght >= 3)
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

    // Render the camera target. This is useful for understanding where
    // the rotation point is, when moving in third-person camera mode.
    //if (renderCameraTarget)
    //{
    //    glm::mat4 modelMatrix = glm::mat4(1);
    //    modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition()); //  +glm::vec3(0.5f, 0, 0));
    //    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
    //    RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    //}
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
    projectile.angle = 0;
    projectile.lenght = 0;
    projectile.shot = false;
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    float cameraSpeed = 2.0f * deltaTime;

    if (window->KeyHold(GLFW_KEY_W)) {
        // TODO(student): Translate the camera forward
        camera->TranslateForward(cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_A)) {
        // TODO(student): Translate the camera to the left
        camera->TranslateRight(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        // TODO(student): Translate the camera backward
        camera->TranslateForward(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        // TODO(student): Translate the camera to the right
        camera->TranslateRight(cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        // TODO(student): Translate the camera downward
        camera->TranslateUpward(-cameraSpeed);
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        // TODO(student): Translate the camera upward
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

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * -deltaX);

            if (projectile.shot == false && window->KeyHold(GLFW_KEY_SPACE)) {
                projectile.shot = true;
            }

        }

       renderCameraTarget = true;
     
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
