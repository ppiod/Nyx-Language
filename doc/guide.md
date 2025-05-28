# <img src="../assets/icon.svg" alt="Nyx Logo" width="80" height="80" style="vertical-align: middle; margin-right: 10px;"> Nyx Language Guide

Welcome to Nyx! A lightweight, dynamically-typed scripting language focused on simplicity.

## Table of Contents

1.  [Introduction](#1-introduction)
2.  [Getting Started](#2-getting-started)
    * [Running Scripts](#running-nyx-scripts)
    * [Command-Line Arguments](#accessing-command-line-arguments)
3.  [Language Basics](#3-language-basics)
    * [Comments](#comments)
    * [Variables](#variables)
    * [Data Types](#data-types)
    * [Predefined Global Variables](#predefined-global-variables)
    * [Operators](#operators)
    * [Control Flow](#control-flow)
    * [Functions](#functions)
4.  [Built-in Tools](#4-built-in-tools)
5.  [Modules](#5-modules)
6.  [Standard Library](#6-standard-library)
7.  [Error Handling](#7-error-handling)

---

## 1. Introduction

Nyx is a simple, interpreted, dynamically-typed scripting language for scripting, data manipulation, and system/graphics interaction via its standard library.

---

## 2. Getting Started

### Running Nyx Scripts

Execute Nyx scripts (files ending with `.nyx`) from your command line:
```bash
nyx path/to/your_script.nyx
````

### Accessing Command-Line Arguments

Pass arguments to your script after the script name. They are available in Nyx as a global list of strings named `SCRIPT_ARGS`.

```bash
nyx your_script.nyx arg1 "hello world" 123
```

Inside `your_script.nyx`:

```cpp
// SCRIPT_ARGS is ["arg1", "hello world", "123"]
output("First script arg: #{SCRIPT_ARGS[0]}");
output("Number of args: #{len(SCRIPT_ARGS)}");
```

-----

## 3\. Language Basics

### Comments

Single-line comments start with `//`.

```cpp
// This is a comment
auto x = 10; // Comment after code
```

### Variables

Declare variables with `auto`. Types are inferred dynamically. Statements usually end with `;`.

```cpp
auto message = "Hello Nyx";
auto count = 100;
auto active = true;
```

### Data Types

  * **Numbers**: Double-precision floating-point values.
    ```cpp
    auto pi = 3.14;
    auto quantity = 10;
    ```
  * **Strings**: Sequences of characters in double quotes (`"`).
      * Escapes: `\"` (double quote), `\\` (backslash).
      * Literal sequences like `\n`, `\r`, `\e` in Nyx strings are processed into actual control characters by output functions (`output`, `put`) and relevant standard library functions when the string is used by them.
      * Interpolation: `#{expression}` embeds an expression's string value.
        ```cpp
        auto version = "1.0";
        output("Nyx v#{version}\nRunning...");
        ```
  * **Booleans**: `true` or `false`.
    ```cpp
    auto isReady = true;
    ```
  * **Lists**: Ordered, mixed-type collections in `[]`. Zero-indexed. Negative indices supported.
    ```cpp
    auto items = [1, "apple", true];
    output(items[0]);    // 1
    output(items[-1]);   // true
    auto combined = items + [2.5, "banana"];
    auto repeated = ["go"] * 2; // ["go", "go"]
    ```

### Predefined Global Variables

  * **`nyx_null`**: Represents a null or absent value.
    ```cpp
    auto data = nyx_null;
    if (data == nyx_null) { output("Data is null."); }
    ```
  * **`SCRIPT_ARGS`**: A list of command-line arguments passed to the script. (See [Getting Started](https://www.google.com/search?q=%23accessing-command-line-arguments)).

### Operators

  * **Arithmetic**: `+`, `-`, `*`, `/`, `%`
  * **Comparison**: `==`, `!=`, `<`, `<=`, `>`, `>=`
  * **Logical**: `and`, `or`, `not` (keyword), `!` (unary)
  * **Assignment**: `=`
  * **Postfix Increment/Decrement**: `++`, `--` (`value = counter++;`)
  * **Member Access**: `.` (for module members)

### Control Flow

  * **`if/else`**: Conditional execution. Parentheses around condition are mandatory.
    ```cpp
    if (x > 10) { output("Greater"); } else { output("Not greater"); }
    ```
  * **`for`**: C-style loop. Parentheses around clauses are mandatory.
    ```cpp
    for (auto i = 0; i < 3; i++) { output(i); }
    ```
  * **`break;` / `continue;`**: Exit or skip to next iteration of the innermost `for` loop.

### Functions

Define with `func`. No semicolon needed after `}`. Implicitly return `nyx_null` if no `return` statement is executed.

```cpp
func multiply(a, b) = {
    return a * b;
}
auto product = multiply(6, 7); // product is 42
```

-----

## 4\. Built-in Tools

  * **`output(expression);`**: Prints the string value of `expression`, then a newline. Processes `\\n`, `\\e`, etc., in string expressions.
  * **`put(expression);`**: Like `output`, but no trailing newline. Flushes output. Processes `\\n`, `\\e`, etc.
  * **`len(collection);`**: Returns length of a string or list.
  * **`@Typedef(expression);`**: A statement that prints the type name of the expression's result (e.g., "NUMBER", "STRING").

-----

## 5\. Modules

Organize code into files.

  * **Importing**: `import "path/to/module.nyx" as alias;` or `import "std:name" as alias;`. Paths are relative to the importing file for local modules. Semicolon is required.
  * **Using**: Access members with `alias.member`.

-----

## 6\. Standard Library

Nyx includes native modules. String arguments to these functions that contain literal escape sequences (e.g., `\\n`, `\\e`) are generally processed into actual control characters by the C++ native code.

  * **`std:io`**: Console I/O, file operations.
  * **`std:list`**: List manipulation utilities.
  * **`std:string`**: String manipulation functions.
  * **`std:time`**: Time-related utilities.
  * **`std:sdl`**: SDL2/SDL\_ttf bindings for graphics, events, text.

-----

## 7\. Error Handling

Nyx reports parse and runtime errors, usually with line numbers.

```cpp
// auto result = 10 / 0; // This would cause a runtime error
```

-----

This guide covers Nyx basics. Happy scripting\!


---
# Nyx Standard Library: std:io

The `std:io` module provides functions for console and file input/output.
String arguments containing literal escape sequences (e.g., `\\n`, `\\e`) passed to functions like `io.print` or used as content for `io.writeFile` will be processed into actual control characters.

## Importing
```cpp
import "std:io" as io;
```

## Functions

### `io.print(value1, value2, ...)`

Prints string representations of values to stdout, space-separated, followed by a newline. Processes escapes in string arguments.

  * **Returns**: `nyx_null`.
  * **Example**: `io.print("Status:", status, "\\nDone.");`

### `io.input([prompt_string])`

Reads a line from stdin.

  * **Parameters**: `prompt_string` (optional `string`, escapes processed).
  * **Returns**: `string` (line read) or `nyx_null` on EOF.
  * **Example**: `auto name = io.input("Name: ");`

### `io.readFile(filepath_string)`

Reads entire file content into a string. Preserves actual newlines from file.

  * **Parameters**: `filepath_string` (`string`).
  * **Returns**: `string` (content). Runtime error on failure.
  * **Example**: `auto data = io.readFile("data.txt");`

### `io.writeFile(filepath_string, content_string)`

Writes/overwrites file with `content_string`. Processes escapes in `content_string`.

  * **Parameters**: `filepath_string` (`string`), `content_string` (`string`).
  * **Returns**: `true` on success. Runtime error on failure.
  * **Example**: `io.writeFile("log.txt", "Error: #{code}\\n");`

### `io.appendFile(filepath_string, content_string)`

Appends `content_string` to file (creates if non-existent). Processes escapes in `content_string`.

  * **Parameters**: `filepath_string` (`string`), `content_string` (`string`).
  * **Returns**: `true` on success. Runtime error on failure.
  * **Example**: `io.appendFile("log.txt", "Update processed.\\n");`

### `io.fileExists(filepath_string)`

Checks if path exists.

  * **Parameters**: `filepath_string` (`string`).
  * **Returns**: `true` or `false`.
  * **Example**: `if (io.fileExists("config.dat")) { ... }`

### `io.deleteFile(filepath_string)`

Deletes a file.

  * **Parameters**: `filepath_string` (`string`).
  * **Returns**: `true` if deleted, `false` if not found. Runtime error on other failures.
  * **Example**: `io.deleteFile("temp_file.tmp");`

<!-- end list -->

---
# Nyx Standard Library: std:list

The `std:list` module provides list manipulation utilities.

## Importing
import "std:list" as list_utils;

## Functions

### `list_utils.append(list_val, item_val)`

Returns a *new* list with `item_val` appended. Original list is unchanged.

  * **Parameters**: `list_val` (`list`), `item_val` (any).
  * **Returns**: New `list`.
  * **Example**: `auto l2 = list_utils.append([1,2], 3); // l2 is [1,2,3]`

### `list_utils.prepend(list_val, item_val)`

Returns a *new* list with `item_val` prepended. Original list is unchanged.

  * **Parameters**: `list_val` (`list`), `item_val` (any).
  * **Returns**: New `list`.
  * **Example**: `auto l2 = list_utils.prepend([1,2], 0); // l2 is [0,1,2]`

### `list_utils.is_empty(list_val)`

Checks if `list_val` is empty.

  * **Parameters**: `list_val` (`list`).
  * **Returns**: `true` or `false`.
  * **Example**: `output(list_utils.is_empty([])); // true`

### `list_utils.slice(list_val, start_index_num, [end_index_num])`

Returns a new list, a slice of `list_val` from `start_index_num` up to (but not including) `end_index_num`. Negative indices count from end. If `end_index_num` is omitted, slices to the end.

  * **Parameters**: `list_val` (`list`), `start_index_num` (`number`), `end_index_num` (optional `number`).
  * **Returns**: New `list`.
  * **Example**: `auto sub = list_utils.slice(["a","b","c","d"], 1, 3); // sub is ["b","c"]`

### `list_utils.join(list_val, [separator_string])`

Joins list elements into a string. Non-string elements are converted. `separator_string` (escapes processed) is placed between elements. Default separator is empty string.

  * **Parameters**: `list_val` (`list`), `separator_string` (optional `string`).
  * **Returns**: `string`.
  * **Example**: `auto s = list_utils.join([1,2,3], "-"); // s is "1-2-3"`

### `list_utils.each(list_val, callback_function)`

Calls `callback_function` for each element in `list_val`.

  * **Parameters**: `list_val` (`list`), `callback_function` (`function` taking one argument).
  * **Returns**: `nyx_null`.
  * **Example**: `func p(x)={ output(x); }; list_utils.each([1,2], p);`

<!-- end list -->

---
# Nyx Standard Library: std:string

The `std:string` module provides string manipulation utilities.
String arguments (like delimiters, substrings) that can contain escape sequences (e.g., `\\n`, `\\e`) will have these processed by the C++ native code. The main string being operated on (e.g., for `toNumber`, `trim`) also has its escapes processed if it's a Nyx string literal.

## Importing
```cpp
import "std:string" as str_utils;
```
## Functions

### `str_utils.toNumber(text_string)`

Converts `text_string` (escapes processed) to a number.

  * **Returns**: `number` or `nyx_null` on failure.
  * **Example**: `auto n = str_utils.toNumber(" -12.5\\n"); // n is -12.5`

### `str_utils.trim(text_string)`

Removes leading/trailing whitespace from `text_string` (escapes processed).

  * **Returns**: New `string`.
  * **Example**: `output(str_utils.trim("  hi  ")); // "hi"`

### `str_utils.toLowerCase(text_string)`

Converts `text_string` (escapes processed) to lowercase.

  * **Returns**: New `string`.
  * **Example**: `output(str_utils.toLowerCase("NyX")); // "nyx"`

### `str_utils.toUpperCase(text_string)`

Converts `text_string` (escapes processed) to uppercase.

  * **Returns**: New `string`.
  * **Example**: `output(str_utils.toUpperCase("NyX")); // "NYX"`

### `str_utils.contains(main_string, substring)`

Checks if `main_string` contains `substring`. Both strings have escapes processed.

  * **Returns**: `true` or `false`.
  * **Example**: `output(str_utils.contains("hello\\nworld", "\\nwo")); // true`

### `str_utils.startsWith(main_string, prefix_string)`

Checks if `main_string` starts with `prefix_string`. Both strings have escapes processed.

  * **Returns**: `true` or `false`.
  * **Example**: `output(str_utils.startsWith("image.png", "image")); // true`

### `str_utils.endsWith(main_string, suffix_string)`

Checks if `main_string` ends with `suffix_string`. Both strings have escapes processed.

  * **Returns**: `true` or `false`.
  * **Example**: `output(str_utils.endsWith("image.png", ".png")); // true`

### `str_utils.split(text_string, delimiter_string)`

Splits `text_string` by `delimiter_string`. `delimiter_string` has escapes processed. If processed delimiter is empty, splits into characters. `text_string` is split as-is (its internal control characters are used).

  * **Returns**: `list` of `string`s.
  * **Example**: `auto parts = str_utils.split("a-b-c", "-"); // ["a","b","c"]`
    `auto content = "line1" + NEWLINE + "line2";`
    `auto lines = str_utils.split(content, "\\n"); // ["line1", "line2"]`

### `str_utils.substring(text_string, start_index_num, [length_num])`

Extracts substring from `text_string`. `text_string` itself is used as-is (its actual characters, not re-processing Nyx literal escapes).

  * **Parameters**: `start_index_num` (`number`), `length_num` (optional `number`). Negative indices count from end.
  * **Returns**: New `string`.
  * **Example**: `output(str_utils.substring("hello", 1, 3)); // "ell"`

### `str_utils.replace(original_string, old_substring, new_substring)`

Replaces all occurrences of `old_substring` with `new_substring` in `original_string`. All three string arguments have their escapes processed before replacement.

  * **Returns**: New `string`.
  * **Example**: `output(str_utils.replace("bob@example.com", "bob", "alice")); // "alice@example.com"`

<!-- end list -->

---
# Nyx Standard Library: std:time

The `std:time` module provides time-related utility functions.

## Importing
```cpp
import "std:time" as time;
```

## Functions

### `time.clock()`

Returns processor time used by the program in seconds.

  * **Returns**: `number` (CPU time in seconds).
  * **Example**: `auto t = time.clock(); output("CPU time: #{t}s");`

### `time.now()`

Returns current time as seconds since the Unix epoch.

  * **Returns**: `number` (timestamp in seconds).
  * **Example**: `output("Timestamp: #{time.now()}");`

<!-- end list -->

---
# Nyx Standard Library: std:sdl

The `std:sdl` module offers bindings for SDL2 and SDL2_ttf, for graphics, windowing, events, and text rendering. For full SDL details, see official SDL2/SDL2_ttf documentation.

## Importing
```cpp
import "std:sdl" as sdl;
```

## Core Concepts

  * Initialize SDL/TTF subsystems first (`sdl.init`, `sdl.ttf_init`).
  * Create a `window`, then a `renderer` for that window.
  * Use an event loop (`sdl.pollEvent`) for user input and window events.
  * Render content (shapes, textures) using the `renderer`.
  * Call `sdl.renderPresent` to show rendered content.
  * Load fonts (`sdl.ttf_openFont`), render text to surfaces (`sdl.ttf_renderTextBlended`), convert surfaces to textures (`sdl.createTextureFromSurface`) for display.
  * Clean up with `sdl.ttf_quit`, `sdl.quit`, and by letting resource handles (window, renderer, font, texture, surface) go out of scope (or assigning `nyx_null` to them). String arguments to SDL functions (like text for `ttf_renderTextBlended`) will have their escape sequences (e.g., `\\n`) processed.

## Key Functions (Partial List)

  * **Initialization/Shutdown**: `sdl.init(flags)`, `sdl.quit()`, `sdl.ttf_init()`, `sdl.ttf_quit()`
  * **Window/Renderer**: `sdl.createWindow(...)`, `sdl.createRenderer(...)`
  * **Drawing**: `sdl.setRenderDrawColor(...)`, `sdl.renderClear(...)`, `sdl.renderFillRect(...)`, `sdl.renderCopy(...)`, `sdl.renderPresent(...)`
  * **Text**: `sdl.ttf_openFont(...)`, `sdl.ttf_renderTextBlended(...)`, `sdl.createTextureFromSurface(...)`, `sdl.queryTexture(...)`
  * **Events/Timing**: `sdl.pollEvent()`, `sdl.delay(ms)`

## Important Constants (Partial List)

Includes init flags (`sdl.SDL_INIT_VIDEO`), window position/flags (`sdl.SDL_WINDOWPOS_CENTERED`, `sdl.SDL_WINDOW_SHOWN`), renderer flags (`sdl.SDL_RENDERER_ACCELERATED`), event types (`sdl.SDL_QUIT`, `sdl.SDL_KEYDOWN`), and key codes (`sdl.K_ESCAPE`, `sdl.K_UP`, etc.).

## Example Snippet

```cpp
// Import necessary standard library modules
import "std:sdl" as sdl; // For graphics, windowing, events, and text rendering
import "std:io" as io;   // For printing error messages to the console

// Define the main function for our SDL test program
func main_sdl_test() = {
    // Window dimensions
    auto WINDOW_WIDTH = 640;
    auto WINDOW_HEIGHT = 480;

    // Initialize SDL video and event subsystems
    if (!sdl.init(sdl.SDL_INIT_VIDEO)) {
        output("SDL Init failed!"); // Print error if SDL initialization fails
        return 1; // Exit the function indicating an error
    }

    // Initialize SDL_ttf library for font rendering
    if (!sdl.ttf_init()) {
        output("SDL_ttf Init failed!"); // Print error if SDL_ttf initialization fails
        sdl.quit(); // Clean up SDL subsystems before exiting
        return 1;   // Exit the function indicating an error
    }

    // Create the main application window
    auto window = sdl.createWindow(
        "Nyx SDL Test", 
        sdl.SDL_WINDOWPOS_CENTERED, // Center window horizontally on screen
        sdl.SDL_WINDOWPOS_CENTERED, // Center window vertically on screen
        WINDOW_WIDTH, 
        WINDOW_HEIGHT, 
        sdl.SDL_WINDOW_SHOWN        // Make the window visible
    );

    // Check if window creation was successful
    if(!window) {
        output("Failed to create window!");
        sdl.ttf_quit(); // Clean up SDL_ttf if window creation fails
        sdl.quit();     // Clean up SDL
        return 1;
    }

    // Create a renderer for the window
    auto renderer = sdl.createRenderer(window, -1, sdl.SDL_RENDERER_ACCELERATED);

    // Check if renderer creation was successful
    if(!renderer) {
        output("Failed to create renderer!");
        sdl.ttf_quit(); // Clean up SDL_ttf
        sdl.quit();     // Clean up SDL
        return 1;
    }

    // Define the path to the font file and load it
    auto FONT_PATH = "examples/snake_game/fonts/Roboto.ttf"; 
    auto font_size = 24;
    auto font = sdl.ttf_openFont(FONT_PATH, font_size); 

    auto running = true; // Flag to keep the main application loop running

    // Main application loop
    for (;running;) {
        // Event handling loop
        for (auto e = sdl.pollEvent(); e; e = sdl.pollEvent()) {
            if (e[0] == sdl.SDL_QUIT) { 
                running = false; 
            }
        }

        // Rendering phase
        sdl.setRenderDrawColor(renderer, 0, 0, 50, 255); // Dark blue background
        sdl.renderClear(renderer); // Clear the screen

        // Text rendering
        if (font) {
            auto text_surface = sdl.ttf_renderTextBlended(font, "Hello Nyx!", 255, 255, 255, 255); // White text
            
            if (text_surface) {
                auto text_texture = sdl.createTextureFromSurface(renderer, text_surface);
                
                if (text_texture) {
                    auto dims = sdl.queryTexture(text_texture); 
                    if (dims) { 
                        auto text_width = dims[0];
                        auto text_height = dims[1];
                        
                        // Calculate position to center the text
                        auto text_x = (WINDOW_WIDTH / 2) - (text_width / 2);
                        auto text_y = (WINDOW_HEIGHT / 2) - (text_height / 2);
                        
                        // Define the destination rectangle for the text
                        auto text_rect = [text_x, text_y, text_width, text_height]; 
                        
                        sdl.renderCopy(renderer, text_texture, nyx_null, text_rect);
                    }
                }
            }
        } else {
            output("Font not loaded, cannot render text.");
        }

        // Update the screen
        sdl.renderPresent(renderer);

        // Control frame rate
        sdl.delay(16); 
    }

    // Shutdown sequence
    output("Exiting SDL test. Cleaning up resources...");
    if (font) {
        font = nyx_null; 
    }
    if (renderer) {
        renderer = nyx_null; 
    }
    if (window) {
        window = nyx_null; 
    }
    
    sdl.ttf_quit(); 
    sdl.quit();     
    
    output("SDL test finished.");
    return 0;       
}

// Call the main function
main_sdl_test();
```