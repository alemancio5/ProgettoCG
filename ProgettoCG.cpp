#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"

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
    const float sphereRadius = 1.0f;
    const float sphereAccel = 50.0f;
    float sphereFriction = 0.95f;
    glm::mat4 sphereMatrix = glm::mat4(1.0f);
    glm::vec3 spherePos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 spherePosSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 spherePosAccel = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sphereRot = glm::vec3(0.0f);
    glm::vec3 sphereRotSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 sphereRotAccel = glm::vec3(0.0f, 0.0f, 0.0f);

    // Projection variables
    glm::mat4 projectionMatrix{};
    glm::mat4 viewProjectionMatrix{};

    // View variables
    const float viewDistance = 5.0f;
    const float viewHeight = 3.0f;
    const float viewSpeed = glm::radians(120.0f);
    glm::mat4 viewMatrix{};
    glm::vec3 viewPos{};
    glm::vec3 viewOffset = glm::vec3(0.0f, 3.0f, 5.0f);
    float Ar{};

    void setWindowParameters() override {
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "???";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.1f, 0.3f, 0.5f, 1.0f};
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
        // System inputs
        float deltaTime;
        auto movementInput = glm::vec3(0.0f);
        auto rotationInput = glm::vec3(0.0f);
        bool fireInput = false;
        getSixAxis(deltaTime, movementInput, rotationInput, fireInput);

        // Keyboard inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // View variables update
        static float viewAzimuth = 0.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            viewAzimuth += viewSpeed * deltaTime;
        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            viewAzimuth -= viewSpeed * deltaTime;
        }
        float x = spherePos.x + viewDistance * glm::sin(viewAzimuth);
        float z = spherePos.z + viewDistance * glm::cos(viewAzimuth);
        float y = spherePos.y + viewHeight;
        viewPos = glm::vec3(x, y, z);
        viewMatrix = glm::lookAt(viewPos, spherePos, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 forwardDir = glm::normalize(glm::vec3(glm::sin(viewAzimuth), 0.0f, glm::cos(viewAzimuth)));

        // Sphere variables update
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            spherePosAccel = -forwardDir * sphereAccel;
        } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            spherePosAccel = forwardDir * sphereAccel;
        } else {
            spherePosAccel = glm::vec3(0.0f);
        }
        spherePosSpeed += spherePosAccel * deltaTime;
        spherePos += spherePosSpeed * deltaTime;
        spherePosSpeed *= sphereFriction;
        float sphereSpeed = glm::length(spherePosSpeed);
        if (sphereSpeed > 0.0001f) {
            float rotationAngle = sphereSpeed * deltaTime / sphereRadius;
            glm::vec3 movementDir = glm::normalize(spherePosSpeed);
            glm::vec3 rotationAxis = glm::cross(-movementDir, glm::vec3(0.0f, 1.0f, 0.0f));
            sphereRot += rotationAxis * rotationAngle;
        }
        sphereMatrix = glm::translate(glm::mat4(1.0f), spherePos) *
                       glm::rotate(glm::mat4(1.0f), sphereRot.x, glm::vec3(1.0f, 0.0f, 0.0f)) *
                       glm::rotate(glm::mat4(1.0f), sphereRot.y, glm::vec3(0.0f, 1.0f, 0.0f)) *
                       glm::rotate(glm::mat4(1.0f), sphereRot.z, glm::vec3(0.0f, 0.0f, 1.0f));

        // Projection variables update
        projectionMatrix = glm::perspective(glm::radians(75.0f), Ar, 0.1f, 160.0f);
        projectionMatrix[1][1] *= -1;
        viewProjectionMatrix = projectionMatrix * viewMatrix;

        // Sphere UBO update
        SphereMatrUBO UBOm_Sphere{};
        UBOm_Sphere.mvpMat = viewProjectionMatrix * sphereMatrix;
        UBOm_Sphere.mMat = sphereMatrix;
        UBOm_Sphere.nMat = glm::mat4(1.0f);
        DS_Sphere.map(currentImage, &UBOm_Sphere, 0);

        // Plane UBO update
        PlaneMatrUBO UBOm_Plane{};
        UBOm_Plane.mvpMat = viewProjectionMatrix;
        UBOm_Plane.mMat = glm::mat4(1.0f);
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
