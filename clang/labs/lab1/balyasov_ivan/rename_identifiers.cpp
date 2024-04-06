#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Rewrite/Core/Rewriter.h"

enum class Types { Var, Func, Class };

class RenameVisitor : public clang::RecursiveASTVisitor<RenameVisitor> {
private:
  clang::Rewriter rewriter;
  Types type;
  std::string oldName;
  std::string newName;

public:
  explicit RenameVisitor(clang::Rewriter rewriter, Types type,
                         clang::StringRef oldName, clang::StringRef newName)
      : rewriter(rewriter), type(type), oldName(oldName), newName(newName) {}

  bool VisitFunctionDecl(clang::FunctionDecl *func) {
    if (type == Types::Func && func->getName() == oldName) {
      rewriter.ReplaceText(func->getNameInfo().getSourceRange(), newName);
    }
    return true;
  }

  bool VisitCallExpr(clang::CallExpr *call) {
    if (type == Types::Func) {
      clang::FunctionDecl *callee = call->getDirectCallee();
      if (callee && callee->getName() == oldName) {
        rewriter.ReplaceText(call->getCallee()->getSourceRange(), newName);
      }
    }
    return true;
  }

  bool VisitVarDecl(clang::VarDecl *var) {
    if (type == Types::Var && var->getName() == oldName) {
      rewriter.ReplaceText(var->getLocation(), oldName.size(), newName);
    }
    if (type == Types::Class &&
        (var->getType().getAsString() == oldName + " *" ||
         var->getType().getAsString() == oldName)) {
      rewriter.ReplaceText(var->getTypeSourceInfo()->getTypeLoc().getBeginLoc(),
                           oldName.size(), newName);
    }
    return true;
  }

  bool VisitDeclRefExpr(clang::DeclRefExpr *expr) {
    clang::VarDecl *var = clang::dyn_cast<clang::VarDecl>(expr->getDecl());
    if (type == Types::Var && var && var->getName() == oldName) {
      rewriter.ReplaceText(expr->getSourceRange(), newName);
    }
    return true;
  }

  bool VisitCXXRecordDecl(clang::CXXRecordDecl *record) {
    if (type == Types::Class && record->getName() == oldName) {
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
    if (type == Types::Class) {
      if (constructor->getNameAsString() == oldName) {
        rewriter.ReplaceText(constructor->getLocation(), oldName.size(),
                             newName);
      }
    }
    return true;
  }

  bool VisitCXXNewExpr(clang::CXXNewExpr *newExpr) {
    if (type == Types::Class) {
      if (newExpr->getConstructExpr()->getType().getAsString() == oldName) {
        rewriter.ReplaceText(newExpr->getExprLoc(), oldName.size() + 4,
                             "new " + newName);
      }
    }
    return true;
  }

  bool save_changes() { return rewriter.overwriteChangedFiles(); }
};

class RenameConsumer : public clang::ASTConsumer {
private:
  RenameVisitor Visitor;

public:
  explicit RenameConsumer(clang::CompilerInstance &CI, Types type,
                          clang::StringRef oldName, clang::StringRef newName)
      : Visitor(clang::Rewriter(CI.getSourceManager(), CI.getLangOpts()), type,
                oldName, newName) {}

  void HandleTranslationUnit(clang::ASTContext &context) override {
    Visitor.TraverseDecl(context.getTranslationUnitDecl());
    Visitor.save_changes();
  }
};

class RenamePlugin : public clang::PluginASTAction {
private:
  Types type;
  std::string oldName;
  std::string newName;

protected:
  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &args) override {
    std::vector<std::pair<std::string, std::string>> params = {
        {"type=", ""}, {"cur-name=", ""}, {"new-name=", ""}};

    if (!args.empty()) {
      for (int i = 0; i < args.size(); i++) {
        if (args[i] == "help") {
          PrintHelp(llvm::errs());
          return false;
        }
      }
    }

    if (args.size() < 3) {
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

    std::vector<std::pair<std::string, Types>> id_type = {
        {"var", Types::Var}, {"func", Types::Func}, {"class", Types::Class}};
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
           "-plugin-arg-renameIds type=[\"Var\", \"Func\", \"Class\"]\n"
           "-plugin-arg-renameIds cur-name=\"Current identifier name\"\n"
           "-plugin-arg-renameIds new-name=\"New identifier name\"\n";
  }

  void PrintParamsError(const clang::CompilerInstance &CI) {
    clang::DiagnosticsEngine &D = CI.getDiagnostics();

    D.Report(D.getCustomDiagID(
        clang::DiagnosticsEngine::Error,
        "Invalid arguments\n"
        "Specify \"-plugin-arg-renameIds help\" for usage\n"));
  }

public:
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI,
                    clang::StringRef InFile) override {
    return std::make_unique<RenameConsumer>(CI, type, oldName, newName);
  }
};

static clang::FrontendPluginRegistry::Add<RenamePlugin>
    X("renameIds", "Rename Variable, Function or Class");
