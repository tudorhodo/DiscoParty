#include "lab_m1/tema3/tema3.h"

#include <vector>
#include <string>
#include <iostream>
#include <direct.h>
using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    //random texture
    {
        mapTextures["random"] = CreateRandomTexture(16, 16);
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader* shader = new Shader("GridShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexGrid.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentGrid.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
        
    {
        Shader* shader = new Shader("ObjectShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexObjects.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentObjects.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("ConeShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexCone.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentCone.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("DiscoBallShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "VertexDiscoBall.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema3", "shaders", "FragmentDiscoBall.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        grid = generateGrid();
        GetSceneCamera()->SetPosition(glm::vec3(3.5f, 2, 6));
        renderGrid = true;
        renderSpots = true;
        renderDisco = true;
    }

    {
        {
            spotPositions.push_back(glm::vec3(1, 4.9f, 1));
            spotPositions.push_back(glm::vec3(1, 4.9f, 6));
            spotPositions.push_back(glm::vec3(6, 4.9f, 1));
            spotPositions.push_back(glm::vec3(6, 4.9f, 6));
        }

        for (int i = 0; i < 4; i++) {
            float r = ((float)rand() / RAND_MAX);
            float g = ((float)rand() / RAND_MAX);
            float b = ((float)rand() / RAND_MAX);

            spotLights.push_back(glm::vec3(r, g, b));

            spotLightsDirections.push_back(glm::vec3(0, -1, 0));

            tendencySpot.push_back(i + 1);
            angleSpot.push_back(0);
            speedSpots = 0.1f;
        }       
    }

    {
        vector<VertexFormat> vertices;
        vertices.push_back(VertexFormat(glm::vec3(0, 0, 0), glm::vec3(1, 0.7, 0), glm::vec3(0.2, 0.8, 0.6)));
        vertices.push_back(VertexFormat(glm::vec3(1, -1, 0), glm::vec3(1, 0.7, 0), glm::vec3(0.2, 0.8, 0.6)));
        for (int i = 2; i < PointsCircle; i++) {
            vertices.push_back(VertexFormat(glm::vec3(cos(grades * i), -1, sin(grades * i)), glm::vec3(1, 0.7, 0), glm::vec3(0.2, 0.8, 0.6)));
        }

        vector<unsigned int> indices;
        for (int i = 0; i < PointsCircle; i++) {
            indices.push_back(i);
        }

        indices.push_back(1);

        meshes["cone"] = new Mesh("cone");
        meshes["cone"]->SetDrawMode(GL_TRIANGLE_FAN);
        meshes["cone"]->InitFromData(vertices, indices);
    }

    {
        for (int i = 0; i < 6; i++) {
            float x = rand() % 6 + 1;
            float z = rand() % 6 + 1;

            dancers.push_back(glm::vec3(x, 0.5f, z));
            dancersDistance.push_back(glm::vec3(rand() % 6 - 1.5f, 0, rand() % 4 - 1.5f));
        }
        dancersSpeed = 1.5f;
    }

    totalOX = totalOZ = 0;

    {
        {
            for (int i = 0; i < 5; i++) {
                string song = "\"song";
                song += '0' + i;
                song += ".wav\"";
                songs.push_back(song);
            }
        }

        {
            getcwd(pathcwd, 100);
            string s0 = pathcwd;
            s0 += "\\sounds\\";

            system("taskkill \/IM \"wscript.exe\" \/F");
            string s1 = "start ";
            s1 += s0;
            s1 += "sound.vbs ";
            s1 += s0;
            songIndex = (int)(rand() % 5);
            s1 += songs[songIndex];

            const char* cmd = s1.c_str();
            system(cmd);
        }
    }
}


void Tema3::rotateSpots(float deltaTimeSeconds) {
    for (int i = 0; i < 4; i++) {
        switch (tendencySpot[i])
        {
        case 1:
            if (abs(angleSpot[i]) > 0.3f) {
                tendencySpot[i] = -1;
            }
            else {
                angleSpot[i] += deltaTimeSeconds * speedSpots;
            }
            break;
        case 2:
            if (abs(angleSpot[i]) > 0.3f) {
                tendencySpot[i] = -2;
            }
            else {
                angleSpot[i] += deltaTimeSeconds * speedSpots;
            }
            break;
        case 3:
            if (abs(angleSpot[i]) > 0.3f) {
                tendencySpot[i] = -3;
            }
            else {
                angleSpot[i] -= deltaTimeSeconds * speedSpots;
            }
            break;
        case 4:
            if (abs(angleSpot[i]) > 0.3f) {
                tendencySpot[i] = -4;
            }
            else {
                angleSpot[i] -= deltaTimeSeconds * speedSpots;
            }
            break;
        case -1:
            if (abs(angleSpot[i]) < 0.01f) {
                tendencySpot[i] = (int)(rand() % 8 - 4);
                if (tendencySpot[i] == 0) {
                    tendencySpot[i] = 2;
                    angleSpot[i] = 0;
                }
            }
            else {
                angleSpot[i] -= deltaTimeSeconds * speedSpots;
            }
            break;
        case -2:
            if (abs(angleSpot[i]) < 0.01f) {
                tendencySpot[i] = (int)(rand() % 8 - 4);
                if (tendencySpot[i] == 0) {
                    tendencySpot[i] = 3;
                    angleSpot[i] = 0;

                }
            }
            else {
                angleSpot[i] -= deltaTimeSeconds * speedSpots;
            }
            break;
        case -3:
            if (abs(angleSpot[i]) < 0.01f) {
                tendencySpot[i] = (int)(rand() % 8 - 4);
                if (tendencySpot[i] == 0) {
                    tendencySpot[i] = 4;
                    angleSpot[i] = 0;

                }
            }
            else {
                angleSpot[i] += deltaTimeSeconds * speedSpots;
            }
            break;
        case -4:
            if (abs(angleSpot[i]) < 0.01f) {
                tendencySpot[i] = (int)(rand() % 8 - 4);
                if (tendencySpot[i] == 0) {
                    tendencySpot[i] = 1;
                    angleSpot[i] = 0;

                }
            }
            else {
                angleSpot[i] += deltaTimeSeconds * speedSpots;
            }
            break;
        default:
            break;
        }
    }
}


void Tema3::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::Update(float deltaTimeSeconds)
{
    MoveDancers(deltaTimeSeconds);

    rotateSpots(deltaTimeSeconds);
    
    for (int i = 0; i < 4; i++) {
        if (abs(tendencySpot[i]) % 2 != 0) {

            glm::mat4 rotationMatixX = glm::rotate(glm::mat4(1.0f), angleSpot[i], glm::vec3(1, 0, 0));
            spotLightsDirections[i] = rotationMatixX * glm::vec4(0, -1, 0, 0);
        }
        if (abs(tendencySpot[i]) % 2 == 0) {
            glm::mat4 rotationMatixZ = glm::rotate(glm::mat4(1.0f), angleSpot[i], glm::vec3(0, 0, 1));
            spotLightsDirections[i] = rotationMatixZ * glm::vec4(0, -1, 0, 0);

        }
    }

    //render walls
    {
        for (int i = 0; i < 8; i++) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(-1, 0, i));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 10, 1));
            RenderWall(meshes["box"], shaders["ObjectShader"], modelMatrix, glm::vec3(-1, 0, i), mapTextures["random"], mapTextures["random"], glm::vec3(3.5f, 4.2f, 3.5f));

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(8, 0, i));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 10, 1));
            RenderWall(meshes["box"], shaders["ObjectShader"], modelMatrix, glm::vec3(8, 0, i), mapTextures["random"], mapTextures["random"], glm::vec3(3.5f, 4.2f, 3.5f));
        }

        for (int i = 0; i < 8; i++) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, -1));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 10, 1));
            RenderWall(meshes["box"], shaders["ObjectShader"], modelMatrix, glm::vec3(i, 0, -1), mapTextures["random"], mapTextures["random"], glm::vec3(3.5f, 4.2f, 3.5f));

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0, 8));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 10, 1));
            RenderWall(meshes["box"], shaders["ObjectShader"], modelMatrix, glm::vec3(i, 0, 8), mapTextures["random"], mapTextures["random"], glm::vec3(3.5f, 4.2f, 3.5f));
        }
    }

    // Render dance floor and ceiling
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 0.01f, j));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(0.02f));
                RenderGrid(meshes["plane"], shaders["GridShader"], modelMatrix, grid[i][j], mapTextures["random"], mapTextures["random"], glm::vec3(3.5f, 4.2f, 3.5f));

                modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, glm::vec3(i, 5.5f, j));
                modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1, 1));
                RenderGrid(meshes["box"], shaders["ObjectShader"], modelMatrix, grid[i][j], mapTextures["random"], mapTextures["random"], glm::vec3(3.5f, 4.2f, 3.5f));
            }
        }
    }

    //render dancers
    {
        for (int i = 0; i < 6; i++) {
            
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, dancers[i]);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 1, 0.5f));
            RenderDancers(meshes["box"], shaders["ObjectShader"], modelMatrix, GetDancerLocation(dancers[i]), mapTextures["random"], mapTextures["random"], glm::vec3(3.5f, 4.2f, 3.5f));
        }
    }

    //render disco ball
    if (renderDisco) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(3.5f, 4.2f, 3.5f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
        RenderDiscoBall(meshes["sphere"], shaders["DiscoBallShader"], modelMatrix, mapTextures["random"], mapTextures["random"]);
    }

    //render cone of light
    {
        if (renderSpots) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            // aceasta directiva este folosita pentru nu se scrie in depth buffer
            glDepthMask(GL_FALSE);

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            for (int i = 0; i < 4; i++) {
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, spotPositions[i]);

                if (abs(tendencySpot[i]) % 2 != 0) {

                    modelMatrix = glm::rotate(modelMatrix, angleSpot[i], glm::vec3(1, 0, 0));

                }
                if (abs(tendencySpot[i]) % 2 == 0) {
                    modelMatrix = glm::rotate(modelMatrix, angleSpot[i], glm::vec3(0, 0, 1));

                }

                modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(RADIANS(15)), 1, tan(RADIANS(15))) * 20.f);

                RenderSimpleMesh(meshes["cone"], shaders["ConeShader"], modelMatrix, spotLights[i]);
            }

            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
            glDisable(GL_CULL_FACE);
        }
    }
}


void Tema3::FrameEnd()
{
    //DrawCoordinateSystem();
}


Texture2D* Tema3::CreateRandomTexture(unsigned int width, unsigned int height)
{
    GLuint textureID = 1;
    unsigned int channels = 3;
    unsigned int size = width * height * channels;
    unsigned char* data = new unsigned char[size];

    // TODO(student): Generate random texture data
    for (int i = 0; i < size; i++) {
        data[i] = (char)rand() % 256;
    }

    // TODO(student): Generate and bind the new texture ID
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    if (GLEW_EXT_texture_filter_anisotropic) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
    }
    // TODO(student): Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //minecraft style
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //minecraft style

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    CheckOpenGLError();

    // Use glTexImage2D to set the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    // TODO(student): Generate texture mip-maps

    glGenerateMipmap(GL_TEXTURE_2D);

    CheckOpenGLError();

    // Save the texture into a wrapper Texture2D class for using easier later during rendering phase
    Texture2D* texture = new Texture2D();
    texture->Init(textureID, width, height, channels);

    SAFE_FREE_ARRAY(data);
    return texture;
}


void Tema3::MoveDancers(float deltaTimeSeconds) {
    for (int i = 0; i < 6; i++) {
        dancers[i] += dancersDistance[i] * dancersSpeed * deltaTimeSeconds;

        glm::vec3 newLocation = GetDancerLocation(dancers[i]);
        if (newLocation.y == 1 || newLocation.x == 0 || newLocation.x == 7 || newLocation.z == 0 || newLocation.z == 7) {
            dancers[i] -= dancersDistance[i] * dancersSpeed * deltaTimeSeconds;
            dancersDistance[i] = glm::vec3(rand() % 6 - 1.5f, 0, rand() % 4 - 1.5f);
        }
    }
}


void Tema3::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, const glm::vec3 &color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema3::RenderDiscoBall(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    float time = Engine::GetElapsedTime();
    GLint rotation_time = glGetUniformLocation(shader->program, "rotation_time");
    glUniform1f(rotation_time, time / 10);

    if (texture1)
    {
        // TODO(student): Do these:
        // - activate texture location 0
        // - bind the texture1 ID
        // - send theuniform value
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    }

    if (texture2)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        // - bind the texture2 ID
        // - send the uniform value
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
    }


    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema3::RenderGrid(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, Texture2D* texture1, Texture2D* texture2, glm::vec3 discoCenter) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    int material_ke = glGetUniformLocation(shader->program, "color");
    glUniform3f(material_ke, color.x, color.y, color.z);

    GLint renderGridLocation = glGetUniformLocation(shader->program, "drawGrid");
    glUniform1i(renderGridLocation, renderGrid);

    int light_position = glGetUniformLocation(shader->program, "spotPositions");
    glUniform3fv(light_position, 4, glm::value_ptr(spotPositions[0]));

    int light_direction = glGetUniformLocation(shader->program, "spotDirections");
    glUniform3fv(light_direction, 4, glm::value_ptr(spotLightsDirections[0]));

    int light_color = glGetUniformLocation(shader->program, "spotColor");
    glUniform3fv(light_color, 4, glm::value_ptr(spotLights[0]));

    int intensitateSpot = glGetUniformLocation(shader->program, "intensitySpot");
    glUniform3f(intensitateSpot, 0.35f, 0.35f, 0.35f);

    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, 5);

    GLint renderSpotLocation = glGetUniformLocation(shader->program, "drawSpot");
    glUniform1i(renderSpotLocation, renderSpots);

    float time = Engine::GetElapsedTime();
    GLint rotation_time = glGetUniformLocation(shader->program, "rotation_time");
    glUniform1f(rotation_time, time / 10);

    GLint renderDiscoLocation = glGetUniformLocation(shader->program, "drawText");
    glUniform1i(renderDiscoLocation, renderDisco);

    if (texture1)
    {
        // TODO(student): Do these:
        // - activate texture location 0
        // - bind the texture1 ID
        // - send theuniform value
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    }

    if (texture2)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        // - bind the texture2 ID
        // - send the uniform value
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
    }

    GLint discoLoc = glGetUniformLocation(shader->program, "discoCenter");
    glUniform3f(discoLoc, discoCenter.x, discoCenter.y, discoCenter.z);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema3::RenderWall(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 location, Texture2D* texture1, Texture2D* texture2, glm::vec3 discoCenter) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    vector<glm::vec3> lightSources;
    vector<glm::vec3> lightColor;

    if (location.x == 0 && location.z == -1) {
        lightSources.push_back(glm::vec3(0, -0.2f, 0));
        lightSources.push_back(glm::vec3(1, -0.2f, 0));

        lightColor.push_back(grid[0][0]);
        lightColor.push_back(grid[1][0]);
    }

    if (location.x == 0 && location.z == 8) {
        lightSources.push_back(glm::vec3(0, -0.2f, 7));
        lightSources.push_back(glm::vec3(1, -0.2f, 7));

        lightColor.push_back(grid[0][7]);
        lightColor.push_back(grid[1][7]);
    }

    if (location.x == 7 && location.z == -1) {
        lightSources.push_back(glm::vec3(7, -0.2f, 0));
        lightSources.push_back(glm::vec3(6, -0.2f, 0));

        lightColor.push_back(grid[7][0]);
        lightColor.push_back(grid[6][0]);
    }

    if (location.x == 7 && location.z == 8) {
        lightSources.push_back(glm::vec3(7, -0.2f, 7));
        lightSources.push_back(glm::vec3(6, -0.2f, 7));

        lightColor.push_back(grid[7][7]);
        lightColor.push_back(grid[6][7]);
    }

    if (location.x == -1 && location.z == 0) {
        lightSources.push_back(glm::vec3(0, -0.2f, 0));
        lightSources.push_back(glm::vec3(0, -0.2f, 1));

        lightColor.push_back(grid[0][0]);
        lightColor.push_back(grid[0][1]);
    }

    if (location.x == 8 && location.z == 0) {
        lightSources.push_back(glm::vec3(7, -0.2f, 0));
        lightSources.push_back(glm::vec3(7, -0.2f, 1));

        lightColor.push_back(grid[7][0]);
        lightColor.push_back(grid[7][1]);
    }

    if (location.x == -1 && location.z == 7) {
        lightSources.push_back(glm::vec3(0, -0.2f, 7));
        lightSources.push_back(glm::vec3(0, -0.2f, 6));

        lightColor.push_back(grid[0][7]);
        lightColor.push_back(grid[0][6]);
    }

    if (location.x == 8 && location.z == 7) {
        lightSources.push_back(glm::vec3(7, -0.2f, 7));
        lightSources.push_back(glm::vec3(7, -0.2f, 6));

        lightColor.push_back(grid[7][7]);
        lightColor.push_back(grid[7][6]);
    }

    if (location.x > 0 && location.x < 7 && location.z == -1) {
        lightSources.push_back(glm::vec3(location.x - 1, -0.2f, 0));
        lightSources.push_back(glm::vec3(location.x, -0.2f, 0));
        lightSources.push_back(glm::vec3(location.x + 1, -0.2f, 0));

        lightColor.push_back(grid[(int)location.x - 1][0]);
        lightColor.push_back(grid[(int)location.x][0]);
        lightColor.push_back(grid[(int)location.x + 1][0]);
    }

    if (location.x > 0 && location.x < 7 && location.z == 8) {
        lightSources.push_back(glm::vec3(location.x - 1, -0.2f, 7));
        lightSources.push_back(glm::vec3(location.x, -0.2f, 7));
        lightSources.push_back(glm::vec3(location.x + 1, -0.2f, 7));

        lightColor.push_back(grid[(int)location.x - 1][7]);
        lightColor.push_back(grid[(int)location.x][7]);
        lightColor.push_back(grid[(int)location.x + 1][7]);
    }

    if (location.z > 0 && location.z < 7 && location.x == -1) {
        lightSources.push_back(glm::vec3(0, -0.2f, location.z - 1));
        lightSources.push_back(glm::vec3(0, -0.2f, location.z));
        lightSources.push_back(glm::vec3(0, -0.2f, location.z + 1));

        lightColor.push_back(grid[0][(int)location.z - 1]);
        lightColor.push_back(grid[0][(int)location.z]);
        lightColor.push_back(grid[0][(int)location.z + 1]);
    }

    if (location.z > 0 && location.z < 7 && location.x == 8) {
        lightSources.push_back(glm::vec3(7, -0.2f, location.z - 1));
        lightSources.push_back(glm::vec3(7, -0.2f, location.z));
        lightSources.push_back(glm::vec3(7, -0.2f, location.z + 1));

        lightColor.push_back(grid[7][(int)location.z - 1]);
        lightColor.push_back(grid[7][(int)location.z]);
        lightColor.push_back(grid[7][(int)location.z + 1]);
    }


    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    int lightNumber = glGetUniformLocation(shader->program, "lightNumber");
    glUniform1i(lightNumber, lightSources.size());

    int objectType = glGetUniformLocation(shader->program, "objectType");
    glUniform1i(objectType, 1);

    GLint renderGridLocation = glGetUniformLocation(shader->program, "drawGrid");
    glUniform1i(renderGridLocation, renderGrid);

    GLint renderSpotLocation = glGetUniformLocation(shader->program, "drawSpot");
    glUniform1i(renderSpotLocation, renderSpots);

    GLint renderDiscoLocation = glGetUniformLocation(shader->program, "drawText");
    glUniform1i(renderDiscoLocation, renderDisco);

    int light_position = glGetUniformLocation(shader->program, "pointLight_position");
    glUniform3fv(light_position, lightSources.size(), glm::value_ptr(lightSources[0]));

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, 10);

    int material_kd = glGetUniformLocation(shader->program, "pointLight_color");
    glUniform3fv(material_kd, lightColor.size(), glm::value_ptr(lightColor[0]));

    int intensitateLight = glGetUniformLocation(shader->program, "pointLight_intensity");
    glUniform3f(intensitateLight, 0.75f, 0.75f, 0.75f);

    int spot_position = glGetUniformLocation(shader->program, "spotPositions");
    glUniform3fv(spot_position, 4, glm::value_ptr(spotPositions[0]));

    int light_direction = glGetUniformLocation(shader->program, "spotDirections");
    glUniform3fv(light_direction, 4, glm::value_ptr(spotLightsDirections[0]));

    int light_color = glGetUniformLocation(shader->program, "spotColor");
    glUniform3fv(light_color, 4, glm::value_ptr(spotLights[0]));

    int intensitateSpot = glGetUniformLocation(shader->program, "intensitySpot");
    glUniform3f(intensitateSpot, 0.35f, 0.35f, 0.35f);

    float time = Engine::GetElapsedTime();
    GLint rotation_time = glGetUniformLocation(shader->program, "rotation_time");
    glUniform1f(rotation_time, time / 10);

    if (texture1)
    {
        // TODO(student): Do these:
        // - activate texture location 0
        // - bind the texture1 ID
        // - send theuniform value
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    }

    if (texture2)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        // - bind the texture2 ID
        // - send the uniform value
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
    }

    GLint discoLoc = glGetUniformLocation(shader->program, "discoCenter");
    glUniform3f(discoLoc, discoCenter.x, discoCenter.y, discoCenter.z);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema3::RenderDancers(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 location, Texture2D* texture1, Texture2D* texture2, glm::vec3 discoCenter) {
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    vector<glm::vec3> lightSources;
    vector<glm::vec3> lightColor;

    int x = location.x;
    int z = location.z;

    if (x - 1 >= 0 && x - 1 <= 7 && z - 1 >= 0 && z - 1 <= 7) {
        lightSources.push_back(glm::vec3(x - 1, -0.2f, z - 1));
        lightColor.push_back(grid[x - 1][z - 1]);
    }

    if (x - 1 >= 0 && x - 1 <= 7 && z >= 0 && z <= 7) {
        lightSources.push_back(glm::vec3(x - 1, -0.2f, z));
        lightColor.push_back(grid[x - 1][z]);
    }

    if (x - 1 >= 0 && x - 1 <= 7 && z + 1 <= 7 && z + 1 >= 0) {
        lightSources.push_back(glm::vec3(x - 1, -0.2f, z + 1));
        lightColor.push_back(grid[x - 1][z + 1]);
    }

    if (x >= 0 && x <= 7 && z - 1 >= 0 && z - 1 <= 7) {
        lightSources.push_back(glm::vec3(x, -0.2f, z - 1));
        lightColor.push_back(grid[x][z - 1]);
    }

    if (x >= 0 && x <= 7 && z >= 0 && z <= 7) {
        lightSources.push_back(glm::vec3(x, -0.2f, z));
        lightColor.push_back(grid[x][z]);
    }

    if (x >= 0 && x <= 7 && z + 1 <= 7 && z + 1 >= 0) {
        lightSources.push_back(glm::vec3(x, -0.2f, z + 1));
        lightColor.push_back(grid[x][z + 1]);
    }

    if (x + 1 <= 7 && x + 1 >= 0 && z - 1 >= 0 && z - 1 <= 7) {
        lightSources.push_back(glm::vec3(x  + 1, -0.2f, z - 1));
        lightColor.push_back(grid[x + 1][z - 1]);
    }

    if (x + 1 <= 7 && x + 1 >= 0 && z >= 0 && z <= 7) {
        lightSources.push_back(glm::vec3(x + 1, -0.2f, z));
        lightColor.push_back(grid[x + 1][z]);
    }

    if (x + 1 <= 7 && x + 1 >= 0 && z + 1 <= 7 && z + 1 >= 0) {
        lightSources.push_back(glm::vec3(x + 1, -0.2f, z + 1));
        lightColor.push_back(grid[x + 1][z + 1]);
    }


    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    int lightNumber = glGetUniformLocation(shader->program, "lightNumber");
    glUniform1i(lightNumber, lightSources.size());

    int objectType = glGetUniformLocation(shader->program, "objectType");
    glUniform1i(objectType, 2);

    GLint renderGridLocation = glGetUniformLocation(shader->program, "drawGrid");
    glUniform1i(renderGridLocation, renderGrid);

    GLint renderSpotLocation = glGetUniformLocation(shader->program, "drawSpot");
    glUniform1i(renderSpotLocation, renderSpots);

    GLint renderDiscoLocation = glGetUniformLocation(shader->program, "drawText");
    glUniform1i(renderDiscoLocation, renderDisco);

    int light_position = glGetUniformLocation(shader->program, "pointLight_position");
    glUniform3fv(light_position, lightSources.size(), glm::value_ptr(lightSources[0]));

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, 10);

    int material_kd = glGetUniformLocation(shader->program, "pointLight_color");
    glUniform3fv(material_kd, lightColor.size(), glm::value_ptr(lightColor[0]));

    int intensitateLight = glGetUniformLocation(shader->program, "pointLight_intensity");
    glUniform3f(intensitateLight, 0.75f, 0.75f, 0.75f);

    int spot_position = glGetUniformLocation(shader->program, "spotPositions");
    glUniform3fv(spot_position, 4, glm::value_ptr(spotPositions[0]));

    int light_direction = glGetUniformLocation(shader->program, "spotDirections");
    glUniform3fv(light_direction, 4, glm::value_ptr(spotLightsDirections[0]));

    int light_color = glGetUniformLocation(shader->program, "spotColor");
    glUniform3fv(light_color, 4, glm::value_ptr(spotLights[0]));

    int intensitateSpot = glGetUniformLocation(shader->program, "intensitySpot");
    glUniform3f(intensitateSpot, 0.35f, 0.35f, 0.35f);

    float time = Engine::GetElapsedTime();
    GLint rotation_time = glGetUniformLocation(shader->program, "rotation_time");
    glUniform1f(rotation_time, time / 10);

    if (texture1)
    {
        // TODO(student): Do these:
        // - activate texture location 0
        // - bind the texture1 ID
        // - send theuniform value
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    }

    if (texture2)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        // - bind the texture2 ID
        // - send the uniform value
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
    }

    GLint discoLoc = glGetUniformLocation(shader->program, "discoCenter");
    glUniform3f(discoLoc, discoCenter.x, discoCenter.y, discoCenter.z);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


glm::vec3 Tema3::GetDancerLocation(glm::vec3 dancerCoord) {
    float x = dancerCoord.x;
    float z = dancerCoord.z;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (x >= i - 0.5f && x <= i + 0.5f && z >= j - 0.5f && z <= j + 0.5f) {
                return glm::vec3(i, 0, j);
            }
        }
    }

    return glm::vec3(1);
}


vector<vector<glm::vec3>> Tema3::generateGrid() {
    vector<vector<glm::vec3>> map;

    for (int i = 0; i < 8; i++) {
        vector<glm::vec3> linie;
        for (int j = 0; j < 8; j++) {
            float r = ((float)rand() / RAND_MAX);
            float g = ((float)rand() / RAND_MAX);
            float b = ((float)rand() / RAND_MAX);

            linie.push_back(glm::vec3(r, g, b));
        }

        map.push_back(linie);
    }

    return map;
}


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 2;

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
        glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));
    }
}


void Tema3::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_ENTER) {
        if (dancersSpeed == 1.5f) {
            dancersSpeed = 0;
            system("taskkill \/IM \"wscript.exe\" \/F");
        }
        else {
            dancersSpeed = 1.5f;
            string s0 = pathcwd;
            s0 += "\\sounds\\";

            string s1 = "start ";
            s1 += s0;
            s1 += "sound.vbs ";
            s1 += s0;
            songIndex = (songIndex + 1) % 5;
            s1 += songs[songIndex];

            const char* cmd = s1.c_str();
            system(cmd);
        }
    }

    if (key == GLFW_KEY_1) {
        renderGrid = !renderGrid;
    }

    if (key == GLFW_KEY_2) {
        renderSpots = !renderSpots;
    }

    if (key == GLFW_KEY_3) {
        renderDisco = !renderDisco;
    }

    if (key == GLFW_KEY_4) {
        if (speedSpots == 0.1f) {
            speedSpots = 0;
        }
        else {
            speedSpots = 0.1f;
        }
    }

    if (key == GLFW_KEY_X) {
        system("taskkill \/IM \"wscript.exe\" \/F");
    }

    if (key == GLFW_KEY_P) {
        string s0 = pathcwd;
        s0 += "\\sounds\\";

        system("taskkill \/IM \"wscript.exe\" \/F");
        string s1 = "start ";
        s1 += s0;
        s1 += "sound.vbs ";
        s1 += s0;
        songIndex = (songIndex + 1) % 5;
        s1 += songs[songIndex];

        const char* cmd = s1.c_str();
        system(cmd);
    }
}


void Tema3::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}
