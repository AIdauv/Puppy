include "Dependencies.lua"

workspace "Puppy"
    architecture "x86_64"
    startproject "Puppy"

    configurations 
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Core"
	include "Puppy"
group ""

group "Dependencies"
    include "vendor/premake"
group ""

