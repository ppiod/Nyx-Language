add_rules("mode.debug", "mode.release")

add_requires("sdl2 >=2.0.12", "sdl2_ttf >=2.0.15")

target("nyx")
    set_kind("binary")
    set_languages("cxx17")

    add_includedirs("src")

    add_files(
        "src/*.cpp",
        "src/common/*.cpp",
        "src/tokenizer/*.cpp",
        "src/parser/*.cpp",
        "src/interpreter/*.cpp",
        "src/stdlib/*.cpp"
    )

    add_packages("sdl2", "sdl2_ttf")
