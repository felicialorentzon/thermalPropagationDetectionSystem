add_rules("mode.debug", "mode.release")

set_warnings("all", "error")
set_languages("cxxlatest")

if is_mode("debug") then
  set_symbols("debug")
  set_optimize("none")
  set_policy("build.sanitizer.address", true)
else
  set_symbols("hidden")
  set_optimize("fastest")
  set_strip("all")
end

target("bmsmonitor")
set_kind("static")
add_files("src/*.cpp")
remove_files("src/Main.cpp")
add_includedirs("src", { public = true })
set_default(false)

target("main")
set_kind("binary")
add_files("src/Main.cpp")
add_deps("bmsmonitor")
set_default(true)
includes("**/xmake.lua")