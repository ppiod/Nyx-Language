#ifndef NYX_STDLIB_SDL_H
#define NYX_STDLIB_SDL_H

#include "../common/Value.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace Nyx {

class Interpreter;

struct NyxSDLWindowWrapper {
    SDL_Window* ptr;
    NyxSDLWindowWrapper(SDL_Window* p) : ptr(p) {}
    ~NyxSDLWindowWrapper() { if (ptr) { SDL_DestroyWindow(ptr); } }
    
    NyxSDLWindowWrapper(const NyxSDLWindowWrapper&) = delete;
    NyxSDLWindowWrapper& operator=(const NyxSDLWindowWrapper&) = delete;

    NyxSDLWindowWrapper(NyxSDLWindowWrapper&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    NyxSDLWindowWrapper& operator=(NyxSDLWindowWrapper&& other) noexcept {
        if (this != &other) {
            if (ptr) { SDL_DestroyWindow(ptr); }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
};

struct NyxSDLRendererWrapper {
    SDL_Renderer* ptr;
    NyxSDLRendererWrapper(SDL_Renderer* p) : ptr(p) {}
    ~NyxSDLRendererWrapper() { if (ptr) { SDL_DestroyRenderer(ptr); } }
    
    NyxSDLRendererWrapper(const NyxSDLRendererWrapper&) = delete;
    NyxSDLRendererWrapper& operator=(const NyxSDLRendererWrapper&) = delete;

    NyxSDLRendererWrapper(NyxSDLRendererWrapper&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    NyxSDLRendererWrapper& operator=(NyxSDLRendererWrapper&& other) noexcept {
        if (this != &other) {
            if (ptr) { SDL_DestroyRenderer(ptr); }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
};

struct NyxSDLFontWrapper {
    TTF_Font* ptr;
    NyxSDLFontWrapper(TTF_Font* p) : ptr(p) {}
    ~NyxSDLFontWrapper() { if (ptr) { TTF_CloseFont(ptr); } }

    NyxSDLFontWrapper(const NyxSDLFontWrapper&) = delete;
    NyxSDLFontWrapper& operator=(const NyxSDLFontWrapper&) = delete;

    NyxSDLFontWrapper(NyxSDLFontWrapper&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    NyxSDLFontWrapper& operator=(NyxSDLFontWrapper&& other) noexcept {
        if (this != &other) {
            if (ptr) { TTF_CloseFont(ptr); }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
};

struct NyxSDLSurfaceWrapper {
    SDL_Surface* ptr;
    NyxSDLSurfaceWrapper(SDL_Surface* p) : ptr(p) {}
    ~NyxSDLSurfaceWrapper() { if (ptr) { SDL_FreeSurface(ptr); } }

    NyxSDLSurfaceWrapper(const NyxSDLSurfaceWrapper&) = delete;
    NyxSDLSurfaceWrapper& operator=(const NyxSDLSurfaceWrapper&) = delete;

    NyxSDLSurfaceWrapper(NyxSDLSurfaceWrapper&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    NyxSDLSurfaceWrapper& operator=(NyxSDLSurfaceWrapper&& other) noexcept {
        if (this != &other) {
            if (ptr) { SDL_FreeSurface(ptr); }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
};

struct NyxSDLTextureWrapper {
    SDL_Texture* ptr;
    NyxSDLTextureWrapper(SDL_Texture* p) : ptr(p) {}
    ~NyxSDLTextureWrapper() { if (ptr) { SDL_DestroyTexture(ptr); } }

    NyxSDLTextureWrapper(const NyxSDLTextureWrapper&) = delete;
    NyxSDLTextureWrapper& operator=(const NyxSDLTextureWrapper&) = delete;

    NyxSDLTextureWrapper(NyxSDLTextureWrapper&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }
    NyxSDLTextureWrapper& operator=(NyxSDLTextureWrapper&& other) noexcept {
        if (this != &other) {
            if (ptr) { SDL_DestroyTexture(ptr); }
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }
};


NyxValue native_sdl_init(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_quit(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_createWindow(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_createRenderer(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_setRenderDrawColor(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_renderClear(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_renderFillRect(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_renderCopy(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_queryTexture(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_renderPresent(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_delay(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_pollEvent(Interpreter& interpreter, const std::vector<NyxValue>& args);

NyxValue native_sdl_ttf_init(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_ttf_quit(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_ttf_openFont(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_ttf_renderTextBlended(Interpreter& interpreter, const std::vector<NyxValue>& args);
NyxValue native_sdl_createTextureFromSurface(Interpreter& interpreter, const std::vector<NyxValue>& args);

void registerStdSdlModule(Interpreter& interpreter);

}

#endif