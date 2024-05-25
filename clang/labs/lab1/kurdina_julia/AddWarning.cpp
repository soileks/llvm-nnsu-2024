#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

using namespace clang;

class AddWarningConsumer : public ASTConsumer {
  CompilerInstance &Instance;
  bool withoutClass;

public:
  AddWarningConsumer(CompilerInstance &Instance, bool withoutClass)
      : Instance(Instance), withoutClass(withoutClass) {}

  void HandleTranslationUnit(ASTContext &context) override {

    struct Visitor : public RecursiveASTVisitor<Visitor> {
      ASTContext *context;
      bool withoutClass;
      Visitor(ASTContext *context, bool withoutClass)
          : context(context), withoutClass(withoutClass) {}

      bool VisitFunctionDecl(FunctionDecl *FD) {
        if (!withoutClass || !FD->isCXXClassMember()) {
          std::string name = FD->getNameInfo().getAsString();
          if (name.find("deprecated") != std::string::npos) {
            DiagnosticsEngine &diag = context->getDiagnostics();
            unsigned diagID = diag.getCustomDiagID(
                DiagnosticsEngine::Warning, "Function or method is deprecated");
            SourceLocation location = FD->getLocation();
            diag.Report(location, diagID);
          }
        }
        return true;
      }
    } v(&Instance.getASTContext(), withoutClass);

    v.TraverseDecl(context.getTranslationUnitDecl());
  }
};

class AddWarningAction : public PluginASTAction {
protected:
  bool withoutClass = false;
  std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &CI, llvm::StringRef InFile) override {
    return std::make_unique<AddWarningConsumer>(CI, withoutClass);
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    for (const auto &arg : args) {
      if (arg == "-notCheckClass") {
        withoutClass = true;
      }
    }
    return true;
  }
};

static FrontendPluginRegistry::Add<AddWarningAction> X("warn_dep", "warn_dep");
