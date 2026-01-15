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
        "src/**.cpp",
        "vendor/stb_image/**.h",
        "vendor/stb_image/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
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

    postbuildcommands
    {
        '{COPY} "vendor/SDL2/lib/x64/SDL2.dll" "%{cfg.targetdir}/"'
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