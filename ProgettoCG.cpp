#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"

// Map variables
const int mapSize = 1000.0f;
float mapHeight[mapSize][mapSize];
float mapItems[mapSize][mapSize];
const float mapGravity = -100.0f;

// Uniform Buffer Object structs
struct SphereMatrUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct PlaneMatrUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct PlaneParamUBO {
    alignas(4) glm::vec3 ambientColor;
    alignas(4) glm::vec3 lightPos;
    alignas(4) glm::vec3 lightColor;
};

#define BORDER_WALL 4
struct BorderWallMatrUBO {
    alignas(16) glm::mat4 mvpMat[BORDER_WALL];
    alignas(16) glm::mat4 mMat[BORDER_WALL];
    alignas(16) glm::mat4 nMat[BORDER_WALL];
};

struct WallParamUBO {
    alignas(4) glm::vec3 ambientColor;
    alignas(4) glm::vec3 lightPos;
    alignas(4) glm::vec3 lightColor;
};

#define LEVEL_0_WALL 10
struct Level0WallMatrUBO {
    alignas(16) glm::mat4 mvpMat[LEVEL_0_WALL];
    alignas(16) glm::mat4 mMat[LEVEL_0_WALL];
    alignas(16) glm::mat4 nMat[LEVEL_0_WALL];
};

// Vertexes structs
struct SphereVertex {
    glm::vec3 pos;
    glm::vec2 uv;
};
struct PlaneVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
};

struct BorderWallVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
};
struct Level0WallVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
};



class Labyball : public BaseProject {
protected:
    // Sphere
    DescriptorSetLayout DSL_Sphere;
    VertexDescriptor VD_Sphere;
    Pipeline P_Sphere;
    Model M_Sphere;
    Texture T_Sphere{};
    DescriptorSet DS_Sphere;

    // Plane
    DescriptorSetLayout DSL_Plane;
    VertexDescriptor VD_Plane;
    Pipeline P_Plane;
    Model M_Plane;
    Texture T_Plane{};
    DescriptorSet DS_Plane;

    // Border Wall
    DescriptorSetLayout DSL_Border_Wall;
    VertexDescriptor VD_Border_Wall;
    Pipeline P_Border_Wall;
    Model M_Border_Wall;
    Texture T_Border_Wall;
    DescriptorSet DS_Border_Wall;

    // Level 0 Wall
    DescriptorSetLayout DSL_Level_0_Wall;
    VertexDescriptor VD_Level_0_Wall;
    Pipeline P_Level_0_Wall;
    Model M_Level_0_Wall;
    Texture T_Level_0_Wall{};
    DescriptorSet DS_Level_0_Wall;

    // Sphere variables
    const float sphereRadius = 1.0f;
    float sphereAccel = 500.0f; //100.0f;
    float sphereFriction = 0.95f;
    bool sphereJumping = false;
    glm::mat4 sphereMatrix = glm::mat4(1.0f);
    glm::vec3 spherePos = glm::vec3(50.0f, 1.0f, 50.0f);
    glm::vec3 sphereCheckpoint = spherePos;
    glm::vec3 spherePosOld = spherePos;
    glm::vec3 spherePosSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 spherePosAccel = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat sphereRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 sphereRotSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sphereRotAccel = glm::vec3(0.0f, 0.0f, 0.0f);

    // Plane variables
    float LInt = 50.0f;
    glm::vec3 LCol = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 LAmb = glm::vec3(0.1f,0.1f, 0.1f);

    // Projection variables
    glm::mat4 projectionMatrix{};
    glm::mat4 projectionViewMatrix{};

    // View variables
    float viewDistance = 8.0f;
    float viewHeight = 3.0f;
    const float viewSpeed = glm::radians(120.0f);
    float viewAzimuth = 0.0f;
    float viewElevation = 0.0f;
    const float viewElevationMax = glm::radians(20.0f);
    const float viewElevationMin = -glm::radians(25.0f);
    float Ar{};
    glm::mat4 viewMatrix{};
    glm::vec3 viewPos{};
    glm::vec3 viewPosOld{};
    glm::vec3 viewPosLock = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 viewDir{};


    void setWindowParameters() override {
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "Labyball";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.1f, 0.3f, 0.5f, 1.0f};
        Ar = (float)windowWidth / (float)windowHeight;
    }

    void onWindowResize(int w, int h) override {
        Ar = (float)w / (float)h;
    }

    void localInit() override {
        // Level variables initialization
        mapInit();

        // Sphere
        DSL_Sphere.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SphereMatrUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
        });
        VD_Sphere.init(this, {
            {0, sizeof(SphereVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SphereVertex, pos),sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(SphereVertex, uv),sizeof(glm::vec2), UV},
        });
        P_Sphere.init(this, &VD_Sphere, "shaders/EmissionVert.spv", "shaders/EmissionFrag.spv", {&DSL_Sphere});
        M_Sphere.init(this, &VD_Sphere, "models/Sphere.gltf", GLTF);
        T_Sphere.init(this, "textures/Sun.jpg");

        // Plane
        DSL_Plane.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(PlaneMatrUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(PlaneParamUBO), 1},
        });

        VD_Plane.init(this, {
            {0, sizeof(PlaneVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PlaneVertex, pos), sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(PlaneVertex, uv), sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PlaneVertex, norm), sizeof(glm::vec3), NORMAL},

        });
        P_Plane.init(this, &VD_Plane, "shaders/PlaneVert.spv", "shaders/PlaneFrag.spv", {&DSL_Plane});
        M_Plane.init(this, &VD_Plane, "models/Plane.obj", OBJ);
        T_Plane.init(this, "textures/Grass.jpg");

        // Border Wall
        DSL_Border_Wall.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(BorderWallMatrUBO), 1},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
                {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(WallParamUBO), 1},
        });

        VD_Border_Wall.init(this, {
                {0, sizeof(BorderWallVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BorderWallVertex, pos), sizeof(glm::vec3), POSITION},
                {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(BorderWallVertex, uv), sizeof(glm::vec2), UV},
                {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BorderWallVertex, norm), sizeof(glm::vec3), NORMAL}
        });
        P_Border_Wall.init(this, &VD_Border_Wall, "shaders/BorderWallVert.spv", "shaders/BorderWallFrag.spv", {&DSL_Border_Wall});
        M_Border_Wall.init(this, &VD_Border_Wall, "models/Border.obj", OBJ);
        T_Border_Wall.init(this, "textures/Bricks.jpg");

        // Level 0 Wall
        DSL_Level_0_Wall.init(this, {
                {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(Level0WallMatrUBO), 1},
                {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
                {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(WallParamUBO), 1},
        });

        VD_Level_0_Wall.init(this, {
                {0, sizeof(BorderWallVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
                {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Level0WallVertex, pos), sizeof(glm::vec3), POSITION},
                {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Level0WallVertex, uv), sizeof(glm::vec2), UV},
                {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Level0WallVertex, norm), sizeof(glm::vec3), NORMAL}
        });
        // TODO: Shader could be the same of the borders, but for now i've mad a shader for level0
        P_Level_0_Wall.init(this, &VD_Level_0_Wall, "shaders/Level0WallVert.spv", "shaders/Level0WallFrag.spv", {&DSL_Level_0_Wall});
        M_Level_0_Wall.init(this, &VD_Level_0_Wall, "models/Wall.obj", OBJ);
        T_Level_0_Wall.init(this, "textures/Bricks.jpg");

        // Others
        DPSZs.uniformBlocksInPool = 7;
        DPSZs.texturesInPool = 4;
        DPSZs.setsInPool = 3 + 1;
    }

    void mapInit() {
        // Height initialization
        for (int i = 0; i < mapSize; i++) { // Ground
            for (int j = 0; j < mapSize; j++) {
                mapHeight[i][j] = 0.0f;
            }
        }
        for (int i = 0; i < 1000; i++) { // Wall #1
            for (int j = 0; j < 20; j++) {
                mapHeight[i][j] = 40.0f;
            }
        }
        for (int i = 0; i < 1000; i++) { // Wall #2
            for (int j = 980; j < 1000; j++) {
                mapHeight[i][j] = 40.0f;
            }
        }
        for (int i = 0; i < 20; i++) { // Wall #3
            for (int j = 0; j < 1000; j++) {
                mapHeight[i][j] = 40.0f;
            }
        }
        for (int i = 980; i < 1000; i++) { // Wall #4
            for (int j = 0; j < 1000; j++) {
                mapHeight[i][j] = 40.0f;
            }
        }

        // Items initialization
        for (int i = 0; i < mapSize; i++) { // No items
            for (int j = 0; j < mapSize; j++) {
                mapItems[i][j] = 0.0f;
            }
        }
        for (int i = 40; i < 60; i++) { // Checkpoint #1
            for (int j = 40; j < 60; j++) {
                mapItems[i][j] = 1.0f;
            }
        }
        for (int i = 790; i < 810; i++) { // Checkpoint #2
            for (int j = 490; j < 510; j++) {
                mapItems[i][j] = 1.0f;
            }
        }
        for (int i = 330; i < 350; i++) { // Checkpoint #3
            for (int j = 710; j < 730; j++) {
                mapItems[i][j] = 1.0f;
            }
        }
        for (int i = 660; i < 680; i++) { // Super Speed #1
            for (int j = 140; j < 160; j++) {
                mapItems[i][j] = 2.0f;
            }
        }
        for (int i = 940; i < 960; i++) { // Super View #1
            for (int j = 760; j < 780; j++) {
                mapItems[i][j] = 3.0f;
            }
        }
        for (int i = 900; i < 920; i++) { // Win #1
            for (int j = 900; j < 920; j++) {
                mapItems[i][j] = 4.0f;
            }
        }
    }

    void pipelinesAndDescriptorSetsInit() override {
        // Sphere
        P_Sphere.create();
        DS_Sphere.init(this, &DSL_Sphere, {&T_Sphere});

        // Plane
        P_Plane.create();
        DS_Plane.init(this, &DSL_Plane, {&T_Plane});

        // Border Wall
        P_Border_Wall.create();
        DS_Border_Wall.init(this, &DSL_Border_Wall, {&T_Border_Wall});

        // Border Wall
        P_Level_0_Wall.create();
        DS_Level_0_Wall.init(this, &DSL_Border_Wall, {&T_Border_Wall});
    }

    void localCleanup() override {
        // Sphere
        DSL_Sphere.cleanup();
        VD_Sphere.cleanup();
        P_Sphere.destroy();
        M_Sphere.cleanup();
        T_Sphere.cleanup();

        // Plane
        DSL_Plane.cleanup();
        VD_Plane.cleanup();
        P_Plane.destroy();
        M_Plane.cleanup();
        T_Plane.cleanup();

        // Border Wall
        DSL_Border_Wall.cleanup();
        VD_Border_Wall.cleanup();
        P_Border_Wall.destroy();
        M_Border_Wall.cleanup();
        T_Border_Wall.cleanup();

        // Level 0 Wall
        DSL_Level_0_Wall.cleanup();
        VD_Level_0_Wall.cleanup();
        P_Level_0_Wall.destroy();
        M_Level_0_Wall.cleanup();
        T_Level_0_Wall.cleanup();

        // Others
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void pipelinesAndDescriptorSetsCleanup() override {
        // Sphere
        P_Sphere.cleanup();
        DS_Sphere.cleanup();

        // Plane
        P_Plane.cleanup();
        DS_Plane.cleanup();

        // Border Wall
        P_Border_Wall.cleanup();
        DS_Border_Wall.cleanup();

        // Level 0 Wall
        P_Level_0_Wall.cleanup();
        DS_Level_0_Wall.cleanup();
    }

    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) override {
        // Sphere
        P_Sphere.bind(commandBuffer);
        M_Sphere.bind(commandBuffer);
        DS_Sphere.bind(commandBuffer, P_Sphere, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Sphere.indices.size()), 1, 0, 0, 0);

        // Plane
        P_Plane.bind(commandBuffer);
        M_Plane.bind(commandBuffer);
        DS_Plane.bind(commandBuffer, P_Plane, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Plane.indices.size()), 1, 0, 0, 0);

        // Border Wall
        P_Border_Wall.bind(commandBuffer);
        M_Border_Wall.bind(commandBuffer);
        DS_Border_Wall.bind(commandBuffer, P_Border_Wall, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Border_Wall.indices.size()), BORDER_WALL, 0, 0, 0);

        // Level 0 Wall
        P_Level_0_Wall.bind(commandBuffer);
        M_Level_0_Wall.bind(commandBuffer);
        DS_Level_0_Wall.bind(commandBuffer, P_Level_0_Wall, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Level_0_Wall.indices.size()), LEVEL_0_WALL, 0, 0, 0);
    }

    void updateUniformBuffer(uint32_t currentImage) override {
        // Get inputs
        float deltaTime;
        auto movementInput = glm::vec3(0.0f);
        auto rotationInput = glm::vec3(0.0f);
        bool fireInput = false;
        getSixAxis(deltaTime, movementInput, rotationInput, fireInput);

        // Use inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) { // Game exit
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (movementInput.x == -1.0f || rotationInput.y == -1.0f) { // View left
            if (!viewPosLock.x)
                viewAzimuth += viewSpeed * deltaTime;
        } else if (movementInput.x == 1.0f || rotationInput.y == 1.0f) { // View right
            if (!viewPosLock.z)
                viewAzimuth -= viewSpeed * deltaTime;
        }
        if (rotationInput.x == -1.0f) { // View up
            viewElevation -= viewSpeed * deltaTime;
            if (viewElevation < viewElevationMin)
                viewElevation = viewElevationMin;
        } else if (rotationInput.x == 1.0f) { // View down
            viewElevation += viewSpeed * deltaTime;
            if (viewElevation > viewElevationMax)
                viewElevation = viewElevationMax;
        }
        if (movementInput.z == -1.0f) { // Sphere forward movement
            spherePosAccel = -viewDir * sphereAccel;
        } else if (movementInput.z == 1.0f) { // Sphere backward movement
            spherePosAccel = viewDir * sphereAccel;
        } else {
            spherePosAccel.x = 0.0f;
            spherePosAccel.z = 0.0f;
        }
        if (fireInput && !sphereJumping) { // Sphere jump
            spherePosSpeed.y = 60.0f;
            spherePos.y = spherePosSpeed.y * deltaTime;
            sphereJumping = true;
        }
        if (rotationInput.z == -1.0f) {
            if (mapItems[(int)spherePos.x][(int)spherePos.z] == 1.0f) {
                sphereCheckpoint = spherePos;
                std::cout << "Checkpoint saved: " << spherePos.x << ", " << spherePos.y << ", " << spherePos.z << std::endl;
            }
            if (mapItems[(int)spherePos.x][(int)spherePos.z] == 2.0f) {
                sphereAccel = 200.0f;
                std::cout << "Super Speed taken: " << spherePos.x << ", " << spherePos.y << ", " << spherePos.z << std::endl;
            }
            if (mapItems[(int)spherePos.x][(int)spherePos.z] == 3.0f) {
                viewDistance = 20.0f + sphereRadius;
                viewHeight = 15.0f + sphereRadius;
                std::cout << "Super View taken: " << spherePos.x << ", " << spherePos.y << ", " << spherePos.z << std::endl;
            }
            if (mapItems[(int)spherePos.x][(int)spherePos.z] == 4.0f) {
                std::cout << "Win taken: " << spherePos.x << ", " << spherePos.y << ", " << spherePos.z << std::endl;
            }
        }

        // View variables update
        updateViewVariables();

        // Sphere variables update
        updateSphereVariables(deltaTime);

        // Projection variables update
        projectionMatrix = glm::perspective(glm::radians(75.0f), Ar, 0.1f, 160.0f);
        projectionMatrix[1][1] *= -1;
        projectionViewMatrix = projectionMatrix * viewMatrix;

        // UBOs update
        updateUBO(currentImage);
    }

    void updateViewVariables() {
        // Compute the new view position
        viewPosOld = viewPos;
        float x = spherePos.x + viewDistance * glm::sin(viewAzimuth);
        float z = spherePos.z + viewDistance * glm::cos(viewAzimuth);
        float y = spherePos.y + viewHeight + viewDistance * glm::sin(viewElevation);

        // Check for conflicts with the map obstacles
        if (mapHeight[(int)x][(int)viewPosOld.z] > mapHeight[(int)spherePos.x][(int)spherePos.z]) {
            x = viewPosOld.x;
            viewPosLock.x = 1.0f;
        } else if (mapHeight[(int)viewPosOld.x][(int)z] > mapHeight[(int)spherePos.x][(int)spherePos.z]) {
            z = viewPosOld.z;
            viewPosLock.z = 1.0f;
        }
        viewPos = glm::vec3(x, y, z);
        viewPosLock = glm::vec3(0.0f);

        // Update
        viewMatrix = glm::lookAt(viewPos, spherePos, glm::vec3(0.0f, 1.0f, 0.0f));
        viewDir = glm::normalize(glm::vec3(glm::sin(viewAzimuth), glm::sin(viewElevation), glm::cos(viewAzimuth)));
    }

    void updateSphereVariables(float deltaTime) {
        // Update position in y
        if (spherePos.y > mapHeight[(int)spherePos.x][(int)spherePos.z] + sphereRadius) {
            spherePosAccel.y = mapGravity;
            spherePosSpeed.y += spherePosAccel.y * deltaTime;
            spherePos.y += spherePosSpeed.y * deltaTime;
            sphereJumping = true;
        }
        else {
            spherePosAccel.y = 0.0f;
            spherePosSpeed.y = 0.0f;
            spherePos.y = mapHeight[(int)spherePos.x][(int)spherePos.z] + sphereRadius;
            sphereJumping = false;
        }

        // Update position in x and z
        spherePosOld = spherePos;
        spherePosSpeed.x += spherePosAccel.x * deltaTime;
        spherePosSpeed.z += spherePosAccel.z * deltaTime;
        spherePos.x += spherePosSpeed.x * deltaTime;
        spherePos.z += spherePosSpeed.z * deltaTime;
        spherePosSpeed.x *= sphereFriction;
        spherePosSpeed.z *= sphereFriction;

        // Collision detection
        if (mapHeight[(int)(spherePos.x + sphereRadius)][(int)(spherePosOld.z)] > mapHeight[(int)(spherePos.x)][(int)(spherePos.z)]) {
            if (spherePosSpeed.x >= 0.0f)
                spherePos.x = spherePosOld.x;
        }
        else if (mapHeight[(int)(spherePos.x - sphereRadius)][(int)(spherePosOld.z)] > mapHeight[(int)(spherePos.x)][(int)(spherePos.z)]) {
            if (spherePosSpeed.x <= 0.0f)
                spherePos.x = spherePosOld.x;
        }
        if (mapHeight[(int)(spherePos.x)][(int)(spherePosOld.z + sphereRadius)] > mapHeight[(int)(spherePos.x)][(int)(spherePos.z)]) {
            if (spherePosSpeed.z >= 0.0f)
                spherePos.z = spherePosOld.z;
        }
        else if (mapHeight[(int)(spherePos.x)][(int)(spherePosOld.z - sphereRadius)] > mapHeight[(int)(spherePos.x)][(int)(spherePos.z)]) {
            if (spherePosSpeed.z <= 0.0f)
                spherePos.z = spherePosOld.z;
        }

        // Update rotation
        float sphereSpeed = glm::length(spherePosSpeed);
        if (sphereSpeed > 0.0001f) {
            float rotationAngle = sphereSpeed * deltaTime / sphereRadius;
            glm::vec3 movementDir = glm::normalize(spherePosSpeed);
            glm::vec3 rotationAxis = glm::cross(movementDir, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::quat rotationIncrement = glm::angleAxis(rotationAngle, -rotationAxis);
            sphereRot = glm::normalize(rotationIncrement * sphereRot);
        }

        // Update matrix
        sphereMatrix = glm::translate(glm::mat4(1.0f), spherePos) * glm::mat4_cast(sphereRot);
    }

    void updateUBO(uint32_t currentImage) {
        // Sphere UBO update
        SphereMatrUBO UBOm_Sphere{};
        UBOm_Sphere.mvpMat = projectionViewMatrix * sphereMatrix;
        UBOm_Sphere.mMat = sphereMatrix;
        UBOm_Sphere.nMat = glm::mat4(1.0f);
        DS_Sphere.map(currentImage, &UBOm_Sphere, 0);

        // Plane UBO update
        PlaneMatrUBO UBOm_Plane{};
        UBOm_Plane.mvpMat = projectionViewMatrix;
        UBOm_Plane.mMat = glm::mat4(1.0f);
        UBOm_Plane.nMat = glm::mat4(1.0f);
        DS_Plane.map(currentImage, &UBOm_Plane, 0);

        PlaneParamUBO UBOp_Plane{};
        UBOp_Plane.lightColor = glm::vec4(LCol, LInt);
        UBOp_Plane.lightPos = spherePos;
        UBOp_Plane.ambientColor = LAmb;
        DS_Plane.map(currentImage, &UBOp_Plane, 2);

        // Border Wall UBO Update
        BorderWallMatrUBO UBOm_BorderWall{};
        for (int i = 0; i < BORDER_WALL; i++) {
            if (i == 0) {
                // Wall 1: Vertical from (0,0) to (0,1000)
                UBOm_BorderWall.mMat[i] = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, 0.0f)) *
                                          glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            } else if (i == 1) {
                // Wall 2: Horizontal from (0,1000) to (1000,1000)
                UBOm_BorderWall.mMat[i] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 980.0f));
            } else if (i == 2) {
                // Wall 3: Vertical from (1000,1000) to (1000,0)
                UBOm_BorderWall.mMat[i] = glm::translate(glm::mat4(1.0f), glm::vec3(1000.0f, 0.0f, 0.0f)) *
                                          glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            } else if (i == 3) {
                // Wall 4: Horizontal from (1000,0) to (0,0)
                UBOm_BorderWall.mMat[i] = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
            }
            UBOm_BorderWall.mvpMat[i] = projectionViewMatrix * UBOm_BorderWall.mMat[i];
            UBOm_BorderWall.nMat[i] = glm::inverse(glm::transpose(UBOm_BorderWall.mMat[i]));
        }
        DS_Border_Wall.map(currentImage, &UBOm_BorderWall, 0);

        // Level 0 Wall UBO Update
        Level0WallMatrUBO UBOm_Level0Wall{};
        for (int i = 0; i < LEVEL_0_WALL; i++) {
            // TODO: Get ata from JSON
            UBOm_Level0Wall.mMat[i] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 0.0f, (i+1)*100.0f))
                    * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            UBOm_Level0Wall.mvpMat[i] = projectionViewMatrix * UBOm_Level0Wall.mMat[i];
            UBOm_Level0Wall.nMat[i] = glm::inverse(glm::transpose(UBOm_Level0Wall.mMat[i]));
        }
        DS_Level_0_Wall.map(currentImage, &UBOm_Level0Wall, 0);

        WallParamUBO UBOp_Wall{};
        UBOp_Wall.lightColor = glm::vec4(LCol, LInt);
        UBOp_Wall.lightPos = spherePos;
        UBOp_Wall.ambientColor = LAmb;
        DS_Border_Wall.map(currentImage, &UBOp_Wall, 2);
        DS_Level_0_Wall.map(currentImage, &UBOp_Wall, 2);
    }
};



int main() {
    Labyball app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
