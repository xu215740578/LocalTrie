#include <stdio.h>
#include <string>

#include "Unicode.hpp"
#include "Trie.hpp"

using namespace LocalTrie;
using namespace std;

int main()
{
	string s = "字典树节点定义a有哪些算法解决多模式匹配问题";
	printf("%s:\n", s.c_str());
	Unicode uc = EncodeUnicodeFromeUtf8(s);
	for(int i = 0; i < uc.size(); i++)
	{
		printf("%u ", uc[i]);
	}
	printf("\n");

	string sOut;
	if(EncodeUtf8FromUnicode(sOut, uc))
	{
		printf("%s\n", sOut.c_str());
	}

	Trie trie;
	string sa[10] = {"字典", "节点", "定义", "字典树", "算法", "解决", "模式", "匹配", "哪些","问题"};
	for(int i = 0; i < 10; i++){
		trie.InsertNode(sa[i]);
	}
	
	vector<string> res;
	trie.Find(s, res);
	printf("s is %s res.size is %d\n", s.c_str(), res.size());
	for(string s1: res){
		printf("%s\n", s1.c_str());
	}

	trie.DeleteNode("afafd");
	string node("字典");
	if(trie.IsIn(node))
	{
		printf("is in\n");
	}
	trie.DeleteNode("字典");
	if(!trie.IsIn(node))
	{
		printf("is not in\n");
	}

	return 0;
}