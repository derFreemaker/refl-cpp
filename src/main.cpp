#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FormatVariadic.h>

#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <set>


// Structure to hold information about declarations
struct DeclInfo {
    std::string name;
    std::string kind;
    std::string location;
    std::string marker;
    
    // For sorting and deduplication
    bool operator<(const DeclInfo& other) const {
        return location < other.location;
    }
};

// Command line options
static llvm::cl::OptionCategory ToolCategory("AST Analyzer Options");
static llvm::cl::opt<bool> VerboseOutput("v", llvm::cl::desc("Enable verbose output"), llvm::cl::cat(ToolCategory));
static llvm::cl::opt<std::string> IncludePath("I", llvm::cl::desc("Include path"), llvm::cl::value_desc("path"), llvm::cl::cat(ToolCategory));
static llvm::cl::opt<std::string> MarkPattern("mark-pattern", llvm::cl::desc("Regex pattern for comment markers (default: MARK)"), 
                                             llvm::cl::init("MARK"), llvm::cl::cat(ToolCategory));

class MarkedDeclVisitor : public clang::RecursiveASTVisitor<MarkedDeclVisitor> {
public:
    explicit MarkedDeclVisitor(clang::ASTContext* context, std::set<DeclInfo>& decls, const std::string& markPattern)
        : astContext(context), declarations(decls), markerPattern(markPattern) {}

    // Helper function to check for marker comments
    bool hasMarkerComment(const clang::Decl* decl, std::string& markerText) {
        // Get the source manager
        clang::SourceManager& srcMgr = astContext->getSourceManager();
        
        // Get the source location
        clang::SourceLocation loc = decl->getLocation();
        if (loc.isInvalid()) {
            return false;
        }
        
        // Check if it's from a system header
        if (srcMgr.isInSystemHeader(loc)) {
            return false;
        }
        
        // Get the file location
        clang::FileID fileID = srcMgr.getFileID(loc);
        if (fileID.isInvalid()) {
            return false;
        }
        
        // Get the start of the line
        unsigned lineNo = srcMgr.getSpellingLineNumber(loc);
        
        // Check comments before the declaration
        for (int i = 1; i <= 5; ++i) { // Look up to 5 lines back
            unsigned prevLine = lineNo - i;
            if (prevLine < 1) break;
            
            clang::SourceLocation commentLoc = srcMgr.translateLineCol(fileID, prevLine, 1);
            if (commentLoc.isInvalid()) continue;
            
            // Get the line content
            bool invalid = false;
            llvm::StringRef lineContent = srcMgr.getBufferData(fileID, &invalid);
            if (invalid) continue;
            
            // Find the start of the line
            unsigned startOffset = srcMgr.getFileOffset(commentLoc);
            if (startOffset >= lineContent.size()) continue;
            
            // Extract just this line
            size_t endOfLine = lineContent.substr(startOffset).find('\n');
            llvm::StringRef line = lineContent.substr(startOffset, endOfLine);
            
            std::string lineStr = line.str();
            
            // Check for marker pattern in comments
            std::regex markRegex("//\\s*" + markerPattern + "|/\\*\\s*" + markerPattern + "\\s*\\*/");
            if (std::regex_search(lineStr, markRegex)) {
                // Extract the marker text
                std::regex extractPattern(markerPattern + ":?\\s*([^\\*\\n\\r]*)");
                std::smatch matches;
                if (std::regex_search(lineStr, matches, extractPattern) && matches.size() > 1) {
                    markerText = matches[1].str();
                } else {
                    markerText = markerPattern;
                }
                return true;
            }
        }
        
        return false;
    }
    
    // Helper function to check for marker attributes
    bool hasMarkerAttribute(const clang::Decl* decl, std::string& markerText) {
        if (const auto* annotateAttr = decl->getAttr<clang::AnnotateAttr>()) {
            llvm::StringRef annotation = annotateAttr->getAnnotation();
            if (annotation.compare("marked") == 0 || annotation.compare("mark") == 0 || 
                annotation.compare(markerPattern) == 0 || annotation.compare(markerPattern.c_str()) == 0) {
                markerText = annotation.str();
                return true;
            }
        }
        
        return false;
    }
    
    // Visit all record declarations (classes and structs)
    bool VisitCXXRecordDecl(clang::CXXRecordDecl* decl) {
        // For headers, we want to catch forward declarations too if marked
        if (!decl->getDeclName().isEmpty()) {
            processDecl(decl, decl->isClass() ? "class" : "struct");
        }
        return true;
    }
    
    // Visit enum declarations
    bool VisitEnumDecl(clang::EnumDecl* decl) {
        processDecl(decl, "enum");
        return true;
    }
    
private:
    clang::ASTContext* astContext;
    std::set<DeclInfo>& declarations;
    std::string markerPattern;
    
    void processDecl(clang::NamedDecl* decl, const std::string& kindStr) {
        // Skip if it has no name
        if (decl->getDeclName().isEmpty()) {
            return;
        }
        
        // Skip if it's in a system header
        if (astContext->getSourceManager().isInSystemHeader(decl->getLocation())) {
            return;
        }
        
        std::string markerText;
        if (hasMarkerComment(decl, markerText) || hasMarkerAttribute(decl, markerText)) {
            DeclInfo info;
            info.name = decl->getNameAsString();
            info.kind = kindStr;
            info.marker = markerText;
            
            // Get source location
            clang::SourceManager& srcMgr = astContext->getSourceManager();
            clang::SourceLocation loc = decl->getLocation();
            
            // Get the filename, fall back to the main file name if it's not available
            llvm::StringRef filename = srcMgr.getFilename(loc);
            if (filename.empty()) {
                filename = srcMgr.getFileEntryForID(srcMgr.getMainFileID())->tryGetRealPathName();
            }
            
            info.location = filename.str() + ":" + 
                            std::to_string(srcMgr.getSpellingLineNumber(loc)) + ":" +
                            std::to_string(srcMgr.getSpellingColumnNumber(loc));
                            
            declarations.insert(info);
        }
    }
};

class MarkedDeclConsumer : public clang::ASTConsumer {
public:
    explicit MarkedDeclConsumer(std::set<DeclInfo>& decls, const std::string& markPattern) 
        : declarations(decls), markerPattern(markPattern) {}
    
    virtual void HandleTranslationUnit(clang::ASTContext& context) override {
        MarkedDeclVisitor visitor(&context, declarations, markerPattern);
        visitor.TraverseDecl(context.getTranslationUnitDecl());
    }
    
private:
    std::set<DeclInfo>& declarations;
    std::string markerPattern;
};

class MarkedDeclAction : public clang::ASTFrontendAction {
public:
    MarkedDeclAction(std::set<DeclInfo>& decls, const std::string& markPattern) 
        : declarations(decls), markerPattern(markPattern) {}
    
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& compiler, 
                                                         llvm::StringRef file) override {
        // Enable all warnings
        compiler.getDiagnostics().setIgnoreAllWarnings(false);
        
        // Set language options for header files
        auto& langOpts = compiler.getLangOpts();
        langOpts.CPlusPlus = true;
        langOpts.CPlusPlus11 = true;
        langOpts.CPlusPlus14 = true;
        langOpts.CPlusPlus17 = true;
        
        return std::make_unique<MarkedDeclConsumer>(declarations, markerPattern);
    }
    
private:
    std::set<DeclInfo>& declarations;
    std::string markerPattern;
};

class MarkedDeclActionFactory : public clang::tooling::FrontendActionFactory {
public:
    MarkedDeclActionFactory(std::set<DeclInfo>& decls, const std::string& markPattern) 
        : declarations(decls), markerPattern(markPattern) {}
    
    std::unique_ptr<clang::FrontendAction> create() override {
        return std::make_unique<MarkedDeclAction>(declarations, markerPattern);
    }
    
private:
    std::set<DeclInfo>& declarations;
    std::string markerPattern;
};

int main(int argc, const char** argv) {
    // Parse command line options
    auto expectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!expectedParser) {
        llvm::errs() << "Error creating options parser: " << expectedParser.takeError() << "\n";
        return 1;
    }

    clang::tooling::CommonOptionsParser& optionsParser = expectedParser.get();
    
    // Print usage if no files are specified
    if (optionsParser.getSourcePathList().empty()) {
        llvm::errs() << "Usage: " << argv[0] << " [options] <source-file> [<source-file> ...]\n\n";
        llvm::errs() << "Options:\n";
        llvm::errs() << "  -v                         Enable verbose output\n";
        llvm::errs() << "  -I <path>                  Add include path\n";
        llvm::errs() << "  --mark-pattern=<pattern>   Set the regex pattern for comment markers (default: MARK)\n\n";
        llvm::errs() << "This program processes declarations that are marked with:\n";
        llvm::errs() << "  - A comment like //" << MarkPattern << " or /* " << MarkPattern << " */\n";
        llvm::errs() << "  - An attribute like [[clang::annotate(\"" << MarkPattern << "\")]] \n\n";
        llvm::errs() << "Example usage for header files:\n";
        llvm::errs() << "  " << argv[0] << " -I/usr/include -I/path/to/project/include header.h\n";
        return 1;
    }
    
    // Create the tool with our options
    clang::tooling::ClangTool tool(optionsParser.getCompilations(), optionsParser.getSourcePathList());
    
    // Add include paths and language options specific for header files
    std::vector<std::string> args;
    
    // Add standard includes
    args.push_back("-I/usr/include");
    args.push_back("-I/usr/local/include");
    
    // Add user-specified include path if provided
    if (!IncludePath.empty()) {
        args.push_back("-I" + IncludePath);
    }
    
    // Force C++ mode for header files
    args.push_back("-x");
    args.push_back("c++-header");
    
    // Enable C++17
    args.push_back("-std=c++17");
    
    // Suppress warnings about missing includes
    args.push_back("-Wno-pragma-once-outside-header");
    args.push_back("-Wno-everything");
    
    tool.appendArgumentsAdjuster(getInsertArgumentAdjuster(args, clang::tooling::ArgumentInsertPosition::END));
    
    // Container for our declarations
    std::set<DeclInfo> declarations;
    auto factory = std::make_unique<MarkedDeclActionFactory>(declarations, MarkPattern);
    
    // Run the tool
    int result = tool.run(factory.get());
    if (result != 0 && VerboseOutput) {
        llvm::errs() << "Note: Clang returned non-zero, but we'll still show any found declarations.\n";
    }
    
    // Print the results
    llvm::outs() << "\nFound " << declarations.size() << " marked declarations:\n\n";
    llvm::outs() << "TYPE       | NAME                      | MARKER                    | LOCATION\n";
    llvm::outs() << "-----------|---------------------------|---------------------------|--------------------------\n";
    
    for (const auto& decl : declarations) {
        llvm::outs() << llvm::formatv("%-10s | %-26s | %-26s | %s\n",
                                    decl.kind.c_str(),
                                    decl.name.c_str(),
                                    decl.marker.c_str(),
                                    decl.location.c_str());
    }
    
    return 0;
}