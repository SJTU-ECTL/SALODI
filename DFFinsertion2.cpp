#include "structure.h"


void init1(string i_file, int x_num){
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
        //tmp_node.visited_num = 0;
        list.push_back(tmp_node);
		input_num++;
    }
    x_num = input_num - x_num;
    ofstream df;
    df.open("dff");
    df<<x_num+2<<endl;//"2" here is a parameter that one can be tuned
    df.close();
	int first_gate = true;
	int diff = 0;
	slpair_t const_s;
	const_s.label = CONST;
	const_s.w.push_back(0);
	kernel_t const_zero;
	const_zero.push_back(const_s);
	const_s.w.clear();
	const_s.w.push_back(0.5);
	kernel_t const_half;
	const_half.push_back(const_s);
	
    while(true){//internal nodes
        cd>>tmp;
        if(tmp.compare(".end") == 0 ) break;
        if(tmp.compare(".gate") ==0 ){
            cd >> tmp;
            //get type and weigh
            
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
							if(tmp_edge.source >= x_num) tmp_edge.delay = tmp_edge.source;
						}
						if(tmp_edge.source < x_num){
							tmp_k.push_back(tmp_s);
							tmp_edge.formula.push_back(const_zero);
							tmp_edge.formula.push_back(tmp_k); // the second val delay slpair
						}
		                if(tmp_edge.source >= x_num && tmp_edge.source < input_num){
		                	tmp_edge.formula.push_back(const_half);
		                }
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


void insertFormula(formula_t& f_res, const kernel_t& tmp_k){

		bool same;//if there is any kernel in the formula is same as the new kernel
        for(int m = 1; m < f_res.size(); m++){	
	        if( f_res[m].size() != tmp_k.size() ){ 
	        	continue;
            }
			same = true;
          	for(int n = 1; n < tmp_k.size(); n++){
            	if(f_res[m][n].label != tmp_k[n].label){
            		same = false;
            		break;
            	}	
        	}
        	
        	if(same){
        		f_res[m][0].w[0] += tmp_k[0].w[0];//add coefficient
		        //list equalities
		      	/*for(int n = 1; n < tmp_k.size(); n++){
					for(int h = 0; h < tmp_k[n].w.size(); h++){
						fp<<"+W"<<tmp_k[n].w[h]<<" ";
					}
					for(int h = 0; h < f_res[m][n].w.size(); h++){
						fp<<"-W"<< f_res[m][n].w[h]<<" ";
					}
					fp<<"= 0"<<endl;
	        	}*/
		       


		        if(f_res[m][0].w[0] == 0 ){
		       		f_res.erase(f_res.begin() + m);
		        }
        		return;
        	}
        }
        
        f_res.push_back(tmp_k);

}

void and2Op(const formula_t& formula1, const formula_t& formula2, formula_t& f_res){
/*	kernel_t const_k;
	slpair_t const_s;
	const_s.w.push_back( (formula1[0][0].w[0]) * (formula2[0][0].w[0]) ); 
	const_s.label = CONST;
	const_k.push_back(const_s);
	f_res.push_back(const_k);*/
    for (int i = 0; i < formula1.size(); ++i)
    {
        for (int j = 0; j < formula2.size(); ++j)
        {
			kernel_t tmp_k;
            slpair_t coef;           
			double coefficient = (formula1[i][0].w[0]) * (formula2[j][0].w[0]);
			if( (i != 0 || j != 0) && coefficient == 0) continue;//when 0 * anything, there is no meaning to further computation
			
			coef.w.push_back(coefficient);
            if(i == 0 && j == 0) coef.label = CONST; 
            else coef.label = COEF;// coef
            tmp_k.push_back(coef);

            for (int m = 1; m < formula1[i].size(); ++m)
            {
                tmp_k.push_back(formula1[i][m]);
            }
            for (int n = 1; n < formula2[j].size(); ++n)
            {
                bool new_val = true;
                for (int m = 1; m < tmp_k.size(); ++m)
                {
                    if (tmp_k[m].label == formula2[j][n].label)
                    {//when they are the same, list equalities
						for(int h = 0; h < tmp_k[m].w.size(); h++){
							fp<<"+W"<<tmp_k[m].w[h]<<" ";
						}
						for(int h = 0; h < formula2[j][n].w.size(); h++){
							fp<<"-W"<< formula2[j][n].w[h]<<" ";
						}
						fp<<"= 0"<<endl;
						//this term, e.g. x[1] in x[1]x[2] does not need to entry the whole equation.
                        new_val = false;
                        break;
                    }
                    /*else if (tmp_k[m].label == CONST && formula2[j][n].label != CONST)
                    {//only coefficient needs to change, already done
                        tmp_k[m] = formula2[j][n];
                        new_val = false;
                        break;
                    }
                    else if (tmp_k[m].label != CONST && formula2[j][n].label == CONST)
                    {
                        new_val = false;
                        break;
                    }*/	
                    else continue;
                }
                if (new_val)
                {
                    tmp_k.push_back(formula2[j][n]);
                }
            }
            sort(tmp_k.begin(), tmp_k.end(), compare_slpair);//O(n log(n) )
            
            insertFormula(f_res, tmp_k);
            
        }
    }
}


void or2Op(const formula_t& formula1, const formula_t& formula2, formula_t& f_res){
    //value = - value1 * value2 + value1 + value2 ;
    
    and2Op(formula1, formula2, f_res);


    for(int i = 0; i < f_res.size(); i++){
		f_res[i][0].w[0] = (-1) * f_res[i][0].w[0];
	}
    f_res[0][0].w[0] += formula1[0][0].w[0];
    f_res[0][0].w[0] += formula2[0][0].w[0];
    
    for (int i = 1; i < formula1.size(); ++i)
    {
        insertFormula(f_res, formula1[i]);
    }
    for (int i = 1; i < formula2.size(); ++i)
    {
    	insertFormula(f_res, formula2[i]);
    }
}
void notOp(const formula_t& formula1, formula_t& f_res){
    //value = 1 - value1;
    f_res = formula1;//I don't want modification on formula1

	for(int i = 0; i<f_res.size(); i++){
		f_res[i][0].w[0] = (-1) * f_res[i][0].w[0];
	}

	f_res[0][0].w[0] += 1;
}

void calculate(){
		for (nodes::iterator it = list.begin(); it != list.end(); it++)
		{
			vector< formula_t > formula(5);
		    formula_t f_res;

				if (it->i_node == false)
				{//cout<<it->index<<"  type; "<<it->type<<" "<<it->edges.size()<<endl;
					for(int i = 1; i <= it->edges.size(); i++){
						formula[i] = edge_list[ (it->edges)[i-1] ].formula;
					}
					if (it->type == NOT)
					{
                        notOp(formula[1], f_res);
					}
					else if  (it->type == AND)
					{
                        and2Op(formula[1], formula[2], f_res);
                    }
					else if  (it->type == OR)
					{
                        or2Op(formula[1], formula[2], f_res);
                    }
                    else if  (it->type == NAND2)
                    {
                        formula_t formula_tmp;
                        and2Op(formula[1], formula[2], formula_tmp);
                        notOp(formula_tmp, f_res);
                    }
                    else if  (it->type == NAND3)
                    {
                        formula_t formula_tmp;
                        and2Op(formula[1], formula[2], formula_tmp);
                        formula_t formula_tmp2;
                        and2Op(formula[3], formula_tmp, formula_tmp2);
                        notOp(formula_tmp2, f_res);
                    }
                    else if  (it->type == NAND4)
                    {                        
                        formula_t formula_tmp;
                        and2Op(formula[1], formula[2], formula_tmp);
                        formula_t formula_tmp2;
                        and2Op(formula[3], formula_tmp, formula_tmp2);
                        formula_t formula_tmp3;
                        and2Op(formula[4], formula_tmp2, formula_tmp3);
                        notOp(formula_tmp3, f_res);
                    }
                    else if  (it->type == NOR2)
                    {
                        formula_t formula_tmp;
                        or2Op(formula[1], formula[2], formula_tmp);
                        notOp(formula_tmp, f_res);
                    }
                    else if  (it->type == NOR3)
                    {
                        formula_t formula_tmp;
                        or2Op(formula[1], formula[2], formula_tmp);
                        formula_t formula_tmp2;
                        or2Op(formula[3], formula_tmp, formula_tmp2);
                        notOp(formula_tmp2, f_res);
                    }
                    else if  (it->type == NOR4)
                    {
                        formula_t formula_tmp;
                        or2Op(formula[1], formula[2], formula_tmp);
                        formula_t formula_tmp2;
                        or2Op(formula[3], formula_tmp, formula_tmp2);
                        formula_t formula_tmp3;
                        or2Op(formula[4], formula_tmp2, formula_tmp3);
                        notOp(formula_tmp3, f_res);
                    }
                    else if  (it->type == XOR2A)
                    {
                        formula_t formula_tmp, formula_tmp2;
                        notOp(formula[1], formula_tmp);
                        notOp(formula[2], formula_tmp2);
                        formula_t formula_tmp3, formula_tmp4;
                        and2Op(formula[1], formula_tmp2, formula_tmp3);
                        and2Op(formula[2], formula_tmp, formula_tmp4);
                        or2Op(formula_tmp3, formula_tmp4, f_res);
                    }
                    else if  (it->type == XOR2B)
                    {
                        formula_t formula_tmp, formula_tmp2;
                        notOp(formula[1], formula_tmp);
                        notOp(formula[2], formula_tmp2);
                        formula_t formula_tmp3, formula_tmp4, formula_tmp5;
                        and2Op(formula[1], formula[2], formula_tmp3);
                        and2Op(formula_tmp, formula_tmp2, formula_tmp4);
                        or2Op(formula[3], formula_tmp4, formula_tmp5);
                        notOp(formula_tmp5, f_res);
                    }
                    else if  (it->type == XNOR2A)
                    {
                        formula_t formula_tmp, formula_tmp2;
                        notOp(formula[1], formula_tmp);
                        notOp(formula[2], formula_tmp2);
                        formula_t formula_tmp3, formula_tmp4;
                        and2Op(formula[1], formula[2], formula_tmp3);//a*b
                        and2Op(formula_tmp, formula_tmp2, formula_tmp4);//!a*!b
                        or2Op( formula_tmp3, formula_tmp4, f_res);
                    }
                    else if  (it->type == XNOR2B)
                    {
                        formula_t formula_tmp, formula_tmp2;
                        notOp( formula[1], formula_tmp);
                        notOp(formula[2], formula_tmp2);
                        formula_t formula_tmp3, formula_tmp4, formula_tmp5;
                        and2Op(formula[1], formula_tmp2, formula_tmp3);
                        and2Op(formula[2], formula_tmp, formula_tmp4);
                        or2Op( formula_tmp3, formula_tmp4, formula_tmp5);
                        notOp(formula_tmp5, f_res);
                    }
                    else if  (it->type == AOI21)
                    {
                        formula_t formula_tmp, formula_tmp2;
                        and2Op(formula[1], formula[2], formula_tmp);
                        or2Op(formula[3], formula_tmp, formula_tmp2);
                        notOp(formula_tmp2, f_res);
                    }
                    else if  ( it->type == AOI22)
                    {
                        formula_t formula_tmp, formula_tmp2, formula_tmp3;
                        and2Op(formula[1], formula[2], formula_tmp);
                        and2Op(formula[3], formula[4], formula_tmp2);
                        or2Op(formula_tmp, formula_tmp2, formula_tmp3);
                        notOp(formula_tmp3, f_res);
                    }
                    else if  (it->type == OAI21)
                    {
                        formula_t formula_tmp, formula_tmp2;
                        or2Op(formula[1], formula[2], formula_tmp);
                        and2Op(formula[3], formula_tmp, formula_tmp2);
                        notOp(formula_tmp2, f_res);
                    }
                    else if  (it->type == OAI22)
                    {
                        formula_t formula_tmp, formula_tmp2, formula_tmp3;
                        or2Op(formula[1], formula[2], formula_tmp); // 9 13, 0???
                        or2Op(formula[3], formula[4], formula_tmp2);

                        and2Op(formula_tmp, formula_tmp2, formula_tmp3);
                        notOp(formula_tmp3, f_res);
                    }
					else if (it->type == CROSS)
					{//TODO:???
						f_res = formula[1];
					}
					//output the result
					if (it->end_point == true)
					{
						out_formula = f_res;
						break;
					}


/*cout<<"index: "<<it->index<<endl;				
for(int i=0;i<f_res.size();i++){	
		cout<<f_res[i][0].delay<<" ";
		for (int j = 1; j < f_res[i].size(); ++j)
		{
			cout<<f_res[i][j].val<<" ";
		}	cout<<endl;
}*/
				//store into the new edge
					for(int p = 0; p < edge_list.size(); p++){
						if( edge_list[p].source == it->index){
							for (int i = 1; i < f_res.size(); ++i)
							{
								for (int j = 1; j < f_res[i].size(); ++j)
								{
									f_res[i][j].label += edge_list[p].delay;
									f_res[i][j].w.push_back(p);
								}
							}
							edge_list[p].formula = f_res;
							//printFormula(f_res);
							//break;//when there is no cross
						}		
					}
				}else continue;
		}

}

void inequal12(const int &i, const int &j, const int &path_count){//w1+w2-0d0_0-1d0_1-2d2_2-... = 0
	//slpair_t: kernel 2 = x[w0+ w1 +...]
	//w1 + w2 - d0_1 - d0_2 - ... = 0
	for(int a = 0; a < out_formula[i][j].w.size(); a++){
		fp<< "+W" << out_formula[i][j].w[a]<<" ";
	}
	for(int a = 1; a < W_MAX_DFF; a++){
		fp<<"-"<<a<<" d"<<path_count<<"_"<<a<<" ";
	}
	fp<<"= 0"<<endl;
	
	//0d0_1 + d0_2 + d0_3 + ... = 0
	for(int a = 0; a < W_MAX_DFF; a++){
		fp<< "+d"<<path_count<<"_"<<a<<" ";
	}
	fp<<"= 1"<<endl;
}

void inequal3(const int& p1, const int& p2){
    for(int a = 0; a < W_MAX_DFF; a++){
		fp<<"+d"<<p1<<"_"<<a<<" +d"<<p2<<"_"<<a<<" <= 1"<<endl;
	}
}

void inequalities(){    

	int path_count = 0;
	vector<vector<double> > path_list;// less by default
	bool first = true;
	//printFormula(out_formula);
	for(int i = 1; i < out_formula.size(); i++){//kernel_t: P1 is constant
		//kernet_t P2 = -1 x[1]x[2]...
		for(int j = 1; j < out_formula[i].size(); j++){//slpair_t: kernel_1 is coefficient
			vector<double> new_path( out_formula[i][j].w );
			sort(new_path.begin(), new_path.end() );

			//find
			vector<vector<double> >::iterator s = find(path_list.begin(), path_list.end(), new_path);
			if(s == path_list.end() ){//the first time new_path appears
				out_formula[i][j].label = path_list.size();
				path_list.push_back(new_path);
				inequal12(i, j, out_formula[i][j].label);
			}else{
				out_formula[i][j].label = s - path_list.begin();
			}
			
		}
	}
	vector<vector<bool> > del;
	for(int i = 0; i < path_list.size(); i++){
		vector<bool> tmp_del(path_list.size(), false);
		del.push_back(tmp_del);
	}
	for(int i = 1; i < out_formula.size(); i++){
		for(int j = 1; j < out_formula[i].size() - 1; j++){
			int p1 = out_formula[i][j].label;
			for(int k = j+1; k < out_formula[i].size(); k++){
				int p2 = out_formula[i][k].label;
				if(!del[p1][p2]){
					del[p1][p2] = true;
					inequal3(p1, p2);
				}
			}
		
		}
	
	}
	
    fp.close();
    fp.open("model_.lp");
	fp<<"Bounds"<<endl;
	fp << "0 <= M <= +Inf"<<endl; 
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source < input_num) 
			fp<<"0 <= "<<"W"<<i<<" <= +Inf"<<endl;
	}

	for(int i = 0; i < path_list.size(); i++){
		for(int j = 0; j < W_MAX_DFF; j++){
			fp<<"0 <= "<<"d"<<i<<"_"<<j<< " <= +Inf" <<endl;
			if(i == path_list.size() - 1 && j == W_MAX_DFF-1) break;
			//fp<<", ";
		}
	}

	fp<<"Integers\nM \n";
	for(int i = 0; i < edge_list.size(); i++){
		fp<<"W"<<i<<endl;
	}

	for(int i = 0; i < path_list.size(); i++){
		for(int j = 0; j < W_MAX_DFF; j++){
			fp<<"d"<<i<<"_"<<j<<endl;
			if(i == path_list.size() - 1 && j == W_MAX_DFF-1) break;
		}
	}
	fp<<"End"<<endl;
	fp.close();

	
	
	/*int path_count = 0;
	for(int i = 1; i < out_formula.size(); i++){//kernel_t: P1 is constant
		//kernet_t P2 = -1 x[1]x[2]...
		for(int j = 1; j < out_formula[i].size(); j++){
			inequal12(i, j, path_count);
		}
	}	
	for(int i = 1; i < out_formula.size(); i++){//kernel_t: P1 is constant
		//kernet_t P2 = -1 x[1]x[2]...
		for(int j = 1; j < out_formula[i].size()-1; j++){
			for(int k = j+1; k < out_formula[i].size(); k++){
				for(int m = 0; m < W_MAX_DFF; m++){
					fp<<"+d"<<out_formula[i][j].label<<"_"<<m<<
                    " +d"<<out_formula[i][k].label<<"_"<<m<<
                    " <= 1"<<endl;
				}
			}
			
		}
	}
	fp.close();
    
    fp.open("model_.lp");
	fp<<"Bounds"<<endl;
	fp << "0 <= M <= +Inf"<<endl; 
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source < input_num) 
			fp<<"0 <= "<<"W"<<i<<" <= +Inf"<<endl;
	}

	for(int i = 0; i < path_count; i++){
		for(int j = 0; j < W_MAX_DFF; j++){
			fp<<"0 <= "<<"d"<<i<<"_"<<j<< " <= +Inf" <<endl;
			//fp<<", ";
		}
	}

	fp<<"Integers\nM \n";
	for(int i = 0; i < edge_list.size(); i++){
		fp<<"W"<<i<<endl;
	}

	for(int i = 0; i < path_count; i++){
		for(int j = 0; j < W_MAX_DFF; j++){
			fp<<"d"<<i<<"_"<<j<<endl;
		}
	}
	fp<<"End"<<endl;
	fp.close();*/
}

void objectiveFunc(){
	fp << "Minimize \nObjective: + M";
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source >= input_num) fp<<" +W"<<i;
	}fp<<endl;
	fp<<"Subject To"<<endl;
	//fp<<";"<<endl;
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source < input_num) 
			fp<<"+W"<<i<<" - M <= 0"<<endl;
	}
}


int critical_path(vector<vector<int> >path_list){
    int c_path = 0;
    for(int i = 0; i < path_list.size(); i++){
        int tmp = list[edge_list[path_list[i][0]].source].weight;
        for(int j = 0; j < path_list[i].size(); j++){
            if( edge_list[ path_list[i][j] ].delay != 0){
                if(tmp > c_path) c_path = tmp;
                tmp = list[edge_list[path_list[i][j]].target].weight;
            }else{
                tmp += list[edge_list[path_list[i][j]].target].weight;
            }
        }
        if(tmp > c_path) c_path = tmp;
    }
    return c_path;
}

int main(int argc, char* argv[])
{
	string i_file = argv[1];
	int x_num = atoi(argv[2]);
	srand(time(NULL));
	init1(i_file, x_num);
	cout<<"end of init"<<endl;
	fp.open("model.lp");
	objectiveFunc();
	calculate();
	inequalities();
	fp.close();
/*    vector<int> path;
    vector< vector<int> > path_list;
    findAllPaths(list[list.size()-1], path, path_list);
    for(int i = 0; i < path_list.size(); i++){
        cout<<"[path"<<i<<"] ";
        for(int j = 0; j < path_list[i].size(); j++)
            cout<<"W"<<path_list[i][j]<<" ";
        cout<<endl;
    }
    int c_path;
    c_path = critical_path(path_list);*/
	return 0;
}

