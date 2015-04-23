newoption({
   trigger = "static-runtime",
   description = "Force the use of the static C runtime (only works with static builds)"
})

PROJECT_FOLDER = os.get() .. "/" .. _ACTION
SOURCE_FOLDER = "../source"
INCLUDE_FOLDER = "../include"
THIRDPARTY_FOLDER = "../thirdparty"

solution("SourceQuery")
	language("C++")
	location(PROJECT_FOLDER)
	flags({"NoPCH", "ExtraWarnings"})

	if os.is("macosx") then
		platforms({"Universal32", "Universal64"})
	else
		platforms({"x32", "x64"})
	end

	configurations({"Release", "Debug", "Static Release", "Static Debug"})

	configuration("Release")
		defines({"NDEBUG"})
		flags({"Optimize", "EnableSSE"})
		kind("SharedLib")
		objdir(PROJECT_FOLDER .. "/intermediate")

		configuration({"Release", "x32 or Universal32"})
			targetdir(PROJECT_FOLDER .. "/release x86")

		configuration({"Release", "x64 or Universal64"})
			targetdir(PROJECT_FOLDER .. "/release x64")

	configuration("Debug")
		defines({"DEBUG"})
		flags({"Symbols"})
		kind("SharedLib")
		objdir(PROJECT_FOLDER .. "/intermediate")

		configuration({"Debug", "x32 or Universal32"})
			targetdir(PROJECT_FOLDER .. "/debug x86")

		configuration({"Debug", "x64 or Universal64"})
			targetdir(PROJECT_FOLDER .. "/debug x64")

	configuration("Static Release")
		defines({"NDEBUG", "SOURCEQUERY_STATIC"})
		flags({"Optimize", "EnableSSE"})
		kind("StaticLib")
		objdir(PROJECT_FOLDER .. "/intermediate")

		configuration({"Static Release", "static-runtime"})
			flags({"StaticRuntime"})

		configuration({"Static Release", "x32 or Universal32"})
			targetdir(PROJECT_FOLDER .. "/static release x86")

		configuration({"Static Release", "x64 or Universal64"})
			targetdir(PROJECT_FOLDER .. "/static release x64")

	configuration("Static Debug")
		defines({"DEBUG", "SOURCEQUERY_STATIC"})
		flags({"Symbols"})
		kind("StaticLib")
		objdir(PROJECT_FOLDER .. "/intermediate")

		configuration({"Static Debug", "static-runtime"})
			flags({"StaticRuntime"})

		configuration({"Static Debug", "x32 or Universal32"})
			targetdir(PROJECT_FOLDER .. "/static debug x86")

		configuration({"Static Debug", "x64 or Universal64"})
			targetdir(PROJECT_FOLDER .. "/static debug x64")

	project("Testing")
		kind("ConsoleApp")
		includedirs({INCLUDE_FOLDER})
		vpaths({["Header files"] = "**.hpp", ["Source files"] = "**.cpp"})
		files({SOURCE_FOLDER .. "/Testing/*.cpp", SOURCE_FOLDER .. "/Testing/*.hpp"})
		links({"SourceQuery"})

		configuration({"windows", "Static Debug or Static Release"})
			links({"ws2_32"})

		configuration("linux")
			linkoptions({"-Wl,-rpath=./"})

	project("SourceQuery")
		defines({"SOURCEQUERY_EXPORT"})
		includedirs({INCLUDE_FOLDER, SOURCE_FOLDER, THIRDPARTY_FOLDER .. "/bzip2"})
		vpaths({["Header files/*"] = {SOURCE_FOLDER .. "/SourceQuery/**.hpp", INCLUDE_FOLDER .. "/SourceQuery/**.hpp", THIRDPARTY_FOLDER .. "/**.h"}, ["Source files/*"] = {SOURCE_FOLDER .. "/SourceQuery/**.cpp", THIRDPARTY_FOLDER .. "/**.c"}})
		files({
			SOURCE_FOLDER .. "/SourceQuery/*.cpp",
			SOURCE_FOLDER .. "/SourceQuery/*.hpp",
			THIRDPARTY_FOLDER .. "/bzip2/*.c",
			THIRDPARTY_FOLDER .. "/bzip2/*.h",
			INCLUDE_FOLDER .. "/SourceQuery/*.hpp"
		})

		configuration({"windows", "Debug or Release"})
			links({"ws2_32"})