#ifndef HUFFMAN
#define HUFFMAN

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <map>
#include <utility>

using namespace std;

map<unsigned char, int> read(const char *filename);

vector<int> transformdictionary(map<unsigned char, int> &dictionary);

struct node;

vector<node> buildtree(const vector<int> &freq);

int search(vector<bool> in, const vector<node> &tree, int num, vector<vector<bool>> &out);
	
vector<vector<bool>> givecodes(const vector<node> &tree);

map<unsigned char, vector<bool>> mapifydictionary(vector<vector<bool>> &codes, map<unsigned char, int> &dictionary);
	
void writespecs(map<unsigned char, int> &dictionary, const unsigned char *filename);
	
void rewrite(map<unsigned char, vector<bool>> &dictionary, const char *filenameIn, const char *filenameOut);

void compress(const char *input, const char *output);

map<vector<bool>, unsigned char> mapifydictionarydecompress(vector<vector<bool>> &codes, map<unsigned char, int> &dictionary);

void decompress(const char *filenameIn, const char *filenameOut);
#endif
