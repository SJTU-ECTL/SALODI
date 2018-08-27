#include "structure.h"

int max_w = 0;
int min_w = 99999;
void init1(string i_file){
    ifstream cd;
    cd.open(i_file.c_str());
    string tmp;
	while(true){
		cd >> tmp;
		if(tmp.compare(".inputs") == 0 ) break;
	}

    struct node tmp_node;
    while(true){//input nodes
        cd>>tmp;
        if(tmp.compare(".outputs") == 0) break;
		tmp_node.index = input_num;
        tmp_node.type = CROSS;
		tmp_node.weight = BUF_W;
        tmp_node.end_point = false;
        tmp_node.i_node = true;
        list.push_back(tmp_node);
		input_num++;
    }

	int first_gate = true;
	int diff = 0;
	slpair_t const_s;
	const_s.label = CONST;
	const_s.w.push_back(0);
	kernel_t const_k;
	const_k.push_back(const_s);
    while(true){//internal nodes
        cd>>tmp;
        if(tmp.compare(".end") == 0 ) break;
        if(tmp.compare(".gate") ==0 ){
            cd >> tmp;
            //get type and weight
            if(tmp.substr(0,3) == "inv"){
				tmp_node.type = NOT;
				if(tmp == "inv1") tmp_node.weight = INV1_W;
				if(tmp == "inv2") tmp_node.weight = INV2_W;
				if(tmp == "inv3") tmp_node.weight = INV3_W;
				if(tmp == "inv4") tmp_node.weight = INV4_W;
			}
            if(tmp == "nand2"){
				tmp_node.type = NAND2;
				tmp_node.weight = NAND2_W;
			}
            if(tmp == "nand3"){
				tmp_node.type = NAND3;
				tmp_node.weight = NAND3_W;
			}
            if(tmp == "nand4"){ 
				tmp_node.type = NAND4;
				tmp_node.weight = NAND4_W;
			}
            if(tmp == "nor2"){
				tmp_node.type = NOR2;
				tmp_node.weight = NOR2_W;
			}
            if(tmp == "nor3"){
				tmp_node.type = NOR3;
				tmp_node.weight = NOR3_W;
			}
            if(tmp == "nor4"){
				tmp_node.type = NOR4;
				tmp_node.weight = NOR4_W;
			}
            if(tmp == "and2"){
				tmp_node.type = AND;
				tmp_node.weight = AND2_W;
			}
            if(tmp == "or2"){
				tmp_node.type = OR;
				tmp_node.weight = AND2_W;
			}
            if(tmp == "xor2a"){
				tmp_node.type = XOR2A;
				tmp_node.weight = XOR2A_W;
			}
            if(tmp == "xor2b"){
				tmp_node.type = XOR2B;
				tmp_node.weight = XOR2B_W;
			}
            if(tmp == "xnor2a"){
				tmp_node.type = XNOR2A;
				tmp_node.weight = XNOR2A_W;
			}
            if(tmp == "xnor2b"){
				tmp_node.type = XNOR2B;
				tmp_node.weight = XNOR2B_W;
			}
            if(tmp == "aoi21"){
				tmp_node.type = AOI21;
				tmp_node.weight = AOI21_W;
			}
            if(tmp == "aoi22"){
				tmp_node.type = AOI22;
				tmp_node.weight = AOI22_W;
			}
            if(tmp == "oai21"){
				tmp_node.type = OAI21;
				tmp_node.weight = OAI21_W;
			}
            if(tmp == "oai22"){
				tmp_node.type = OAI22;
				tmp_node.weight = OAI22_W;
			}
			if(tmp == "buf"){
				tmp_node.type = CROSS;
				tmp_node.weight = BUF_W;
			}
			if(max_w < tmp_node.weight) max_w = tmp_node.weight;
			if(min_w > tmp_node.weight) min_w = tmp_node.weight;
            vector<int> tmp_in;//store the input nodes of one node

            while(true){
                cd >> tmp;
                if(tmp[0]!='O'){
                    if(tmp[2] == 'n') tmp_in.push_back( atoi(tmp.substr(3,tmp.length()-3).c_str()) - diff );
                    if(tmp[2] == 'x') tmp_in.push_back( atoi(tmp.substr(3,tmp.length()-3).c_str()) );
                    //notice that for first gate/ first line, all input nodes are x_i
                }else{//tmp[0] == 'O'
                	//get index, end_point, i_node
                	if(first_gate){//flash 'diff' when the first gate is read
                		diff = atoi(tmp.substr(3,tmp.length()-3).c_str()) - input_num;
                		first_gate = false;	
                	}
                    if(tmp[2] != 'z'){
						tmp_node.index = atoi(tmp.substr(3,tmp.length()-3).c_str()) - diff;
						tmp_node.end_point = false;
					}
                    else{
						tmp_node.index = list.size();
						tmp_node.end_point = true;
					}
                    tmp_node.i_node = false;
                    for(int i = 0; i < tmp_in.size(); i++){
                        struct edge tmp_edge;
                        tmp_edge.target = tmp_node.index;
                        tmp_edge.source = tmp_in[i];
                        kernel_t tmp_k;
						slpair_t coef;
						coef.label = COEF; // coefficient
						coef.w.push_back(1); // start with 1
						tmp_k.push_back(coef);// the first slpair is coef
						slpair_t tmp_s;
                        if(tmp_edge.source >= input_num){
                        	tmp_s.label = 0;
                        	tmp_edge.delay = 0;
                        }
						else{//this is an edge connecting to an input node
							tmp_s.w.push_back(edge_list.size());
							tmp_s.label = tmp_edge.source;//source = DFF number
							tmp_edge.delay = 0;
						}
						tmp_k.push_back(tmp_s);
						tmp_edge.formula.push_back(const_k);
						tmp_edge.formula.push_back(tmp_k); // the second val delay slpair
                        edge_list.push_back(tmp_edge);
                        tmp_node.edges.push_back(edge_list.size() - 1);
                    }
                    list.push_back(tmp_node);//the order is just a topologic one
                    tmp_node.edges.clear();
					break;
				}
            }
        }
    }

}


int critical_path(const vector<vector<int> >& path_list){
    int c_path = 0; 
    for(int i = 0; i < path_list.size(); i++){
        int tmp = list[edge_list[path_list[i][0]].target].weight;
        for(int j = 0; j < path_list[i].size(); j++){
            if( edge_list[ path_list[i][j] ].delay != 0){
                if(tmp > c_path) c_path = tmp;//renew critical path
                tmp = list[edge_list[path_list[i][j]].source].weight;
            }else{
                tmp += list[edge_list[path_list[i][j]].source].weight;
            }
			
        }
		if(tmp > c_path) c_path = tmp;

    }
    return c_path;
}

int init_delay(string filename){
    ifstream sol;
    sol.open(filename.c_str());
    string tmp;
    while(tmp != "M") sol >> tmp;
    int obj_value;
    sol >> obj_value;
    while(sol >> tmp && tmp[0] == 'W'){
        double d_delay;
        sol >> d_delay;
		d_delay = round(d_delay);
		int delay = (int)d_delay;
        edge_list[atoi(tmp.substr(1,tmp.length()-1).c_str())].delay = delay;
        edge_list[atoi(tmp.substr(1,tmp.length()-1).c_str())].formula[0][0].label = delay;
		//delay_info.insert(pair<int,int>(atoi(tmp.substr(1,tmp.length()-1).c_str()),delay));
    }
	sol.close();
}

int critical_path_first(const vector<vector<int> >& path_list){
    int c_path = 0; 
    for(int i = 0; i < path_list.size(); i++){
        int tmp = list[edge_list[path_list[i][0]].target].weight;
        for(int j = 0; j < path_list[i].size(); j++)
            tmp += list[edge_list[path_list[i][j]].source].weight;
		if(tmp > c_path) c_path = tmp;
    }
    return c_path;
}

void add_cpath_con(vector<vector<int> >path_list, string o_file, int c_path_tmp, int is_upper){
	ifstream id;
	id.open("last_c_path");
	int u_bound,l_bound,m_value;
	id >> u_bound>>m_value>>l_bound;
	id.close();
	ofstream of, cd;
	of.open(o_file.c_str());
	cd.open("c_path_range");
	cout<<endl<<endl<<endl;
	cout<<is_upper<<"  "<<u_bound<<"   "<<m_value<<"   "<<l_bound<<endl;
	if(u_bound == 0){//first time
		int first_cpath = critical_path_first(path_list);
		ofstream fof;
		fof.open("first_cpath");
		fof<<first_cpath<<endl;
		fof.close();
		cd <<c_path_tmp<<endl;
		c_path_tmp = c_path_tmp * 0.75;
		cd << c_path_tmp<<endl;
		cd << "0"<<endl;
	}else if(is_upper){//failed, new c_path_tmp does not make sense
		if(u_bound == max_w){
			cd <<"-1"<<endl;
			cd <<max_w<<endl;
		}
		l_bound = m_value;
		cd << u_bound<<endl;
		c_path_tmp = l_bound + (u_bound - l_bound) * 0.75;
		cd << c_path_tmp<<endl;
		cd << l_bound<<endl;
	}else{
		if(u_bound == max_w)	cd <<"-1"<<endl;
		cout<<u_bound<<" "<<m_value<<" "<<l_bound<<endl;
		cout<<c_path_tmp<<endl;
		u_bound = c_path_tmp;
		cd << u_bound<<endl;
		c_path_tmp = l_bound + (u_bound - l_bound) *0.75;
		cd << c_path_tmp<<endl;
		cd << l_bound<<endl;
	}
	cd.close();
	cout<<is_upper<<"  "<<u_bound<<"   "<<c_path_tmp<<"   "<<l_bound<<endl;
	cout<<c_path_tmp<<endl;
    for(int i = 0; i < path_list.size(); i++){
        int tmp;
        for(int j = 0; j < path_list[i].size(); j++){
            int k = j;
            tmp = list[edge_list[path_list[i][j]].source].weight;
            int last_node = j;
            for(; k < path_list[i].size(); k++){
                tmp += list[edge_list[path_list[i][k]].target].weight;
                if(tmp > c_path_tmp){
                    //cout<<last_node<<" "<<k<<endl;
                    for(int l = last_node; l <= k; l++)  
                        of<<"+W"<<path_list[i][l]<<" ";
                    of<<" >= 1"<<endl;
                    break;
                }
            }
            if(tmp < c_path_tmp && k == path_list[i].size()-1) break;
        }
    }
    of.close();
}



int main(int argc, char* argv[]){	
	int is_upper = atoi(argv[2]);
	string i_file = argv[1];
	srand(time(NULL));
    init1(i_file);
    init_delay("last_model.sol");//add new delay from result file
	vector<int> path;
	vector< vector<int> > path_list;
	findAllPaths(list[list.size()-1], path, path_list);
	/*for(int i = 0; i < path_list.size(); i++){
		cout<<"[path"<<i<<"] ";
		cout<<"n"<<edge_list[path_list[i][0]].target<<" ";
		for(int j = 0; j < path_list[i].size(); j++){
			cout<<"W"<<path_list[i][j]<<" ("<<edge_list[path_list[i][j]].delay<<") ";
			cout<<"n"<<edge_list[path_list[i][j]].source <<" ";
		}
		cout<<endl;
	}*/
    int c_path;//, target_cpath;
    /*ifstream i_cfp;
    i_cfp.open("tmp_c_path");
    i_cfp >> target_cpath;
    i_cfp.close();*/
    c_path = critical_path(path_list);
    cout<<"critical "<<c_path<<endl;

    /*ofstream cfp;    
    cfp.open("tmp_c_path");
    cfp<<c_path<<endl;
    cfp.close();
    cout<<"================="<<endl;
	cout<<target_cpath-1<<endl;*/
    add_cpath_con(path_list, "delays.lp", c_path, is_upper);
}

