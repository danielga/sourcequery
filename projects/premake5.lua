newoption({
   trigger = "static-runtime",
   description = "Force the use of the static C runtime (only works with static builds)"
})

PROJECT_FOLDER = os.target() .. "/" .. _ACTION
SOURCE_FOLDER = "../source"
INCLUDE_FOLDER = "../include"

solution("sourcequery")
	language("C++")
	cppdialect("C++11")
	location(PROJECT_FOLDER)
	warnings("Extra")
	flags("NoPCH")
	platforms({"x86", "x64"})
	configurations({"Release", "Debug", "StaticRelease", "StaticDebug"})

	filter("platforms:x86")
		architecture("x32")

	filter("platforms:x64")
		architecture("x64")

	filter("configurations:Release")
		kind("SharedLib")
		defines("NDEBUG")
		optimize("On")
		vectorextensions("SSE2")
		objdir(PROJECT_FOLDER .. "/intermediate")

		filter({"configurations:Release", "platforms:x86"})
			targetdir(PROJECT_FOLDER .. "/release_x86")

		filter({"configurations:Release", "platforms:x64"})
			targetdir(PROJECT_FOLDER .. "/release x64")

	filter("configurations:Debug")
		kind("SharedLib")
		defines("DEBUG")
		symbols("On")
		objdir(PROJECT_FOLDER .. "/intermediate")

		filter({"configurations:Debug", "platforms:x86"})
			targetdir(PROJECT_FOLDER .. "/debug_x86")

		filter({"configurations:Debug", "platforms:x64"})
			targetdir(PROJECT_FOLDER .. "/debug_x64")

	filter("configurations:StaticRelease")
		kind("StaticLib")
		defines({"NDEBUG", "SOURCEQUERY_STATIC"})
		optimize("On")
		vectorextensions("SSE2")
		objdir(PROJECT_FOLDER .. "/intermediate")

		filter({"configurations:StaticRelease", "options:static-runtime"})
			flags("StaticRuntime")

		filter({"configurations:StaticRelease", "platforms:x86"})
			targetdir(PROJECT_FOLDER .. "/static_release_x86")

		filter({"configurations:StaticRelease", "platforms:x64"})
			targetdir(PROJECT_FOLDER .. "/static_release_x64")

	filter("configurations:StaticDebug")
		kind("StaticLib")
		defines({"DEBUG", "SOURCEQUERY_STATIC"})
		symbols("On")
		objdir(PROJECT_FOLDER .. "/intermediate")

		filter({"configurations:StaticDebug", "options:static-runtime"})
			flags("StaticRuntime")

		filter({"configurations:StaticDebug", "platforms:x86"})
			targetdir(PROJECT_FOLDER .. "/static_debug_x86")

		filter({"configurations:StaticDebug", "platforms:x64"})
			targetdir(PROJECT_FOLDER .. "/static_debug_x64")

	project("testing")
		kind("ConsoleApp")
		includedirs(INCLUDE_FOLDER)
		vpaths({["Header files"] = "**.hpp", ["Source files"] = "**.cpp"})
		files({SOURCE_FOLDER .. "/testing/*.cpp", SOURCE_FOLDER .. "/testing/*.hpp"})
		links("SourceQuery")

		filter({"system:windows", "configurations:StaticDebug or StaticRelease"})
			links("ws2_32")

	project("sourcequery")
		defines("SOURCEQUERY_EXPORT")
		includedirs({INCLUDE_FOLDER, SOURCE_FOLDER, SOURCE_FOLDER .. "/bzip2"})
		vpaths({
			["Header files/*"] = {
				SOURCE_FOLDER .. "/sourcequery/**.hpp",
				INCLUDE_FOLDER .. "/sourcequery/**.hpp",
				SOURCE_FOLDER .. "/**.h"
			},
			["Source files/*"] = {
				SOURCE_FOLDER .. "/sourcequery/**.cpp",
				SOURCE_FOLDER .. "/**.c"
			}
		})
		files({
			SOURCE_FOLDER .. "/sourcequery/*.cpp",
			SOURCE_FOLDER .. "/sourcequery/*.hpp",
			SOURCE_FOLDER .. "/bzip2/*.c",
			SOURCE_FOLDER .. "/bzip2/*.h",
			INCLUDE_FOLDER .. "/sourcequery/*.hpp"
		})

		filter("system:windows")
			defines({"_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_WARNINGS"})
			disablewarnings("4127")

		filter({"system:windows", "configurations:Debug or Release"})
			links("ws2_32")
