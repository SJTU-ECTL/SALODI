#include "structure.h"

int main(int argc, char *argv[]){
	int is_upper = atoi(argv[1]);
	init_delay("last_model.sol");
	init(true);
	for(int i=0;i<input_nodes.size();i++){
		path.clear();
		struct edge tmp;
		tmp.target=input_nodes[i];
		tmp.label=-1;
		tmp.delay=-1;
		path.push_back(tmp);
		find(input_nodes[i],input_nodes[i]);
	}
	for(int i=0;i<node_num;i++)	{
		if(list[i].o_deg==0) {
			final_node=i;
			break;
		}
	}
	critical_path();
	c_path_equations("delays.lp", is_upper);
}

void init_delay(string filename){
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
		delay_info.insert(pair<int,int>(atoi(tmp.substr(1,tmp.length()-1).c_str()),delay));
    }
	sol.close();
}

void c_path_equations(string filename, bool is_upper){
	ifstream id;
	id.open("last_c_path");
	int u_bound,l_bound,m_value;
	id >> u_bound>>m_value>>l_bound;
	id.close();
	ofstream fd, cd;
	fd.open(filename.c_str());
	cd.open("c_path_range");
	if(u_bound == 0){//first time
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
	for(int i=0;i<graph.size();i++){
		for(int j=2;j<graph[i].size();j++){
			int tmp=0;
			int k=j;
			tmp+=list[graph[i][k-1].target].weight;
			for(k=j;k<graph[i].size();k++){
				if(k!=0)	fd<<"+W"<<graph[i][k].label<<" ";
				tmp+=list[graph[i][k].target].weight;
				if(tmp>c_path_tmp) {
					fd<<" >= 1 \n";
					break;
				}
			}if(k==graph[i].size()) fd<<">= 0 \n";
		}
	}
	fd.close();
}

void equations(){
	for(int i=0;i<topo_list.size();i++){
		int cur=topo_list[i];
		if(list[cur].i_node) {//init node
			struct W_str tmp_w_str;
			tmp_w_str.delay=0;
			tmp_w_str.sub=list[cur].sub;
			struct adds_str tmp_a_str;
			tmp_a_str.addss.push_back(tmp_w_str);
			tmp_a_str.subs.insert(list[cur].sub);
			set<int> tmp_set;		tmp_set.insert(0);
			tmp_a_str.sub_delay.insert(pair<int, set<int> > (list[cur].sub,tmp_set));
			multimap<int, set<int> >::iterator it=tmp_a_str.sub_delay.begin();
			tmp_a_str.a_coef=1;
			list[cur].recorder.push_back(tmp_a_str);
		}
		else if(list[cur].type ==AND )	and_gate(cur, delay_add(cur), false);
		else if(list[cur].type ==OR)	or_gate(cur);		
		else if(list[cur].type == CROSS) not_gate(cur, false);
		else if(list[cur].type ==NOT) not_gate(cur, true);
	}
	mul t;
	t = list[final_node].recorder;
	cof<<"Minimize\nObjective: + M ";
	s::iterator it_s=n_init_list.begin();
	while(it_s!=n_init_list.end()) {
		cof<<"+ W"<<*it_s<<" ";
		it_s++;
	}
	cof<<" \nSubject To\n";//the first line over
	s::iterator it=init_list.begin();
	while(it!=init_list.end()){
		cof<<"+ W"<<*it<<" - M <= 0 \n";
		it++;	
	}
	for(int i=0;i<graph.size();i++){//for one particular path.
		for(int j=1;j<graph[i].size();j++)	cof<<"+W"<<graph[i][j].label<<" ";
		for(int k=1;k<=MAXDELAY;k++)	cof<<"- "<<k<<" d"<<i<<"_"<<k<<" ";
		cof<<"= 0 \n";
		for(int k=0;k<=MAXDELAY;k++)	cof<<"+ d"<<i<<"_"<<k<<" ";
		cof<<"= 1 \n";
	}
	
	//合并同类项:
	for(int i=0;i<t.size()-1;i++){
		vector<int> tmps;   tmps.push_back(i);
		int sum=t[i].a_coef;
		if(t[i].removed || t[i].checked) continue;
		for(int j=i+1;j<t.size();j++){
			if(t[j].removed) continue;
			if(t[i].sub_delay==t[j].sub_delay) {
				t[i].checked=t[j].checked=1;
				sum+=t[j].a_coef;
				tmps.push_back(j);
			}
		}
		if (sum==0)	{
			for(int k=0;k<tmps.size();k++)		t[tmps[k]].removed=true;
			for(int m=0;m<tmps.size()-1;m++){
				for(int n=m+1;n<tmps.size();n++){
					int tmp=rand_num;
					while(tmp>0){
						for(int k1=0;k1<t[tmps[m]].addss.size();k1++){
							if(t[tmps[m]].addss[k1].sub!=tmp) continue;
							for(int k2=0;k2<t[tmps[n]].addss.size();k2++){
								if(t[tmps[n]].addss[k2].sub!=tmp) continue;
								for(int j1=0;j1<t[tmps[m]].addss[k1].ws.size();j1++)	cof<<"+ W"<<t[tmps[m]].addss[k1].ws[j1]<<" ";
								for(int j2=0;j2<t[tmps[n]].addss[k2].ws.size();j2++)    cof<<"- W"<<t[tmps[n]].addss[k2].ws[j2]<<" ";
								cof<<"= 0 "<<endl;
							}							
						}
						tmp--;
					}
				}
			}
		}
	}

//------------------------------------------------------------------------------------------------------------
	vector<int> a,b;
	for(int i=0;i<t.size();i++){//danxiang
		if(t[i].removed) continue;
		for(int j1=0;j1<t[i].addss.size()-1;j1++){
			if(t[i].addss.size() == 0) break;
			for(int j2=j1+1;j2<t[i].addss.size();j2++){
				if(t[i].addss[j1].delay==t[i].addss[j2].delay){
					for(int k1=0;k1<t[i].addss[j1].ws.size();k1++) cof<<"+ W"<<t[i].addss[j1].ws[k1]<<" ";
					for(int k2=0;k2<t[i].addss[j2].ws.size();k2++) cof<<"- W"<<t[i].addss[j2].ws[k2]<<" ";
					cof<<"= 0 \n";	
				}
				else{
					int p1,p2;	
					for(int p=0;p<graph.size();p++){
						int cur_pt = 0;
						bool flag = false;
						while(cur_pt < t[i].addss[j1].ws.size()){
							if(graph[p][cur_pt+1].label!=t[i].addss[j1].ws[cur_pt]){
								flag = true;
								break;
							}
							cur_pt++;
						}
						if(!flag){
							p1 = p; break;
						}
					}
					for(int p=0;p<graph.size();p++){
						int cur_pt = 0;
						bool flag = false;
						while(cur_pt < t[i].addss[j2].ws.size()){
							if(graph[p][cur_pt+1].label!=t[i].addss[j2].ws[cur_pt]){
								flag = true;break;
							}
							cur_pt++;
						}
						if(!flag){
							p2 = p; break;
						}
					}
					bool existed = false;
					for(int s = 0; s < a.size(); s++){
						if(a.size() == 0 ) break;
						if((p1 == a[s] && p2 == b[s])||(p2 == a[s] && p1 == b[s])) {
							existed = true;
							break;
						}
					}
					if(!existed) {
						a.push_back(p1);b.push_back(p2);
						for(int k=0;k<=MAXDELAY;k++)	cof<<"+ d"<<p1<<"_"<<k<<" + d"<<p2<<"_"<<k<<" <= 1 \n";
					}
				}
			}
		}
	}//danxiang
	
	//positive number condition
	cof.close();
	cof.open("bounds.lp");
	cof<<"Bounds"<<endl;
	cof<<"0 <= M <= +Inf"<<endl;
	for(int i=1;i<label;i++) cof<<"0 <= W"<<i<<" <= +Inf \n";
	for(int i=0;i<graph.size();i++)
		for(int j=0;j<=MAXDELAY;j++)	cof<<"0 <= d"<<i<<"_"<<j<<" <= +Inf \n";	
	cof<<"Integers\n M ";
	for(int i=1;i<label;i++) cof<<" "<<"W"<<i;
	for(int i=0;i<graph.size();i++)
		for(int j=0;j<=MAXDELAY;j++)	cof<<" "<<"d"<<i<<"_"<<j;
	cof<<" \n";
	total_col=label+graph.size()*(MAXDELAY+1)+1;
	cof<<"End"<<endl;
}



int critical_path(){
	int c_path2=-1;
	for(int i=0;i<graph.size();i++){
		int tmp=list[graph[i][0].target].weight;
		for(int j=1;j<graph[i].size();j++){
			o_edges::iterator it_path=list[graph[i][j-1].target].edges.begin();
			while(it_path!=list[graph[i][j-1].target].edges.end()){
				if(it_path->label==graph[i][j].label){
					graph[i][j].delay=it_path->delay;
					break;
				}
				it_path++;
			}
			if(graph[i][j].delay>0){
				if(tmp>c_path2) c_path2=tmp;
				tmp=list[graph[i][j].target].weight;
			}else tmp+=list[graph[i][j].target].weight;
		}
		if(tmp>c_path2) c_path2=tmp;
	}
	//cout<<"critical path is:  "<<c_path2<<endl;
	c_path=c_path2;
	if(c_path>=c_path_tmp && c_path_tmp!=-1) return -1;
	c_path_tmp=c_path2;
	return 0;
}

