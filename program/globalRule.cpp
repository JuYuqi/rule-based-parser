#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <utility>
#include <algorithm>
#include <cstdlib>

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

struct outdata{
    string pos1;
    string pos2;
    string action;
    int total;
    int leftArc;
    int rightArc;
    double percent;
    outdata():total(0),leftArc(0),rightArc(0),percent(0){}
};

class Key{
public:
    string pos1;
    string pos2;
    Key(string p1, string p2):pos1(p1),pos2(p2){}
    
    friend bool operator <(const Key &k1, const Key &k2){
	if (k1.pos1 < k2.pos1 || (k1.pos1 == k2.pos1 && k1.pos2 < k2.pos2)){
	    return true;
	}else{
	    return false;
	}
    }
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

typedef map<Key, statis> my_map_type;

void countLink(my_map_type& m, word* words, int index1, int index2){
    string pos1, pos2;
    map<Key, statis>::iterator findit;
    pos1 = words[index1].pos;
    pos2 = words[index2].pos;
    Key key(pos1, pos2);
    findit =  m.find(key);
    if (findit == m.end()){
	statis c;
	if (words[index1].index == words[index2].supIndex){
	    c.rightArc++;
	}
	if (words[index1].supIndex == words[index2].index){
	    c.leftArc++;
	}
	c.total++;
	m.insert(make_pair(key, c));
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

int main(int argc, char** argv)
{
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    int distance = atoi(argv[3]);
    string line, temp;
    stringstream ss;
    my_map_type m;
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
                if (i + distance < index){
		    countLink(m, words, i, i + distance);
                }
            //    if (i + 2 < index){
	    //        countLink(m, words, i, i + 2);
            //    }
            }
	    total += index - 1;
            index = 0;
        }
    }
    map<Key, statis>::iterator it = m.begin();
    outdata data[3000];
    index = 0;
    while(it != m.end()){
        data[index].pos1 = (it -> first).pos1;
        data[index].pos2 = (it -> first).pos2;
        data[index].total = (it -> second).total;
        data[index].leftArc = (it -> second).leftArc;
        data[index].rightArc = (it -> second).rightArc;
	double leftArcPercent = (double) (it -> second).leftArc / (it -> second).total;
	double rightArcPercent = (double) (it -> second).rightArc / (it -> second).total;
        data[index].percent = leftArcPercent > rightArcPercent ? leftArcPercent : rightArcPercent;
        index++;
        it++;
    }
    sort(data, &(data[index]), cmp);
    int leftTotal = 0;
    int rightTotal = 0;
    for (int i = 0;i < index;++i){
	fout << data[i].pos1 << '\t' << data[i].pos2 << '\t' 
	     << data[i].leftArc << '\t' << data[i].rightArc << '\t'
	     << data[i].total << '\t' 
	     << data[i].percent << endl;
	leftTotal += data[i].leftArc;
	rightTotal += data[i].rightArc;
    }
    fout << leftTotal << '\t' << rightTotal << '\t' << total << endl;
    return 0;
}
