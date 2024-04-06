#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/ADT/StringRef.h"
#include <algorithm>
#include <cctype>
#include <memory>
#include <string>
#include <vector>

bool CaseSensitiveSearch = true;

bool contains_ci(std::string const &text, std::string const &substr) {
  if (substr.length() > text.length())
    return false;

  auto it = std::search(text.begin(), text.end(), substr.begin(), substr.end(),
                        [](char ch1, char ch2) {
                          return std::toupper(ch1) == std::toupper(ch2);
                        });

  return it != text.end();
}

class DeprecationWarnConsumer : public clang::ASTConsumer {
public:
  void HandleTranslationUnit(clang::ASTContext &Context) override {
    struct DeprecationWarnVisitor
        : public clang::RecursiveASTVisitor<DeprecationWarnVisitor> {
      clang::ASTContext &Context;

    public:
      explicit DeprecationWarnVisitor(clang::ASTContext &Context)
          : Context(Context) {}

      bool VisitFunctionDecl(clang::FunctionDecl *Func) {
        bool check = CaseSensitiveSearch
                         ? Func->getNameAsString().find("deprecated") !=
                               std::string::npos
                         : contains_ci(Func->getNameAsString(), "deprecated");
        if (check) {
          clang::DiagnosticsEngine &Diags = Context.getDiagnostics();
          unsigned DiagID =
              Diags.getCustomDiagID(clang::DiagnosticsEngine::Warning,
                                    "Deprecated function found here");
          Diags.Report(Func->getLocation(), DiagID);
        }
        return true;
      }
    } Visitor(Context);
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class DeprecationWarnPlugin : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<DeprecationWarnConsumer>();
  }

protected:
  bool ParseArgs(const clang::CompilerInstance &Compiler,
                 const std::vector<std::string> &args) override {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      if (args[i] == "insensitive") {
        CaseSensitiveSearch = false;
      }
    }
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<DeprecationWarnPlugin>
    X("deprecation-plugin", "Finds deprecated functions");