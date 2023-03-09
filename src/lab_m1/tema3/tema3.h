#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"
#include <iostream>

using namespace std;

namespace m1
{
    class Tema3 : public gfxc::SimpleScene
    {
     public:
         Tema3();
        ~Tema3();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color = glm::vec3(1));
        void RenderGrid(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, Texture2D* texture1, Texture2D* texture2, glm::vec3 discoCenter);
        void RenderWall(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 location, Texture2D* texture1, Texture2D* texture2, glm::vec3 discoCenter);
        void RenderDancers(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 location, Texture2D* texture1, Texture2D* texture2, glm::vec3 discoCenter);
        void RenderDiscoBall(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1, Texture2D* texture2);
        void MoveDancers(float deltaTimeSeconds);
        glm::vec3 GetDancerLocation(glm::vec3 dancerCoord);
        void rotateSpots(float deltaTimeSeconds);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        vector<vector<glm::vec3>> generateGrid();

        Texture2D* CreateRandomTexture(unsigned int width, unsigned int height);

        unsigned int materialShininess;


        // TODO(student): If you need any other class variables, define them here.
        float totalOZ;
        float totalOX;
        vector<vector<glm::vec3>> grid;
        vector<glm::vec3> dancers;
        vector<glm::vec3> dancersDistance;
        float dancersSpeed;
        bool renderGrid;
        bool renderSpots;
        bool renderDisco;
        vector<glm::vec3> spotLights;
        vector<glm::vec3> spotLightsDirections;
        vector<float> angleSpot;
        vector<int> tendencySpot;

        float PointsCircle = 501;
        float grades = 360.0 / (PointsCircle - 1) * 0.0174532925;
        vector<glm::vec3> spotPositions;

        vector<string> songs;
        char pathcwd[100];
        int songIndex;
        float speedSpots;

        unordered_map<string, Texture2D*> mapTextures;
    };
}   // namespace m1
