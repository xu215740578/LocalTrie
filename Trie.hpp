#ifndef CPPJIEBA_TRIE_HPP
#define CPPJIEBA_TRIE_HPP

#include <vector>
#include <queue>
#include <unordered_map>
#include "Unicode.hpp"

namespace LocalTrie {

using namespace std;

const size_t MAX_WORD_LENGTH = 512;

struct DictUnit {
  Unicode word;
}; // struct DictUnit

// for debugging
// inline ostream & operator << (ostream& os, const DictUnit& unit) {
//   string s;
//   s << unit.word;
//   return os << StringFormat("%s %s %.3lf", s.c_str(), unit.tag.c_str(), unit.weight);
// }

typedef Rune TrieKey;

class TrieNode {
 public :
  TrieNode(): next(NULL), ptValue(NULL) {
  }
 public:
  typedef unordered_map<TrieKey, TrieNode*> NextMap;
  NextMap *next;
  const DictUnit *ptValue;
};

class Trie {
 public:
  Trie():root_(new TrieNode){}
  // Trie(const vector<Unicode>& keys, const vector<const DictUnit*>& valuePointers)
  //  : root_(new TrieNode) {
  //   CreateTrie(keys, valuePointers);
  // }
  ~Trie() {
    DeleteNode(root_);
  }

  bool IsIn(string& s) const{
    if(s.size() == 0)
      return false;
    Unicode sUc = EncodeUnicodeFromeUtf8(s);

    const DictUnit* ptValue = Find(sUc);
    if(ptValue != NULL)
      return true;
    return false;
  }

  void Find(string& s, 
        vector<string>& res, 
        size_t max_word_len = MAX_WORD_LENGTH) const {
    if(s.size() == 0)
      return;
    res.reserve(s.size()/2);

    Unicode sUc = EncodeUnicodeFromeUtf8(s);
    vector<struct DictUnit*> vdu;
    Find(sUc, vdu, max_word_len);
    for (auto pdu:vdu) {
      string sShow;
      if (EncodeUtf8FromUnicode(sShow, pdu->word)) {
        res.push_back(sShow);
      }
    }
  }

  void InsertNode(const string& key){
    Unicode uc = EncodeUnicodeFromeUtf8(key);
    DictUnit* ptValue = new DictUnit;
    ptValue->word = uc;
    InsertNode(ptValue->word, ptValue);
  }

  void DeleteNode(const string& key){
    Unicode uc = EncodeUnicodeFromeUtf8(key);
    const DictUnit* ptValue = DeleteNode(uc);
    //if(ptValue != NULL)
    delete ptValue;
  }

 private:
  void CreateTrie(const vector<Unicode>& keys, const vector<const DictUnit*>& valuePointers) {
    if (valuePointers.empty() || keys.empty()) {
      return;
    }
    assert(keys.size() == valuePointers.size());

    for (size_t i = 0; i < keys.size(); i++) {
      InsertNode(keys[i], valuePointers[i]);
    }
  }

  void DeleteNode(TrieNode* node) {
    if (NULL == node) {
      return;
    }
    if (NULL != node->next) {
      for (TrieNode::NextMap::iterator it = node->next->begin(); it != node->next->end(); ++it) {
        DeleteNode(it->second);
      }
      delete node->next;
    }
    delete node;
  }

  void InsertNode(const Unicode& key, const DictUnit* ptValue) {
    if (key.begin() == key.end()) {
      return;
    }

    TrieNode::NextMap::const_iterator kmIter;
    TrieNode *ptNode = root_;
    for (Unicode::const_iterator citer = key.begin(); citer != key.end(); ++citer) {
      if (NULL == ptNode->next) {
        ptNode->next = new TrieNode::NextMap;
      }
      kmIter = ptNode->next->find(*citer);
      if (ptNode->next->end() == kmIter) {
        TrieNode *nextNode = new TrieNode;

        ptNode->next->insert(make_pair(*citer, nextNode));
        ptNode = nextNode;
      } else {
        ptNode = kmIter->second;
      }
    }
    assert(ptNode != NULL);
    if(ptNode->ptValue != NULL)
      delete ptNode->ptValue;
    ptNode->ptValue = ptValue;
  }

  const DictUnit* DeleteNode(const Unicode& key)
  {
    if (key.begin() == key.end()) {
      return NULL;
    }
    TrieNode::NextMap::const_iterator kmIter;
    TrieNode *ptNode = root_;
    for (Unicode::const_iterator citer = key.begin(); citer != key.end(); ++citer) {
      if (NULL == ptNode->next) {
        return NULL;
      }
      kmIter = ptNode->next->find(*citer);
      if (ptNode->next->end() == kmIter) {
        return NULL;
      } else {
        ptNode = kmIter->second;
      }
    }
    assert(ptNode != NULL);
    const DictUnit* retValue = ptNode->ptValue;
    ptNode->ptValue = NULL;
    return retValue;
  }

  void Find(Unicode& unicode, 
        vector<struct DictUnit*>& res, 
        size_t max_word_len = MAX_WORD_LENGTH) const {
    res.reserve(unicode.size()/2);

    const TrieNode *ptNode = NULL;
    TrieNode::NextMap::const_iterator citer;

    for ( size_t i = 0; i < unicode.size(); i++ ) {

      if ( root_->next != NULL && root_->next->end() != ( citer = root_->next->find(unicode[i]) ) ) {
        ptNode = citer->second;
      } else {
        ptNode = NULL;
      }

      for ( size_t j = i+1; j < unicode.size() && (j-i+1) <= max_word_len; j++ ) {
        if ( ptNode == NULL || ptNode->next == NULL )
          break;
        citer = ptNode->next->find(unicode[j]);
        if(ptNode->next->end() == citer)
          break;
        ptNode = citer->second;
        if (NULL != ptNode->ptValue){
          res.push_back((struct DictUnit*)ptNode->ptValue);
        }
      }

    }
  }

  const DictUnit* Find(Unicode& unicode) const{
    if(unicode.size() == 0)
      return NULL;

    const TrieNode* ptNode = root_;
    TrieNode::NextMap::const_iterator citer;
    for (int i = 0; i < unicode.size(); ++i)
    {
      if(NULL == ptNode->next)
        return NULL;
      citer = ptNode->next->find(unicode[i]);
      if (ptNode->next->end() == citer) 
      {
          return NULL;
      }
      ptNode = citer->second;
    }

    return ptNode->ptValue;
  }

  TrieNode* root_;
}; // class Trie
} // namespace cppjieba

#endif // CPPJIEBA_TRIE_HPP
