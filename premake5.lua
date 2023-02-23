workspace "IRun"
    configurations {"Debug", "Release"}
    architecture "x86_64"

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

    print("Make sure to set VULKAN_SDK to the location of the vulkan files")

    VULKAN_SDK = "C:/VulkanSDK/1.3.239.0"

    startproject "App"
    
    project "GLFW"
        kind "StaticLib"
        language "C"
        cdialect "C99"
        location "Deps/GLFW"

        files {
            "%{prj.location}/src/context.c",
            "%{prj.location}/src/egl_context.c",
            "%{prj.location}/src/egl_context.h",
            "%{prj.location}/src/init.c",
            "%{prj.location}/src/input.c",
            "%{prj.location}/src/internal.h",
            "%{prj.location}/src/monitor.c",
            "%{prj.location}/src/osmesa_context.c",
            "%{prj.location}/src/osmesa_context.h",
            "%{prj.location}/src/vulkan.c",
            "%{prj.location}/src/window.c"
        }
    
        defaultBuildLocation()

        filter "system:windows"
            files {
                "%{prj.location}/src/wgl_context.c",
                "%{prj.location}/src/wgl_context.h",
                "%{prj.location}/src/win32_init.c",
                "%{prj.location}/src/win32_joystick.c",
                "%{prj.location}/src/win32_joystick.h",
                "%{prj.location}/src/win32_monitor.c",
                "%{prj.location}/src/win32_platform.h",
                "%{prj.location}/src/win32_thread.c",
                "%{prj.location}/src/win32_time.c",
                "%{prj.location}/src/win32_window.c"
            }
    
            defines  { 
                "_GLFW_WIN32",
                "_CRT_SECURE_NO_WARNINGS"
            }

            links  {
                "User32",
                "Gdi32",
                "Shell32"
            }
    
        filter "system:linux"
            files
            {
                "%{prj.location}/src/glx_context.c",
                "%{prj.location}/src/glx_context.h",
                "%{prj.location}/src/linux_joystick.c",
                "%{prj.location}/src/linux_joystick.h",
                "%{prj.location}/src/posix_time.c",
                "%{prj.location}/src/posix_time.h",
                "%{prj.location}/src/posix_thread.c",
                "%{prj.location}/src/posix_thread.h",
                "%{prj.location}/src/x11_init.c",
                "%{prj.location}/src/x11_monitor.c",
                "%{prj.location}/src/x11_platform.h",
                "%{prj.location}/src/x11_window.c",
                "%{prj.location}/src/xkb_unicode.c",
                "%{prj.location}/src/xkb_unicode.h"
            }

            links {
                "dl",
                "pthread",
                "X11"
            }
    
            defines  { 
                "_GLFW_X11"
            }

            defaultBuildCfg()

    project "IWindow"
        location "Deps/IWindow"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"

        files { 
            "%{prj.location}/**.cpp",
            "%{prj.location}/**.h",
        }

        includedirs { 
            "%{prj.location}",
            VULKAN_SDK .. "/Include",
        }
        
        libdirs {
            VULKAN_SDK .. "/Lib"
        }

        defaultBuildLocation()
        defaultBuildCfg()

        filter "system:windows"
            links {"User32", "OpenGL32", "XInput", "vulkan-1"}
        filter "system:linux"
            links {"X11", "GLX", "GL", "Xcursor", "vulkan-1"}
        
    project "ILog"
        location "Deps/ILog"
        kind "StaticLib"
        language "C"
        cdialect "C99"

        files { 
            "%{prj.location}/**.c",
            "%{prj.location}/**.h",
        }

        includedirs { 
            "%{prj.location}" 
        }

        defaultBuildLocation()
        defaultBuildCfg()
    
    project "IBin" 
        location "Deps/IBin"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"

        files {
            "%{prj.location}/**.cpp",
            "%{prj.location}/**.h",
        }

        includedirs { 
            "%{prj.location}" 
        }

        defaultBuildLocation()
        defaultBuildCfg()

    project "Engine"
        location "IRun"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"

        files { 
            "%{prj.location}/**.cpp",
            "%{prj.location}/**.h", 
        }

        includedirs { 
            "%{prj.location}", 
            "Deps/IWindow",
            "Deps/IBin",
            "Deps/ILog",
            "Deps/GLM",
            "Deps/GLFW/include/GLFW",
            VULKAN_SDK .. "/Include",
        }

        defaultBuildLocation()
        defaultBuildCfg()
    
    project "App"
        location "App"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"

        files { 
            "%{prj.location}/**.cpp",
            "%{prj.location}/**.h",
        }

        includedirs { 
            "%{prj.location}",
            "IRun",
            "Deps/IWindow",
            "Deps/IBin",
            "Deps/ILog",
            "Deps/GLM",
            "Deps/GLFW/include/GLFW",
            VULKAN_SDK .. "/Include",
        }


        defaultBuildLocation()
        defaultBuildCfg()

        filter "system:windows"
            links {"User32", "OpenGL32", "XInput", "IWindow", "ILog", "IBin", "GLFW", "Engine"}
        filter "system:linux"
            links {"X11", "GLX", "GL", "Xcursor", "IWindow", "ILog", "IBin", "GLFW", "Engine"}