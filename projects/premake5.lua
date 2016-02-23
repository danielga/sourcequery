newoption({
   trigger = "static-runtime",
   description = "Force the use of the static C runtime (only works with static builds)"
})

PROJECT_FOLDER = os.get() .. "/" .. _ACTION
SOURCE_FOLDER = "../source"
INCLUDE_FOLDER = "../include"
THIRDPARTY_FOLDER = "../thirdparty"

solution("sourcequery")
	language("C++")
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
			targetdir(PROJECT_FOLDER .. "/release x86")

		filter({"configurations:Release", "platforms:x64"})
			targetdir(PROJECT_FOLDER .. "/release x64")

	filter("configurations:Debug")
		kind("SharedLib")
		defines("DEBUG")
		flags("Symbols")
		objdir(PROJECT_FOLDER .. "/intermediate")

		filter({"configurations:Debug", "platforms:x86"})
			targetdir(PROJECT_FOLDER .. "/debug x86")

		filter({"configurations:Debug", "platforms:x64"})
			targetdir(PROJECT_FOLDER .. "/debug x64")

	filter("configurations:StaticRelease")
		kind("StaticLib")
		defines({"NDEBUG", "SOURCEQUERY_STATIC"})
		optimize("On")
		vectorextensions("SSE2")
		objdir(PROJECT_FOLDER .. "/intermediate")

		filter({"configurations:StaticRelease", "options:static-runtime"})
			flags("StaticRuntime")

		filter({"configurations:StaticRelease", "platforms:x86"})
			targetdir(PROJECT_FOLDER .. "/static release x86")

		filter({"configurations:StaticRelease", "platforms:x64"})
			targetdir(PROJECT_FOLDER .. "/static release x64")

	filter("configurations:StaticDebug")
		kind("StaticLib")
		defines({"DEBUG", "SOURCEQUERY_STATIC"})
		flags("Symbols")
		objdir(PROJECT_FOLDER .. "/intermediate")

		filter({"configurations:StaticDebug", "options:static-runtime"})
			flags("StaticRuntime")

		filter({"configurations:StaticDebug", "platforms:x86"})
			targetdir(PROJECT_FOLDER .. "/static debug x86")

		filter({"configurations:StaticDebug", "platforms:x64"})
			targetdir(PROJECT_FOLDER .. "/static debug x64")

	project("testing")
		kind("ConsoleApp")
		includedirs(INCLUDE_FOLDER)
		vpaths({["Header files"] = "**.hpp", ["Source files"] = "**.cpp"})
		files({SOURCE_FOLDER .. "/testing/*.cpp", SOURCE_FOLDER .. "/testing/*.hpp"})
		links("SourceQuery")

		filter({"system:windows", "configurations:StaticDebug or StaticRelease"})
			links("ws2_32")

		filter("system:linux")
			linkoptions("-Wl,-rpath=./")

	project("sourcequery")
		defines("SOURCEQUERY_EXPORT")
		includedirs({INCLUDE_FOLDER, SOURCE_FOLDER, THIRDPARTY_FOLDER .. "/bzip2"})
		vpaths({
			["Header files/*"] = {
				SOURCE_FOLDER .. "/sourcequery/**.hpp",
				INCLUDE_FOLDER .. "/sourcequery/**.hpp",
				THIRDPARTY_FOLDER .. "/**.h"
			},
			["Source files/*"] = {
				SOURCE_FOLDER .. "/sourcequery/**.cpp",
				THIRDPARTY_FOLDER .. "/**.c"
			}
		})
		files({
			SOURCE_FOLDER .. "/sourcequery/*.cpp",
			SOURCE_FOLDER .. "/sourcequery/*.hpp",
			THIRDPARTY_FOLDER .. "/bzip2/*.c",
			THIRDPARTY_FOLDER .. "/bzip2/*.h",
			INCLUDE_FOLDER .. "/sourcequery/*.hpp"
		})

		filter({"system:windows", "configurations:Debug or Release"})
			links("ws2_32")
