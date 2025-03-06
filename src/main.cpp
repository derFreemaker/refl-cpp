#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include <iostream>

using namespace clang::ast_matchers;
using namespace clang::tooling;

// Define a command line option category for our tool
static llvm::cl::OptionCategory MatcherCategory("Annotation Matcher Options");

class MarkerFinder : public MatchFinder::MatchCallback {
public:
    void run(const MatchFinder::MatchResult &Result) override {
        if (const auto *RD = Result.Nodes.getNodeAs<clang::RecordDecl>("record")) {
            llvm::outs() << "Found annotated declaration: " << RD->getNameAsString() << "\n";
        }
    }
};

int main(int argc, const char **argv) {
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, MatcherCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());

    MarkerFinder FinderCallback;
    MatchFinder Finder;
    // Match record declarations (structs/classes) with our annotation.
    auto Matcher = recordDecl(hasAttr(clang::attr::Annotate)).bind("record");
    Finder.addMatcher(Matcher, &FinderCallback);

    return Tool.run(newFrontendActionFactory(&Finder).get());
}
