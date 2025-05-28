#include "./sdl_module.h"
#include "../interpreter/Interpreter.h"
#include "../interpreter/Environment.h"
#include "../common/Utils.h"
#include <iostream>
#include <map>

namespace Nyx {

NyxValue native_sdl_init(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        std::cerr << "[Nyx C++] Error: 'sdl.init' expects one number argument (flags)." << std::endl;
        return NyxValue(false);
    }
    Uint32 flags = static_cast<Uint32>(std::get<double>(args[0].data));
    if (SDL_Init(flags) < 0) {
        std::cerr << "[DEBUG C++] SDL_Init failed. SDL_Error: " << SDL_GetError() << std::endl;
        return NyxValue(false);
    }
    return NyxValue(true);
}

NyxValue native_sdl_quit(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (!args.empty()){
        std::cerr << "[Nyx C++] Error: 'sdl.quit' expects no arguments." << std::endl;
        return NyxValue(std::monostate{});
    }
    SDL_Quit();
    return NyxValue(std::monostate{});
}

NyxValue native_sdl_createWindow(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 6 ||
        !std::holds_alternative<std::string>(args[0].data) ||
        !std::holds_alternative<double>(args[1].data) ||
        !std::holds_alternative<double>(args[2].data) ||
        !std::holds_alternative<double>(args[3].data) ||
        !std::holds_alternative<double>(args[4].data) ||
        !std::holds_alternative<double>(args[5].data)) {
        std::cerr << "[Nyx C++] Error: 'sdl.createWindow' expects (string title, num x, num y, num w, num h, num flags)." << std::endl;
        return NyxValue(std::monostate{});
    }
    std::string title = std::get<std::string>(args[0].data);
    int x = static_cast<int>(std::get<double>(args[1].data));
    int y = static_cast<int>(std::get<double>(args[2].data));
    int w = static_cast<int>(std::get<double>(args[3].data));
    int h = static_cast<int>(std::get<double>(args[4].data));
    Uint32 flags = static_cast<Uint32>(std::get<double>(args[5].data));

    SDL_Window* window_ptr = SDL_CreateWindow(title.c_str(), x, y, w, h, flags);
    if (!window_ptr) {
        std::cerr << "[DEBUG C++] SDL_CreateWindow failed. SDL_Error: " << SDL_GetError() << std::endl;
        return NyxValue(std::monostate{});
    }
    std::shared_ptr<NyxSDLWindowWrapper> wrapper_sptr(new NyxSDLWindowWrapper(window_ptr));
    return NyxValue(wrapper_sptr);
}

NyxValue native_sdl_createRenderer(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 3 ||
        !std::holds_alternative<SDLWindowNyxPtr>(args[0].data) ||
        !std::holds_alternative<double>(args[1].data) ||
        !std::holds_alternative<double>(args[2].data)) {
        std::cerr << "[Nyx C++] Error: 'sdl.createRenderer' expects (window_handle, num index, num flags)." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto window_wrapper_ptr = std::get<SDLWindowNyxPtr>(args[0].data);
    if (!window_wrapper_ptr || !window_wrapper_ptr->ptr) {
         std::cerr << "[Nyx C++] Error: Invalid window handle passed to 'sdl.createRenderer'." << std::endl;
        return NyxValue(std::monostate{});
    }
    SDL_Window* sdl_window = window_wrapper_ptr->ptr;
    int index = static_cast<int>(std::get<double>(args[1].data));
    Uint32 flags = static_cast<Uint32>(std::get<double>(args[2].data));

    SDL_Renderer* renderer_ptr = SDL_CreateRenderer(sdl_window, index, flags);
    if (!renderer_ptr) {
        std::cerr << "[DEBUG C++] SDL_CreateRenderer failed. SDL_Error: " << SDL_GetError() << std::endl;
        return NyxValue(std::monostate{});
    }
    std::shared_ptr<NyxSDLRendererWrapper> wrapper_sptr(new NyxSDLRendererWrapper(renderer_ptr));
    return NyxValue(wrapper_sptr);
}

NyxValue native_sdl_setRenderDrawColor(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 5 ||
        !std::holds_alternative<SDLRendererNyxPtr>(args[0].data) ||
        !std::holds_alternative<double>(args[1].data) ||
        !std::holds_alternative<double>(args[2].data) ||
        !std::holds_alternative<double>(args[3].data) ||
        !std::holds_alternative<double>(args[4].data)) {
        std::cerr << "[Nyx C++] Error: 'sdl.setRenderDrawColor' expects (renderer, r, g, b, a)." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto renderer_wrapper_ptr = std::get<SDLRendererNyxPtr>(args[0].data);
     if (!renderer_wrapper_ptr || !renderer_wrapper_ptr->ptr) {
         std::cerr << "[Nyx C++] Error: Invalid renderer handle passed to 'sdl.setRenderDrawColor'." << std::endl;
        return NyxValue(std::monostate{});
    }
    SDL_Renderer* sdl_renderer = renderer_wrapper_ptr->ptr;
    Uint8 r = static_cast<Uint8>(std::get<double>(args[1].data));
    Uint8 g = static_cast<Uint8>(std::get<double>(args[2].data));
    Uint8 b = static_cast<Uint8>(std::get<double>(args[3].data));
    Uint8 a = static_cast<Uint8>(std::get<double>(args[4].data));
    if (SDL_SetRenderDrawColor(sdl_renderer, r, g, b, a) != 0) {
        std::cerr << "[DEBUG C++] SDL_SetRenderDrawColor failed. SDL_Error: " << SDL_GetError() << std::endl;
    }
    return NyxValue(std::monostate{});
}

NyxValue native_sdl_renderClear(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<SDLRendererNyxPtr>(args[0].data)) {
         std::cerr << "[Nyx C++] Error: 'sdl.renderClear' expects a renderer handle." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto renderer_wrapper_ptr = std::get<SDLRendererNyxPtr>(args[0].data);
    if (!renderer_wrapper_ptr || !renderer_wrapper_ptr->ptr) {
         std::cerr << "[Nyx C++] Error: Invalid renderer handle passed to 'sdl.renderClear'." << std::endl;
        return NyxValue(std::monostate{});
    }
    if (SDL_RenderClear(renderer_wrapper_ptr->ptr) != 0) {
         std::cerr << "[DEBUG C++] SDL_RenderClear failed. SDL_Error: " << SDL_GetError() << std::endl;
    }
    return NyxValue(std::monostate{});
}

NyxValue native_sdl_renderFillRect(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 || !std::holds_alternative<SDLRendererNyxPtr>(args[0].data) ||
        !std::holds_alternative<NyxList>(args[1].data) ) {
         std::cerr << "[Nyx C++] Error: 'sdl.renderFillRect' expects (renderer, rect_list[x,y,w,h])." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto renderer_wrapper_ptr = std::get<SDLRendererNyxPtr>(args[0].data);
    if (!renderer_wrapper_ptr || !renderer_wrapper_ptr->ptr) {
         std::cerr << "[Nyx C++] Error: Invalid renderer handle passed to 'sdl.renderFillRect'." << std::endl;
        return NyxValue(std::monostate{});
    }
    
    const auto& rect_list_val = std::get<NyxList>(args[1].data);
    if (rect_list_val.size() != 4) {
        std::cerr << "[Nyx C++] Error: Rect argument for 'sdl.renderFillRect' must be a list of 4 numbers [x,y,w,h]." << std::endl;
        return NyxValue(std::monostate{});
    }
    SDL_Rect rect;
    for(size_t i=0; i < 4; ++i) {
        if(!std::holds_alternative<double>(rect_list_val[i].data)){
            std::cerr << "[Nyx C++] Error: Rect arguments for 'sdl.renderFillRect' must be numbers." << std::endl;
            return NyxValue(std::monostate{});
        }
    }
    rect.x = static_cast<int>(std::get<double>(rect_list_val[0].data));
    rect.y = static_cast<int>(std::get<double>(rect_list_val[1].data));
    rect.w = static_cast<int>(std::get<double>(rect_list_val[2].data));
    rect.h = static_cast<int>(std::get<double>(rect_list_val[3].data));

    if (SDL_RenderFillRect(renderer_wrapper_ptr->ptr, &rect) != 0) {
        std::cerr << "[DEBUG C++] SDL_RenderFillRect failed. SDL_Error: " << SDL_GetError() << std::endl;
    }
    return NyxValue(std::monostate{});
}

NyxValue native_sdl_renderCopy(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 4 ||
        !std::holds_alternative<SDLRendererNyxPtr>(args[0].data) ||
        !std::holds_alternative<SDLTextureNyxPtr>(args[1].data) ||
        !(std::holds_alternative<std::monostate>(args[2].data) || std::holds_alternative<NyxList>(args[2].data)) ||
        !std::holds_alternative<NyxList>(args[3].data) ) {
        std::cerr << "[Nyx C++] Error: 'sdl.renderCopy' expects (renderer, texture, src_rect_or_null, dst_rect_list)." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto renderer_wrapper_ptr = std::get<SDLRendererNyxPtr>(args[0].data);
    if (!renderer_wrapper_ptr || !renderer_wrapper_ptr->ptr) {
         std::cerr << "[Nyx C++] Error: Invalid renderer handle for 'sdl.renderCopy'." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto texture_wrapper_ptr = std::get<SDLTextureNyxPtr>(args[1].data);
    if (!texture_wrapper_ptr || !texture_wrapper_ptr->ptr) {
         std::cerr << "[Nyx C++] Error: Invalid texture handle for 'sdl.renderCopy'." << std::endl;
        return NyxValue(std::monostate{});
    }

    SDL_Rect* src_rect_ptr = nullptr;
    SDL_Rect src_rect_actual;
    if (!std::holds_alternative<std::monostate>(args[2].data)) {
        const auto& rect_list_val = std::get<NyxList>(args[2].data);
        if (rect_list_val.size() != 4) {
             std::cerr << "[Nyx C++] Error: src_rect for 'sdl.renderCopy' must be [x,y,w,h]." << std::endl;
             return NyxValue(std::monostate{});
        }
        for(size_t i=0; i < 4; ++i) {
            if(!std::holds_alternative<double>(rect_list_val[i].data)) {
                 std::cerr << "[Nyx C++] Error: src_rect components must be numbers." << std::endl;
                 return NyxValue(std::monostate{});
            }
        }
        src_rect_actual.x = static_cast<int>(std::get<double>(rect_list_val[0].data));
        src_rect_actual.y = static_cast<int>(std::get<double>(rect_list_val[1].data));
        src_rect_actual.w = static_cast<int>(std::get<double>(rect_list_val[2].data));
        src_rect_actual.h = static_cast<int>(std::get<double>(rect_list_val[3].data));
        src_rect_ptr = &src_rect_actual;
    }

    const auto& dst_rect_list_val = std::get<NyxList>(args[3].data);
    if (dst_rect_list_val.size() != 4) {
         std::cerr << "[Nyx C++] Error: dst_rect for 'sdl.renderCopy' must be [x,y,w,h]." << std::endl;
         return NyxValue(std::monostate{});
    }
    SDL_Rect dst_rect_actual;
    for(size_t i=0; i < 4; ++i) {
         if(!std::holds_alternative<double>(dst_rect_list_val[i].data)) {
            std::cerr << "[Nyx C++] Error: dst_rect components must be numbers." << std::endl;
            return NyxValue(std::monostate{});
        }
    }
    dst_rect_actual.x = static_cast<int>(std::get<double>(dst_rect_list_val[0].data));
    dst_rect_actual.y = static_cast<int>(std::get<double>(dst_rect_list_val[1].data));
    dst_rect_actual.w = static_cast<int>(std::get<double>(dst_rect_list_val[2].data));
    dst_rect_actual.h = static_cast<int>(std::get<double>(dst_rect_list_val[3].data));

    if (SDL_RenderCopy(renderer_wrapper_ptr->ptr, texture_wrapper_ptr->ptr, src_rect_ptr, &dst_rect_actual) != 0) {
        std::cerr << "[DEBUG C++] SDL_RenderCopy failed. SDL_Error: " << SDL_GetError() << std::endl;
    }
    return NyxValue(std::monostate{});
}

NyxValue native_sdl_queryTexture(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<SDLTextureNyxPtr>(args[0].data)) {
        std::cerr << "[Nyx C++] Error: 'sdl.queryTexture' expects one texture handle argument." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto texture_wrapper_ptr = std::get<SDLTextureNyxPtr>(args[0].data);
    if (!texture_wrapper_ptr || !texture_wrapper_ptr->ptr) {
        std::cerr << "[Nyx C++] Error: Invalid texture handle for 'sdl.queryTexture'." << std::endl;
        return NyxValue(std::monostate{});
    }
    int w, h;
    if (SDL_QueryTexture(texture_wrapper_ptr->ptr, NULL, NULL, &w, &h) != 0) {
        std::cerr << "[DEBUG C++] SDL_QueryTexture failed. SDL_Error: " << SDL_GetError() << std::endl;
        return NyxValue(std::monostate{});
    }
    NyxList dims;
    dims.push_back(NyxValue(static_cast<double>(w)));
    dims.push_back(NyxValue(static_cast<double>(h)));
    return NyxValue(dims);
}

NyxValue native_sdl_renderPresent(Interpreter& interpreter, const std::vector<NyxValue>& args) {
     if (args.size() != 1 || !std::holds_alternative<SDLRendererNyxPtr>(args[0].data)) {
         std::cerr << "[Nyx C++] Error: 'sdl.renderPresent' expects a renderer handle." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto renderer_wrapper_ptr = std::get<SDLRendererNyxPtr>(args[0].data);
    if (!renderer_wrapper_ptr || !renderer_wrapper_ptr->ptr) {
         std::cerr << "[Nyx C++] Error: Invalid renderer handle passed to 'sdl.renderPresent'." << std::endl;
        return NyxValue(std::monostate{});
    }
    SDL_RenderPresent(renderer_wrapper_ptr->ptr);
    return NyxValue(std::monostate{});
}

NyxValue native_sdl_delay(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 1 || !std::holds_alternative<double>(args[0].data)) {
        std::cerr << "[Nyx C++] Error: 'sdl.delay' expects one number argument (milliseconds)." << std::endl;
        return NyxValue(std::monostate{});
    }
    Uint32 ms = static_cast<Uint32>(std::get<double>(args[0].data));
    SDL_Delay(ms);
    return NyxValue(std::monostate{});
}

NyxValue native_sdl_pollEvent(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (!args.empty()){
        std::cerr << "[Nyx C++] Error: 'sdl.pollEvent' expects no arguments." << std::endl;
        return NyxValue(std::monostate{});
    }
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        NyxList event_data_list;
        event_data_list.push_back(NyxValue(static_cast<double>(event.type))); // e[0] = eventType

        switch (event.type) {
            case SDL_QUIT:
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                event_data_list.push_back(NyxValue(static_cast<double>(event.key.keysym.sym)));    // e[1] = keyCode
                event_data_list.push_back(NyxValue(static_cast<double>(event.key.keysym.scancode))); // e[2] = scancode
                event_data_list.push_back(NyxValue(static_cast<double>(event.key.keysym.mod)));    // e[3] = keyModifier
                event_data_list.push_back(NyxValue(static_cast<bool>(event.key.repeat != 0)));   // e[4] = isRepeat
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                event_data_list.push_back(NyxValue(static_cast<double>(event.button.button))); // e[1] = button
                event_data_list.push_back(NyxValue(static_cast<double>(event.button.x)));      // e[2] = mouse_x
                event_data_list.push_back(NyxValue(static_cast<double>(event.button.y)));      // e[3] = mouse_y
                event_data_list.push_back(NyxValue(static_cast<double>(event.button.clicks))); // e[4] = 1 for single-click, 2 for double-click
                break;
            case SDL_MOUSEMOTION:
                event_data_list.push_back(NyxValue(static_cast<double>(event.motion.x)));      // e[1] = mouse_x
                event_data_list.push_back(NyxValue(static_cast<double>(event.motion.y)));      // e[2] = mouse_y
                event_data_list.push_back(NyxValue(static_cast<double>(event.motion.xrel)));   // e[3] = relative_x
                event_data_list.push_back(NyxValue(static_cast<double>(event.motion.yrel)));   // e[4] = relative_y
                break;
        }
        return NyxValue(event_data_list); 
    }
    return NyxValue(std::monostate{}); 
}

NyxValue native_sdl_ttf_init(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    //std::cerr << "[DEBUG C++] native_sdl_ttf_init: Entered function." << std::endl;
    if (!args.empty()) {
        std::cerr << "[DEBUG C++] native_sdl_ttf_init: Argument check failed (expects no arguments)." << std::endl; // This is an error message related to Nyx binding, keep it
        return NyxValue(false);
    }
    if (TTF_Init() == -1) {
        std::cerr << "[DEBUG C++] native_sdl_ttf_init: TTF_Init call FAILED. TTF_Error: " << TTF_GetError() << std::endl; 
        return NyxValue(false);
    }
    //std::cerr << "[DEBUG C++] native_sdl_ttf_init: TTF_Init call SUCCEEDED." << std::endl;
    return NyxValue(true);
}

NyxValue native_sdl_ttf_quit(Interpreter& interpreter, const std::vector<NyxValue>& args) {
     if (!args.empty()) {
        std::cerr << "[Nyx C++] Error: 'sdl.ttf_quit' expects no arguments." << std::endl;
        return NyxValue(std::monostate{});
    }
    TTF_Quit();
    return NyxValue(std::monostate{});
}

NyxValue native_sdl_ttf_openFont(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    //std::cerr << "[DEBUG C++] native_sdl_ttf_openFont: Entered function." << std::endl;

    if (args.size() != 2 || !std::holds_alternative<std::string>(args[0].data) || !std::holds_alternative<double>(args[1].data)) {
        std::cerr << "[DEBUG C++] native_sdl_ttf_openFont: Argument check failed. Expected (string filepath, num point_size)." << std::endl; // Nyx binding error, keep
        return NyxValue(std::monostate{});
    }

    std::string filepath = std::get<std::string>(args[0].data);
    int point_size = static_cast<int>(std::get<double>(args[1].data));
    //std::cerr << "[DEBUG C++] native_sdl_ttf_openFont: Attempting to load '" << filepath << "' at size " << point_size << "." << std::endl;

    TTF_Font* font_ptr = TTF_OpenFont(filepath.c_str(), point_size);
    if (!font_ptr) {
        std::cerr << "[DEBUG C++] native_sdl_ttf_openFont: TTF_OpenFont call FAILED. TTF_Error: " << TTF_GetError() << std::endl;
        return NyxValue(std::monostate{});
    }

    //std::cerr << "[DEBUG C++] native_sdl_ttf_openFont: TTF_OpenFont call SUCCEEDED." << std::endl;
    std::shared_ptr<NyxSDLFontWrapper> wrapper_sptr(new NyxSDLFontWrapper(font_ptr));
    return NyxValue(wrapper_sptr);
}

NyxValue native_sdl_ttf_renderTextBlended(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 6 ||
        !std::holds_alternative<SDLFontNyxPtr>(args[0].data) ||
        !std::holds_alternative<std::string>(args[1].data) ||
        !std::holds_alternative<double>(args[2].data) ||
        !std::holds_alternative<double>(args[3].data) ||
        !std::holds_alternative<double>(args[4].data) ||
        !std::holds_alternative<double>(args[5].data)) {
        std::cerr << "[Nyx C++] Error: 'sdl.ttf_renderTextBlended' expects (font, text, r, g, b, a)." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto font_wrapper_ptr = std::get<SDLFontNyxPtr>(args[0].data);
    if (!font_wrapper_ptr || !font_wrapper_ptr->ptr) {
        std::cerr << "[Nyx C++] Error: Invalid font handle for 'sdl.ttf_renderTextBlended'." << std::endl;
        return NyxValue(std::monostate{});
    }
    std::string text = Common::process_escapes(std::get<std::string>(args[1].data));
    SDL_Color color = {
        static_cast<Uint8>(std::get<double>(args[2].data)),
        static_cast<Uint8>(std::get<double>(args[3].data)),
        static_cast<Uint8>(std::get<double>(args[4].data)),
        static_cast<Uint8>(std::get<double>(args[5].data))
    };
    SDL_Surface* surface_ptr = TTF_RenderText_Blended(font_wrapper_ptr->ptr, text.c_str(), color);
    if (!surface_ptr) {
        std::cerr << "[DEBUG C++] TTF_RenderText_Blended failed. TTF_Error: " << TTF_GetError() << std::endl;
        return NyxValue(std::monostate{});
    }
    std::shared_ptr<NyxSDLSurfaceWrapper> wrapper_sptr(new NyxSDLSurfaceWrapper(surface_ptr));
    return NyxValue(wrapper_sptr);
}

NyxValue native_sdl_createTextureFromSurface(Interpreter& interpreter, const std::vector<NyxValue>& args) {
    if (args.size() != 2 ||
        !std::holds_alternative<SDLRendererNyxPtr>(args[0].data) ||
        !std::holds_alternative<SDLSurfaceNyxPtr>(args[1].data)) {
        std::cerr << "[Nyx C++] Error: 'sdl.createTextureFromSurface' expects (renderer, surface)." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto renderer_wrapper_ptr = std::get<SDLRendererNyxPtr>(args[0].data);
     if (!renderer_wrapper_ptr || !renderer_wrapper_ptr->ptr) {
        std::cerr << "[Nyx C++] Error: Invalid renderer handle for 'sdl.createTextureFromSurface'." << std::endl;
        return NyxValue(std::monostate{});
    }
    auto surface_wrapper_ptr = std::get<SDLSurfaceNyxPtr>(args[1].data);
     if (!surface_wrapper_ptr || !surface_wrapper_ptr->ptr) {
        std::cerr << "[Nyx C++] Error: Invalid surface handle for 'sdl.createTextureFromSurface'." << std::endl;
        return NyxValue(std::monostate{});
    }

    SDL_Texture* texture_ptr = SDL_CreateTextureFromSurface(renderer_wrapper_ptr->ptr, surface_wrapper_ptr->ptr);
    if (!texture_ptr) {
        std::cerr << "[DEBUG C++] SDL_CreateTextureFromSurface failed. SDL_Error: " << SDL_GetError() << std::endl;
        return NyxValue(std::monostate{});
    }
    std::shared_ptr<NyxSDLTextureWrapper> wrapper_sptr(new NyxSDLTextureWrapper(texture_ptr));
    return NyxValue(wrapper_sptr);
}


void registerStdSdlModule(Interpreter& interpreter) {
    Interpreter::NativeModuleBuilder builder = [&]() {
        auto module_env = std::make_shared<Environment>(interpreter.globals);
        
        module_env->define("init", NyxValue(std::make_shared<NyxNativeFunction>("init", native_sdl_init, 1)));
        module_env->define("quit", NyxValue(std::make_shared<NyxNativeFunction>("quit", native_sdl_quit, 0)));
        module_env->define("createWindow", NyxValue(std::make_shared<NyxNativeFunction>("createWindow", native_sdl_createWindow, 6)));
        module_env->define("createRenderer", NyxValue(std::make_shared<NyxNativeFunction>("createRenderer", native_sdl_createRenderer, 3)));
        module_env->define("setRenderDrawColor", NyxValue(std::make_shared<NyxNativeFunction>("setRenderDrawColor", native_sdl_setRenderDrawColor, 5)));
        module_env->define("renderClear", NyxValue(std::make_shared<NyxNativeFunction>("renderClear", native_sdl_renderClear, 1)));
        module_env->define("renderFillRect", NyxValue(std::make_shared<NyxNativeFunction>("renderFillRect", native_sdl_renderFillRect, 2)));
        module_env->define("renderCopy", NyxValue(std::make_shared<NyxNativeFunction>("renderCopy", native_sdl_renderCopy, 4)));
        module_env->define("queryTexture", NyxValue(std::make_shared<NyxNativeFunction>("queryTexture", native_sdl_queryTexture, 1)));
        module_env->define("renderPresent", NyxValue(std::make_shared<NyxNativeFunction>("renderPresent", native_sdl_renderPresent, 1)));
        module_env->define("delay", NyxValue(std::make_shared<NyxNativeFunction>("delay", native_sdl_delay, 1)));
        module_env->define("pollEvent", NyxValue(std::make_shared<NyxNativeFunction>("pollEvent", native_sdl_pollEvent, 0)));
        
        module_env->define("ttf_init", NyxValue(std::make_shared<NyxNativeFunction>("ttf_init", native_sdl_ttf_init, 0)));
        module_env->define("ttf_quit", NyxValue(std::make_shared<NyxNativeFunction>("ttf_quit", native_sdl_ttf_quit, 0)));
        module_env->define("ttf_openFont", NyxValue(std::make_shared<NyxNativeFunction>("ttf_openFont", native_sdl_ttf_openFont, 2)));
        module_env->define("ttf_renderTextBlended", NyxValue(std::make_shared<NyxNativeFunction>("ttf_renderTextBlended", native_sdl_ttf_renderTextBlended, 6)));
        module_env->define("createTextureFromSurface", NyxValue(std::make_shared<NyxNativeFunction>("createTextureFromSurface", native_sdl_createTextureFromSurface, 2)));
        
        module_env->define("SDL_INIT_VIDEO", NyxValue(static_cast<double>(SDL_INIT_VIDEO)));
        module_env->define("SDL_INIT_AUDIO", NyxValue(static_cast<double>(SDL_INIT_AUDIO)));
        module_env->define("SDL_INIT_TIMER", NyxValue(static_cast<double>(SDL_INIT_TIMER)));
        module_env->define("SDL_INIT_EVENTS", NyxValue(static_cast<double>(SDL_INIT_EVENTS)));
        module_env->define("SDL_INIT_EVERYTHING", NyxValue(static_cast<double>(SDL_INIT_EVERYTHING)));

        module_env->define("SDL_WINDOWPOS_UNDEFINED", NyxValue(static_cast<double>(SDL_WINDOWPOS_UNDEFINED)));
        module_env->define("SDL_WINDOWPOS_CENTERED", NyxValue(static_cast<double>(SDL_WINDOWPOS_CENTERED)));
        
        module_env->define("SDL_WINDOW_FULLSCREEN", NyxValue(static_cast<double>(SDL_WINDOW_FULLSCREEN)));
        module_env->define("SDL_WINDOW_OPENGL", NyxValue(static_cast<double>(SDL_WINDOW_OPENGL)));
        module_env->define("SDL_WINDOW_SHOWN", NyxValue(static_cast<double>(SDL_WINDOW_SHOWN)));
        module_env->define("SDL_WINDOW_HIDDEN", NyxValue(static_cast<double>(SDL_WINDOW_HIDDEN)));
        module_env->define("SDL_WINDOW_BORDERLESS", NyxValue(static_cast<double>(SDL_WINDOW_BORDERLESS)));
        module_env->define("SDL_WINDOW_RESIZABLE", NyxValue(static_cast<double>(SDL_WINDOW_RESIZABLE)));
        module_env->define("SDL_WINDOW_MINIMIZED", NyxValue(static_cast<double>(SDL_WINDOW_MINIMIZED)));
        module_env->define("SDL_WINDOW_MAXIMIZED", NyxValue(static_cast<double>(SDL_WINDOW_MAXIMIZED)));

        module_env->define("SDL_RENDERER_SOFTWARE", NyxValue(static_cast<double>(SDL_RENDERER_SOFTWARE)));
        module_env->define("SDL_RENDERER_ACCELERATED", NyxValue(static_cast<double>(SDL_RENDERER_ACCELERATED)));
        module_env->define("SDL_RENDERER_PRESENTVSYNC", NyxValue(static_cast<double>(SDL_RENDERER_PRESENTVSYNC)));
        module_env->define("SDL_RENDERER_TARGETTEXTURE", NyxValue(static_cast<double>(SDL_RENDERER_TARGETTEXTURE)));
        
        Uint32 combined_renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
        module_env->define("SDL_RENDERER_ACCELERATED_VSYNC", NyxValue(static_cast<double>(combined_renderer_flags)));

        module_env->define("SDL_QUIT", NyxValue(static_cast<double>(SDL_QUIT)));
        module_env->define("SDL_KEYDOWN", NyxValue(static_cast<double>(SDL_KEYDOWN)));
        module_env->define("SDL_KEYUP", NyxValue(static_cast<double>(SDL_KEYUP)));
        module_env->define("SDL_MOUSEMOTION", NyxValue(static_cast<double>(SDL_MOUSEMOTION)));
        module_env->define("SDL_MOUSEBUTTONDOWN", NyxValue(static_cast<double>(SDL_MOUSEBUTTONDOWN)));
        module_env->define("SDL_MOUSEBUTTONUP", NyxValue(static_cast<double>(SDL_MOUSEBUTTONUP)));

        module_env->define("K_UNKNOWN", NyxValue(static_cast<double>(SDLK_UNKNOWN)));
        module_env->define("K_RETURN", NyxValue(static_cast<double>(SDLK_RETURN)));
        module_env->define("K_ESCAPE", NyxValue(static_cast<double>(SDLK_ESCAPE)));
        module_env->define("K_BACKSPACE", NyxValue(static_cast<double>(SDLK_BACKSPACE)));
        module_env->define("K_TAB", NyxValue(static_cast<double>(SDLK_TAB)));
        module_env->define("K_SPACE", NyxValue(static_cast<double>(SDLK_SPACE)));
        module_env->define("K_0", NyxValue(static_cast<double>(SDLK_0)));
        module_env->define("K_1", NyxValue(static_cast<double>(SDLK_1)));
        module_env->define("K_2", NyxValue(static_cast<double>(SDLK_2)));
        module_env->define("K_3", NyxValue(static_cast<double>(SDLK_3)));
        module_env->define("K_4", NyxValue(static_cast<double>(SDLK_4)));
        module_env->define("K_5", NyxValue(static_cast<double>(SDLK_5)));
        module_env->define("K_6", NyxValue(static_cast<double>(SDLK_6)));
        module_env->define("K_7", NyxValue(static_cast<double>(SDLK_7)));
        module_env->define("K_8", NyxValue(static_cast<double>(SDLK_8)));
        module_env->define("K_9", NyxValue(static_cast<double>(SDLK_9)));
        module_env->define("K_a", NyxValue(static_cast<double>(SDLK_a)));
        module_env->define("K_b", NyxValue(static_cast<double>(SDLK_b)));
        module_env->define("K_c", NyxValue(static_cast<double>(SDLK_c)));
        module_env->define("K_d", NyxValue(static_cast<double>(SDLK_d)));
        module_env->define("K_e", NyxValue(static_cast<double>(SDLK_e)));
        module_env->define("K_f", NyxValue(static_cast<double>(SDLK_f)));
        module_env->define("K_g", NyxValue(static_cast<double>(SDLK_g)));
        module_env->define("K_h", NyxValue(static_cast<double>(SDLK_h)));
        module_env->define("K_i", NyxValue(static_cast<double>(SDLK_i)));
        module_env->define("K_j", NyxValue(static_cast<double>(SDLK_j)));
        module_env->define("K_k", NyxValue(static_cast<double>(SDLK_k)));
        module_env->define("K_l", NyxValue(static_cast<double>(SDLK_l)));
        module_env->define("K_m", NyxValue(static_cast<double>(SDLK_m)));
        module_env->define("K_n", NyxValue(static_cast<double>(SDLK_n)));
        module_env->define("K_o", NyxValue(static_cast<double>(SDLK_o)));
        module_env->define("K_p", NyxValue(static_cast<double>(SDLK_p)));
        module_env->define("K_q", NyxValue(static_cast<double>(SDLK_q)));
        module_env->define("K_r", NyxValue(static_cast<double>(SDLK_r)));
        module_env->define("K_s", NyxValue(static_cast<double>(SDLK_s)));
        module_env->define("K_t", NyxValue(static_cast<double>(SDLK_t)));
        module_env->define("K_u", NyxValue(static_cast<double>(SDLK_u)));
        module_env->define("K_v", NyxValue(static_cast<double>(SDLK_v)));
        module_env->define("K_w", NyxValue(static_cast<double>(SDLK_w)));
        module_env->define("K_x", NyxValue(static_cast<double>(SDLK_x)));
        module_env->define("K_y", NyxValue(static_cast<double>(SDLK_y)));
        module_env->define("K_z", NyxValue(static_cast<double>(SDLK_z)));
        module_env->define("K_UP", NyxValue(static_cast<double>(SDLK_UP)));
        module_env->define("K_DOWN", NyxValue(static_cast<double>(SDLK_DOWN)));
        module_env->define("K_LEFT", NyxValue(static_cast<double>(SDLK_LEFT)));
        module_env->define("K_RIGHT", NyxValue(static_cast<double>(SDLK_RIGHT)));
        
        return module_env;
    };
    interpreter.registerNativeModule("std:sdl", builder);
}

}