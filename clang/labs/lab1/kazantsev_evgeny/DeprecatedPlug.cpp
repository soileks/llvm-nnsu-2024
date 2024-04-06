#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

class DeprecatedFuncName : public RecursiveASTVisitor<DeprecatedFuncName> {
public:
  explicit DeprecatedFuncName(
      ASTContext
          *Context) // get the AST tree context and initialize to "deprecated"
      : Context(Context), DeprecatedIdentifier("deprecated") {}

  bool VisitFunctionDecl(FunctionDecl *FD) {
    // Check if the function name contains the word "deprecated"
    std::string FuncName = FD->getNameAsString();
    std::transform(FuncName.begin(), FuncName.end(), FuncName.begin(),
                   ::tolower);
    if (FuncName.find(DeprecatedIdentifier) != std::string::npos) {
      // Emit a warning
      SourceLocation Loc = FD->getLocation();
      unsigned DiagID = Context->getDiagnostics().getCustomDiagID(
          DiagnosticsEngine::Warning, "Deprecated function name");
      Context->getDiagnostics().Report(Loc, DiagID);
    }
    return true;
  }

private:
  ASTContext *Context;
  std::string DeprecatedIdentifier;
};

class DeprecatedConsumer : public ASTConsumer {
public:
  explicit DeprecatedConsumer(ASTContext *Context) : Visitor(Context) {}

  // traversal from the root node
  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  DeprecatedFuncName Visitor;
};

class DeprecatedFuncAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<DeprecatedConsumer>(&CI.getASTContext());
  }
  bool ParseArgs(const CompilerInstance &Compiler,
                 const std::vector<std::string> &args) override {
    if (!args.empty() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }

  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "Deprecated Plugin version 1.0\n";
  }
};

// Register the plugin
static FrontendPluginRegistry::Add<DeprecatedFuncAction>
    X("deprecated-function", "check for deprecated function names");
