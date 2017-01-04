#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <utility>
#include <algorithm>
#include <cstdlib>
#include <list>
#include <cstdio>
#define PERCENTMIN 1
#define TOTALMIN 5

using namespace std;


struct word{
    int index;
    string name;
    string pos;
    int supIndex;
    string tag;
};

struct statis{
    int leftArc;
    int rightArc;
    int total;
    statis():leftArc(0),rightArc(0),total(0){}
};

class Key{
public:
    word* words;
    int index;
    int i1;
    int i2;
    string dis;
    list<string> rules;
    Key(word* w, int i, int ii1, int ii2){
	words = w;
	index = i;
	i1 = ii1;
	i2 = ii2;
	dis = int2string(i2 - i1);
	allWordRule();
	nearposRule();
    }

    list<string> getRules(){
	return rules;
    }


    string int2string(int i){
	char s[12];
	sprintf(s, "%d", i);
	return s;
    }

    string simpleRule(){
	return words[i1].pos + "\t" + words[i2].pos + "\t" + dis;
    }

    void allWordRule(){
	rules.push_back(simpleRule());
	rules.push_back(simpleRule() + "\t" + words[i1].name + "\t");
	rules.push_back(simpleRule() + "\t" + "\t" + words[i2].name);
	rules.push_back(simpleRule() + "\t" + words[i1].name + "\t" + words[i2].name);
    }

    void nearposRule(){
	string bp1 = "_", ap1, bp2, ap2 = "_";
	if (i1 - 1 >= 0){
	    bp1 = words[i1 - 1].pos;
	}
	ap1 = words[i1 + 1].pos;
	bp2 = words[i2 - 1].pos;
	if (i2 + 1 < index){
	    ap2 = words[i2 + 1].pos;
	}
	rules.push_back(simpleRule() + "\t(" + "," + ap1 + "," + "," + ")");
	rules.push_back(simpleRule() + "\t(" + "," + "," + "," + ap2 + ")");
	rules.push_back(simpleRule() + "\t(" + bp1 + "," + "," + "," + ")");
	rules.push_back(simpleRule() + "\t(" + "," + "," + bp2 + "," + ")");
    }
};


struct outdata{
    string key;
    int total;
    int leftArc;
    int rightArc;
    double percent;
    outdata():total(0),leftArc(0),rightArc(0),percent(0){}
};

bool cmp(const outdata &d1, const outdata &d2){
    if (d1.percent > d2.percent){
        return true;
    }else if (d1.percent == d2.percent){
        if (d1.total > d2.total){
            return true;
        }
    }
    return false;
}

typedef map<string, statis> Rulemap;

void countLink(Rulemap& m, word* words, int index, int index1, int index2){
    map<string, statis>::iterator findit;
    Key key(words, index, index1, index2);
    
    list<string> rules = key.getRules();
    list<string>::iterator ruleit;
    for (ruleit = rules.begin(); ruleit != rules.end(); ++ruleit){
	findit =  m.find(*ruleit);
	if (findit == m.end()){
	    statis c;
	    if (words[index1].index == words[index2].supIndex){
		c.rightArc++;
	    }
	    if (words[index1].supIndex == words[index2].index){
		c.leftArc++;
	    }
	    c.total++;
	    m.insert(make_pair(*ruleit, c));
	}else{
	    if (words[index1].index == words[index2].supIndex){
		(findit -> second).rightArc++;
	    }
	    if (words[index1].supIndex == words[index2].index){
		(findit -> second).leftArc++;
	    }
	    (findit -> second).total++;
	}
    }
}

void validationLink(Rulemap& m, word* words, int index, int index1, int index2, int& correct, int& error){
    int arc = 0;
    if (words[index1].index == words[index2].supIndex){
	arc = 1;
    }else if (words[index1].supIndex == words[index2].index){
	arc = 2;
    }
    map<string, statis>::iterator findit;
    Key key(words, index, index1, index2);
    list<string> rules = key.getRules();
    list<string>::iterator ruleit;
    for (ruleit = rules.begin(); ruleit != rules.end(); ++ruleit){
	findit =  m.find(*ruleit);
	if (findit != m.end()){
	    if (arc == 1){
		if ((findit -> second).rightArc > 0){
		    correct++;
		}else{
		    error++;
		    cout << *ruleit << '\t' << (findit -> second).leftArc << '\t' << (findit -> second).rightArc 
		     << "\trightArc" << endl;
		}
	    }else if (arc == 2){
		if ((findit -> second).leftArc > 0){
		    correct++;
		}else{
		    cout << *ruleit << '\t' << (findit -> second).leftArc << '\t' << (findit -> second).rightArc
		     << "\tleftArc" << endl;
		    error++;
		}
	    }else{
		    cout << *ruleit << '\t' << (findit -> second).leftArc << '\t' << (findit -> second).rightArc
		     << "\tnoArc" << endl;
		error++;
	    }
	    return;
	}
    }
}

void generateRules(Rulemap& m, char* trainPath, char* rulePath){
    ifstream fin(trainPath);
    ofstream fout(rulePath);
    string line, temp;
    stringstream ss;
    word words[300];
    int index = 0;
    int total = 0;
    while (getline(fin, line)){
        if (line != ""){
            ss.clear();
            ss << line;
            ss >> words[index].index >> words[index].name >> temp >> temp >> words[index].pos
                >> temp >> temp >> temp >> words[index].supIndex >> words[index].tag;
            index++;
        }else{
            for (int i = 0;i < index;++i){
		for (int j = i + 1;j < index;++j){
		    countLink(m, words, index, i, j);
		}
            }
	    total += index - 1;
            index = 0;
        }
    }
    map<string, statis>::iterator it = m.begin();
    outdata* data;
    data = new outdata[1000000];
    index = 0;
    int successTotal = 0;
    while(it != m.end()){
	double leftArcPercent = (double) (it -> second).leftArc / (it -> second).total;
	double rightArcPercent = (double) (it -> second).rightArc / (it -> second).total;
	double percent = leftArcPercent > rightArcPercent ? leftArcPercent : rightArcPercent;
	if (percent < PERCENTMIN || (it -> second).total < TOTALMIN){
	    m.erase(it++);
	}else{
	    data[index].percent = percent; 
	    data[index].key = it -> first;
	    data[index].total = (it -> second).total;
	    data[index].leftArc = (it -> second).leftArc;
	    data[index].rightArc = (it -> second).rightArc;
	    index++;
	    successTotal += data[index].total;
	    it++;
	}
    }
    sort(data, &(data[index]), cmp);
    for (int i = 0;i < index;++i){
	fout << data[i].key << '\t'
	     << data[i].leftArc << '\t' << data[i].rightArc << '\t'
	     << data[i].total << '\t' 
	     << data[i].percent << endl;
    }
    cout << successTotal << '\t' << total << endl;
    fin.close();
    fout.close();
}

void validationRules(Rulemap& m, char* testPath){
    ifstream fin(testPath);
    string line, temp;
    stringstream ss;
    word words[300];
    int index = 0;
    int total = 0, correct = 0, error = 0;
    while (getline(fin, line)){
        if (line != ""){
            ss.clear();
            ss << line;
            ss >> words[index].index >> words[index].name >> temp >> temp >> words[index].pos
                >> temp >> temp >> temp >> words[index].supIndex >> words[index].tag;
            index++;
        }else{
            for (int i = 0;i < index;++i){
		for (int j = i + 1;j < index;++j){
		    validationLink(m, words, index, i, j, correct, error);
		}
            }
	    total += index - 1;
            index = 0;
        }
    }
    cout << correct << '\t' << error << '\t' << total << endl;
}

int main(int argc, char** argv)
{
    if (argc != 4){
	return -1;
    }
    Rulemap m;
    generateRules(m, argv[1], argv[2]);
    validationRules(m, argv[3]);
    return 0;
}
