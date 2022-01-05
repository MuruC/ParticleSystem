local arch = "x64"

-- Solution
workspace("ParticleSystem")
	location("build")
	configurations { "Debug", "Release" }
	systemversion("latest")
	architecture(arch)
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

-- Examples
local projectName = "ParticleSystem"
project(projectName)
	kind "ConsoleApp"
	cppdialect("C++20")
	language("C++")
	location("build/"..projectName)
	targetdir("build/bin/"..projectName.."/%{cfg.buildcfg}/"..arch)

	files
	{
		"source/external/**",
		"source/**",
		"shader/**"
	}
	
	includedirs
	{
		"source/external/",
		"source/external/imgui/",
		"library/include/",
	}
	
	libdirs
	{
		"library/lib/",
	}
	
	links
	{
		"glfw3",
	}