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
};

class SunGolf : public BaseProject {
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
    const float spherePosSpeed = 10.0f;
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
        windowTitle = "Sun Golf";
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
            {0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(PlaneVertex, uv), sizeof(glm::vec2), UV}
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

        // Update when keys pressed
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            sphereRot.x -= sphereRotSpeed * deltaTime;
            sphereDir += glm::vec3(0.0f, 0.0f, 1.0f);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            sphereDir += glm::vec3(0.0f, 0.0f, -1.0f);
            sphereRot.x += sphereRotSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            sphereDir += glm::vec3(1.0f, 0.0f, 0.0f);
            sphereRot.z += sphereRotSpeed * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            sphereDir += glm::vec3(-1.0f, 0.0f, 0.0f);
            sphereRot.z -= sphereRotSpeed * deltaTime;
        }

        // Sphere variables update
        if (glm::length(sphereDir) > 0.0f) {
            sphereDir = glm::normalize(sphereDir);
            spherePos += -sphereDir * spherePosSpeed * deltaTime;
            sphereRotAxis = glm::cross(sphereDir, glm::vec3(0.0f, 1.0f, 0.0f));
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
    SunGolf app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
