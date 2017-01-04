#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <utility>
#include <algorithm>
#include <stack>
#include <list>
#define PERCENTMIN 1
#define TOTALMIN 5

using namespace std;

struct word{
    int index;
    string name;
    string pos;
    int supIndex;
    string tag;
    list<word*> childs;
};

struct statis{
    int leftArc;
    int rightArc;
    int total;
    statis():leftArc(0),rightArc(0),total(0){}
};    


class Key{
public:
    word* top;
    word* second;
    list<string> rules;
    Key(word* t, word* s){
	top = t;
	second = s;
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
	return top -> pos + "\t" + second -> pos;
    }

    void allWordRule(){
	rules.push_back(simpleRule());
	rules.push_back(simpleRule() + "\t" + top -> name + "\t");
	rules.push_back(simpleRule() + "\t" + "\t" + second -> name);
	rules.push_back(simpleRule() + "\t" + top -> name + "\t" + second -> name);
    }

    void nearposRule(){
	string bp1 = "_", ap1, bp2, ap2 = "_";
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

void countLink(Rulemap& m, word* top, word* second, int arc){
    map<string, statis>::iterator findit;
    Key key(top, second);
    
    list<string> rules = key.getRules();
    list<string>::iterator ruleit;
    for (ruleit = rules.begin(); ruleit != rules.end(); ++ruleit){
	findit =  m.find(*ruleit);
	if (findit == m.end()){
	    statis c;
	    if (arc == 2){
		c.rightArc++;
	    }
	    if (arc == 1){
		c.leftArc++;
	    }
	    c.total++;
	    m.insert(make_pair(*ruleit, c));
	}else{
	    if (arc == 2){
		(findit -> second).rightArc++;
	    }
	    if (arc == 1){
		(findit -> second).leftArc++;
	    }
	    (findit -> second).total++;
	}
    }
}

void generateRules(Rulemap& m, char* trainPath, char* rulePath){
    ifstream fin(trainPath);
    ofstream fout(rulePath);
    string line, temp;
    stringstream ss;
    word* words[1000];
    int index = 0, total = 0;
    map<Rulemap, statis>::iterator findit;
    stack<word*> stack;
    while (getline(fin, line)){
        if (line != ""){
            ss.clear();
            ss << line;
	    words[index] = new word();
            ss >> words[index] -> index >> words[index] -> name >> temp >> temp >> words[index] -> pos
                >> temp >> temp >> temp >> words[index] -> supIndex >> words[index] -> tag;
            index++;
        }else{
	    word* root = new word();
	    root -> index = 0;
	    root -> supIndex = 0;
	    root -> name = "ROOT";
	    root -> pos = "ROOT";
	    root -> tag = "ROOT";
	    words[index++] = root;
            for (int i = 0;i < index;++i){
		stack.push(words[i]);
		cout << "SHIFT" << endl;
		while (stack.size() > 1){
		    word* top = stack.top();
		    stack.pop();
		    word* second = stack.top();
		    stack.pop();
		    if (top -> supIndex == second -> index){
			(second -> childs).push_back(top);
			countLink(m, top, second, 2);
			stack.push(second);
		    }else if (top -> index == second -> supIndex){
			(top -> childs).push_back(second);
			countLink(m, top, second, 1);
			stack.push(top);
		    }else{
			countLink(m, top, second, 0);
			stack.push(second);
			stack.push(top);
			break;
		    }
		}	
            }
	    stack.pop();
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

void validationLink(Rulemap& m, word* top, word* second, int arc, int& correct, int& error){
    map<string, statis>::iterator findit;
    Key key(top, second);
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

void validationRules(Rulemap& m, char* testPath){
    ifstream fin(testPath);
    string line, temp;
    stringstream ss;
    word* words[1000];
    int index = 0, total = 0;
    int correct = 0, error = 0;
    map<Rulemap, statis>::iterator findit;
    stack<word*> stack;
    while (getline(fin, line)){
        if (line != ""){
            ss.clear();
            ss << line;
	    words[index] = new word();
            ss >> words[index] -> index >> words[index] -> name >> temp >> temp >> words[index] -> pos
                >> temp >> temp >> temp >> words[index] -> supIndex >> words[index] -> tag;
            index++;
        }else{
	    word* root = new word();
	    root -> index = 0;
	    root -> supIndex = 0;
	    root -> name = "ROOT";
	    root -> pos = "ROOT";
	    root -> tag = "ROOT";
	    words[index++] = root;
            for (int i = 0;i < index;++i){
		stack.push(words[i]);
		cout << "SHIFT" << endl;
		while (stack.size() > 1){
		    word* top = stack.top();
		    stack.pop();
		    word* second = stack.top();
		    stack.pop();
		    if (top -> supIndex == second -> index){
			(second -> childs).push_back(top);
			validationLink(m, top, second, 2, correct, error);
			stack.push(second);
		    }else if (top -> index == second -> supIndex){
			(top -> childs).push_back(second);
			validationLink(m, top, second, 1, correct, error);
			stack.push(top);
		    }else{
			validationLink(m, top, second, 0, correct, error);
			stack.push(second);
			stack.push(top);
			break;
		    }
		}	
            }
	    stack.pop();
	    total += index - 1;
            index = 0;
        }
    }
    cout << correct << '\t' << error << '\t' << total << endl;
}

int main(int argc, char ** argv)
{
    if (argc != 4){
	return -1;
    }
    Rulemap m;
    generateRules(m, argv[1], argv[2]);
    validationRules(m, argv[3]);
    return 0;
}
