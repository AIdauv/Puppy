project "Puppy"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime"off"

    targetdir("%{wks.location}/bin/" .. outputdir .. "/")
    objdir("%{wks.location}/bin-int/" .. outputdir .. "/")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.glm}",
        "%{IncludeDir.SDL2}"
    }

    libdirs
    {
        "%{LibraryDir.SDL2}/x64"
    }

    links 
    {
        "SDL2",
        "SDL2main"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "BC_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "BC_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "BC_DIST"
        runtime "Release"
        optimize "on"