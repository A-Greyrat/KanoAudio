#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

#include "KanoAudio/Audio.h"
#include "KanoAudio/Decoder/PCMDecoder.h"
#include "KanoAudio/Decoder/WAVDecoder.h"
#include "KanoAudio/Decoder/MP3Decoder.h"
#include "KanoAudio/Decoder/OGGDecoder.h"
#include "KanoAudio/Decoder/FLACDecoder.h"

using namespace KanoAudio;

int main()
{
    auto audio = Audio::Create();
    //audio->Load<PCMDecoder>("C:\\Users\\21129\\Desktop\\LiSA (织部里沙) - unlasting.pcm");
    //audio->Load<WAVDecoder>("C:\\Users\\21129\\Desktop\\YOASOBI-_ヨアソビ_-夜に駆ける-_奔向黑夜_.wav");
    //audio->Load<OGGDecoder>("C:\\Users\\21129\\Desktop\\YOASOBI-_ヨアソビ_-群青.ogg");
    //audio->Load<FLACDecoder>("C:\\Users\\21129\\Music\\米津玄師 (よねづ けんし) - LOSER.flac");
    audio->Load<MP3Decoder>("C:\\Users\\21129\\Desktop\\KICK BACK.mp3");

    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(800, 600, "Kano Kano", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 0;
    }

    glfwMakeContextCurrent(window);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return 0;
    }

    // sync
    glfwSwapInterval(1);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (true)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Audio Player");
        ImGui::Text("Audio Player");

        if (ImGui::Button("Play"))
        {
            audio->Play();
        }
        ImGui::SameLine();

        if (ImGui::Button("Pause"))
        {
            audio->Pause();
        }
        ImGui::SameLine();

        if (ImGui::Button("Stop"))
        {
            audio->Stop();
        }

        // Show timeline
        auto currentTime = (float)audio->GetCurrentTime();
        float lCurrentTime = currentTime;
        if (ImGui::SliderFloat("Time", &currentTime, 0.0f, (float)audio->GetDuration(), "%.2f"))
        {
            // 更改进度小于1s不更新
            if (std::abs(currentTime - lCurrentTime) > 1.f)
                audio->SetCurrentTime(currentTime);
        }


        float volume = audio->GetVolume();
        if (ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f))
        {
            audio->SetVolume(volume);
        }

        float pitch = audio->GetPitch();
        if (ImGui::SliderFloat("Pitch", &pitch, 0.0f, 2.0f))
        {
            audio->SetPitch(pitch);
        }

        bool isLoop = audio->IsLooping();
        if (ImGui::Checkbox("Loop", &isLoop))
        {
            audio->SetLooping(isLoop);
        }

        ImGui::End();

        // Rendering
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(glfwGetCurrentContext(), &display_w, &display_h);

        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfwGetCurrentContext());
        glfwPollEvents();

        if (glfwWindowShouldClose(glfwGetCurrentContext()))
            break;
    }

    ShutdownOpenAL();

    return 0;

}
