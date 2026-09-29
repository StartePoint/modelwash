#include "modelwash/command.hpp"

namespace modelwash {

void CommandStack::push(std::unique_ptr<ICommand> cmd, Document& doc) {
  cmd->execute(doc);
  undo_.push_back(std::move(cmd));
  redo_.clear();
}

bool CommandStack::undo(Document& doc) {
  if (undo_.empty()) {
    return false;
  }
  auto cmd = std::move(undo_.back());
  undo_.pop_back();
  cmd->undo(doc);
  redo_.push_back(std::move(cmd));
  return true;
}

bool CommandStack::redo(Document& doc) {
  if (redo_.empty()) {
    return false;
  }
  auto cmd = std::move(redo_.back());
  redo_.pop_back();
  cmd->execute(doc);
  undo_.push_back(std::move(cmd));
  return true;
}

void CommandStack::clear() {
  undo_.clear();
  redo_.clear();
}

void ReplaceDocumentCommand::execute(Document& doc) {
  if (!captured_) {
    prev_ = doc.clone();
    captured_ = true;
  }
  doc = next_.clone();
  doc.recomputeStats();
}

void ReplaceDocumentCommand::undo(Document& doc) {
  doc = prev_.clone();
  doc.recomputeStats();
}

void ApplyWashResultCommand::execute(Document& doc) {
  if (!captured_) {
    prev_ = doc.clone();
    captured_ = true;
  }
  doc = next_.clone();
  doc.recomputeStats();
}

void ApplyWashResultCommand::undo(Document& doc) {
  doc = prev_.clone();
  doc.recomputeStats();
}

}  // namespace modelwash
