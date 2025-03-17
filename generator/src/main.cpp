#include <clang/ExtractAPI/API.h>
#include <clang/AST/AST.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/AST/Attr.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/Tooling.h>
#include <llvm/Support/raw_ostream.h>

#include "reflect_data.hpp"
#include "generate_reflect_data.hpp"

static llvm::cl::OptionCategory ReflectCategory("reflect-tool options");

std::vector<ReflCpp::ReflectData> G_ReflectData;

bool isMarked(const clang::TagDecl* Decl) {
    return std::any_of(Decl->attrs().begin(), Decl->attrs().end(), [](const auto* Attr) {
        if (const auto* Ann = dyn_cast<clang::AnnotateAttr>(Attr))
            if (Ann->getAnnotation() == "reflect")
                return true;

        return false;
    });
}

std::string findIncludePath(const std::string& absolutePath, const std::vector<std::string>& includeDirs) {
    const std::string normalizedPath = llvm::sys::path::convert_to_slash(absolutePath);

    for (const auto& includeDir : includeDirs) {
        std::string normalizedIncludeDir = llvm::sys::path::convert_to_slash(includeDir);
        if (!normalizedIncludeDir.empty() && normalizedIncludeDir.back() != '/')
            normalizedIncludeDir += '/';

        if (normalizedPath.find(normalizedIncludeDir) == 0) {
            return normalizedPath.substr(normalizedIncludeDir.length());
        }
    }

    // If we can't find it in include directories, return the filename only
    return llvm::sys::path::filename(normalizedPath).str();
}

class ReflectVisitor final : public clang::RecursiveASTVisitor<ReflectVisitor> {
public:
    explicit ReflectVisitor(clang::ASTContext* Context) : Context(Context) {}

    // ReSharper disable once CppDFAConstantFunctionResult
    bool VisitCXXRecordDecl(const clang::CXXRecordDecl* decl) const {
        if (!decl->isCompleteDefinition() || !isMarked(decl))
            return true;

        auto& data = G_ReflectData.emplace_back();
        data.qualified_name = decl->getQualifiedNameAsString();

        data.name = decl->getNameAsString();

        if (const auto& _namespace = llvm::dyn_cast<clang::NamespaceDecl>(decl->getEnclosingNamespaceContext())) {
            data._namespace = _namespace->getQualifiedNameAsString();
        }

        clang::SourceLocation loc = decl->getLocation();
        clang::SourceManager& src_manager = decl->getASTContext().getSourceManager();
        if (loc.isValid()) {
            const clang::FileID fileID = src_manager.getFileID(loc);
            if (const clang::FileEntry* fileEntry = src_manager.getFileEntryForID(fileID)) {
                data.source_file = fileEntry->tryGetRealPathName().str();
            }
        }
        
        const clang::SourceManager& SM = Context->getSourceManager();
        if (const clang::SourceLocation Loc = decl->getLocation(); Loc.isValid()) {
            const clang::FileID FID = SM.getFileID(Loc);
            if (const clang::FileEntry* FileEntry = SM.getFileEntryForID(FID)) {
                data.source_file = src_manager.getFilename(loc).str();
            }
        }

        if (decl->getNumBases() + decl->getNumVBases() > 0) {
            for (const auto& base : decl->bases()) {
                if (const clang::CXXRecordDecl* baseClass = base.getType()->getAsCXXRecordDecl(); baseClass && isMarked(baseClass)) {
                    data.bases.push_back(baseClass->getQualifiedNameAsString());
                }
            }
        }

        for (const auto* member : decl->decls()) {
            if (const auto* field = dyn_cast<clang::FieldDecl>(member)) {
                auto& field_data = data.fields.emplace_back();
                field_data.qualified_name = field->getQualifiedNameAsString();

                field_data.name = field->getNameAsString();
            }
            else if (const auto* method = dyn_cast<clang::CXXMethodDecl>(member)) {
                if (isa<clang::CXXConstructorDecl>(method) || isa<clang::CXXDestructorDecl>(method))
                    continue;

                auto& method_data = data.methods.emplace_back();
                method_data.qualified_name = method->getQualifiedNameAsString();

                method_data.name = method->getNameAsString();

                for (const auto& arg : method->parameters()) {
                    method_data.arg_names.push_back(arg->getNameAsString());
                }
            }
        }

        return true;
    }

private:
    clang::ASTContext* Context;
};

class ReflectASTConsumer final : public clang::ASTConsumer {
public:
    explicit ReflectASTConsumer(clang::ASTContext* Context)
        : Visitor(Context) {}

    void HandleTranslationUnit(clang::ASTContext& Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    ReflectVisitor Visitor;
};

class ReflectFrontendAction final : public clang::ASTFrontendAction {
public:
    bool BeginSourceFileAction(clang::CompilerInstance& CI) override {
        // Use an IgnoringDiagConsumer to drop all diagnostics.
        CI.getDiagnostics().setClient(new clang::IgnoringDiagConsumer(), /*ShouldOwnClient=*/true);
        return ASTFrontendAction::BeginSourceFileAction(CI);
    }

    std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& CI,
                                                          clang::StringRef InFile) override {
        return std::make_unique<ReflectASTConsumer>(&CI.getASTContext());
    }
};

int main(int argc, const char** argv) {
    static llvm::cl::opt<std::string> OutputDir("o",
                                                llvm::cl::desc("Output directory for generated reflection data"),
                                                llvm::cl::value_desc("directory"),
                                                llvm::cl::init("refl-cpp.generated"));

    llvm::cl::list<std::string> IncludeDirs("I",
                                            llvm::cl::desc("Include search path"),
                                            llvm::cl::value_desc("directory"),
                                            llvm::cl::ZeroOrMore);

    auto OptionsParser = clang::tooling::CommonOptionsParser::create(argc, argv, ReflectCategory);
    if (!OptionsParser) {
        llvm::errs() << "Error: " << llvm::toString(OptionsParser.takeError()) << "\n";
        return 1;
    }
    clang::tooling::ClangTool Tool(OptionsParser->getCompilations(), OptionsParser->getSourcePathList());
    const auto clang_result = Tool.run(clang::tooling::newFrontendActionFactory<ReflectFrontendAction>().get());

    if (clang_result != 0) {
        //TODO: decide if we want to stop if the code has any error
        printf("clang didn't finish with error\n");
    }

    // ReflCpp::GenerateReflectionCode(OutputDir, G_ReflectData);

    printf("%llu\n", IncludeDirs.size());
}
