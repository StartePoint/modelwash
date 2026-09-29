#pragma once

#include "modelwash/document.hpp"

#include <memory>
#include <string>
#include <vector>

namespace modelwash {

class ICommand {
 public:
  virtual ~ICommand() = default;
  virtual std::string nameZh() const = 0;
  virtual void execute(Document& doc) = 0;
  virtual void undo(Document& doc) = 0;
};

class CommandStack {
 public:
  void push(std::unique_ptr<ICommand> cmd, Document& doc);
  bool undo(Document& doc);
  bool redo(Document& doc);
  bool canUndo() const { return !undo_.empty(); }
  bool canRedo() const { return !redo_.empty(); }
  void clear();

 private:
  std::vector<std::unique_ptr<ICommand>> undo_;
  std::vector<std::unique_ptr<ICommand>> redo_;
};

class ReplaceDocumentCommand : public ICommand {
 public:
  explicit ReplaceDocumentCommand(Document next) : next_(std::move(next)) {}
  std::string nameZh() const override { return "打开模型"; }
  void execute(Document& doc) override;
  void undo(Document& doc) override;

 private:
  Document next_;
  Document prev_;
  bool captured_ = false;
};

class ApplyWashResultCommand : public ICommand {
 public:
  explicit ApplyWashResultCommand(Document washed) : next_(std::move(washed)) {}
  std::string nameZh() const override { return "应用清洗"; }
  void execute(Document& doc) override;
  void undo(Document& doc) override;

 private:
  Document next_;
  Document prev_;
  bool captured_ = false;
};

}  // namespace modelwash
