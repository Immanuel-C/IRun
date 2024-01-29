workspace "IRun"
    configurations {"Debug", "Release"}
    architecture "x86_64"

    print ("Make sure to set the vulkan sdk path!")

    vulkanSdk = os.getenv("VULKAN_SDK");

    function defaultBuildCfg()
        filter "configurations:Debug"
            defines { "DEBUG" }
            symbols "On"
            runtime "Debug"
            optimize "Debug"

        filter "configurations:Release"
            defines { "NDEBUG" }
            symbols "Off"
            runtime "Release"
            optimize "Speed"
    end

    function defaultBuildLocation()
        targetdir ("bin/%{prj.name}/%{cfg.buildcfg}")
        objdir ("bin-int/%{prj.name}/%{cfg.buildcfg}")
    end

    startproject "TestApplication"

    project "TestApplication"
        location "test"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"

        includedirs { 
            "%{prj.location}/", 
            "%{prj.location}/../deps/glad/include", 
            "%{prj.location}/../deps/ILog/", 
            "%{prj.location}/../deps/IWindow/src",
            "%{prj.location}/../deps/imgui/",
            "%{prj.location}/../src/",
            vulkanSdk .. "/Include",
            "%{prj.location}/../deps/glm",
            "%{prj.location}/../deps/CNtity/include"
        }

        files {"%{prj.location}/**.cpp", "%{prj.location}/**.h",}
        
        defines { "_CRT_SECURE_NO_WARNINGS" }

        links {"IRun", "ImGui"}

        defaultBuildLocation()
        defaultBuildCfg()


    project "IRun"
        location "src"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"

        includedirs { 
            "%{prj.location}/", 
            "%{prj.location}/../deps/glad/include", 
            "%{prj.location}/../deps/ILog/", 
            "%{prj.location}/../deps/IWindow/src",
            "%{prj.location}/../deps/imgui/",
            vulkanSdk .. "/Include",
            "%{prj.location}/../deps/glm",
            "%{prj.location}/../deps/CNtity/include"
        }

        files {"%{prj.location}/**.cpp", "%{prj.location}/**.h",}
        
        libdirs { vulkanSdk .. "/Lib" }
        links {"OpenGL32", "Vulkan-1", "IWindow", "ILog", "ImGui", "Glad", "Dxcompiler", "glslang", "HLSL", "OSDependent", "OGLCompiler", "SPIRV"}

        defines { "_CRT_SECURE_NO_WARNINGS" }

        defaultBuildLocation()
        defaultBuildCfg()

    project "Glad"
        location "deps/glad"
        kind "StaticLib"
        language "C"
        cdialect "C17"

        includedirs { "%{prj.location}/include" }

        files {"%{prj.location}/**.c", "%{prj.location}/**.h",}

        defaultBuildLocation()
        defaultBuildCfg()
        
    project "ILog"
        location "deps/ILog"
        kind "StaticLib"
        language "C"
        cdialect "C17"

        includedirs { "%{prj.location}" }

        files {"%{prj.location}/**.c", "%{prj.location}/**.h",}

        defaultBuildLocation()
        defaultBuildCfg()
    
    project "IWindow"
        location "deps/IWindow"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"

        includedirs { "%{prj.location}", vulkanSdk .. "/Include", "%{prj.location}/../imgui"}

        files {
            "%{prj.location}/src/*.cpp", 
            "%{prj.location}/src/*.h",
        }

        libdirs { vulkanSdk .. "/Lib" }
        links {"Vulkan-1"}

        defaultBuildLocation()
        defaultBuildCfg()

    project "ImGui"
        location "deps/imgui"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"

        files {
            "%{prj.location}/imgui.cpp", 
            "%{prj.location}/imgui.h", 
            "%{prj.location}/imgui_draw.cpp", 
            "%{prj.location}/imgui_tables.cpp",
            "%{prj.location}/imgui_widgets.cpp",
            "%{prj.location}/backends/imgui_impl_opengl3.cpp",
            "%{prj.location}/imgui_demo.cpp",
        }

        includedirs {
            "%{prj.location}"
        }

        links {"User32"}

        defaultBuildLocation()

        defaultBuildCfg()