#pragma once

#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <memory>
#include <functional>
#include <map> 

namespace Nyx {

class Environment;        
class NyxDefinedFunction; 
struct NyxModuleData;     
class Interpreter;        

struct NyxSDLWindowWrapper;   
struct NyxSDLRendererWrapper; 
struct NyxSDLFontWrapper;
struct NyxSDLSurfaceWrapper;
struct NyxSDLTextureWrapper;

struct NyxStructDefinition; 
struct NyxStructInstance;  

struct NyxValueData;
using UserDefinedFunctionPtr = std::shared_ptr<NyxDefinedFunction>;
using NyxModule = std::shared_ptr<NyxModuleData>;
struct NyxNativeFunction; 
using NativeFunctionPtr = std::shared_ptr<NyxNativeFunction>;
using SDLWindowNyxPtr = std::shared_ptr<NyxSDLWindowWrapper>;
using SDLRendererNyxPtr = std::shared_ptr<NyxSDLRendererWrapper>;
using SDLFontNyxPtr = std::shared_ptr<NyxSDLFontWrapper>;
using SDLSurfaceNyxPtr = std::shared_ptr<NyxSDLSurfaceWrapper>;
using SDLTextureNyxPtr = std::shared_ptr<NyxSDLTextureWrapper>;
using StructDefinitionPtr = std::shared_ptr<NyxStructDefinition>;
using StructInstancePtr = std::shared_ptr<NyxStructInstance>;


struct NyxValueData {
    std::variant<
        std::monostate,
        bool,
        double,
        std::string,
        std::vector<NyxValueData>, 
        UserDefinedFunctionPtr,
        NyxModule,
        NativeFunctionPtr,
        SDLWindowNyxPtr,    
        SDLRendererNyxPtr,
        SDLFontNyxPtr,
        SDLSurfaceNyxPtr,
        SDLTextureNyxPtr,
        StructDefinitionPtr, 
        StructInstancePtr  
    > data;

    NyxValueData();
    NyxValueData(std::monostate val);
    NyxValueData(bool val);
    NyxValueData(double val);
    NyxValueData(const char* val);
    NyxValueData(const std::string& val);
    NyxValueData(std::string&& val);
    NyxValueData(const std::vector<NyxValueData>& val); 
    NyxValueData(std::vector<NyxValueData>&& val);    
    NyxValueData(const UserDefinedFunctionPtr& val);
    NyxValueData(UserDefinedFunctionPtr&& val);
    NyxValueData(const NyxModule& val);
    NyxValueData(NyxModule&& val);
    NyxValueData(const NativeFunctionPtr& val);
    NyxValueData(NativeFunctionPtr&& val);
    NyxValueData(const SDLWindowNyxPtr& val);   
    NyxValueData(SDLWindowNyxPtr&& val);      
    NyxValueData(const SDLRendererNyxPtr& val); 
    NyxValueData(SDLRendererNyxPtr&& val);    
    NyxValueData(const SDLFontNyxPtr& val);
    NyxValueData(SDLFontNyxPtr&& val);
    NyxValueData(const SDLSurfaceNyxPtr& val);
    NyxValueData(SDLSurfaceNyxPtr&& val);
    NyxValueData(const SDLTextureNyxPtr& val);
    NyxValueData(SDLTextureNyxPtr&& val);
    NyxValueData(const StructDefinitionPtr& val); 
    NyxValueData(StructDefinitionPtr&& val);    
    NyxValueData(const StructInstancePtr& val); 
    NyxValueData(StructInstancePtr&& val);    

    template<
        typename T,
        typename = std::enable_if_t<
            !std::is_same_v<std::decay_t<T>, NyxValueData> &&
            std::is_constructible_v<decltype(data), T>
        >
    >
    NyxValueData(T&& value);
};

using NyxValue = NyxValueData;
using NyxList = std::vector<NyxValue>; 
using NativeFunctionCallback = NyxValue (*)(Interpreter&, const std::vector<NyxValue>&);

struct NyxNativeFunction {
    std::string name;
    NativeFunctionCallback callback;
    int arity; 

    NyxNativeFunction(std::string n, NativeFunctionCallback cb, int ar) 
        : name(std::move(n)), callback(cb), arity(ar) {}
};

struct Token; 

struct NyxStructDefinition {
    std::string name;
    std::vector<std::string> field_names_in_order;
    std::map<std::string, size_t> field_indices;

    NyxStructDefinition(std::string n, const std::vector<Token>& field_tokens); 
};

struct NyxStructInstance {
    StructDefinitionPtr definition;
    std::vector<NyxValue> field_values;

    NyxStructInstance(StructDefinitionPtr def);
};


inline NyxValueData::NyxValueData() : data(std::monostate{}) {}
inline NyxValueData::NyxValueData(std::monostate val) : data(val) {}
inline NyxValueData::NyxValueData(bool val) : data(val) {}
inline NyxValueData::NyxValueData(double val) : data(val) {}
inline NyxValueData::NyxValueData(const char* val) : data(std::string(val)) {}
inline NyxValueData::NyxValueData(const std::string& val) : data(val) {}
inline NyxValueData::NyxValueData(std::string&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const std::vector<NyxValueData>& val) : data(val) {}
inline NyxValueData::NyxValueData(std::vector<NyxValueData>&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const UserDefinedFunctionPtr& val) : data(val) {}
inline NyxValueData::NyxValueData(UserDefinedFunctionPtr&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const NyxModule& val) : data(val) {}
inline NyxValueData::NyxValueData(NyxModule&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const NativeFunctionPtr& val) : data(val) {}
inline NyxValueData::NyxValueData(NativeFunctionPtr&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const SDLWindowNyxPtr& val) : data(val) {}
inline NyxValueData::NyxValueData(SDLWindowNyxPtr&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const SDLRendererNyxPtr& val) : data(val) {}
inline NyxValueData::NyxValueData(SDLRendererNyxPtr&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const SDLFontNyxPtr& val) : data(val) {}
inline NyxValueData::NyxValueData(SDLFontNyxPtr&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const SDLSurfaceNyxPtr& val) : data(val) {}
inline NyxValueData::NyxValueData(SDLSurfaceNyxPtr&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const SDLTextureNyxPtr& val) : data(val) {}
inline NyxValueData::NyxValueData(SDLTextureNyxPtr&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const StructDefinitionPtr& val) : data(val) {}
inline NyxValueData::NyxValueData(StructDefinitionPtr&& val) : data(std::move(val)) {}
inline NyxValueData::NyxValueData(const StructInstancePtr& val) : data(val) {}
inline NyxValueData::NyxValueData(StructInstancePtr&& val) : data(std::move(val)) {}


template<typename T, typename U>
NyxValueData::NyxValueData(T&& value) : data(std::forward<T>(value)) {}

std::string nyxValueToString(const NyxValue& value);
std::string nyxValueTypeToString(const NyxValue& value);
std::ostream& operator<<(std::ostream& os, const NyxValue& value);

}