# Nyx Language <img src="./assets/icon.svg" alt="Nyx Logo" width="40" height="40" style="vertical-align: middle;">
Nyx is a lightweight, dynamically-typed scripting language designed for simplicity and ease of use.

## Prerequisites

* **Xmake**: A C/C++ build system. (See [xmake.io](https://xmake.io))
* **C++17 Compiler**: Such as GCC, Clang, or MSVC.
* **SDL2 Libraries (Optional, for `std:sdl` module)**:
    * SDL2 (version >=2.0.12)
    * SDL2_ttf (version >=2.0.15)

## Build from Source

1.  Navigate to the root directory of the Nyx project.
2.  Build using Xmake:
    ```bash
    xmake
    ```

## Install (System-Wide)

After a successful build, you can install the `nyx` interpreter to a system location (e.g., `/usr/local`):

```bash
sudo xmake install --root
````

## Running Nyx Scripts

Once `nyx` is built (and optionally installed), you can run Nyx scripts:

```bash
nyx /path/to/your_script.nyx [script_arguments...]
```

**Example:**

```bash
nyx examples/snake_game/snake_game.nyx
```

To see the interpreter's command-line options:

```bash
nyx --help
```

## Documentation

For more details on the Nyx language, its features, and standard library:

  * See the [Nyx Language Guide](./doc/guide.md) and other documents in the `/doc` folder.