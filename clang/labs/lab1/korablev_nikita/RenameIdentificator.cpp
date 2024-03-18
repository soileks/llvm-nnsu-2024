#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

class RenameVisitor : public clang::RecursiveASTVisitor<RenameVisitor> {
private:
  clang::Rewriter Rewriter;
  std::string OldName;
  std::string NewName;

public:
  explicit RenameVisitor(clang::Rewriter Rewriter, std::string OldName,
                         std::string NewName)
      : Rewriter(Rewriter), OldName(OldName), NewName(NewName){};

  bool VisitFunctionDecl(clang::FunctionDecl *FD) {
    std::string Name = FD->getNameAsString();

    if (Name == OldName) {
      Rewriter.ReplaceText(FD->getNameInfo().getSourceRange(), NewName);
      Rewriter.overwriteChangedFiles();
    }

    return true;
  }

  bool VisitDeclRefExpr(clang::DeclRefExpr *DRE) {
    std::string Name = DRE->getNameInfo().getAsString();

    if (Name == OldName) {
      Rewriter.ReplaceText(DRE->getNameInfo().getSourceRange(), NewName);
      Rewriter.overwriteChangedFiles();
    }

    return true;
  }

  bool VisitVarDecl(clang::VarDecl *VD) {
    std::string Name = VD->getNameAsString();

    if (Name == OldName) {
      Rewriter.ReplaceText(VD->getLocation(), Name.size(), NewName);
      Rewriter.overwriteChangedFiles();
    }

    if (VD->getType().getAsString() == OldName + " *" ||
        VD->getType().getAsString() == OldName) {
      Rewriter.ReplaceText(VD->getTypeSourceInfo()->getTypeLoc().getBeginLoc(),
                           Name.size(), NewName);
      Rewriter.overwriteChangedFiles();
    }

    return true;
  }

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *CXXRD) {
    std::string Name = CXXRD->getNameAsString();

    if (Name == OldName) {
      Rewriter.ReplaceText(CXXRD->getLocation(), Name.size(), NewName);

      const clang::CXXDestructorDecl *CXXDD = CXXRD->getDestructor();
      if (CXXDD)
        Rewriter.ReplaceText(CXXDD->getLocation(), Name.size() + 1,
                             "~" + NewName);

      Rewriter.overwriteChangedFiles();
    }

    return true;
  }

  bool VisitCXXNewExpr(clang::CXXNewExpr *CXXNE) {
    std::string Name = CXXNE->getConstructExpr()->getType().getAsString();

    if (Name == OldName) {
      Rewriter.ReplaceText(CXXNE->getExprLoc(), Name.size() + 4,
                           "new " + NewName);
      Rewriter.overwriteChangedFiles();
    }

    return true;
  }
};

class RenameIdConsumer : public clang::ASTConsumer {
  RenameVisitor visitor;

public:
  explicit RenameIdConsumer(clang::CompilerInstance &CI, std::string OldName,
                            std::string NewName)
      : visitor(clang::Rewriter(CI.getSourceManager(), CI.getLangOpts()),
                OldName, NewName) {}

  void HandleTranslationUnit(clang::ASTContext &Context) override {
    visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
};

class RenameIdPlugin : public clang::PluginASTAction {
private:
  std::string OldName;
  std::string NewName;

protected:
  bool ParseArgs(const clang::CompilerInstance &Compiler,
                 const std::vector<std::string> &args) override {
    OldName = args[0];
    NewName = args[1];

    if (OldName.find("=") == 0 || OldName.find("=") == std::string::npos) {
      llvm::errs() << "Error entering the `OldName` parameter."
                   << "\n";
    }
    if (NewName.find("=") == 0 || NewName.find("=") == std::string::npos) {
      llvm::errs() << "Error entering the `NewName` parameter."
                   << "\n";
    }

    OldName = OldName.substr(OldName.find("=") + 1);
    NewName = NewName.substr(NewName.find("=") + 1);

    return true;
  }

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler,
                    llvm::StringRef InFile) override {
    return std::make_unique<RenameIdConsumer>(Compiler, OldName, NewName);
  }
};

static clang::FrontendPluginRegistry::Add<RenameIdPlugin>
    X("renamed-id", "Idetificator was renamed.");