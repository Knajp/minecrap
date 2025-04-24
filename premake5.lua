workspace "minecrap"
	configurations {"Debug", "Release"}
	architecture "x86_64"

project "minecrap"
	kind "ConsoleApp"
	language "C++"

	defines{"GLM_ENABLE_EXPERIMENTAL"}

	targetdir "bin/%{cfg.buildcfg}-%{cfg.architecture}"
	objdir "bin-int/%{cfg.buildcfg}-%{cfg.architecture}"

	files {"src/**.cpp", "src/**.h", "src/**.c"}

	includedirs {"vendor/include"}
	libdirs {"vendor/lib"}

	links{"glfw3.lib", "glfw3dll.lib", "glfw3_mt.lib"}