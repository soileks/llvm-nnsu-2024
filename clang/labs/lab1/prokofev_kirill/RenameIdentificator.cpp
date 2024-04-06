#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"

enum class IdType { Var, Func, Class };

class RenameVisitor : public clang::RecursiveASTVisitor<RenameVisitor> {
public:
  explicit RenameVisitor(clang::Rewriter rewriter, IdType type,
                         clang::StringRef oldName, clang::StringRef newName)
      : rewriter(rewriter), type(type), oldName(oldName), newName(newName) {}

  bool VisitFunctionDecl(clang::FunctionDecl *func) {
    if (type == IdType::Func && func->getName() == oldName) {
      rewriter.ReplaceText(func->getNameInfo().getSourceRange(), newName);
    }
    return true;
  }

  bool VisitCallExpr(clang::CallExpr *call) {
    if (type == IdType::Func) {
      clang::FunctionDecl *callee = call->getDirectCallee();
      if (callee && callee->getName() == oldName) {
        rewriter.ReplaceText(call->getCallee()->getSourceRange(), newName);
      }
    }
    return true;
  }

  bool VisitVarDecl(clang::VarDecl *var) {
    if ((type == IdType::Var && var->getName() == oldName) ||
        (type == IdType::Class && var->getType().getAsString() == oldName) ||
        (type == IdType::Class &&
         var->getType().getAsString() == oldName + " *")) {
      if (type == IdType::Var) {
        rewriter.ReplaceText(var->getLocation(), oldName.size(), newName);
      } else {
        if (var->getType().getAsString() == oldName + " *") {
          rewriter.ReplaceText(
              var->getTypeSourceInfo()->getTypeLoc().getBeginLoc(),
              oldName.size(), newName);
        } else {
          rewriter.ReplaceText(
              var->getTypeSourceInfo()->getTypeLoc().getSourceRange(), newName);
        }
      }
    }
    return true;
  }

  bool VisitDeclRefExpr(clang::DeclRefExpr *expr) {
    clang::VarDecl *var = clang::dyn_cast<clang::VarDecl>(expr->getDecl());
    if (type == IdType::Var && var && var->getName() == oldName) {
      rewriter.ReplaceText(expr->getSourceRange(), newName);
    }
    return true;
  }

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *record) {
    if (type == IdType::Class && record->getName() == oldName) {
      rewriter.ReplaceText(record->getLocation(), newName);
      const auto *destructor = record->getDestructor();
      if (destructor) {
        rewriter.ReplaceText(destructor->getLocation(), oldName.size() + 1,
                             '~' + newName);
      }
    }
    return true;
  }

  bool VisitCXXConstructorDecl(clang::CXXConstructorDecl *constructor) {
    if (type == IdType::Class) {
      if (constructor->getNameAsString() == oldName) {
        rewriter.ReplaceText(constructor->getLocation(), oldName.size(),
                             newName);
      }
    }
    return true;
  }

  bool VisitCXXNewExpr(clang::CXXNewExpr *newExpr) {
    if (type == IdType::Class) {
      if (newExpr->getConstructExpr()->getType().getAsString() == oldName) {
        rewriter.ReplaceText(newExpr->getExprLoc(), oldName.size() + 4,
                             "new " + newName);
      }
    }
    return true;
  }

  bool save_changes() { return rewriter.overwriteChangedFiles(); }

private:
  clang::Rewriter rewriter;
  IdType type;
  std::string oldName;
  std::string newName;
};

class RenameASTConsumer : public clang::ASTConsumer {
public:
  explicit RenameASTConsumer(clang::CompilerInstance &CI, IdType type,
                             clang::StringRef cur_name,
                             clang::StringRef new_name)
      : Visitor(clang::Rewriter(CI.getSourceManager(), CI.getLangOpts()), type,
                cur_name, new_name) {}

  void HandleTranslationUnit(clang::ASTContext &context) override {
    Visitor.TraverseDecl(context.getTranslationUnitDecl());
    if (Visitor.save_changes()) {
      llvm::errs() << "An error occurred while saving changes to a file!\n";
    }
  }

private:
  RenameVisitor Visitor;
};

class RenamePlugin : public clang::PluginASTAction {
public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    clang::StringRef InFile) override {
    return std::make_unique<RenameASTConsumer>(CI, type, oldName, newName);
  }

protected:
  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    std::vector<std::pair<std::string, std::string>> params = {
        {"type=", ""}, {"oldName=", ""}, {"newName=", ""}};
    if (!args.empty()) {
      for (int i = 0; i < args.size(); i++) {
        if (args[i] == "help") {
          PrintHelp(llvm::errs());
          return false;
        }
      }
    }
    if (args.size() != 3) {
      PrintParamsError(CI);
      return false;
    }
    for (const auto &arg : args) {
      bool is_found = false;
      for (auto &param : params) {
        if (arg.find(param.first) == 0 && param.second.empty()) {
          param.second = arg.substr(param.first.size());
          is_found = true;
          break;
        }
      }
      if (!is_found) {
        PrintParamsError(CI);
        return false;
      }
    }
    std::vector<std::pair<std::string, IdType>> id_type = {
        {"var", IdType::Var}, {"func", IdType::Func}, {"class", IdType::Class}};
    size_t i;
    for (i = 0; i < id_type.size(); i++) {
      if (params[0].second == id_type[i].first) {
        type = id_type[i].second;
        break;
      }
    }
    if (i == id_type.size()) {
      PrintParamsError(CI);
      return false;
    }
    oldName = params[1].second;
    newName = params[2].second;
    return true;
  }

  void PrintHelp(llvm::raw_ostream &ros) {
    ros << "Specify three required arguments:\n"
           "-plugin-arg-rename type=[\"var\", \"func\", \"class\"]\n"
           "-plugin-arg-rename oldName=\"Current identifier name\"\n"
           "-plugin-arg-rename newName=\"New identifier name\"\n";
  }
  void PrintParamsError(const clang::CompilerInstance &CI) {
    clang::DiagnosticsEngine &D = CI.getDiagnostics();

    D.Report(
        D.getCustomDiagID(clang::DiagnosticsEngine::Error,
                          "Invalid arguments\n"
                          "Specify \"-plugin-arg-rename help\" for usage\n"));
  }

private:
  IdType type;
  std::string oldName;
  std::string newName;
};

static clang::FrontendPluginRegistry::Add<RenamePlugin>
    X("rename", "Rename variable, function or class");
