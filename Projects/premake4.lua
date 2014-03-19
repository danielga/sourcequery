newoption({
   trigger = "static-runtime",
   description = "Force the use of the static C runtime (only works with static builds)"
})

PROJECT_FOLDER = os.get() .. "/" .. _ACTION
SOURCE_FOLDER = "../Source/"
INCLUDE_FOLDER = "../Include/"
THIRDPARTY_FOLDER = "../ThirdParty/"

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
		objdir(PROJECT_FOLDER .. "/Intermediate")

		configuration({"Release", "x32 or Universal32"})
			targetdir(PROJECT_FOLDER .. "/Release x86")

		configuration({"Release", "x64 or Universal64"})
			targetdir(PROJECT_FOLDER .. "/Release x64")

	configuration("Debug")
		defines({"DEBUG"})
		flags({"Symbols"})
		kind("SharedLib")
		objdir(PROJECT_FOLDER .. "/Intermediate")

		configuration({"Debug", "x32 or Universal32"})
			targetdir(PROJECT_FOLDER .. "/Debug x86")

		configuration({"Debug", "x64 or Universal64"})
			targetdir(PROJECT_FOLDER .. "/Debug x64")

	configuration("Static Release")
		defines({"NDEBUG", "SOURCEQUERY_STATIC"})
		flags({"Optimize", "EnableSSE"})
		kind("StaticLib")
		objdir(PROJECT_FOLDER .. "/Intermediate")

		configuration({"Static Release", "static-runtime"})
			flags({"StaticRuntime"})

		configuration({"Static Release", "x32 or Universal32"})
			targetdir(PROJECT_FOLDER .. "/Static Release x86")

		configuration({"Static Release", "x64 or Universal64"})
			targetdir(PROJECT_FOLDER .. "/Static Release x64")

	configuration("Static Debug")
		defines({"DEBUG", "SOURCEQUERY_STATIC"})
		flags({"Symbols"})
		kind("StaticLib")
		objdir(PROJECT_FOLDER .. "/Intermediate")

		configuration({"Static Debug", "static-runtime"})
			flags({"StaticRuntime"})

		configuration({"Static Debug", "x32 or Universal32"})
			targetdir(PROJECT_FOLDER .. "/Static Debug x86")

		configuration({"Static Debug", "x64 or Universal64"})
			targetdir(PROJECT_FOLDER .. "/Static Debug x64")

	project("Testing")
		kind("ConsoleApp")
		includedirs({"Include"})
		vpaths({["Header files"] = "**.hpp", ["Source files"] = "**.cpp"})
		files({SOURCE_FOLDER .. "Testing/*.cpp", SOURCE_FOLDER .. "Testing/*.hpp"})
		links({"SourceQuery"})

		configuration({"windows", "Static Debug or Static Release"})
			links({"ws2_32"})

		configuration("linux")
			linkoptions({"-Wl,-rpath=./"})

	project("SourceQuery")
		defines({"SOURCEQUERY_EXPORT"})
		includedirs({"Include", "Source", "ThirdParty/bzip2"})
		vpaths({["Header files/*"] = {SOURCE_FOLDER .. "SourceQuery/**.hpp", INCLUDE_FOLDER .. "SourceQuery/**.hpp", THIRDPARTY_FOLDER .. "**.h"}, ["Source files/*"] = {SOURCE_FOLDER .. "SourceQuery/**.cpp", THIRDPARTY_FOLDER .. "**.c"}})
		files({
			SOURCE_FOLDER .. "SourceQuery/*.cpp",
			SOURCE_FOLDER .. "SourceQuery/*.hpp",
			THIRDPARTY_FOLDER .. "bzip2/*.c",
			THIRDPARTY_FOLDER .. "bzip2/*.h",
			INCLUDE_FOLDER .. "SourceQuery/*.hpp"
		})

		configuration({"windows", "Debug or Release"})
			links({"ws2_32"})