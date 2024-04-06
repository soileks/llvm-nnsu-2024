#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

class PrintClassesVisitor
    : public clang::RecursiveASTVisitor<PrintClassesVisitor> {
public:
  bool VisitCXXRecordDecl(clang::CXXRecordDecl *decl) {
    llvm::outs() << decl->getNameAsString() << "\n";

    for (auto field : decl->fields()) {
      llvm::outs() << "|_" << field->getNameAsString() << "\n";
    }
    return true;
  }

  bool VisitCXXConstructorDecl(clang::CXXConstructorDecl *decl) {
    llvm::outs() << "|_" << decl->getNameAsString() << "\n";
    return true;
  }
  bool VisitVarDecl(clang::VarDecl *decl) {
    llvm::outs() << "|_" << decl->getNameAsString() << "\n";
    return true;
  }
};

class PrintClassesConsumer : public clang::ASTConsumer {
public:
  void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  PrintClassesVisitor Visitor;
};

class PrintClassesPlugin : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<PrintClassesConsumer>();
  }

protected:
  bool ParseArgs(const clang::CompilerInstance &Compiler,
                 const std::vector<std::string> &Args) override {
    for (const std::string &arg : Args) {
      if (arg == "--help") {
        llvm::outs() << "Help text";
      }
    }
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<PrintClassesPlugin>
    X("print-classes", "Prints description of class.");