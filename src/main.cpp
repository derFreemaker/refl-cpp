#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Attr.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include <llvm/Support/raw_ostream.h>

using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory ReflectCategory("reflect-tool options");

bool IsMarked(const TagDecl* Decl) {
    return std::any_of(Decl->attrs().begin(), Decl->attrs().end(), [](const auto* Attr) {
        if (const auto* Ann = dyn_cast<AnnotateAttr>(Attr))
            if (Ann->getAnnotation() == "reflect")
                return true;

        return false;
    });
}

// Visitor that checks for REFLECT-marked classes/structs and enums.
// Define your REFLECT macro as:
//   #define REFLECT __attribute__((annotate("reflect")))
class ReflectVisitor : public RecursiveASTVisitor<ReflectVisitor> {
public:
    explicit ReflectVisitor(ASTContext* Context) : Context(Context) {}

    // ReSharper disable once CppDFAConstantFunctionResult
    static bool VisitCXXRecordDecl(const CXXRecordDecl* Decl) {
        if (!Decl->isCompleteDefinition() || !IsMarked(Decl))
            return true;

        llvm::outs() << "Reflect: " << Decl->getNameAsString();

        if (Decl->isClass()) {
            llvm::outs() << " class";
        }
        else if (Decl->isStruct()) {
            llvm::outs() << " struct";
        }

        if (const auto enclosing_context = Decl->getEnclosingNamespaceContext(); enclosing_context->getDeclKind() == Decl::Kind::Namespace) {
            const auto _namespace = clang::cast<clang::NamespaceDecl>(enclosing_context);
            llvm::outs() << " in namespace: " << _namespace->getNameAsString();
        }
        
        llvm::outs() << "\n";

        for (const auto* member : Decl->decls()) {
            llvm::outs() << "  Member (" << member->getDeclKindName() << "): ";
            if (const auto* field = dyn_cast<FieldDecl>(member)) {
                auto lang_options = LangOptions();
                lang_options.Bool = true;

                llvm::outs() << field->getNameAsString() << " type: " << field->getType().getAsString(lang_options);
            }
            else if (const auto* method = dyn_cast<CXXMethodDecl>(member)) {
                llvm::outs() << method->getNameAsString();
            }
            else if (const auto* inner = dyn_cast<CXXRecordDecl>(member)) {
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
    static bool VisitEnumDecl(const EnumDecl* Decl) {
        if (!Decl->isCompleteDefinition() || !IsMarked(Decl))
            return true;

        llvm::outs() << "Reflect: " << Decl->getName() << " enum\n";

        for (const auto* enumerator : Decl->enumerators()) {
            llvm::outs() << "  Enumerator: " << enumerator->getNameAsString() << "\n";
        }

        return true;
    }

private:
    ASTContext* Context;
};

class ReflectASTConsumer : public ASTConsumer {
public:
    explicit ReflectASTConsumer(ASTContext* Context)
        : Visitor(Context) {}

    void HandleTranslationUnit(ASTContext& Context) override {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }

private:
    ReflectVisitor Visitor;
};

class ReflectFrontendAction : public ASTFrontendAction {
public:
    bool BeginSourceFileAction(CompilerInstance& CI) override {
        // Use an IgnoringDiagConsumer to drop all diagnostics.
        CI.getDiagnostics().setClient(new IgnoringDiagConsumer(), /*ShouldOwnClient=*/true);
        return ASTFrontendAction::BeginSourceFileAction(CI);
    }

    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& CI,
                                                   StringRef InFile) override {
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
