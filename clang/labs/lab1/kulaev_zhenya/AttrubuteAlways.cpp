#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Attr.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include <stack>

class AlwaysInlineVisitor
    : public clang::RecursiveASTVisitor<AlwaysInlineVisitor> {
public:
  AlwaysInlineVisitor(clang::ASTContext *MyContext) : MyContext(MyContext) {}

  bool VisitFunctionDecl(clang::FunctionDecl *Func) {
    bool ContainsConditional = false;
    std::stack<clang::Stmt *> stack;
    stack.push(Func->getBody());

    while (!stack.empty()) {
      clang::Stmt *CurrentNode = stack.top();
      stack.pop();

      if (clang::isa<clang::IfStmt>(CurrentNode) ||
          clang::isa<clang::SwitchStmt>(CurrentNode) ||
          clang::isa<clang::ForStmt>(CurrentNode) ||
          clang::isa<clang::WhileStmt>(CurrentNode) ||
          clang::isa<clang::DoStmt>(CurrentNode)) {
        ContainsConditional = true;
        break;
      }

      if (auto parent = clang::dyn_cast<clang::CompoundStmt>(CurrentNode)) {
        for (auto Child : parent->body()) {
          stack.push(Child);
        }
      }
    }

    if (!ContainsConditional) {
      clang::SourceRange FuncRange = Func->getSourceRange();
      Func->addAttr(
          clang::AlwaysInlineAttr::CreateImplicit(*MyContext, FuncRange));
    }

    return true;
  }

private:
  clang::ASTContext *MyContext;
};

class AlwaysInlineConsumer : public clang::ASTConsumer {
public:
  AlwaysInlineConsumer(clang::ASTContext *MyContext) : MyVisitor(MyContext) {}

  void HandleTranslationUnit(clang::ASTContext &MyContext) override {
    MyVisitor.TraverseDecl(MyContext.getTranslationUnitDecl());
  }

private:
  AlwaysInlineVisitor MyVisitor;
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
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<AlwaysInlinePlugin>
    X("always_inlines-plugin",
      "Print a function without conditions with an attribute");
