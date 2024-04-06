#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

class AddAlwaysInlineConsumer : public clang::ASTConsumer {
public:
  bool hasCondition(clang::Stmt *S) {
    if (!S) {
      return false;
    }

    if (llvm::isa<clang::IfStmt>(S) || llvm::isa<clang::SwitchStmt>(S) ||
        llvm::isa<clang::WhileStmt>(S) || llvm::isa<clang::DoStmt>(S) ||
        llvm::isa<clang::ForStmt>(S)) {
      return true;
    }

    for (clang::Stmt *Child : S->children()) {
      if (hasCondition(Child)) {
        return true;
      }
    }

    return false;
  }

  bool HandleTopLevelDecl(clang::DeclGroupRef D) override {
    clang::FunctionDecl *FD = nullptr;

    for (clang::Decl *Decl : D) {
      FD = clang::dyn_cast<clang::FunctionDecl>(Decl);
      if (FD) {
        if (!FD->hasAttr<clang::AlwaysInlineAttr>() &&
            !hasCondition(FD->getBody())) {
          FD->addAttr(
              clang::AlwaysInlineAttr::CreateImplicit(FD->getASTContext()));
          FD->print(llvm::outs() << "__attribute__((always_inline)) ");
        } else
          FD->print(llvm::outs());
      }
    }
    return true;
  }
};

class AddAlwaysInlineAction : public clang::PluginASTAction {
protected:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<AddAlwaysInlineConsumer>();
  }

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &Args) override {
    for (const std::string &arg : Args) {
      if (arg == "--help") {
        llvm::outs() << "This plugin adds the always_inline attribute to "
                        "functions if they do not have conditions!\n";
      } else
        llvm::outs()
            << "Use the --help argument to understand the plugin's purpose!\n";
    }
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<AddAlwaysInlineAction>
    X("AddAlwaysInline", "Adds always_inline to functions without conditions");
