#include "modules/Starter.hpp"
#include "modules/TextMaker.hpp"

#define WIDTH 800
#define HEIGHT 600

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

class SunGolf : public BaseProject {
protected:
    // Sphere
    DescriptorSetLayout DSL_Sphere;
    VertexDescriptor VD_Sphere;
    Pipeline P_Sphere;
    Model M_Sphere;
    Texture T_Sphere;
    DescriptorSet DS_Sphere;

    // Others
    glm::vec3 CameraOffset = glm::vec3(0.0f, 2.0f, 5.0f);
    glm::vec3 SpherePosition = glm::vec3(0.0f, 0.0f, 0.0f);  // Sphere at origin
    glm::mat4 ViewMatrix;
    float Ar;

    void setWindowParameters() {
        windowWidth = WIDTH;
        windowHeight = HEIGHT;
        windowTitle = "Sun Golf";
        windowResizable = GLFW_TRUE;
        initialBackgroundColor = {0.1f, 0.1f, 0.1f, 1.0f};
        Ar = (float)windowWidth / (float)windowHeight;
    }

    void onWindowResize(int w, int h) {
        Ar = (float)w / (float)h;
    }

    void localInit() {
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

        // Others
        DPSZs.uniformBlocksInPool = 2;
        DPSZs.texturesInPool = 2;
        DPSZs.setsInPool = 2;
        ViewMatrix = glm::translate(glm::mat4(1.0f), -CameraOffset);
    }

    void pipelinesAndDescriptorSetsInit() {
        // Sphere
        P_Sphere.create();
        DS_Sphere.init(this, &DSL_Sphere, {&T_Sphere});
    }

    void localCleanup() {
        // Sphere
        DSL_Sphere.cleanup();
        VD_Sphere.cleanup();
        P_Sphere.destroy();
        M_Sphere.cleanup();
        T_Sphere.cleanup();

        // Others
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void pipelinesAndDescriptorSetsCleanup() {
        // Sphere
        P_Sphere.cleanup();
        DS_Sphere.cleanup();
    }

    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
        // Sphere
        P_Sphere.bind(commandBuffer);
        M_Sphere.bind(commandBuffer);
        DS_Sphere.bind(commandBuffer, P_Sphere, 0, currentImage);
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(M_Sphere.indices.size()), 1, 0, 0, 0);
    }

    void updateUniformBuffer(uint32_t currentImage) {
        // Presets variables
        static bool debounce = false;
        static int curDebounce = 0;
        float deltaT;
        glm::vec3 m = glm::vec3(0.0f), r = glm::vec3(0.0f);
        bool fire = false;
        getSixAxis(deltaT, m, r, fire);

        // Time
        static float autoTime = true;
        static float cTime = 0.0;
        const float turnTime = 72.0f;
        const float angTurnTimeFact = 2.0f * M_PI / turnTime;
        if(autoTime) {
            cTime = cTime + deltaT;
            cTime = (cTime > turnTime) ? (cTime - turnTime) : cTime;
        }
        static float tTime = 0.0;
        const float tTurnTime = 60.0f;
        const float tangTurnTimeFact = 1.0f / tTurnTime;
        if(autoTime) {
            tTime = tTime + deltaT;
            tTime = (tTime > tTurnTime) ? (tTime - tTurnTime) : tTime;
        }

        // Update the view
        const float ROT_SPEED = glm::radians(120.0f);
        const float MOVE_SPEED = 2.0f;
        ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.x * deltaT,glm::vec3(1, 0, 0)) * ViewMatrix;
        ViewMatrix = glm::rotate(glm::mat4(1), ROT_SPEED * r.y * deltaT,glm::vec3(0, 1, 0)) * ViewMatrix;
        ViewMatrix = glm::rotate(glm::mat4(1), -ROT_SPEED * r.z * deltaT,glm::vec3(0, 0, 1)) * ViewMatrix;
        ViewMatrix = glm::translate(glm::mat4(1), -glm::vec3(MOVE_SPEED * m.x * deltaT, MOVE_SPEED * m.y * deltaT, MOVE_SPEED * m.z * deltaT)) * ViewMatrix;

        // Update when keys pressed
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Preset matrices
        glm::vec3 cameraPosition = SpherePosition + CameraOffset;

        // Set the view matrix to look from cameraPosition to the sphere position
        ViewMatrix = glm::lookAt(cameraPosition, SpherePosition, glm::vec3(0.0f, 1.0f, 0.0f)); // Up direction is Y-axis

        // Projection matrix (perspective projection)
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), Ar, 0.1f, 160.0f);
        projectionMatrix[1][1] *= -1; // Flip Y for Vulkan

        // Combine projection and view matrices
        glm::mat4 ViewPrj = projectionMatrix * ViewMatrix;


        // Sphere
        SphereMatrUBO UBOm_Sphere{};
        UBOm_Sphere.mvpMat = ViewPrj;
        UBOm_Sphere.mMat = glm::mat4(1.0f);
        UBOm_Sphere.nMat = glm::mat4(1.0f);
        DS_Sphere.map(currentImage, &UBOm_Sphere, 0);
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
