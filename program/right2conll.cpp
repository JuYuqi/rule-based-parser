#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

int main(int argc, char **argv){
	if (argc != 3){
		return -1;
	}
	ifstream fin(argv[1]);
	ofstream fout(argv[2]);
	string line;
	string word[10];
	int linemap[10] = {0, 1, 2, 3, 4, 5, 8, 9, 6, 7};
	stringstream ss;
	while (getline(fin, line)){
		if (line != ""){
			ss.clear();
			ss << line;
			for (int i = 0;i < 10;++i){
				ss >> word[i];
			}
			for (int i = 0;i < 10;++i){
				fout << word[linemap[i]] << '\t';
			}
			fout << endl;
		}else{
			fout << endl;
		}
	}
	return 0;
}
