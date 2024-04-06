#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

class PrintClassVisitor : public clang::RecursiveASTVisitor<PrintClassVisitor> {
public:
  explicit PrintClassVisitor(clang::ASTContext *Context) : Context(Context) {}

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl) {
    llvm::outs() << decl->getNameAsString() << "\n";
    for (clang::Decl *d : decl->decls()) {
      if (auto *member = clang::dyn_cast<clang::FieldDecl>(d)) {
        llvm::outs() << "|_" << member->getNameAsString() << "\n";
      } else if (auto *staticMember = clang::dyn_cast<clang::VarDecl>(d)) {
        if (staticMember->isStaticDataMember()) {
          llvm::outs() << "|_" << staticMember->getNameAsString() << "\n";
        }
      }
    }
    return true;
  }

private:
  clang::ASTContext *Context;
};

class PrintClassConsumer : public clang::ASTConsumer {
public:
  explicit PrintClassConsumer(clang::ASTContext *Context) : Visitor(Context) {}

  void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  PrintClassVisitor Visitor;
};

class PrintClassPlugin : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<PrintClassConsumer>(&Compiler.getASTContext());
  }

protected:
  bool ParseArgs(const clang::CompilerInstance &Compiler,
                 const std::vector<std::string> &args) override {
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<PrintClassPlugin>
    X("print-class", "Prints description of class.");
