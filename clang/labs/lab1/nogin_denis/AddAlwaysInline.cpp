#include "clang/AST/ASTContext.h"
#include "clang/AST/Attr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

class AlwaysInlineVisitor
    : public clang::RecursiveASTVisitor<AlwaysInlineVisitor> {
private:
  clang::ASTContext *Context;
  bool HasConditional;

public:
  explicit AlwaysInlineVisitor(clang::ASTContext *Context)
      : Context(Context), HasConditional(false) {}

  bool VisitFunctionDecl(clang::FunctionDecl *Func) {
    HasConditional = false;
    TraverseStmt(Func->getBody());

    if (!Func->hasAttr<clang::AlwaysInlineAttr>()) {
      if (!HasConditional) {
        clang::SourceRange Range = Func->getSourceRange();
        Func->addAttr(clang::AlwaysInlineAttr::CreateImplicit(*Context, Range));
      }
    }
    Func->dump();
    return true;
  }

  bool VisitIfStmt(clang::IfStmt *IfStatement) {
    HasConditional = true;
    return true;
  }

  bool VisitSwitchStmt(clang::SwitchStmt *SwitchStatement) {
    HasConditional = true;
    return true;
  }

  bool VisitWhileStmt(clang::WhileStmt *WhileStatement) {
    HasConditional = true;
    return true;
  }

  bool VisitForStmt(clang::ForStmt *ForStatement) {
    HasConditional = true;
    return true;
  }
};

class AlwaysInlineConsumer : public clang::ASTConsumer {
private:
  AlwaysInlineVisitor Visitor;

public:
  AlwaysInlineConsumer(clang::ASTContext *Context) : Visitor(Context) {}

  void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class AlwaysInlinePlugin : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef) override {
    return std::make_unique<AlwaysInlineConsumer>(&Compiler.getASTContext());
  }
  bool ParseArgs(const clang::CompilerInstance &Compiler,
                 const std::vector<std::string> &Args) override {
    for (const std::string &Arg : Args) {
      if (Arg == "--help") {
        llvm::outs() << "This plugin adds an __attribute__((always_inline)) to "
                        "functions without any conditions\n";
      }
    }
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<AlwaysInlinePlugin>
    X("add-always-inline", "Automatically adds __attribute__((always_inline)) "
                           "to functions without conditional statements.");