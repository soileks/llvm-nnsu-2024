#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "llvm/Support/raw_ostream.h"

class Visitor : public clang::RecursiveASTVisitor<Visitor> {
public:
  explicit Visitor(clang::ASTContext *Context) : Context(Context) {}
  bool VisitCXXRecordDecl(clang::CXXRecordDecl *Declaration) {
    llvm::outs() << Declaration->getNameAsString() << "\n";
    for (const auto &decl : Declaration->decls()) {
      if (clang::FieldDecl *field = clang::dyn_cast<clang::FieldDecl>(decl)) {
        llvm::outs() << "  |_" << field->getNameAsString() << "\n";
      } else if (clang::VarDecl *vard = clang::dyn_cast<clang::VarDecl>(decl)) {
        if (vard->isStaticDataMember()) {
          llvm::outs() << "  |_" << vard->getNameAsString() << "\n";
        }
      }
    }
    return true;
  }

private:
  clang::ASTContext *Context;
};

class Consumer : public clang::ASTConsumer {
public:
  explicit Consumer(clang::ASTContext *Context) : vis(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    vis.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  Visitor vis;
};

class Plugin : public clang::PluginASTAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::make_unique<Consumer>(&Compiler.getASTContext());
  }

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &args) {
    if (args.size() && args[0] == "help")
      PrintHelp(llvm::errs());
    return true;
  }
  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "This plugin prints the names of all classes and their fields\n";
  }
};

static clang::FrontendPluginRegistry::Add<Plugin>
    X("vetoshnikova-plugin-print-classes",
      "Printing the names of all classes and their fields");
