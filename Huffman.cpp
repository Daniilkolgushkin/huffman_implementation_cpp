#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <queue>
#include <map>
#include <utility>

using namespace std;

map<unsigned char, int> read(const char *filename) {
	FILE *input = fopen(filename, "rb");
	map <unsigned char, int> dictionary;
	unsigned char istm[1];
	while (!feof(input)) {
		fread(istm, sizeof(unsigned char), 1, input);
		try{
			dictionary.at(istm[0])++;
		}
		catch (std::out_of_range) {
			dictionary.insert({istm[0], 1});
		}
	}
	fclose(input);
	return dictionary;
}

vector<int> transformdictionary(map<unsigned char, int> &dictionary) {
	map<unsigned char, int>::iterator runner = dictionary.begin();
	vector<int> out = vector<int>(0);
	while (runner != dictionary.end()) {
		pair<unsigned char, int> tmp = *runner;
		out.push_back(tmp.second);
		++runner;
	}
	return out;
}

struct node{
	vector<int> nod;
};

vector<node> buildtree(const vector<int> &freq) {
	int n = freq.size();
	vector<node> add = vector<node>(2*n - 1);
	for (int i = 0 ; i < 2*n - 1; i++) {
		add[i].nod = vector<int>(4);
	}

	auto cmp = [](vector<int> left,vector<int> right) { return (left[0]) > (right[0]); };
	priority_queue<vector<int>, vector<vector<int>>, decltype(cmp)> vertexies(cmp);
	for (int i = 0 ; i < n; i++) {
		vertexies.push({freq[i], i, -1, -1});
		add[i].nod = {freq[i], i, -1, -1};
	}

	vector<int> lltest = vector<int>(4);

	vector<int> sltest = vector<int>(4);

	for (int i = n; i < 2*n - 1; i++) {
		lltest = vertexies.top();
		vertexies.pop();
		sltest = vertexies.top();
		vertexies.pop();
		add[i].nod = {lltest[0] + sltest[0], i, lltest[1], sltest[1]};
		vertexies.push(add[i].nod);
	}

	lltest.clear();
	sltest.clear();

	return add;
}

int search(vector<bool> in, const vector<node> &tree, int num, vector<vector<bool>> &out){
	if (tree[num].nod[2] == -1) {
		out[num] = in;
		return 0;
	}
	else {
		in.push_back(true);
		int a = search(in, tree, tree[num].nod[2], out);
		in.pop_back();
		in.push_back(false);
		int b = search(in, tree, tree[num].nod[3], out);
	}
	return 0;
}

vector<vector<bool>> givecodes(const vector<node> &tree) {
	vector<bool> nin = {};
	vector<vector<bool>> answer = vector<vector<bool>>((tree.size() + 1) / 2);
	int a = search(nin, tree, tree.size() - 1, answer);
	return answer;
}

map<unsigned char, vector<bool>> mapifydictionary(vector<vector<bool>> &codes, map<unsigned char, int> &dictionary) {
	map<unsigned char, int>::iterator runner = dictionary.begin();
	map<unsigned char, vector<bool>> out = map<unsigned char, vector<bool>>();
	int i = 0;
	while (runner != dictionary.end()) {
		pair<unsigned char, int> tmp = *runner;
		out.insert({tmp.first, codes[i]});
		++runner;
		i++;
	}
	return out;
}

void writespecs(map<unsigned char, int> &dictionary, const char *filename) {
	FILE *output = fopen(filename, "wb");
	map<unsigned char, int>::iterator runner = dictionary.begin();
	unsigned char instream[5];
	instream[0] = (unsigned char)(dictionary.size() - 1);
	fwrite(instream, sizeof(unsigned char), 1, output);
	while (runner != dictionary.end()) {
		pair<unsigned char, int> tmp = *runner;
		instream[0] = tmp.first;
		for (int i = 0; i < sizeof(int); i++) {
			instream[i + 1] = (tmp.second << (8*i)) >> (8*(sizeof(int) - 1));
		}
		fwrite(instream, sizeof(unsigned char), 5, output);
		++runner;
	}
	fclose(output);
}

void writebuffer(queue<bool> &buff, unsigned char &outputchar) {
	for (int i = 0; i < 8; i++) {
			outputchar = outputchar << 1;
			if(buff.front()) {
				outputchar ++;
			}
			buff.pop();
		}
}

void rewrite(map<unsigned char, vector<bool>> &dictionary, const char *filenameIn, const char *filenameOut) {
	FILE *input = fopen(filenameIn, "rb");
	FILE *output = fopen(filenameOut, "ab");
	unsigned char inputchar[1];
	unsigned char outputchar[1];
	queue<bool> buff;
	while(!feof(input)) {
		fread(inputchar, sizeof(unsigned char), 1, input);

		for (int i = 0; i < dictionary[inputchar[0]].size(); i ++) {
			buff.push(dictionary[inputchar[0]][i]);
		}

		while (buff.size() >= 8) {
			/*for (int i = 0; i < 8; i ++) {
				outputchar[0] = outputchar[0] << 1;
				if(buff.front()) {
					outputchar[0] ++;
				}
				buff.pop();
			}*/
			writebuffer(buff, outputchar[0]);
			fwrite(outputchar, sizeof(unsigned char), 1, output);
		} 
	}
	
	if (buff.size() != 0) {
		while (buff.size() != 8) {
			buff.push(true);
		}
	
		/*for (int i = 0; i < 8; i++) {
			outputchar[0] = outputchar[0] << 1;
			if(buff.front()) {
				outputchar[0] ++;
			}
			buff.pop();
		}*/
		writebuffer(buff, outputchar[0]);
	}

	fwrite(outputchar, sizeof(unsigned char), 1, output);

	fclose(input);
	fclose(output);
}

void compress(const char *input, const char *output) {
	map<unsigned char, int> freqdictionary = read(input);
	vector<int> frequency = transformdictionary(freqdictionary);
	vector<node> tree = buildtree(frequency);
	vector<vector<bool>> codes = givecodes(tree);
	map<unsigned char, vector<bool>> dictionary = mapifydictionary(codes, freqdictionary);
	writespecs(freqdictionary, output);
	rewrite(dictionary, input, output);
}

int readSpecDecompress(map<unsigned char, int> &freqdictionary, vector<unsigned char> &altdictionary, FILE *input, FILE *output) {
//	FILE *input = fopen(filenameIn, "rb");
//	FILE *output = fopen(filenameOut, "wb");
	unsigned char istm[5];
	fread(istm, sizeof(unsigned char), 1, input);
	int count = (int)istm[0] + 1;
	int sumlen = 0;

	for (int i = 0; i < count; i++) {
		fread(istm, sizeof(unsigned char), 5, input);
		int w = 0;
		for (int j = 0; j < sizeof(int); j++) {
			w = (int)(w << ((sizeof(int) - j)*8)) + istm[j + 1];
		}

		freqdictionary.insert({istm[0], w});
		altdictionary.push_back(istm[0]);

		sumlen += w;
	
	}
	return sumlen;
}

void writeDecompressFinal(vector<node> &tree, FILE *input, FILE *output, int sumlen, vector<unsigned char> altdictionary) {
	unsigned char currentchar[1];
	unsigned char outstream[1];

	int count = 0;

	int currentbitnum = 0;

	int curvertex = tree.size() - 1;

	while (count < sumlen) {
		while (tree[curvertex].nod[2] != -1 || tree[curvertex].nod[3] != -1) {
			currentbitnum = currentbitnum % 8;
			if (currentbitnum == 0) {
				fread(currentchar, sizeof(unsigned char), 1, input);
				currentchar[0] = currentchar[0];
			}
			unsigned char byte = currentchar[0] >> 7;
			curvertex = tree[curvertex].nod[3 - byte];
			currentbitnum ++;
			currentchar[0] = currentchar[0] << 1;
		}
		outstream[0] = altdictionary[curvertex];
		curvertex = tree.size() - 1;
		fwrite(outstream, sizeof(unsigned char), 1, output);
		count ++;
	}

	fclose(input);
	fclose(output);
}


void decompress(const char *filenameIn, const char *filenameOut) {
	FILE *input = fopen(filenameIn, "rb");
	FILE *output = fopen(filenameOut, "wb");
	map <unsigned char, int> freqdictionary;
	vector<unsigned char> altdictionary;

	int sumlen = readSpecDecompress(freqdictionary, altdictionary, input, output);
	vector<int> frequency = transformdictionary(freqdictionary);
	vector<node> tree = buildtree(frequency);

	writeDecompressFinal(tree, input, output, sumlen, altdictionary);

}
