#ifndef STRUCTURE_H
#define STRUCTURE_H
#include <vector>
#include <math.h>
#include <algorithm>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
using namespace std;
//all operation is based on below expressions
#define CROSS 0
#define NOT   1
#define AND   2
#define OR    3
#define MAXDELAY 10


//#define INV1	1	//O=!a;		PIN * INV 1 999 0.9 0.3 0.9 0.3
//#define INV2	2	//O=!a;		PIN * INV 2 999 1.0 0.1 1.0 0.1
//#define INV3	3	//O=!a;		PIN * INV 3 999 1.1 0.09 1.1 0.09
//#define INV4	4	//O=!a;		PIN * INV 4 999 1.2 0.07 1.2 0.07
#define NAND2	5	//O=!(a*b);	PIN * INV 1 999 1.0 0.2 1.0 0.2
#define NAND3	6	//O=!(a*b*c);	PIN * INV 1 999 1.1 0.3 1.1 0.3
#define NAND4 	7	//O=!(a*b*c*d);	PIN * INV 1 999 1.4 0.4 1.4 0.4
#define NOR2	8	//O=!(a+b);	PIN * INV 1 999 1.4 0.5 1.4 0.5
#define NOR3	9	//O=!(a+b+c);	PIN * INV 1 999 2.4 0.7 2.4 0.7
#define NOR4	10	//O=!(a+b+c+d);	PIN * INV 1 999 3.8 1.0 3.8 1.0
//#define AND2	11	//O=a*b;		PIN * NONINV 1 999 1.9 0.3 1.9 0.3
//#define OR2	12	//O=a+b;		PIN * NONINV 1 999 2.4 0.3 2.4 0.3
#define XOR2A	13	//O=a*!b+!a*b;	PIN * UNKNOWN 2 999 1.9 0.5 1.9 0.5
#define XOR2B	14	//O=!(a*b+!a*!b);	PIN * UNKNOWN 2 999 1.9 0.5 1.9 0.5
#define XNOR2A	15	//O=a*b+!a*!b;	PIN * UNKNOWN 2 999 2.1 0.5 2.1 0.5
#define XNOR2B	16	//O=!(!a*b+a*!b);	PIN * UNKNOWN 2 999 2.1 0.5 2.1 0.5
#define AOI21	17	//O=!(a*b+c);	PIN * INV 1 999 1.6 0.4 1.6 0.4
#define AOI22	18	//O=!(a*b+c*d);	PIN * INV 1 999 2.0 0.4 2.0 0.4
#define OAI21	19	//O=!((a+b)*c);	PIN * INV 1 999 1.6 0.4 1.6 0.4
#define OAI22	20	//O=!((a+b)*(c+d)); PIN * INV 1 999 2.0 0.4 2.0 0.4

#define INV1_W  90
#define INV2_W	100
#define INV3_W	110
#define INV4_W	120
#define NAND2_W	100
#define NAND3_W	110
#define NAND4_W	140
#define NOR2_W	140
#define NOR3_W	240
#define NOR4_W	380
#define AND2_W	190
#define OR2_W	240
#define XOR2A_W	190
#define XOR2B_W	190
#define XNOR2A_W 210
#define XNOR2B_W 210
#define AOI21_W	160
#define AOI22_W	200
#define OAI21_W	160
#define OAI22_W	200
#define BUF_W 0

#define CONST -1
#define COEF -2
#define W_MAX_DFF 10
typedef struct sub_label //x_i[j]
{
	//int sub; //don't need for Peng's situation, where there is only one variable
	int label;//sum of w_i s
	vector<double> w;//edge index, w_i
}slpair_t;

static bool compare_slpair(const slpair_t& a1,const slpair_t& a2)  
{  

    if (a1.label < a2.label) return true;
	else if(a1.label == a2.label){
		if(a1.w.size() < a2.w.size()) return true;
		else if(a1.w.size() == a2.w.size() && a1.w[0] < a2.w[0]) return true;
	}
	else return false; 
}  

typedef vector<slpair_t> kernel_t;
//P1: x_i[j] * x_m[n] * ...; 
//the first pair indicates coefficient: label = -1;
//w[0] = true coefficent
typedef vector<kernel_t> formula_t;//const + P1 + P2 + P3 ...


struct edge{
	int source;
	int target;
	int delay;
	formula_t formula;
};

vector<edge> edge_list;

struct node{
	int index;
	int weight;
	int type;
	bool end_point;// if  the last node F_Q
	bool i_node; // if input node
	vector<int> edges;
};
typedef vector <struct node> nodes;
nodes list;

int input_num = 0;
ofstream fp;
formula_t out_formula;

map<int, int> delay_info;
struct inequal_cmp{  
    bool operator()(const vector<int> &a,const vector<int> &b) const  
    {  
		/*bool same = true
        for(int i = 0; i < min(a.size(), b.size() ); i++){
			if(a[i] == b[i]) same = false;
		}
		if(!same && a.size() == b.size() ) return false;*/

        for(int i = 0; i < min(a.size(), b.size() ); i++){
			if(a[i] > b[i]) return false;
		}
		return a.size() < b.size();
    }  
};  


void printFormula(const formula_t f_res){
    for(int i = 0; i < f_res.size(); i++){
    	cout<<"P"<<i<<"       ";
		for(int j = 0; j < f_res[i].size(); j++){
			cout<<"x["<<f_res[i][j].label<<"]";
			for(int k = 0; k < f_res[i][j].w.size(); k++)
					cout<<f_res[i][j].w[k]<<" ";
		}cout<<endl;
	}
}

void findAllPaths(const struct node& n, vector<int>& path, vector<vector <int> > &path_list){
	if(n.index < input_num){
		path_list.push_back(path);
	}else{
		for(int i = 0; i < n.edges.size(); i++){
			path.push_back(n.edges[i]);
			int j = 0;
			for(; j < list.size(); j++){
				if(list[j].index == edge_list[n.edges[i]].source) break;
			}
			findAllPaths(list[j], path, path_list);
		}
	}
	path.erase(path.end() - 1);
}

#endif
