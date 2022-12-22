#include "primer/p0_trie.h"

// This is a placeholder file for clang-tidy check.
//
// With this file, we can fire run_clang_tidy.py to check `p0_trie.h`,
// as it will filter out all header files and won't check header-only code.
//
// This file is not part of the submission. All of the modifications should
// be done in `src/include/primer/p0_trie.h`.

using bustub::Trie;
using bustub::TrieNode;
using bustub::TrieNodeWithValue;

/**
 * @brief Construct a new Trie Node object with the given key char.
 * is_end_ flag should be initialized to false in this constructor.
 *
 * @param key_char Key character of this trie node
 */
TrieNode::TrieNode(char key_char): key_char_(key_char) {}

/**
 * @brief Move constructor for trie node object. The unique pointers stored
 * in children_ should be moved from other_trie_node to new trie node.
 *
 * @param other_trie_node Old trie node.
 */
TrieNode::TrieNode(TrieNode &&other_trie_node) noexcept {
  key_char_ = other_trie_node.GetKeyChar();
  children_.swap(other_trie_node.children_);
}

/**
 * @brief Whether this trie node has a child node with specified key char.
 *
 * @param key_char Key char of child node.
 * @return True if this trie node has a child with given key, false otherwise.
 */
bool TrieNode::HasChild(char key_char) const {
  return children_.count(key_char);
}

/**
 * @brief Whether this trie node has any children at all. This is useful
 * when implementing 'Remove' functionality.
 *
 * @return True if this trie node has any child node, false if it has no child node.
 */
bool TrieNode::HasChildren() const { return !children_.empty(); }

/**
 * @brief Whether this trie node is the ending character of a key string.
 *
 * @return True if is_end_ flag is true, false if is_end_ is false.
 */
bool TrieNode::IsEndNode() const { return is_end_; }

/**
 * @brief Return key char of this trie node.
 *
 * @return key_char_ of this trie node.
 */
char TrieNode::GetKeyChar() const { return key_char_; }

/**
 * @brief Insert a child node for this trie node into children_ map, given the key char and
 * unique_ptr of the child node. If specified key_char already exists in children_,
 * return nullptr. If parameter `child`'s key char is different than parameter
 * `key_char`, return nullptr.
 *
 * Note that parameter `child` is rvalue and should be moved when it is
 * inserted into children_map.
 *
 * The return value is a pointer to unique_ptr because pointer to unique_ptr can access the
 * underlying data without taking ownership of the unique_ptr. Further, we can set the return
 * value to nullptr when error occurs.
 *
 * @param key Key of child node
 * @param child Unique pointer created for the child node. This should be added to children_ map.
 * @return Pointer to unique_ptr of the inserted child node. If insertion fails, return nullptr.
 */
std::unique_ptr<TrieNode> *TrieNode::InsertChildNode(
    char key_char, std::unique_ptr<TrieNode> &&child) {
  if (key_char != child->GetKeyChar()) return nullptr;
  auto res = children_.try_emplace(key_char, std::move(child));
  return res.second ? &(res.first->second) : nullptr;
}

/**
 * @brief Get the child node given its key char. If child node for given key char does
 * not exist, return nullptr.
 *
 * @param key Key of child node
 * @return Pointer to unique_ptr of the child node, nullptr if child
 *         node does not exist.
 */
std::unique_ptr<TrieNode> *TrieNode::GetChildNode(char key_char) {
  auto res = children_.find(key_char);
  return res == children_.end() ? nullptr : &(res->second);
}

/**
 * @brief Remove child node from children_ map.
 * If key_char does not exist in children_, return immediately.
 *
 * @param key_char Key char of child node to be removed
 */
void TrieNode::RemoveChildNode(char key_char) {
  children_.erase(key_char);
}

/**
 * @brief Set the is_end_ flag to true or false.
 *
 * @param is_end Whether this trie node is ending char of a key string
 */
void TrieNode::SetEndNode(bool is_end) {
  is_end_ = is_end;
}

/**
 * @brief Construct a new Trie object. Initialize the root node with '\0'
 * character.
 */
Trie::Trie(): root_(std::make_unique<TrieNode>('\0')) {}

/**
 * @brief Remove key value pair from the trie.
 * This function should also remove nodes that are no longer part of another
 * key. If key is empty or not found, return false.
 *
 * You should:
 * 1) Find the terminal node for the given key.
 * 2) If this terminal node does not have any children, remove it from its
 * parent's children_ map.
 * 3) Recursively remove nodes that have no children and are not terminal node
 * of another key.
 *
 * @param key Key used to traverse the trie and find the correct node
 * @return True if the key exists and is removed, false otherwise
 */
bool Trie::Remove(const std::string &key) {
  if (key.empty())  return false;
  LOG_DEBUG("Remove key %s", key.c_str());
  auto res = RemoveHelper(key, 0, &root_);
  LOG_DEBUG("Remove done");
  return res;
}

bool Trie::RemoveHelper(
    const std::string &key, uint idx, std::unique_ptr<TrieNode> *prev) {
  std::unique_ptr<TrieNode> *node = (*prev)->GetChildNode(key[0]);
  bool reach_end = (key.size() == idx + 1), node_exists = (node != nullptr);
  bool has_val = node_exists && (*node)->IsEndNode();
  LOG_DEBUG("idx %u, keychar %c, reach_end %d, node_exists %d, has_val %d.", idx, key[idx], reach_end, node_exists, has_val);

  if (reach_end) {
    if (node_exists) {
      LOG_DEBUG("Curr node val %c", (*node)->GetKeyChar());
      if (has_val) {
        LOG_DEBUG("has val, delete node %c", key[idx]);
        auto tmp = std::make_unique<TrieNode>(std::move(**node));
        (*prev)->RemoveChildNode(key[idx]);
        if (tmp->HasChildren()) {
          LOG_DEBUG("replace curr with no val node");
          // Downgrade to TrieNode.
          (*prev)->InsertChildNode(
            key[idx], std::move(tmp));
        }
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  }

  if (!node_exists) {
    LOG_DEBUG("node does not exists, delete failed");
    return false;
  }
  bool res = RemoveHelper(key, idx + 1, node);
  if (!((*node)->HasChildren()) && !((*node)->IsEndNode())) {
    LOG_DEBUG("Delete chained node %c", (*node)->GetKeyChar());
    (*prev)->RemoveChildNode(key[idx]);
  }
  return res;
}
