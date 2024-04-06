#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/ADT/Sequence.h"

class RenameVisitor : public clang::RecursiveASTVisitor<RenameVisitor> {
private:
  clang::Rewriter Rewriter;
  std::string OldName;
  std::string NewName;

  void inline renameNameAndType(std::string &OldName, std::string &NewName,
                                std::string TypeName,
                                clang::DeclaratorDecl *Decl,
                                const clang::TypeLoc &TypeLoc) {
    std::string Name = Decl->getNameAsString();
    if (Name == OldName) {
      Rewriter.ReplaceText(Decl->getLocation(), Name.size(), NewName);
    }
    if (TypeName == OldName || TypeName == OldName + " *" ||
        TypeName == OldName + " &") {
      Rewriter.ReplaceText(TypeLoc.getBeginLoc(), OldName.size(), NewName);
    }
  }

public:
  explicit RenameVisitor(clang::Rewriter Rewriter, std::string OldName,
                         std::string NewName)
      : Rewriter(Rewriter), OldName(OldName), NewName(NewName){};

  bool VisitFunctionDecl(clang::FunctionDecl *Func) {
    renameNameAndType(OldName, NewName, Func->getReturnType().getAsString(),
                      Func, Func->getFunctionTypeLoc().getAs<clang::TypeLoc>());

    if (!Func->param_empty()) {
      for (auto IVar = Func->param_begin(); IVar != Func->param_end(); IVar++) {
        auto Var = static_cast<clang::VarDecl *>(*IVar);
        auto TypeLoc = Var->getTypeSourceInfo()->getTypeLoc();
        renameNameAndType(OldName, NewName, Var->getType().getAsString(), Var,
                          TypeLoc);
      }
    }
    return true;
  }

  bool VisitDeclRefExpr(clang::DeclRefExpr *Expr) {
    std::string Name = Expr->getNameInfo().getAsString();

    if (Name == OldName) {
      Rewriter.ReplaceText(Expr->getNameInfo().getSourceRange(), NewName);
    }

    return true;
  }

  bool VisitDeclStmt(clang::DeclStmt *Stmt) {
    auto IVar = Stmt->decl_begin();
    auto Var = static_cast<clang::VarDecl *>(*IVar);
    auto TypeLoc = Var->getTypeSourceInfo()->getTypeLoc();
    std::string Name;
    renameNameAndType(OldName, NewName, Var->getType().getAsString(), Var,
                      TypeLoc);

    for (IVar++; IVar != Stmt->decl_end(); IVar++) {

      Var = static_cast<clang::VarDecl *>(*IVar);
      Name = Var->getNameAsString();
      if (Name == OldName) {
        Rewriter.ReplaceText(Var->getLocation(), Name.size(), NewName);
      }
    }

    return true;
  }

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *RecordDecl) {
    std::string Name = RecordDecl->getNameAsString();

    if (Name == OldName) {
      Rewriter.ReplaceText(RecordDecl->getLocation(), Name.size(), NewName);

      const clang::CXXDestructorDecl *RecordDestr = RecordDecl->getDestructor();
      if (RecordDestr)
        Rewriter.ReplaceText(RecordDestr->getLocation(), Name.size() + 1,
                             "~" + NewName);
    }

    return true;
  }

  bool VisitCXXNewExpr(clang::CXXNewExpr *NewExpr) {
    std::string Name = NewExpr->getConstructExpr()->getType().getAsString();

    if (Name == OldName) {
      Rewriter.ReplaceText(NewExpr->getExprLoc(), Name.size() + 4,
                           "new " + NewName);
    }

    return true;
  }

  bool OverwriteChangedFiles() { return Rewriter.overwriteChangedFiles(); }
};

class RenameIDConsumer : public clang::ASTConsumer {
protected:
  RenameVisitor visitor;

public:
  explicit RenameIDConsumer(clang::CompilerInstance &CI, std::string OldName,
                            std::string NewName)
      : visitor(clang::Rewriter(CI.getSourceManager(), CI.getLangOpts()),
                OldName, NewName) {}

  void HandleTranslationUnit(clang::ASTContext &Context) override {
    visitor.TraverseDecl(Context.getTranslationUnitDecl());
    visitor.OverwriteChangedFiles();
  }
};

class RenameIDPlugin : public clang::PluginASTAction {
private:
  std::string OldName;
  std::string NewName;

protected:
  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    if (args[0].find("OldName=") != 0 || args[1].find("NewName=") != 0) {
      llvm::errs() << "Error in parameters input.\n"
                      "Format of input:\n"
                      "OldName='MyOldName'\n"
                      "NewName='MyNewName'\n";
      return false;
    }

    OldName = args[0].substr(args[0].find("=") + 1);
    NewName = args[1].substr(args[1].find("=") + 1);

    return true;
  }

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    clang::StringRef InFile) override {
    return std::make_unique<RenameIDConsumer>(CI, OldName, NewName);
  }
};

static clang::FrontendPluginRegistry::Add<RenameIDPlugin>
    X("RenameID", "Rename variable, function or class identificators");
