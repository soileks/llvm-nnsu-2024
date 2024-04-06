#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/LLVM.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Sema/Sema.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>
#include <string>
#include <vector>
using namespace clang;

namespace {

class MyASTConsumer : public ASTConsumer {

public:
  void HandleTranslationUnit(ASTContext &Context) override {
    struct Visitor : public RecursiveASTVisitor<Visitor> {

      bool VisitCXXRecordDecl(CXXRecordDecl *CxxRDecl) {
        if (CxxRDecl->isClass() || CxxRDecl->isStruct()) {
          llvm::outs() << CxxRDecl->getNameAsString() << "\n";

          for (auto It = CxxRDecl->decls_begin(); It != CxxRDecl->decls_end();
               ++It)
            if (FieldDecl *NDecl = dyn_cast<FieldDecl>(*It))
              llvm::outs() << "|_ " << NDecl->getNameAsString() << "\n";
            else if (VarDecl *NDecl = dyn_cast<VarDecl>(*It))
              llvm::outs() << "|_ " << NDecl->getNameAsString() << "\n";

          llvm::outs() << "\n";
        }
        return true;
      }
    } V;
    V.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class PrintFunctionNamesAction : public PluginASTAction {
protected:
  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 llvm::StringRef) override {
    return std::make_unique<MyASTConsumer>();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    if (!Args.empty() && Args[0] == "help")
      PrintHelp(llvm::errs());
    return true;
  }

  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "Plugin prints names of classes and their fields\n";
  }
};

} // namespace

static FrontendPluginRegistry::Add<PrintFunctionNamesAction>
    X("classprinter", "prints names of classes and their fields");