add_rules("mode.debug", "mode.release")

add_requires("sdl2 >=2.0.12")
add_requires("sdl2_ttf >=2.0.15")

target("nyx") do
    set_kind("binary")
    add_includedirs("src")
    add_files(
        "src/*.cpp",
        "src/common/*.cpp",
        "src/tokenizer/*.cpp",
        "src/parser/*.cpp",
        "src/interpreter/*.cpp",
        "src/stdlib/*.cpp"
    )
    set_languages("cxx17")
    add_packages("sdl2", "sdl2_ttf")
end
