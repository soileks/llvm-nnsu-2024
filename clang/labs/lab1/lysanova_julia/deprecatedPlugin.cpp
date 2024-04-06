#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

class DeprecatedVisitor : public RecursiveASTVisitor<DeprecatedVisitor> {
private:
  ASTContext *Context;

public:
  explicit DeprecatedVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitFunctionDecl(FunctionDecl *Func) {
    if (Func->getNameInfo().getAsString().find("deprecated") !=
        std::string::npos) {
      DiagnosticsEngine &Diags = Context->getDiagnostics();
      size_t CustomDiagID = Diags.getCustomDiagID(
          DiagnosticsEngine::Warning, "Function have deprecated in its name!");
      Diags.Report(Func->getLocation(), CustomDiagID)
          << Func->getNameInfo().getAsString();
    }
    return true;
  }
};

class DeprecatedConsumer : public ASTConsumer {
public:
  void HandleTranslationUnit(ASTContext &Context) override {
    DeprecatedVisitor Visitor(&Context);
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class DeprecatedPlugin : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<DeprecatedConsumer>();
  }

  bool ParseArgs(const CompilerInstance &Compiler,
                 const std::vector<std::string> &Args) override {
    if (!Args.empty() && Args[0] == "help") {
      Help(llvm::errs());
    }
    return true;
  }

  void Help(llvm::raw_ostream &ros) {
    ros << "This plugin throws warning if func name contains 'deprecated'\n";
  }
};

static FrontendPluginRegistry::Add<DeprecatedPlugin>
    X("depWarning",
      "Plugin that throws warning if func name contains 'deprecated'");