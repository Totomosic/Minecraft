workspace "Minecraft"
    architecture "x64"

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

-- Path to directory containing Bolt-Core
BoltDir = "Bolt/"

include (BoltDir .. "BoltInclude.lua")

IncludeDirs["MinecraftClient"] = "Minecraft-Client/src"

group ("Minecraft")
include ("Minecraft-Client")