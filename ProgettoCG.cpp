#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"

std::string scene = "Level1";
bool closeapp = false;

// Texts of the game
std::vector<SingleText> textLives = {
    {1, {"LIVES: 0", "", "",""}, 0, 0},
    {1, {"LIVES: 1", "", "",""}, 0, 0},
    {1, {"LIVES: 2", "", "",""}, 0, 0},
    {1, {"LIVES: 3", "","",""}, 0, 0}
};
std::vector<SingleText> textMessage = {
    {2, {"", "E:", "",""}, 0, 0},
    {2, {"", "E: CHECKPOINT", "",""}, 0, 0},
    {2, {"", "E: SUPER SPEED", "",""}, 0, 0},
    {2, {"", "E: SUPER VIEW", "",""}, 0, 0},
    {2, {"", "E: WIN", "",""}, 0, 0},
    {2, {"", "YOU WIN", "",""}, 0, 0},
    {2, {"", "DONE!", "",""}, 0, 0}
};
std::vector<SingleText> textFinish = {
    {3, {"", "", "",""}, 0, 0},
    {3, {"", "", "YOU WIN",""}, 0, 0},
    {3, {"", "", "GAME OVER",""}, 0, 0}
};

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
struct ItemMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct StepMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct IronMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
};
struct DecorationMUBO {
    alignas(16) glm::mat4 mvpMat;
    alignas(16) glm::mat4 mMat;
    alignas(16) glm::mat4 nMat;
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
struct ShaderLightPUBO {
    alignas(16) glm::vec3 viewPos;
    alignas(16) glm::vec3 lightPos;
    alignas(16) glm::vec4 lightColor;
    alignas(16) glm::vec4 lightStatus;
    alignas(4) float ambientStrength;
    alignas(4) float specularStrength;
    alignas(4) float shininess;
    alignas(4) float cosIn;
    alignas(4) float cosOut;
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
struct StepVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
};
struct IronVertex {
    glm::vec3 pos;
    glm::vec2 uv;
    glm::vec3 norm;
};
struct DecorationVertex {
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



class Level : public BaseProject {
protected:

    // Level
    static const int levelSize = 100;
    float levelHeight[levelSize][levelSize];
    float levelType[levelSize][levelSize];
    const glm::vec3 levelStart = glm::vec3(5.0f, 0.0f, 5.0f);
    std::string levelPathPrefix = "";
    std::string levelPathHeight = "jsons/Height.json";
    std::string levelPathType = "jsons/Type.json";

    // Shader parameters
    ShaderLightPUBO UBOp_ShaderLights;
    glm::vec4 lightStatus = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 LCol = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 LAmb = glm::vec3(0.1f,0.1f, 0.1f);
    float LInt = 0.5f;
    float ambientStrength = 10.0f;
    float specularStrength = 0.1f;
    float shininess = 10.0f;
    float cosIn = glm::cos(0.4f);
    float cosOut = glm::cos(0.5f);
    std::string lightPathVert = "shaders/NormalVert.spv";
    std::string lightPathFrag = "shaders/NormalFrag.spv";

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
    int sphereLives = 3;
    float sphereAccel = 28.0f;
    float sphereAccelSuper = 33.0f;
    float sphereAccelUp = 23.0f;
    float sphereAccelUpSuper = 28.0f;
    float sphereJump = 6.0f;
    float sphereJumpSuper = 9.0f;
    float sphereFriction = 0.95f;
    float sphereGravity = -20.0f;
    float sphereRadius = 0.5f;
    glm::mat4 sphereMatrix = glm::mat4(1.0f);
    glm::vec3 spherePos{};
    glm::vec3 sphereCheckpoint{};
    glm::vec3 spherePosOld{};
    glm::vec3 spherePosSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 spherePosAccel = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat sphereRot = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 sphereRotSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sphereRotAccel = glm::vec3(0.0f, 0.0f, 0.0f);
    std::string spherePathVert = "shaders/SphereVert.spv";
    std::string spherePathFrag = "shaders/SphereFrag.spv";
    std::string spherePathModel = "models/Sphere.obj";
    std::string spherePathTexture = "textures/Sphere.jpg";

    // Plane
    DescriptorSetLayout DSL_Plane;
    VertexDescriptor VD_Plane;
    Pipeline P_Plane;
    Model M_Plane;
    Texture T_Plane{};
    DescriptorSet DS_Plane;
    PlaneMUBO UBOm_Plane{};
    std::string planePathVert = "shaders/PlaneVert.spv";
    std::string planePathFrag = "shaders/PlaneFrag.spv";
    std::string planePathModel = "models/Plane.obj";
    std::string planePathTexture = "textures/Plane.jpg";

    // Item
    DescriptorSetLayout DSL_Item;
    VertexDescriptor VD_Item;
    Pipeline P_Item;
    Model M_Item;
    Texture T_Item{};
    DescriptorSet DS_Item;
    ItemMUBO UBOm_Item{};
    std::string itemPathVert = "shaders/ItemVert.spv";
    std::string itemPathFrag = "shaders/ItemFrag.spv";
    std::string itemPathModel = "models/Item.obj";
    std::string itemPathTexture = "textures/Item.jpg";

    // Step
    DescriptorSetLayout DSL_Step;
    VertexDescriptor VD_Step;
    Pipeline P_Step;
    Model M_Step;
    Texture T_Step{};
    DescriptorSet DS_Step;
    StepMUBO UBOm_Step{};
    std::string stepPathVert = "shaders/StepVert.spv";
    std::string stepPathFrag = "shaders/StepFrag.spv";
    std::string stepPathModel = "models/Step.obj";
    std::string stepPathTexture = "textures/Step.jpg";

    // Iron
    DescriptorSetLayout DSL_Iron;
    VertexDescriptor VD_Iron;
    Pipeline P_Iron;
    Model M_Iron;
    Texture T_Iron{};
    DescriptorSet DS_Iron;
    IronMUBO UBOm_Iron{};
    std::string ironPathVert = "shaders/IronVert.spv";
    std::string ironPathFrag = "shaders/IronFrag.spv";
    std::string ironPathModel = "models/Iron.obj";
    std::string ironPathTexture = "textures/Iron.jpg";

    // Decoration
    DescriptorSetLayout DSL_Decoration;
    VertexDescriptor VD_Decoration;
    Pipeline P_Decoration;
    Model M_Decoration;
    Texture T_Decoration{};
    DescriptorSet DS_Decoration;
    DecorationMUBO UBOm_Decoration{};
    std::string decorationPathVert = "shaders/DecorationVert.spv";
    std::string decorationPathFrag = "shaders/DecorationFrag.spv";
    std::string decorationPathModel = "models/Decoration.obj";
    std::string decorationPathTexture = "textures/Decoration.jpg";

    // Border
    DescriptorSetLayout DSL_Border;
    VertexDescriptor VD_Border;
    Pipeline P_Border;
    Model M_Border;
    Texture T_Border;
    DescriptorSet DS_Border;
    BorderMUBO UBOm_Border;
    std::string borderPathVert = "shaders/BorderVert.spv";
    std::string borderPathFrag = "shaders/BorderFrag.spv";
    std::string borderPathModel = "models/Border.obj";
    std::string borderPathTexture = "textures/Border.jpg";

    // Wall
    DescriptorSetLayout DSL_Wall;
    VertexDescriptor VD_Wall;
    Pipeline P_Wall;
    Model M_Wall;
    Texture T_Wall{};
    DescriptorSet DS_Wall;
    WallMUBO UBOm_Wall{};
    std::string wallPathVert = "shaders/WallVert.spv";
    std::string wallPathFrag = "shaders/WallFrag.spv";
    std::string wallPathModel = "models/Wall.obj";
    std::string wallPathTexture = "textures/Wall.jpg";

    // View
    float viewDistance = 2.0f + sphereRadius;
    float viewDistanceSuper = 3.0f + sphereRadius;
    float viewHeight = 2.0f + sphereRadius;
    float viewHeightSuper = 3.0f + sphereRadius;
    float viewAzimuth = 0.0f;
    float viewElevation = 0.0f;
    float viewSpeed = glm::radians(170.0f);
    const float viewElevationMax = glm::radians(90.0f);
    const float viewElevationMin = -glm::radians(90.0f);
    float Ar{};
    glm::mat4 viewMatrix{};
    glm::vec3 viewPos{};
    glm::vec3 viewPosOld{};
    glm::vec3 viewPosLock = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 viewDir{};

    // Text
    TextMaker textLivesBanner;
    TextMaker textMessageBanner;
    int textMessageIndex = 0;
    TextMaker textFinishBanner;
    int textFinishIndex = 0;

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
        // Level
        levelInit();


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
        P_Sphere.init(this, &VD_Sphere,  spherePathVert, spherePathFrag, {&DSL_Sphere});
        M_Sphere.init(this, &VD_Sphere, levelPathPrefix + spherePathModel, OBJ);
        T_Sphere.init(this, levelPathPrefix + spherePathTexture);

        spherePos = levelStart + glm::vec3(0.0f, sphereRadius, 0.0f);
        spherePosOld = spherePos;
        sphereCheckpoint = spherePos;


        // Plane
        DSL_Plane.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(PlaneMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ShaderLightPUBO), 1},
        });
        VD_Plane.init(this, {
            {0, sizeof(PlaneVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PlaneVertex, pos), sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(PlaneVertex, uv), sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(PlaneVertex, norm), sizeof(glm::vec3), NORMAL}
        });
        P_Plane.init(this, &VD_Plane, lightPathVert, lightPathFrag, {&DSL_Plane});
        M_Plane.init(this, &VD_Plane, levelPathPrefix + planePathModel, OBJ);
        T_Plane.init(this, levelPathPrefix + planePathTexture);


        // Item
        DSL_Item.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(ItemMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ShaderLightPUBO), 1},
        });
        VD_Item.init(this, {
            {0, sizeof(ItemVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ItemVertex, pos), sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(ItemVertex, uv), sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(ItemVertex, norm), sizeof(glm::vec3), NORMAL},
        });
        P_Item.init(this, &VD_Item, lightPathVert, lightPathFrag, {&DSL_Item});
        M_Item.init(this, &VD_Item, levelPathPrefix + itemPathModel, OBJ);
        T_Item.init(this, levelPathPrefix + itemPathTexture);


        // Step
        DSL_Step.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(StepMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ShaderLightPUBO), 1},
        });
        VD_Step.init(this, {
            {0, sizeof(StepVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(StepVertex, pos), sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(StepVertex, uv), sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(StepVertex, norm), sizeof(glm::vec3), NORMAL},
        });
        P_Step.init(this, &VD_Step, lightPathVert, lightPathFrag, {&DSL_Step});
        M_Step.init(this, &VD_Step, levelPathPrefix + stepPathModel, OBJ);
        T_Step.init(this, levelPathPrefix + stepPathTexture);


        // Iron
        DSL_Iron.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(IronMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ShaderLightPUBO), 1},
        });
        VD_Iron.init(this, {
            {0, sizeof(IronVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(IronVertex, pos), sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(IronVertex, uv), sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(IronVertex, norm), sizeof(glm::vec3), NORMAL},
        });
        P_Iron.init(this, &VD_Iron, lightPathVert, lightPathFrag, {&DSL_Iron});
        M_Iron.init(this, &VD_Iron, levelPathPrefix + ironPathModel, OBJ);
        T_Iron.init(this, levelPathPrefix + ironPathTexture);


        // Decoration
        DSL_Decoration.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(DecorationMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ShaderLightPUBO), 1},
        });
        VD_Decoration.init(this, {
            {0, sizeof(DecorationVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DecorationVertex, pos), sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(DecorationVertex, uv), sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(DecorationVertex, norm), sizeof(glm::vec3), NORMAL},
        });
        P_Decoration.init(this, &VD_Decoration, lightPathVert, lightPathFrag, {&DSL_Decoration});
        M_Decoration.init(this, &VD_Decoration, levelPathPrefix + decorationPathModel, OBJ);
        T_Decoration.init(this, levelPathPrefix + decorationPathTexture);


        // Border
        DSL_Border.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(BorderMUBO), 1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ShaderLightPUBO), 1},
        });
        VD_Border.init(this, {
                {0, sizeof(BorderVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BorderVertex, pos),  sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(BorderVertex, uv),      sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(BorderVertex, norm), sizeof(glm::vec3), NORMAL}
        });
        P_Border.init(this, &VD_Border, lightPathVert, lightPathFrag, {&DSL_Border});
        M_Border.init(this, &VD_Border, levelPathPrefix + borderPathModel, OBJ);
        T_Border.init(this, levelPathPrefix + borderPathTexture);


        // Wall
        DSL_Wall.init(this, {
            {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, sizeof(WallMUBO),   1},
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0,        1},
            {2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(ShaderLightPUBO), 1},
        });
        VD_Wall.init(this, {
            {0, sizeof(BorderVertex), VK_VERTEX_INPUT_RATE_VERTEX}
        }, {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(WallVertex, pos),  sizeof(glm::vec3), POSITION},
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(WallVertex, uv),      sizeof(glm::vec2), UV},
            {0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(WallVertex, norm), sizeof(glm::vec3), NORMAL}
        });
        P_Wall.init(this, &VD_Wall, lightPathVert, lightPathFrag, {&DSL_Wall});
        M_Wall.init(this, &VD_Wall, levelPathPrefix + wallPathModel, OBJ);
        T_Wall.init(this, levelPathPrefix + wallPathTexture);


        // Text
        textLivesBanner.init(this, &textLives);
        textMessageBanner.init(this, &textMessage);
        textFinishBanner.init(this, &textFinish);


        // Others
        DPSZs.uniformBlocksInPool = 15;
        DPSZs.texturesInPool = 11;
        DPSZs.setsInPool = 11;
    }

    void levelInit() {
        // Height initialization
        std::ifstream heightJsonFile(levelPathPrefix + levelPathHeight);
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
                    levelHeight[i][j] = height;
                }
            }
        }

        // Type initialization
        std::ifstream itemsJsonFile(levelPathPrefix + levelPathType);
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
                    levelType[i][j] = type;
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

        // Step
        P_Step.create();
        DS_Step.init(this, &DSL_Step, {&T_Step});

        // Iron
        P_Iron.create();
        DS_Iron.init(this, &DSL_Iron, {&T_Iron});

        // Decoration
        P_Decoration.create();
        DS_Decoration.init(this, &DSL_Decoration, {&T_Decoration});
        
        // Border
        P_Border.create();
        DS_Border.init(this, &DSL_Border, {&T_Border});

        // Wall
        P_Wall.create();
        DS_Wall.init(this, &DSL_Border, {&T_Border});

        // Text
        textLivesBanner.pipelinesAndDescriptorSetsInit();
        textMessageBanner.pipelinesAndDescriptorSetsInit();
        textFinishBanner.pipelinesAndDescriptorSetsInit();
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

        // Step
        DSL_Step.cleanup();
        VD_Step.cleanup();
        P_Step.destroy();
        M_Step.cleanup();
        T_Step.cleanup();

        // Iron
        DSL_Iron.cleanup();
        VD_Iron.cleanup();
        P_Iron.destroy();
        M_Iron.cleanup();
        T_Iron.cleanup();

        // Decoration
        DSL_Decoration.cleanup();
        VD_Decoration.cleanup();
        P_Decoration.destroy();
        M_Decoration.cleanup();
        T_Decoration.cleanup();

        // Border
        DSL_Border.cleanup();
        VD_Border.cleanup();
        P_Border.destroy();
        M_Border.cleanup();
        T_Border.cleanup();

        // Wall
        DSL_Wall.cleanup();
        VD_Wall.cleanup();
        P_Wall.destroy();
        M_Wall.cleanup();
        T_Wall.cleanup();

        // Text
        textLivesBanner.localCleanup();
        textMessageBanner.localCleanup();
        textFinishBanner.localCleanup();

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

        // Step
        P_Step.cleanup();
        DS_Step.cleanup();

        // Iron
        P_Iron.cleanup();
        DS_Iron.cleanup();

        // Decoration
        P_Decoration.cleanup();
        DS_Decoration.cleanup();

        // Border
        P_Border.cleanup();
        DS_Border.cleanup();

        // Wall
        P_Wall.cleanup();
        DS_Wall.cleanup();

        // Text
        textLivesBanner.pipelinesAndDescriptorSetsCleanup();
        textMessageBanner.pipelinesAndDescriptorSetsCleanup();
        textFinishBanner.pipelinesAndDescriptorSetsCleanup();
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

        // Step
        P_Step.bind(commandBuffer);
        M_Step.bind(commandBuffer);
        DS_Step.bind(commandBuffer, P_Step, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Step.indices.size()), 1, 0, 0, 0);

        // Iron
        P_Iron.bind(commandBuffer);
        M_Iron.bind(commandBuffer);
        DS_Iron.bind(commandBuffer, P_Iron, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Iron.indices.size()), 1, 0, 0, 0);

        // Decoration
        P_Decoration.bind(commandBuffer);
        M_Decoration.bind(commandBuffer);
        DS_Decoration.bind(commandBuffer, P_Decoration, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Decoration.indices.size()), 1, 0, 0, 0);

        // Border
        P_Border.bind(commandBuffer);
        M_Border.bind(commandBuffer);
        DS_Border.bind(commandBuffer, P_Border, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Border.indices.size()), 1, 0, 0, 0);

        // Wall
        P_Wall.bind(commandBuffer);
        M_Wall.bind(commandBuffer);
        DS_Wall.bind(commandBuffer, P_Wall, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Wall.indices.size()), 1, 0, 0, 0);

        // Text
        textLivesBanner.populateCommandBuffer(commandBuffer, currentImage, sphereLives);
        textMessageBanner.populateCommandBuffer(commandBuffer, currentImage, textMessageIndex);
        textFinishBanner.populateCommandBuffer(commandBuffer, currentImage, textFinishIndex);
    }

    void updateUniformBuffer(uint32_t currentImage) override {
        static bool debounce = false;
        static int curDebounce = 0;


        // Get inputs
        float deltaTime;
        auto movementInput = glm::vec3(0.0f);
        auto rotationInput = glm::vec3(0.0f);
        bool fireInput = false;
        getSixAxis(deltaTime, movementInput, rotationInput, fireInput);

        if(glfwGetKey(window, GLFW_KEY_1)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_1;
                lightStatus.x = 1 - lightStatus.x;
            }
        } else {
            if((curDebounce == GLFW_KEY_1) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_2)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_2;
                lightStatus.y = 1 - lightStatus.y;
            }
        } else {
            if((curDebounce == GLFW_KEY_2) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }
        if(glfwGetKey(window, GLFW_KEY_3)) {
            if(!debounce) {
                debounce = true;
                curDebounce = GLFW_KEY_3;
                lightStatus.z = 1 - lightStatus.z;
            }
        } else {
            if((curDebounce == GLFW_KEY_3) && debounce) {
                debounce = false;
                curDebounce = 0;
            }
        }

        // Use inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) { // Game exit
            glfwSetWindowShouldClose(window, GL_TRUE);
            scene = "Menu";
            closeapp = false;
        }
        if (movementInput.x == -1.0f || rotationInput.y == -1.0f) { // View left
            if (!viewPosLock.x)
                viewAzimuth += viewSpeed * deltaTime;
        } else if (movementInput.x == 1.0f || rotationInput.y == 1.0f) { // View right
            if (!viewPosLock.z)
                viewAzimuth -= viewSpeed * deltaTime;
        }
        if (rotationInput.x == -1.0f) { // View up
            viewElevation += viewSpeed * deltaTime;
            if (viewElevation > viewElevationMax)
                viewElevation = viewElevationMax;
        } else if (rotationInput.x == 1.0f) { // View down
            viewElevation -= viewSpeed * deltaTime;
            if (viewElevation < viewElevationMin)
                viewElevation = viewElevationMin;
        }
        if (movementInput.z == -1.0f) { // Sphere forward
            if (sphereOnGround()) {
                spherePosAccel = -viewDir * sphereAccel;
            } else {
                spherePosAccel = -viewDir * sphereAccelUp;
            }
        } else if (movementInput.z == 1.0f) { // Sphere backward
            if (sphereOnGround()) {
                spherePosAccel = viewDir * sphereAccel;
            } else {
                spherePosAccel = viewDir * sphereAccelUp;
            }
        } else {
            spherePosAccel.x = 0.0f;
            spherePosAccel.z = 0.0f;
        }
        if (fireInput && !sphereJumping) { // Sphere jump
            spherePosSpeed.y = sphereJump;
            sphereGoingUp = true;
            sphereJumping = true;
        }
        if (rotationInput.z == -1.0f  && sphereOnGround()) { // Interact
            if (levelType[(int)spherePos.x][(int)spherePos.z] == 1.0f) {
                sphereCheckpoint = spherePos;
            }
            if (levelType[(int)spherePos.x][(int)spherePos.z] == 2.0f) {
                sphereAccel = sphereAccelSuper;
                sphereAccelUp = sphereAccelUpSuper;
                sphereJump = sphereJumpSuper;
            }
            if (levelType[(int)spherePos.x][(int)spherePos.z] == 3.0f) {
                viewDistance = viewDistanceSuper;
                viewHeight = viewHeightSuper;
            }
            if (levelType[(int)spherePos.x][(int)spherePos.z] == 4.0f) {
                textFinishIndex = 1;
                sphereAccel = 0.0f;
                sphereJump = 0.0f;
                viewSpeed = 0.0f;
            }
        }

        // View update
        updateView();

        // Sphere update
        updateSphere(deltaTime);

        // Projection update
        projectionMatrix = glm::perspective(glm::radians(75.0f), Ar, 0.1f, 160.0f);
        projectionMatrix[1][1] *= -1;
        projectionViewMatrix = projectionMatrix * viewMatrix;

        // UBOs update
        updateUBO(currentImage);
    }

    void updateView() {
        // Compute the new view position
        viewPosOld = viewPos;
        float x = spherePos.x + viewDistance * glm::sin(viewAzimuth);
        float z = spherePos.z + viewDistance * glm::cos(viewAzimuth);
        float y = spherePos.y + viewHeight + viewDistance * glm::sin(viewElevation);

        // Collision detection
        if (levelHeight[(int)x][(int)viewPosOld.z] > spherePos.y) {
            x = viewPosOld.x;
            viewPosLock.x = 1.0f;
        } else if (levelHeight[(int)viewPosOld.x][(int)z] > spherePos.y) {
            z = viewPosOld.z;
            viewPosLock.z = 1.0f;
        }
        viewPos = glm::vec3(x, y, z);
        viewPosLock = glm::vec3(0.0f);

        // Update matrix
        viewMatrix = glm::lookAt(viewPos, spherePos, glm::vec3(0.0f, 1.0f, 0.0f));
        viewDir = glm::normalize(glm::vec3(glm::sin(viewAzimuth), glm::sin(viewElevation), glm::cos(viewAzimuth)));
    }

    void updateSphere(float deltaTime) {
        // Type check
        if (levelType[(int)spherePos.x][(int)spherePos.z] == 1.0 && sphereOnGround()) {
            textMessageIndex = 1;
            RebuildPipeline();
        } else if (levelType[(int)spherePos.x][(int)spherePos.z] == 2.0 && sphereOnGround()) {
            textMessageIndex = 2;
            RebuildPipeline();
        } else if (levelType[(int)spherePos.x][(int)spherePos.z] == 3.0 && sphereOnGround()) {
            textMessageIndex = 3;
            RebuildPipeline();
        } else if (levelType[(int)spherePos.x][(int)spherePos.z] == 4.0 && sphereOnGround()) {
            textMessageIndex = 4;
            RebuildPipeline();
        } else if (levelType[(int)spherePos.x][(int)spherePos.z] == 5.0 && sphereOnGround()) {
            spherePos = sphereCheckpoint;
            sphereLives--;
            if (sphereLives == 0) {
                textFinishIndex = 2;
                sphereAccel = 0.0f;
                sphereJump = 0.0f;
                viewSpeed = 0.0f;
            }
            RebuildPipeline();
        } else {
            textMessageIndex = 0;
            RebuildPipeline();
        }

        // Update position in y
        if (sphereGoingUp) {
            spherePosSpeed.y = sphereJump;
            spherePos.y += spherePosSpeed.y * deltaTime;
            if (spherePos.y > 2 * sphereRadius) {
                sphereGoingUp = false;
            }
        }
        else if (spherePos.y > levelHeight[(int)spherePos.x][(int)spherePos.z] + sphereRadius) {
            spherePosAccel.y = sphereGravity;
            spherePosSpeed.y += spherePosAccel.y * deltaTime;
            spherePos.y += spherePosSpeed.y * deltaTime;
            sphereJumping = true;
        }
        else {
            spherePosAccel.y = 0.0f;
            spherePosSpeed.y = 0.0f;
            spherePos.y = levelHeight[(int)spherePos.x][(int)spherePos.z] + sphereRadius;
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
        if (levelHeight[(int)(spherePos.x + sphereRadius)][(int)(spherePosOld.z)] > spherePos.y) {
            if (spherePosSpeed.x >= 0.0f)
                spherePos.x = spherePosOld.x;
        }
        else if (levelHeight[(int)(spherePos.x - sphereRadius)][(int)(spherePosOld.z)] > spherePos.y) {
            if (spherePosSpeed.x <= 0.0f)
                spherePos.x = spherePosOld.x;
        }
        if (levelHeight[(int)(spherePos.x)][(int)(spherePosOld.z + sphereRadius)] > spherePos.y) {
            if (spherePosSpeed.z >= 0.0f)
                spherePos.z = spherePosOld.z;
        }
        else if (levelHeight[(int)(spherePos.x)][(int)(spherePosOld.z - sphereRadius)] > spherePos.y) {
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
        if (levelHeight[(int)spherePos.x][(int)spherePos.z] + sphereRadius == spherePos.y)
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

        // Item
        UBOm_Item.mMat = glm::mat4(1.0f);
        UBOm_Item.nMat = glm::mat4(1.0f);
        UBOm_Item.mvpMat = projectionViewMatrix * UBOm_Item.mMat;
        DS_Item.map(currentImage, &UBOm_Item, 0);

        // Step
        UBOm_Step.mMat = glm::mat4(1.0f);
        UBOm_Step.nMat = glm::mat4(1.0f);
        UBOm_Step.mvpMat = projectionViewMatrix * UBOm_Step.mMat;
        DS_Step.map(currentImage, &UBOm_Step, 0);
        
        // Iron
        UBOm_Iron.mMat = glm::mat4(1.0f);
        UBOm_Iron.nMat = glm::mat4(1.0f);
        UBOm_Iron.mvpMat = projectionViewMatrix * UBOm_Iron.mMat;
        DS_Iron.map(currentImage, &UBOm_Iron, 0);

        // Decoration
        UBOm_Decoration.mMat = glm::mat4(1.0f);
        UBOm_Decoration.nMat = glm::mat4(1.0f);
        UBOm_Decoration.mvpMat = projectionViewMatrix * UBOm_Decoration.mMat;
        DS_Decoration.map(currentImage, &UBOm_Decoration, 0);
        
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

        // Shader Lights
        UBOp_ShaderLights.lightColor = glm::vec4(LCol, LInt);
        UBOp_ShaderLights.lightPos =  spherePos;
        UBOp_ShaderLights.viewPos = viewPos;
        UBOp_ShaderLights.ambientStrength = ambientStrength;
        UBOp_ShaderLights.specularStrength = specularStrength;
        UBOp_ShaderLights.shininess = shininess;
        UBOp_ShaderLights.lightStatus = lightStatus;
        UBOp_ShaderLights.cosIn = cosIn;
        UBOp_ShaderLights.cosOut = cosOut;

        DS_Plane.map(currentImage, &UBOp_ShaderLights, 2);
        DS_Border.map(currentImage, &UBOp_ShaderLights, 2);
        DS_Wall.map(currentImage, &UBOp_ShaderLights, 2);
        DS_Item.map(currentImage, &UBOp_ShaderLights, 2);
        DS_Step.map(currentImage, &UBOp_ShaderLights, 2);
        DS_Iron.map(currentImage, &UBOp_ShaderLights, 2);
        DS_Decoration.map(currentImage, &UBOp_ShaderLights, 2);
    }
};

class Level1 : public Level {
public:
    Level1() {
        levelPathPrefix = "levels/level1/";
    }
};

class Level2 : public Level {
public:
    Level2() {
        levelPathPrefix = "levels/level2/";
    }
};



int main() {
    Level1 app1;
    Level2 app2;



    try {
        while (!closeapp) {
            if (scene == "Menu") {
                app2.run();
            }
            else if (scene == "Level1") {
                app1.run();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
