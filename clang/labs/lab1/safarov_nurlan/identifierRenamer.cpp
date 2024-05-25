#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/ADT/Sequence.h"

class RenameVisitor : public clang::RecursiveASTVisitor<RenameVisitor> {
public:
  explicit RenameVisitor(clang::Rewriter Scribe, std::string formerName,
                         std::string renewedName)
      : Scribe(Scribe), formerName(formerName), renewedName(renewedName){};

  bool VisitFunctionDecl(clang::FunctionDecl *function) {
    replaceDesignationAndType(
        formerName, renewedName, function->getReturnType().getAsString(),
        function, function->getFunctionTypeLoc().getAs<clang::TypeLoc>());

    if (!function->param_empty()) {
      for (auto paramIterator = function->param_begin();
           paramIterator != function->param_end(); paramIterator++) {
        auto paramVar = static_cast<clang::VarDecl *>(*paramIterator);
        auto objTypeLoc = paramVar->getTypeSourceInfo()->getTypeLoc();
        replaceDesignationAndType(formerName, renewedName,
                                  paramVar->getType().getAsString(), paramVar,
                                  objTypeLoc);
      }
    }
    return true;
  }

  bool VisitDeclRefExpr(clang::DeclRefExpr *objDeclRefExpr) {
    std::string designation = objDeclRefExpr->getNameInfo().getAsString();

    if (designation == formerName) {
      Scribe.ReplaceText(objDeclRefExpr->getNameInfo().getSourceRange(),
                         renewedName);
    }

    return true;
  }

  bool VisitDeclStmt(clang::DeclStmt *objDeclStmt) {
    auto paramIterator = objDeclStmt->decl_begin();
    auto paramVar = static_cast<clang::VarDecl *>(*paramIterator);
    auto objTypeLoc = paramVar->getTypeSourceInfo()->getTypeLoc();
    std::string designation;
    replaceDesignationAndType(formerName, renewedName,
                              paramVar->getType().getAsString(), paramVar,
                              objTypeLoc);

    for (paramIterator++; paramIterator != objDeclStmt->decl_end();
         paramIterator++) {

      paramVar = static_cast<clang::VarDecl *>(*paramIterator);
      designation = paramVar->getNameAsString();
      if (designation == formerName) {
        Scribe.ReplaceText(paramVar->getLocation(), designation.size(),
                           renewedName);
      }
    }

    return true;
  }

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *objCXXRecordDecl) {
    std::string designation = objCXXRecordDecl->getNameAsString();

    if (designation == formerName) {
      Scribe.ReplaceText(objCXXRecordDecl->getLocation(), designation.size(),
                         renewedName);

      const clang::CXXDestructorDecl *classDestructor =
          objCXXRecordDecl->getDestructor();
      if (classDestructor)
        Scribe.ReplaceText(classDestructor->getLocation(),
                           designation.size() + 1, "~" + renewedName);
    }

    return true;
  }

  bool VisitCXXNewExpr(clang::CXXNewExpr *objCXXNewExpr) {
    std::string designation =
        objCXXNewExpr->getConstructExpr()->getType().getAsString();

    if (designation == formerName) {
      Scribe.ReplaceText(objCXXNewExpr->getExprLoc(), designation.size() + 4,
                         "new " + renewedName);
    }

    return true;
  }

  bool OverwriteChangedFiles() { return Scribe.overwriteChangedFiles(); }

private:
  clang::Rewriter Scribe;
  std::string formerName;
  std::string renewedName;

  void inline replaceDesignationAndType(
      std::string &formerName, std::string &renewedName, std::string typeOfVar,
      clang::DeclaratorDecl *objDeclaratorDecl,
      const clang::TypeLoc &objTypeLoc) {
    std::string designation = objDeclaratorDecl->getNameAsString();
    if (designation == formerName) {
      Scribe.ReplaceText(objDeclaratorDecl->getLocation(), designation.size(),
                         renewedName);
    }
    if (typeOfVar == formerName || typeOfVar == formerName + " *" ||
        typeOfVar == formerName + " &") {
      Scribe.ReplaceText(objTypeLoc.getBeginLoc(), formerName.size(),
                         renewedName);
    }
  }
};

class RenameIDConsumer : public clang::ASTConsumer {
protected:
  RenameVisitor objRenameVisitor;

public:
  explicit RenameIDConsumer(clang::CompilerInstance &objCompilerInstance,
                            std::string formerName, std::string renewedName)
      : objRenameVisitor(clang::Rewriter(objCompilerInstance.getSourceManager(),
                                         objCompilerInstance.getLangOpts()),
                         formerName, renewedName) {}

  void HandleTranslationUnit(clang::ASTContext &objASTContext) override {
    objRenameVisitor.TraverseDecl(objASTContext.getTranslationUnitDecl());
    objRenameVisitor.OverwriteChangedFiles();
  }
};

class RenameIDPlugin : public clang::PluginASTAction {
private:
  std::string formerName;
  std::string renewedName;

protected:
  bool ParseArgs(const clang::CompilerInstance &objCompilerInstance,
                 const std::vector<std::string> &parameters) override {
    if (parameters[0].find("formerName=") != 0 ||
        parameters[1].find("renewedName=") != 0) {
      llvm::errs() << "Error: incorrect parameters input.\n";
      return false;
    }

    formerName = parameters[0].substr(parameters[0].find("=") + 1);
    renewedName = parameters[1].substr(parameters[1].find("=") + 1);

    return true;
  }

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &objCompilerInstance,
                    clang::StringRef objStringRef) override {
    return std::make_unique<RenameIDConsumer>(objCompilerInstance, formerName,
                                              renewedName);
  }
};

static clang::FrontendPluginRegistry::Add<RenameIDPlugin>
    X("identifierRenamer", "Rename variable, function or class identificators");
