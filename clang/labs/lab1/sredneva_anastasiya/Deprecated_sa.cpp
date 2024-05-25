#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

class DeprecatedVisitor : public RecursiveASTVisitor<DeprecatedVisitor> {
public:
  explicit DeprecatedVisitor(bool isWarning) : isWarning(isWarning) {}
  bool VisitFunctionDecl(FunctionDecl *fDecl) {
    if (fDecl->getNameInfo().getAsString().find("deprecated") !=
        std::string::npos) {
      DiagnosticsEngine &diagn = fDecl->getASTContext().getDiagnostics();
      unsigned diagnID;
      diagnID = diagn.getCustomDiagID(isWarning ? DiagnosticsEngine::Warning
                                                : DiagnosticsEngine::Error,
                                      "The function name has 'deprecated'");
      diagn.Report(fDecl->getLocation(), diagnID)
          << fDecl->getNameInfo().getAsString();
    }
    return true;
  }

private:
  bool isWarning;
};

class DeprecatedConsumer : public ASTConsumer {
public:
  explicit DeprecatedConsumer(bool isWarning) : isWarning(isWarning) {}
  void HandleTranslationUnit(ASTContext &Context) override {
    DeprecatedVisitor Visitor(isWarning);
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  bool isWarning;
};

class DeprecatedAction : public PluginASTAction {
  bool isWarning = true;

protected:
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<DeprecatedConsumer>(isWarning);
  }
  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (const auto &arg : args) {
      if (arg == "-err") {
        isWarning = false;
      }
    }
    return true;
  }
};

static FrontendPluginRegistry::Add<DeprecatedAction>
    X("deprecated_plugin",
      "adds warning if there is a 'deprecated' in the function name");
