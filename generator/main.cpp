#include <clang/ExtractAPI/API.h>

#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Attr.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include <llvm/Support/raw_ostream.h>

static llvm::cl::OptionCategory ReflectCategory("reflect-tool options");

bool isMarked(const clang::TagDecl* Decl) {
    return std::any_of(Decl->attrs().begin(), Decl->attrs().end(), [](const auto* Attr) {
        if (const auto* Ann = dyn_cast<clang::AnnotateAttr>(Attr))
            if (Ann->getAnnotation() == "reflect")
                return true;

        return false;
    });
}

// Visitor that checks for REFLECT-marked classes/structs and enums.
// Define your REFLECT macro as:
//   #define REFLECT __attribute__((annotate("reflect")))
class ReflectVisitor : public clang::RecursiveASTVisitor<ReflectVisitor> {
public:
    explicit ReflectVisitor(clang::ASTContext* Context) : Context(Context) {}

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool VisitCXXRecordDecl(const clang::CXXRecordDecl* decl) {
        if (!decl->isCompleteDefinition() || !isMarked(decl))
            return true;

        llvm::outs() << "\nReflect: " << decl->getNameAsString();

        if (decl->isClass()) {
            llvm::outs() << " class";
        }
        else if (decl->isStruct()) {
            llvm::outs() << " struct";
        }

        if (decl->getNumBases() + decl->getNumVBases() > 0) {
            llvm::ListSeparator sep;
            llvm::outs() << " with bases: ";
            for (const auto& base : decl->bases()) {
                const clang::CXXRecordDecl* baseClass = base.getType()->getAsCXXRecordDecl();
                if (baseClass && isMarked(baseClass)) {
                    llvm::outs() << sep << baseClass->getNameAsString();
                }
            }
        }

        if (const auto enclosingContext = decl->getEnclosingNamespaceContext();
            enclosingContext->getDeclKind() == clang::Decl::Kind::Namespace) {
            const auto _namespace = clang::cast<clang::NamespaceDecl>(enclosingContext);
            llvm::outs() << " in namespace: " << _namespace->getNameAsString();
        }

        llvm::outs() << "\n";

        for (const auto* member : decl->decls()) {
            llvm::outs() << "  Member (" << member->getDeclKindName() << "): ";
            if (const auto* field = dyn_cast<clang::FieldDecl>(member)) {
                auto langOptions = clang::LangOptions();
                langOptions.Bool = true;

                llvm::outs() << field->getNameAsString() << " type: " << field->getType().getAsString(langOptions);
            }
            else if (const auto* method = dyn_cast<clang::CXXMethodDecl>(member)) {
                llvm::outs() << method->getNameAsString();
            }
            else if (const auto* inner = dyn_cast<clang::CXXRecordDecl>(member)) {
                llvm::outs() << inner->getQualifiedNameAsString();
            }
            else {
                llvm::outs() << member->getDeclKindName();
            }
            llvm::outs() << "\n";
        }

        return true;
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool VisitEnumDecl(const clang::EnumDecl* Decl) {
        if (!Decl->isCompleteDefinition() || !isMarked(Decl))
            return true;

        llvm::outs() << "\nReflect: " << Decl->getName() << " enum\n";

        for (const auto* enumerator : Decl->enumerators()) {
            llvm::outs() << "  Enumerator: " << enumerator->getNameAsString() << "\n";
        }

        return true;
    }

private:
    clang::ASTContext* Context;
};

class ReflectASTConsumer : public clang::ASTConsumer {
public:
    explicit ReflectASTConsumer(clang::ASTContext* Context)
        : Visitor(Context) {}

    void HandleTranslationUnit(clang::ASTContext& Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    ReflectVisitor Visitor;
};

class ReflectFrontendAction : public clang::ASTFrontendAction {
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
    auto OptionsParser = clang::tooling::CommonOptionsParser::create(argc, argv, ReflectCategory);
    if (!OptionsParser) {
        llvm::errs() << "Error: " << llvm::toString(OptionsParser.takeError()) << "\n";
        return 1;
    }
    clang::tooling::ClangTool Tool(OptionsParser->getCompilations(), OptionsParser->getSourcePathList());
    return Tool.run(clang::tooling::newFrontendActionFactory<ReflectFrontendAction>().get());
}
