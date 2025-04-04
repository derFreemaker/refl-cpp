#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <algorithm>
#include <unordered_set>

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")
#else
#include <regex>

#include <execinfo.h>
#include <cxxabi.h>
#include <unistd.h>
#include <dlfcn.h>
#include <libgen.h>  // for basename()
#endif

class StackTraceHelper {
public:
    static std::vector<std::string> captureStackTrace(
        int skipFrames = 1, 
        int maxDepth = 50,
        bool onlyAppFrames = true,
        const std::vector<std::string>& appModulePrefixes = {},
        bool excludeCRTStartup = true
    ) {
        std::vector<std::string> stackTraceResult;
        
        // Get the application name/path for filtering
        std::string appPath = getApplicationPath();
        std::string appName = getApplicationName(appPath);
        
        // Build a list of module prefixes to consider part of the application
        std::vector<std::string> modulePrefixes = appModulePrefixes;
        if (appName.length() > 0 && std::find(modulePrefixes.begin(), modulePrefixes.end(), appName) == modulePrefixes.end()) {
            modulePrefixes.push_back(appName);
        }
        
        // Patterns to exclude for CRT startup functions
        std::unordered_set<std::string> crtFunctions = {
            "invoke_main", 
            "mainCRTStartup", 
            "__scrt_common_main", 
            "__scrt_common_main_seh",
            "BaseThreadInitThunk",
            "RtlUserThreadStart",
            "_initterm",
            "_cinit"
        };
        
#ifdef _WIN32
        // Windows implementation
        HANDLE process = GetCurrentProcess();
        
        // Initialize symbol handler
        SymInitialize(process, NULL, TRUE);
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_UNDNAME);
        
        void* stack[100];
        WORD frames = CaptureStackBackTrace(skipFrames, maxDepth, stack, NULL);
        
        // Allocate space for the symbol info
        const int MAX_NAME_LENGTH = 256;
        SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + (MAX_NAME_LENGTH * sizeof(char)), 1);
        symbol->MaxNameLen = MAX_NAME_LENGTH;
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD displacement;
        
        // Track the frame index for user output
        int outputFrameIndex = 0;
        
        for (int i = 0; i < frames; i++) {
            // Get module information to determine if this is an app frame
            IMAGEHLP_MODULE64 moduleInfo;
            moduleInfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
            bool isAppFrame = !onlyAppFrames;  // If not filtering, include all frames
            
            // Get symbol info first to check if it's a CRT startup function
            SymFromAddr(process, (DWORD64)stack[i], 0, symbol);
            std::string functionName = symbol->Name;
            
            // Check if this is a CRT startup function to exclude
            if (excludeCRTStartup) {
                bool isCRTFunction = false;
                
                // Check if the function name matches any in our exclusion list
                for (const auto& crtFunc : crtFunctions) {
                    if (functionName.find(crtFunc) != std::string::npos) {
                        isCRTFunction = true;
                        break;
                    }
                }
                
                // Check if the file path contains vcstartup
                bool isStartupFile = false;
                if (SymGetLineFromAddr64(process, (DWORD64)stack[i], &displacement, &line)) {
                    std::string filePath = line.FileName;
                    if (filePath.find("vcstartup") != std::string::npos || 
                        filePath.find("crt") != std::string::npos) {
                        isStartupFile = true;
                    }
                }
                
                // Skip this frame if it's a CRT startup function
                if (isCRTFunction || isStartupFile) {
                    continue;
                }
            }
            
            std::string moduleName;
            if (SymGetModuleInfo64(process, (DWORD64)stack[i], &moduleInfo)) {
                moduleName = moduleInfo.ModuleName;
                
                // Check if this module is part of the application
                if (onlyAppFrames) {
                    // Check system DLLs to exclude
                    if (strstr(moduleInfo.ModuleName, "ntdll") == moduleInfo.ModuleName ||
                        strstr(moduleInfo.ModuleName, "KERNELBASE") == moduleInfo.ModuleName ||
                        strstr(moduleInfo.ModuleName, "kernel32") == moduleInfo.ModuleName ||
                        strstr(moduleInfo.ModuleName, "vcruntime") == moduleInfo.ModuleName ||
                        strstr(moduleInfo.ModuleName, "msvcrt") == moduleInfo.ModuleName ||
                        strstr(moduleInfo.ModuleName, "ucrtbase") == moduleInfo.ModuleName) {
                        continue;
                    }
                    
                    // If we're filtering for application modules, check the module prefixes
                    bool isIncludedModule = false;
                    for (const auto& prefix : modulePrefixes) {
                        if (_stricmp(moduleInfo.ModuleName, prefix.c_str()) == 0 || 
                            (strstr(moduleInfo.ModuleName, prefix.c_str()) == moduleInfo.ModuleName)) {
                            isIncludedModule = true;
                            break;
                        }
                    }
                    
                    if (!isIncludedModule) continue;
                }
            }
            
            std::stringstream ss;
            
            // Try to get file and line info
            if (SymGetLineFromAddr64(process, (DWORD64)stack[i], &displacement, &line)) {
                ss << "#" << outputFrameIndex << ": " << symbol->Name << " [" 
                   << line.FileName << ":" << line.LineNumber << "]";
            } else {
                ss << "#" << outputFrameIndex << ": " << symbol->Name << " in "
                   << moduleName << " at 0x" << std::hex << (DWORD64)stack[i];
            }
            
            stackTraceResult.push_back(ss.str());
            outputFrameIndex++;
        }
        
        free(symbol);
        SymCleanup(process);
#else
        // Linux implementation
        void* array[maxDepth];
        int size = backtrace(array, maxDepth);
        char** messages = backtrace_symbols(array, size);
        
        if (messages == nullptr) {
            stackTraceResult.push_back("Failed to get backtrace symbols");
            return stackTraceResult;
        }
        
        // Track the frame index for user output
        int outputFrameIndex = 0;
        
        // Patterns to exclude for Linux
        std::regex crtPattern("(__libc_start|_start|__cxa_|_dl_)");
        
        // Start from skipFrames to skip this function and any caller frames as requested
        for (int i = skipFrames; i < size; i++) {
            std::string message(messages[i]);
            
            // Check if this is a CRT/libc startup function to exclude
            if (excludeCRTStartup && std::regex_search(message, crtPattern)) {
                continue;
            }
            
            // Try to determine if this is an application frame
            bool isAppFrame = !onlyAppFrames;  // If not filtering, include all frames
            std::string moduleName;
            
            // Parse the module name from the output
            std::size_t moduleEnd = message.find('(');
            if (moduleEnd != std::string::npos) {
                moduleName = message.substr(0, moduleEnd);
                
                // Check if this is a system library to exclude
                if (moduleName.find("libc.so") != std::string::npos ||
                    moduleName.find("libpthread.so") != std::string::npos ||
                    moduleName.find("libstdc++.so") != std::string::npos ||
                    moduleName.find("ld-linux") != std::string::npos) {
                    if (onlyAppFrames) continue;
                }
                
                // Check if this module is part of the application
                if (onlyAppFrames) {
                    bool isIncludedModule = false;
                    
                    for (const auto& prefix : modulePrefixes) {
                        // Extract just the basename
                        std::string baseName = moduleName;
                        size_t lastSlash = baseName.find_last_of('/');
                        if (lastSlash != std::string::npos) {
                            baseName = baseName.substr(lastSlash + 1);
                        }
                        
                        if (baseName == prefix || 
                            baseName.find(prefix) == 0 || 
                            baseName.find("lib" + prefix) == 0) {
                            isIncludedModule = true;
                            break;
                        }
                    }
                    
                    if (!isIncludedModule && !moduleName.empty() && moduleName[0] != '[') continue;
                }
            }
            
            // Parse the message to extract function name if possible
            std::string functionName;
            void* addr = array[i];
            
            // Format is typically: module(function+offset) [address]
            std::string mangled;
            std::size_t begin = message.find('(');
            std::size_t end = message.find('+', begin);
            
            if (begin != std::string::npos && end != std::string::npos) {
                mangled = message.substr(begin + 1, end - begin - 1);
                
                int status;
                char* demangled = abi::__cxa_demangle(mangled.c_str(), nullptr, nullptr, &status);
                
                if (status == 0 && demangled) {
                    functionName = demangled;
                    free(demangled);
                } else {
                    functionName = mangled;
                }
                
                // Get source info if possible using dladdr
                Dl_info info;
                if (dladdr(addr, &info)) {
                    std::stringstream ss;
                    ss << "#" << outputFrameIndex << ": " << functionName;
                    
                    if (info.dli_sname) {
                        uintptr_t offset = (uintptr_t)addr - (uintptr_t)info.dli_saddr;
                        ss << " +" << offset;
                    }
                    
                    if (info.dli_fname) {
                        ss << " in " << info.dli_fname;
                    }
                    
                    stackTraceResult.push_back(ss.str());
                } else {
                    stackTraceResult.push_back("#" + std::to_string(outputFrameIndex) + ": " + 
                                              functionName + " in " + moduleName);
                }
            } else {
                // Fall back to the raw message
                stackTraceResult.push_back("#" + std::to_string(outputFrameIndex) + ": " + message);
            }
            
            outputFrameIndex++;
        }
        
        free(messages);
#endif
        
        return stackTraceResult;
    }
    
    static void printStackTrace(
        int skipFrames = 1, 
        int maxDepth = 50,
        bool onlyAppFrames = true,
        const std::vector<std::string>& appModulePrefixes = {},
        bool excludeCRTStartup = true
    ) {
        std::vector<std::string> trace = captureStackTrace(skipFrames + 1, maxDepth, onlyAppFrames, appModulePrefixes, excludeCRTStartup);
        for (const auto& frame : trace) {
            std::cout << frame << std::endl;
        }
    }

private:
    static std::string getApplicationPath() {
        std::string path;
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        path = buffer;
#else
        char buffer[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", buffer, PATH_MAX);
        if (count != -1) {
            path = std::string(buffer, count);
        }
#endif
        return path;
    }
    
    static std::string getApplicationName(const std::string& path) {
        if (path.empty()) return "";
        
#ifdef _WIN32
        // Extract filename (without extension) from path
        size_t lastSlash = path.find_last_of("\\/");
        size_t lastDot = path.find_last_of(".");
        
        if (lastSlash == std::string::npos) lastSlash = 0;
        else lastSlash++;
        
        if (lastDot == std::string::npos || lastDot < lastSlash) {
            return path.substr(lastSlash);
        }
        
        return path.substr(lastSlash, lastDot - lastSlash);
#else
        // Use basename to get the filename
        char* pathCopy = strdup(path.c_str());
        char* baseNamePtr = basename(pathCopy);
        std::string result = baseNamePtr;
        free(pathCopy);
        
        // Remove extension if present
        size_t lastDot = result.find_last_of(".");
        if (lastDot != std::string::npos) {
            result = result.substr(0, lastDot);
        }
        
        return result;
#endif
    }
};

// Example usage
void functionC() {
    // Print trace excluding CRT startup frames
    StackTraceHelper::printStackTrace(1, 50, true, {}, true);
}

void functionB() {
    functionC();
}

void functionA() {
    functionB();
}

int main() {
    // Default: print only application frames and exclude CRT startup frames
    functionA();
    
    std::cout << "\n--- With CRT startup frames ---\n";
    // Include CRT startup frames
    StackTraceHelper::printStackTrace(1, 50, true, {}, false);
    
    std::cout << "\n--- Only Executable Frames ---\n";
    StackTraceHelper::printStackTrace(1, 50, false, {}, true);
    
    return 0;
}