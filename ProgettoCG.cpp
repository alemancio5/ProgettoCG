#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"



// UBO structs
struct SphereMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct PlaneMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct PlanePUBO {
    alignas(4) glm::vec3 ambientColor;
    alignas(4) glm::vec3 lightPos;
    alignas(4) glm::vec3 lightColor;
};
struct ItemMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct ItemPUBO {
    alignas(4) glm::vec3 ambientColor;
    alignas(4) glm::vec3 lightPos;
    alignas(4) glm::vec3 lightColor;
};
struct BorderMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct WallMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct WallPUBO {
    alignas(4) glm::vec3 ambientColor;
    alignas(4) glm::vec3 lightPos;
    alignas(4) glm::vec3 lightColor;
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
struct ItemVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
};
struct BorderVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
};
struct WallVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
};



class Level1 : public BaseProject {
protected:

    // Sphere
    DescriptorSetLayout DSL_Sphere;
    VertexDescriptor VD_Sphere;
    Pipeline P_Sphere;
    Model M_Sphere;
    Texture T_Sphere{};
    DescriptorSet DS_Sphere;
    SphereMUBO UBOm_Sphere{};

    bool sphereJumping = false;
    bool sphereGoingUp = false;
    float sphereAccel = 500.0f;
    float sphereAccelSuper = 300.0f;
    float sphereJump = 70.0f;
    float sphereFriction = 0.95f;
    const float sphereRadius = 5.0f;
    glm::mat4 sphereMatrix = glm::mat4(1.0f);
    glm::vec3 spherePos{};
    glm::vec3 sphereCheckpoint{};
    glm::vec3 spherePosOld{};
    glm::vec3 spherePosSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 spherePosAccel = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat sphereRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 sphereRotSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sphereRotAccel = glm::vec3(0.0f, 0.0f, 0.0f);

    // Plane
    DescriptorSetLayout DSL_Plane;
    VertexDescriptor VD_Plane;
    Pipeline P_Plane;
    Model M_Plane;
    Texture T_Plane{};
    DescriptorSet DS_Plane;
    PlaneMUBO UBOm_Plane{};
    PlanePUBO UBOp_Plane{};

    float LInt = 50.0f;
    glm::vec3 LCol = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 LAmb = glm::vec3(0.1f,0.1f, 0.1f);

    // Item
    DescriptorSetLayout DSL_Item;
    VertexDescriptor VD_Item;
    Pipeline P_Item;
    Model M_Item;
    Texture T_Item{};
    DescriptorSet DS_Item;
    ItemMUBO UBOm_Item{};
    ItemPUBO UBOp_Item{};

    // Border
    DescriptorSetLayout DSL_Border;
    VertexDescriptor VD_Border;
    Pipeline P_Border;
    Model M_Border;
    Texture T_Border;
    DescriptorSet DS_Border;
    BorderMUBO UBOm_Border;

    // Wall
    DescriptorSetLayout DSL_Wall;
    VertexDescriptor VD_Wall;
    Pipeline P_Wall;
    Model M_Wall;
    Texture T_Wall{};
    DescriptorSet DS_Wall;
    WallMUBO UBOm_Wall{};
    WallPUBO UBOp_Wall{};

    // Map
    static const int mapSize = 1000;
    float mapHeight[mapSize][mapSize];
    float mapType[mapSize][mapSize];
    const float mapGravity = -150.0f;
    const glm::vec3 mapStartPos = glm::vec3(50.0f, 0.0f, 50.0f);

    // View
    float viewDistance = 15.0f + sphereRadius;
    float viewDistanceSuper = 30.0f + sphereRadius;
    float viewHeight = 10.0f + sphereRadius;
    float viewHeightSuper = 20.0f + sphereRadius;
    float viewAzimuth = 0.0f;
    float viewElevation = 0.0f;
    const float viewSpeed = glm::radians(170.0f);
    const float viewElevationMax = glm::radians(20.0f);
    const float viewElevationMin = -glm::radians(25.0f);
    float Ar{};
    glm::mat4 viewMatrix{};
    glm::vec3 viewPos{};
    glm::vec3 viewPosOld{};
    glm::vec3 viewPosLock = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 viewDir{};

    // Projection
    glm::mat4 projectionMatrix{};
    glm::mat4 projectionViewMatrix{};

    void setWindowParameters() override {
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "Labyball";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
        Ar = (float)windowWidth / (float)windowHeight;
    }

    void onWindowResize(int w, int h) override {
        Ar = (float)w / (float)h;
    }

    void localInit() override {
        // Sphere
        DSL_Sphere.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(SphereMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
        });
        VD_Sphere.init(this, {
            {0, sizeof(SphereVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(SphereVertex, pos),sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(SphereVertex, uv),sizeof(glm::vec2), UV},
        });
        P_Sphere.init(this, &VD_Sphere, "shaders/SphereVert.spv", "shaders/SphereFrag.spv", {&DSL_Sphere});
        M_Sphere.init(this, &VD_Sphere, "models/Sphere.obj", OBJ);
        T_Sphere.init(this, "textures/Sun.jpg");

        spherePos = mapStartPos + glm::vec3(0.0f, sphereRadius, 0.0f);
        spherePosOld = spherePos;
        sphereCheckpoint = spherePos;

        // Plane
        DSL_Plane.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(PlaneMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(PlanePUBO), 1},
        });
        VD_Plane.init(this, {
            {0, sizeof(PlaneVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PlaneVertex, pos), sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(PlaneVertex, uv), sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PlaneVertex, norm), sizeof(glm::vec3), NORMAL},

        });
        P_Plane.init(this, &VD_Plane, "shaders/PlaneFrag.spv", "shaders/PlaneVert.spv", {&DSL_Plane});
        M_Plane.init(this, &VD_Plane, "models/Plane.obj", OBJ);
        T_Plane.init(this, "textures/Grass.jpg");

        // Item
        DSL_Item.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(ItemMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ItemPUBO), 1},
        });
        VD_Item.init(this, {
            {0, sizeof(ItemVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ItemVertex, pos), sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(ItemVertex, uv), sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ItemVertex, norm), sizeof(glm::vec3), NORMAL},
        });
        P_Item.init(this, &VD_Item, "shaders/ItemFrag.spv", "shaders/ItemVert.spv", {&DSL_Item});
        M_Item.init(this, &VD_Item, "models/Item.obj", OBJ);
        T_Item.init(this, "textures/Bricks.jpg");

        // Border
        DSL_Border.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(BorderMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(WallPUBO), 1},
        });
        VD_Border.init(this, {
            {0, sizeof(BorderVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BorderVertex, pos),  sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(BorderVertex, uv),      sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BorderVertex, norm), sizeof(glm::vec3), NORMAL}
        });
        P_Border.init(this, &VD_Border, "shaders/BorderFrag.spv", "shaders/BorderVert.spv", {&DSL_Border});
        M_Border.init(this, &VD_Border, "models/Border.obj", OBJ);
        T_Border.init(this, "textures/Bricks.jpg");

        // Wall
        DSL_Wall.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(WallMUBO),   1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0,        1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(WallPUBO), 1},
        });
        VD_Wall.init(this, {
            {0, sizeof(BorderVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(WallVertex, pos),  sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(WallVertex, uv),      sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(WallVertex, norm), sizeof(glm::vec3), NORMAL}
        });
        P_Wall.init(this, &VD_Wall, "shaders/WallFrag.spv", "shaders/WallVert.spv", {&DSL_Wall});
        M_Wall.init(this, &VD_Wall, "models/Wall.obj", OBJ);
        T_Wall.init(this, "textures/Bricks.jpg");

        // Map
        mapInit();

        // Others
        DPSZs.uniformBlocksInPool = 9;
        DPSZs.texturesInPool = 5;
        DPSZs.setsInPool = 5;
    }

    void mapInit() {
        // Height initialization
        std::ifstream heightJsonFile("jsons/Height.json");
        nlohmann::json heightJson;
        heightJsonFile >> heightJson;
        for (const auto& loop : heightJson["loops"]) {
            int iStart = loop["i"]["start"];
            int iEnd = loop["i"]["end"];
            int jStart = loop["j"]["start"];
            int jEnd = loop["j"]["end"];
            float height = loop["height"];
            for (int i = iStart; i < iEnd; i++) {
                for (int j = jStart; j < jEnd; j++) {
                    mapHeight[i][j] = height;
                }
            }
        }

        // Type initialization
        std::ifstream itemsJsonFile("jsons/Type.json");
        nlohmann::json itemsJson;
        itemsJsonFile >> itemsJson;
        for (const auto& loop : itemsJson["loops"]) {
            int iStart = loop["i"]["start"];
            int iEnd = loop["i"]["end"];
            int jStart = loop["j"]["start"];
            int jEnd = loop["j"]["end"];
            float type = loop["type"];
            for (int i = iStart; i < iEnd; i++) {
                for (int j = jStart; j < jEnd; j++) {
                    mapType[i][j] = type;
                }
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

        // Item
        P_Item.create();
        DS_Item.init(this, &DSL_Item, {&T_Item});

        // Border Wall
        P_Border.create();
        DS_Border.init(this, &DSL_Border, {&T_Border});

        // Border Wall
        P_Wall.create();
        DS_Wall.init(this, &DSL_Border, {&T_Border});
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

        // Item
        DSL_Item.cleanup();
        VD_Item.cleanup();
        P_Item.destroy();
        M_Item.cleanup();
        T_Item.cleanup();

        // Border Wall
        DSL_Border.cleanup();
        VD_Border.cleanup();
        P_Border.destroy();
        M_Border.cleanup();
        T_Border.cleanup();

        // Level 0 Wall
        DSL_Wall.cleanup();
        VD_Wall.cleanup();
        P_Wall.destroy();
        M_Wall.cleanup();
        T_Wall.cleanup();

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

        // Item
        P_Item.cleanup();
        DS_Item.cleanup();


        // Border Wall
        P_Border.cleanup();
        DS_Border.cleanup();

        // Level 0 Wall
        P_Wall.cleanup();
        DS_Wall.cleanup();
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

        // Item
        P_Item.bind(commandBuffer);
        M_Item.bind(commandBuffer);
        DS_Item.bind(commandBuffer, P_Item, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Item.indices.size()), 1, 0, 0, 0);


        // Border Wall
        P_Border.bind(commandBuffer);
        M_Border.bind(commandBuffer);
        DS_Border.bind(commandBuffer, P_Border, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Border.indices.size()), 1, 0, 0, 0);

        // Level 0 Wall
        P_Wall.bind(commandBuffer);
        M_Wall.bind(commandBuffer);
        DS_Wall.bind(commandBuffer, P_Wall, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Wall.indices.size()), 1, 0, 0, 0);
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
            spherePosSpeed.y = sphereJump;
            sphereGoingUp = true;
            sphereJumping = true;
        }
        if (rotationInput.z == -1.0f  && sphereOnGround()) {
            if (mapType[(int)spherePos.x][(int)spherePos.z] == 1.0f) {
                sphereCheckpoint = spherePos;
                std::cout << "Checkpoint saved: " << spherePos.x << ", " << spherePos.y << ", " << spherePos.z << std::endl;
            }
            if (mapType[(int)spherePos.x][(int)spherePos.z] == 2.0f) {
                sphereAccel = sphereAccelSuper;
                std::cout << "Super Speed taken: " << spherePos.x << ", " << spherePos.y << ", " << spherePos.z << std::endl;
            }
            if (mapType[(int)spherePos.x][(int)spherePos.z] == 3.0f) {
                viewDistance = viewDistanceSuper;
                viewHeight = viewHeightSuper;
                std::cout << "Super View taken: " << spherePos.x << ", " << spherePos.y << ", " << spherePos.z << std::endl;
            }
            if (mapType[(int)spherePos.x][(int)spherePos.z] == 4.0f) {
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

        // Collision detection
        if (mapHeight[(int)x][(int)viewPosOld.z] > spherePos.y) {
            x = viewPosOld.x;
            viewPosLock.x = 1.0f;
        } else if (mapHeight[(int)viewPosOld.x][(int)z] > spherePos.y) {
            z = viewPosOld.z;
            viewPosLock.z = 1.0f;
        }
        viewPos = glm::vec3(x, y, z);
        viewPosLock = glm::vec3(0.0f);

        // Update matrix
        viewMatrix = glm::lookAt(viewPos, spherePos, glm::vec3(0.0f, 1.0f, 0.0f));
        viewDir = glm::normalize(glm::vec3(glm::sin(viewAzimuth), glm::sin(viewElevation), glm::cos(viewAzimuth)));
    }

    void updateSphereVariables(float deltaTime) {
        // Update position in y
        if (sphereGoingUp) {
            spherePosSpeed.y = sphereJump;
            spherePos.y += spherePosSpeed.y * deltaTime;
            if (spherePos.y > 2 * sphereRadius) {
                sphereGoingUp = false;
            }
        }
        else if (spherePos.y > mapHeight[(int)spherePos.x][(int)spherePos.z] + sphereRadius) {
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
        if (mapHeight[(int)(spherePos.x + sphereRadius)][(int)(spherePosOld.z)] > spherePos.y) {
            if (spherePosSpeed.x >= 0.0f)
                spherePos.x = spherePosOld.x;
        }
        else if (mapHeight[(int)(spherePos.x - sphereRadius)][(int)(spherePosOld.z)] > spherePos.y) {
            if (spherePosSpeed.x <= 0.0f)
                spherePos.x = spherePosOld.x;
        }
        if (mapHeight[(int)(spherePos.x)][(int)(spherePosOld.z + sphereRadius)] > spherePos.y) {
            if (spherePosSpeed.z >= 0.0f)
                spherePos.z = spherePosOld.z;
        }
        else if (mapHeight[(int)(spherePos.x)][(int)(spherePosOld.z - sphereRadius)] > spherePos.y) {
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

    bool sphereOnGround() {
        if (mapHeight[(int)spherePos.x][(int)spherePos.z] + sphereRadius == spherePos.y)
            return true;
        else
            return false;
    }

    void updateUBO(uint32_t currentImage) {
        // Sphere
        UBOm_Sphere.mvpMat = projectionViewMatrix * sphereMatrix;
        UBOm_Sphere.mMat = sphereMatrix;
        UBOm_Sphere.nMat = glm::mat4(1.0f);
        DS_Sphere.map(currentImage, &UBOm_Sphere, 0);

        // Plane
        UBOm_Plane.mvpMat = projectionViewMatrix;
        UBOm_Plane.mMat = glm::mat4(1.0f);
        UBOm_Plane.nMat = glm::mat4(1.0f);
        DS_Plane.map(currentImage, &UBOm_Plane, 0);

        UBOp_Plane.lightColor = glm::vec4(LCol, LInt);
        UBOp_Plane.lightPos = spherePos;
        UBOp_Plane.ambientColor = LAmb;
        DS_Plane.map(currentImage, &UBOp_Plane, 2);

        // Item
        UBOm_Item.mMat = glm::mat4(1.0f);
        UBOm_Item.nMat = glm::mat4(1.0f);
        UBOm_Item.mvpMat = projectionViewMatrix * UBOm_Item.mMat;
        DS_Item.map(currentImage, &UBOm_Item, 0);

        UBOp_Item.lightColor = glm::vec4(LCol, LInt);
        UBOp_Item.lightPos = spherePos;
        UBOp_Item.ambientColor = LAmb;
        DS_Item.map(currentImage, &UBOp_Item, 2);

        // Border
        UBOm_Border.mMat = glm::mat4(1.0f);
        UBOm_Border.nMat = glm::mat4(1.0f);
        UBOm_Border.mvpMat = projectionViewMatrix * UBOm_Border.mMat;
        DS_Border.map(currentImage, &UBOm_Border, 0);

        // Wall
        UBOm_Wall.mMat = glm::mat4(1.0f);
        UBOm_Wall.nMat = glm::mat4(1.0f);
        UBOm_Wall.mvpMat = projectionViewMatrix * UBOm_Wall.mMat;
        DS_Wall.map(currentImage, &UBOm_Wall, 0);

        UBOp_Wall.lightColor = glm::vec4(LCol, LInt);
        UBOp_Wall.lightPos = spherePos;
        UBOp_Wall.ambientColor = LAmb;
        DS_Border.map(currentImage, &UBOp_Wall, 2);
        DS_Wall.map(currentImage, &UBOp_Wall, 2);
    }
};



int main() {
    Level1 app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// TODO Muri che lampeggiano (è un problema di shader)
