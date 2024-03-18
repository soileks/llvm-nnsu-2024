#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

class WarningDepricatedVisitor
    : public RecursiveASTVisitor<WarningDepricatedVisitor> {
private:
  ASTContext *Context;

public:
  explicit WarningDepricatedVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitFunctionDecl(FunctionDecl *F) {

    std::string FuncName = F->getNameInfo().getAsString();
    const std::string DeprecatedKeyword = "deprecated";

    if ((FuncName.find(DeprecatedKeyword) != std::string::npos)) {

      DiagnosticsEngine &Diagnostics = Context->getDiagnostics();
      unsigned WarnId = Diagnostics.getCustomDiagID(
          DiagnosticsEngine::Warning,
          "Function '%0' contains 'deprecated' in its name");

      Diagnostics.Report(F->getLocation(), WarnId) << FuncName;
    }
    return true;
  }
};

class WarningDepricatedConsumer : public ASTConsumer {
private:
  WarningDepricatedVisitor Visitor;

public:
  explicit WarningDepricatedConsumer(CompilerInstance &CI)
      : Visitor(&CI.getASTContext()) {}

  void HandleTranslationUnit(ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class WarningDeprecatedPlugin : public clang::PluginASTAction {
public:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::unique_ptr<ASTConsumer>(
        new WarningDepricatedConsumer(Compiler));
  }

protected:
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    for (unsigned I = 0, E = Args.size(); I != E; ++I) {
      if (Args[I] == "help") {
        llvm::errs() << "#Clang Plugin Help : WarningDeprecatedPlugin\n\n";
        llvm::errs() << "##Description\n";
        llvm::errs() << "This plugin adds a warning if there is a function "
                        "whose name contains 'deprecated'\n";
        llvm::errs() << "##Version\n";
        llvm::errs() << "Version 1.1\n\n";
      }
    }
    return true;
  }
};

static FrontendPluginRegistry::Add<WarningDeprecatedPlugin>
    X("WarningDeprecatedPlugin",
      "Outputs a warning if there is a function with 'deprecated' in its name");
