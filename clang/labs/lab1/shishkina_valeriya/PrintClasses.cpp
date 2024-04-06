#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"

class PrintClassesVisitor
    : public clang::RecursiveASTVisitor<PrintClassesVisitor> {
public:
  explicit PrintClassesVisitor(clang::ASTContext *Context) : Context(Context) {}

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *Res) {
    llvm::outs() << Res->getNameAsString() << "\n";

    for (clang::Decl *decl : Res->decls()) {
      if (auto *field = clang::dyn_cast<clang::FieldDecl>(decl)) {
        llvm::outs() << "  |_ " << field->getNameAsString() << "\n";
      } else if (auto *staticField = clang::dyn_cast<clang::VarDecl>(decl)) {
        if (staticField->isStaticDataMember()) {
          llvm::outs() << "  |_ " << staticField->getNameAsString() << "\n";
        }
      }
    }
    return true;
  }

private:
  clang::ASTContext *Context;
};

class PrintClassesConsumer : public clang::ASTConsumer {
public:
  explicit PrintClassesConsumer(clang::ASTContext *Context)
      : Visitor(Context) {}

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
    return std::make_unique<PrintClassesConsumer>(&Compiler.getASTContext());
  }

protected:
  bool ParseArgs(const clang::CompilerInstance &Compiler,
                 const std::vector<std::string> &Args) override {
    for (const std::string &arg : Args) {
      if (arg == "--help") {
        llvm::outs()
            << "This plugin displays the name of the class and it's fields.\n";
      }
    }
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<PrintClassesPlugin>
    X("print-classes", "Prints classes description.");