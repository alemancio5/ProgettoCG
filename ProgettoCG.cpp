#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"
glm::vec3 lastDir;
float savedSpeed = 0.0f;
float savedRotSpeed = 0.0f;
float movementTimer = 0.0f;
float stopTimer = 0.0f;
bool isMoving = false;
bool wasMoving = false;

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

class NamelessGame : public BaseProject {
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

    // Sphere variables
    glm::vec3 sphereDir = glm::vec3(0.0f);
    glm::vec3 spherePos = glm::vec3(0.0f, 0.0f, 0.0f);
    float spherePosSpeed = 0.0f;
    float spherePosSpeedAccel = 0.0;
    float spherePosSpeedDecel = 0.0;
    const float spherePosAccel = 10.0f;
    const float spherePosMaxSpeed = 40.0f;
    const float sphereRotAccel = glm::radians(300.0f);
    float sphereRotSpeedAccel = 0.0;
    float sphereRotSpeedDecel = 0.0;
    glm::vec3 sphereRot = glm::vec3(0.0f);
    glm::mat4 sphereRotMatrix = glm::mat4(1.0f);

    float sphereRotSpeed = glm::radians(310.0f);
    glm::vec3 sphereRotAxis{};
    float sphereRotAngle{};
    glm::mat4 sphereProjectionMatrix{};
    glm::mat4 sphereViewProjectionMatrix{};

    // Plane variables
    glm::mat4 planeModelMatrix = glm::mat4(1.0f);

    // View variables
    glm::mat4 viewMatrix{};
    glm::vec3 viewPos{};
    glm::vec3 viewOffset = glm::vec3(0.0f, 2.0f, 5.0f);
    float Ar{};

    void setWindowParameters() override {
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "???";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.1f, 0.3f, 0.1f, 1.0f};
        Ar = (float)windowWidth / (float)windowHeight;
    }

    void onWindowResize(int w, int h) override {
        Ar = (float)w / (float)h;
    }

    void localInit() override {
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
            {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, 1}
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

        // Others
        DPSZs.uniformBlocksInPool = 2;
        DPSZs.texturesInPool = 2;
        DPSZs.setsInPool = 2;
        viewMatrix = glm::translate(glm::mat4(1.0f), -viewOffset);
    }

    void pipelinesAndDescriptorSetsInit() override {
        // Sphere
        P_Sphere.create();
        DS_Sphere.init(this, &DSL_Sphere, {&T_Sphere});

        // Plane
        P_Plane.create();
        DS_Plane.init(this, &DSL_Plane, {&T_Plane});
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
    }

    void updateUniformBuffer(uint32_t currentImage) override {
        // System variables initialization
        float deltaTime;
        auto movementInput = glm::vec3(0.0f);
        auto rotationInput = glm::vec3(0.0f);
        bool fireInput = false;
        getSixAxis(deltaTime, movementInput, rotationInput, fireInput);

        // Sphere variables initialization
        sphereDir = glm::vec3(0.0f);
        bool isKeyPressed = false;


        // Update when keys pressed
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            sphereRot.x -= sphereRotSpeed * deltaTime;
            sphereDir += glm::vec3(0.0f, 0.0f, 1.0f);
            lastDir = sphereDir;
            isKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            sphereDir += glm::vec3(0.0f, 0.0f, -1.0f);
            lastDir = sphereDir;
            sphereRot.x += sphereRotSpeed * deltaTime;
            isKeyPressed = true;

        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            sphereDir += glm::vec3(1.0f, 0.0f, 0.0f);
            lastDir = sphereDir;
            sphereRot.z += sphereRotSpeed * deltaTime;
            isKeyPressed = true;

        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            sphereDir += glm::vec3(-1.0f, 0.0f, 0.0f);
            lastDir = sphereDir;
            sphereRot.z -= sphereRotSpeed * deltaTime;
            isKeyPressed = true;

        }


        // Timer logic
        if (isKeyPressed) {
            // Moving: Start or continue the movement timer
            if (!isMoving) {

                // Log stop duration and reset stop timer when movement starts
                std::cout << "Stop duration: " << stopTimer << " seconds\n";
                std::cout << "Speed: " << spherePosSpeed << " m/s\n";
                std::cout << "Rotation speed: " << sphereRotSpeed << " rad/s\n";
                stopTimer = 0.0f; // Reset stop timer
                isMoving = true;
            }

            // Increment movement timer
            movementTimer += deltaTime;
            spherePosSpeedAccel = spherePosAccel * movementTimer;
            sphereRotSpeedAccel = sphereRotAccel * movementTimer;
        } else {
            // Stopping: Start or continue the stop timer
            if (isMoving) {


                // Log movement duration and reset movement timer when stopping
                std::cout << "Movement duration: " << movementTimer << " seconds\n";

                std::cout << "Speed: " << spherePosSpeed << " m/s\n";
                std::cout << "Rotation speed: " << sphereRotSpeed << " rad/s\n";
                movementTimer = 0.0f; // Reset movement timer
                savedSpeed = spherePosSpeed;
                savedRotSpeed = sphereRotSpeed;
                isMoving = false;
            }

            // Increment stop timer
            stopTimer += deltaTime;

            spherePosSpeedDecel = spherePosAccel * stopTimer;
            sphereRotSpeedDecel = sphereRotAccel * stopTimer;
            if (spherePosSpeedDecel > savedSpeed) {
                spherePosSpeedDecel = 0.0f;
                spherePosSpeedAccel = 0.0f;
            }
            if (sphereRotSpeedDecel > savedRotSpeed) {
                sphereRotSpeedDecel = 0.0f;
                sphereRotSpeedAccel = 0.0f;
            }
        }

        spherePosSpeed = glm::clamp(spherePosSpeedAccel - spherePosSpeedDecel, 0.0f, spherePosMaxSpeed);
        sphereRotSpeed = glm::clamp(sphereRotSpeedAccel - sphereRotSpeedDecel, 0.0f, 900.0f);

        // Sphere variables update
        if (glm::length(lastDir) > 0.0f) {
            lastDir = glm::normalize(lastDir);
            spherePos += -lastDir * spherePosSpeed * deltaTime;
            sphereRotAxis = glm::cross(lastDir, glm::vec3(0.0f, 1.0f, 0.0f));
            sphereRotAngle = sphereRotSpeed * deltaTime;
            sphereRotMatrix = glm::rotate(glm::mat4(1.0f), sphereRotAngle, sphereRotAxis) * sphereRotMatrix;
        }
        sphereProjectionMatrix = glm::perspective(glm::radians(75.0f), Ar, 0.1f, 160.0f);
        sphereProjectionMatrix[1][1] *= -1;
        sphereViewProjectionMatrix = sphereProjectionMatrix * viewMatrix;

        // View variables update
        viewPos = spherePos + viewOffset;
        viewMatrix = glm::lookAt(viewPos, spherePos, glm::vec3(0.0f, 1.0f, 0.0f));

        // Sphere UBO update
        SphereMatrUBO UBOm_Sphere{};
        UBOm_Sphere.mvpMat = sphereViewProjectionMatrix * glm::translate(glm::mat4(1.0f), spherePos) * sphereRotMatrix;
        UBOm_Sphere.mMat = glm::translate(glm::mat4(1.0f), spherePos);
        UBOm_Sphere.nMat = glm::mat4(1.0f);
        DS_Sphere.map(currentImage, &UBOm_Sphere, 0);

        // Plane UBO update
        PlaneMatrUBO UBOm_Plane{};
        UBOm_Plane.mvpMat = sphereViewProjectionMatrix * planeModelMatrix;
        UBOm_Plane.mMat = planeModelMatrix;
        UBOm_Plane.nMat = glm::mat4(1.0f);
        DS_Plane.map(currentImage, &UBOm_Plane, 0);
    }
};



int main() {
    NamelessGame app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
