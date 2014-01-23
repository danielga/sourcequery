newoption({
   trigger = "static-runtime",
   description = "Force the use of the static C runtime (only works with static builds)"
})

solution("SourceQuery")
	language("C++")
	location("Projects/" .. os.get() .. "/" .. _ACTION)
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
		objdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Intermediate")

		configuration({"Release", "x32 or Universal32"})
			targetdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Release x86")

		configuration({"Release", "x64 or Universal64"})
			targetdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Release x64")

	configuration("Debug")
		defines({"DEBUG"})
		flags({"Symbols"})
		kind("SharedLib")
		objdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Intermediate")

		configuration({"Debug", "x32 or Universal32"})
			targetdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Debug x86")

		configuration({"Debug", "x64 or Universal64"})
			targetdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Debug x64")

	configuration("Static Release")
		defines({"NDEBUG", "SOURCEQUERY_STATIC"})
		flags({"Optimize", "EnableSSE"})
		kind("StaticLib")
		objdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Intermediate")

		configuration({"Static Release", "static-runtime"})
			flags({"StaticRuntime"})

		configuration({"Static Release", "x32 or Universal32"})
			targetdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Static Release x86")

		configuration({"Static Release", "x64 or Universal64"})
			targetdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Static Release x64")

	configuration("Static Debug")
		defines({"DEBUG", "SOURCEQUERY_STATIC"})
		flags({"Symbols"})
		kind("StaticLib")
		objdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Intermediate")

		configuration({"Static Debug", "static-runtime"})
			flags({"StaticRuntime"})

		configuration({"Static Debug", "x32 or Universal32"})
			targetdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Static Debug x86")

		configuration({"Static Debug", "x64 or Universal64"})
			targetdir("Projects/" .. os.get() .. "/" .. _ACTION .. "/Static Debug x64")

	project("Testing")
		kind("ConsoleApp")
		includedirs({"Include"})
		vpaths({["Header files"] = {"**.hpp"}, ["Source files"] = {"**.cpp"}})
		files({"Source/Testing/*.cpp", "Source/Testing/*.hpp"})
		links({"SourceQuery"})

		configuration({"windows", "Static Debug or Static Release"})
			links({"ws2_32"})

		configuration("linux")
			linkoptions({"-Wl,-rpath=./"})

	project("SourceQuery")
		defines({"SOURCEQUERY_EXPORT"})
		includedirs({"Include", "Source", "ThirdParty/bzip2"})
		vpaths({["Header files/*"] = {"Source/SourceQuery/**.hpp", "Include/SourceQuery/**.hpp", "ThirdParty/**.h"}, ["Source files/*"] = {"Source/SourceQuery/**.cpp", "ThirdParty/**.c"}})
		files({
			"Source/SourceQuery/*.cpp",
			"Source/SourceQuery/*.hpp",
			"ThirdParty/bzip2/*.c",
			"ThirdParty/bzip2/*.h",
			"Include/SourceQuery/*.hpp"
		})

		configuration({"windows", "Debug or Release"})
			links({"ws2_32"})