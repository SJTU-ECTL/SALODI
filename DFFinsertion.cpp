#include "structure.h"


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
							tmp_edge.delay = tmp_edge.source;
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

void init2(){//clear label and w
	for(int i = 0; i < edge_list.size()-1; i++){
		for(int j = 0; j < edge_list[i].formula.size(); j++){
			for(int k = 0; k < edge_list[i].formula[j].size(); k++){
				edge_list[i].formula[j][k].w.clear();
				edge_list[i].formula[j][k].label = edge_list[i].delay;
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
        		f_res[m][0].w[0] += tmp_k[0].w[0];
		        if(f_res[m][0].w[0] == 0 ){
		       		f_res.erase(f_res.begin() + m);
		        }
        		return;
        	}
        }
        
        f_res.push_back(tmp_k);

}

void and2Op(const formula_t& formula1, const formula_t& formula2, formula_t& f_res){
	kernel_t const_k;
	slpair_t const_s;
	const_s.w.push_back( (formula1[0][0].w[0]) * (formula2[0][0].w[0]) ); 
	const_s.label = CONST;
	const_k.push_back(const_s);
	f_res.push_back(const_k);
    for (int i = 1; i < formula1.size(); ++i)
    {
        for (int j = 1; j < formula2.size(); ++j)
        {
			kernel_t tmp_k;
            slpair_t coef;           
			int coefficient = (formula1[i][0].w[0]) * (formula2[j][0].w[0]);
			if(coefficient == 0) continue;
			coef.w.push_back(coefficient);
            coef.label = COEF; // coef
            tmp_k.push_back(coef);

            for (int m = 1; m < formula1[i].size(); ++m)
            {
                tmp_k.push_back(formula1[i][m]);
            }
            for (int n = 1; n < formula2[j].size(); ++n)
            {
                bool new_val = 1;
                for (int m = 1; m < tmp_k.size(); ++m)
                {
                    if (tmp_k[m].label == formula2[j][n].label)
                    {//when they are the same, list equalities
						for(int h = 0; h < tmp_k[m].w.size(); h++){
							fp<<"+ W"<<tmp_k[m].w[h]<<" ";
						}
						for(int h = 0; h < formula2[j][n].w.size(); h++){
							fp<<"- W"<< formula2[j][n].w[h]<<" ";
						}
						fp<<"= 0;"<<endl;
                        new_val = 0;
                        break;
                    }
                    else if (tmp_k[m].label == CONST && formula2[j][n].label != CONST)
                    {//only coefficient require to change, already done
                        tmp_k[m] = formula2[j][n];
                        new_val = 0;
                        break;
                    }
                    else if (tmp_k[m].label != CONST && formula2[j][n].label == CONST)
                    {
                        new_val = 0;
                        break;
                    }	
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
				{cout<<"type; "<<it->type<<" "<<it->edges.size()<<endl;
					for(int i = 1; i <= it->edges.size(); i++){
						formula[i] = edge_list[ (it->edges)[i-1] ].formula;
						//printFormula(formula[i]);
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
							//break;//when there is no cross
						}		
					}
				}else continue;
		}

}

void inequal12(const int &i, const int &j, int &path_count){//w1+w2-0d0_0-1d0_1-2d2_2-... = 0
	//slpair_t: kernel 2 = x[w0+ w1 +...]
	for(int a = 0; a < out_formula[i][j].w.size(); a++){
		fp<< "+ W" << out_formula[i][j].w[a]<<" ";
	}
	for(int a = 1; a < W_MAX_DFF; a++){
		fp<<"- "<<a<<" d"<<path_count<<"_"<<a<<" ";
	}
	fp<<"= 0;"<<endl;
	
	for(int a = 0; a < W_MAX_DFF; a++){
		fp<< "+ d"<<path_count<<"_"<<a<<" ";
	}
	fp<<"= 1;"<<endl;
	out_formula[i][j].label = path_count;
	path_count++;
}

int inequalities(){    vector<vector<bool> > del_re;
	int path_count = 0;
	vector<vector<int> > inequal_path;// less by default
	bool first = true;
	printFormula(out_formula);
	for(int i = 1; i < out_formula.size(); i++){//kernel_t: P1 is constant
		//kernet_t P2 = -1 x[1]x[2]...
		int j = 1;
		vector<int> new_path( out_formula[i][j].w );
		sort(new_path.begin(), new_path.end() );
		int pos;
		for(pos = 0; pos < inequal_path.size(); pos++){
			if(inequal_path[pos] == new_path){
				break;
			}
		}
		if(first || pos==inequal_path.size()){cout<<pos<<endl;
			inequal_path.push_back(new_path);
			inequal12(i, j, path_count);
            if(del_re.size() == 0){
                vector<bool> tmp_re(1);
                del_re.push_back(tmp_re);
            }
            for(int d = 0; d < del_re.size(); d++)
                del_re[d].push_back(false);
            vector<bool> tmp_re(del_re.size()+1);
            del_re.push_back(tmp_re);
			first = false;
		}else{
			out_formula[i][j].label = pos;
		}
		
		for(j = 1; j < out_formula[i].size() - 1; j++){//slpair_t: kernel_1 is coefficient
			vector<int> new_path( out_formula[i][j+1].w );
			sort(new_path.begin(), new_path.end() );
			//for(int kk = 0; kk<new_path.size(); kk++) cout<<new_path[kk]<<" ";
			for(pos = 0; pos < inequal_path.size(); pos++){
				if(inequal_path[pos] == new_path){
					break;
				}
			}
			if(pos==inequal_path.size()){cout<<pos<<endl;
				inequal_path.push_back(new_path);
				inequal12(i, j+1, path_count);
                for(int d = 0; d < del_re.size(); d++)
                    del_re[d].push_back(false);
                vector<bool> tmp_re(del_re.size()+1);
                del_re.push_back(tmp_re);
			}else{
				out_formula[i][j+1].label = pos;
			}

			for(int k = j + 1; k < out_formula[i].size(); k++){
                if((out_formula[i][j].label <= out_formula[i][k].label && !del_re[out_formula[i][j].label][out_formula[i][k].label])
                    || (out_formula[i][j].label > out_formula[i][k].label && !del_re[out_formula[i][k].label][out_formula[i][j].label]) ){ // &&!del_re[out_formula[i][k].label][out_formula[i][j].label]){
    				for(int a = 0; a < W_MAX_DFF; a++){
                        fp<<"+ d"<<out_formula[i][j].label<<"_"<<a<<
                        " + d"<<out_formula[i][k].label<<"_"<<a<<
                        " <= 1;"<<endl;
                    }
                    if(out_formula[i][j].label <= out_formula[i][k].label){
                        del_re[out_formula[i][j].label][out_formula[i][k].label] = true;
                    }else{
                        del_re[out_formula[i][k].label][out_formula[i][j].label] = true;
                    }
                    //del_re[out_formula[i][k].label][out_formula[i][j].label] = true;
                }
			}
		}
	}
	fp << "max >= 0;"<<endl; 
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source < input_num) 
			fp<<"W"<<i<<" >= 0;"<<endl;
	}

	for(int i = 0; i < inequal_path.size(); i++){
		for(int j = 0; j < W_MAX_DFF; j++){
			fp<<"d"<<i<<"_"<<j<< " >= 0;" <<endl;
			if(i == inequal_path.size() - 1 && j == W_MAX_DFF-1) break;
			//fp<<", ";
		}
	}

	fp<<"int max, ";
	for(int i = 0; i < edge_list.size(); i++){
		fp<<"W"<<i<<", ";
	}

	for(int i = 0; i < inequal_path.size(); i++){
		for(int j = 0; j < W_MAX_DFF; j++){
			fp<<"d"<<i<<"_"<<j;
			if(i == inequal_path.size() - 1 && j == W_MAX_DFF-1) break;
			fp<<", ";
		}
	}
	fp<<";"<<endl;
	return ( 1 + edge_list.size() + inequal_path.size()*W_MAX_DFF);
}


int lpsolve(int Ncol){
  lprec *lp;
  int  *colno = NULL, j, ret = 0;
  REAL *row = NULL;
  /* We will build the model row by row
     So we start with creating a model with 0 rows and 2 columns */
  //Ncol = 82; /* there are 82 variables in the model */
  lp = make_lp(0, Ncol);
  set_mip_gap(lp, TRUE, 0.5);
  set_presolve(lp, PRESOLVE_BOUNDS | PRESOLVE_IMPLIEDFREE | PRESOLVE_PROBEFIX | PRESOLVE_PROBEREDUCE, get_presolveloops(lp)); 
  if(lp == NULL)
    ret = 1; /* couldn't construct a new model... */
  if(ret == 0) {

    /* create space large enough for one row */
    colno = (int *) malloc(Ncol * sizeof(*colno));
    row = (REAL *) malloc(Ncol * sizeof(*row));
    if((colno == NULL) || (row == NULL))
      ret = 2;
  }
  lp = read_LP("model.lp", NORMAL, "test model");
  if(lp == NULL) {
    fprintf(stderr, "Unable to read model\n");
    return(1);
  }
  solve(lp);
  if(ret == 0) {
    /* a solution is calculated, now lets get some results */
    /* objective value */
    printf("Objective value: %lf\n", get_objective(lp));
    /*min_dff=min_dff_tmp;*/
    /* variable values */
    get_variables(lp, row);
	int row_num = 1;//skip max
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source >= input_num) edge_list[i].delay = row[row_num];
		row_num++;
	}
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source < input_num) edge_list[i].delay = row[row_num];
		row_num++;
	}
  }
  /* free allocated memory */
  if(row != NULL)	free(row);
  if(colno != NULL) free(colno);
  if(lp != NULL) { //clean up such that all used memory by lpsolve is freed 
    delete_lp(lp);
  }
  return(ret);
}


void objectiveFunc(){
	fp << "min: + max";
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source >= input_num) fp<<" + W"<<i;
	}
	//fp<<"Subject To"<<endl;
	fp<<";"<<endl;
	for(int i = 0; i < edge_list.size(); i++){
		if(edge_list[i].source < input_num) 
			fp<<"+ W"<<i<<" - max <= 0;"<<endl;
	}
}


int main(int argc, char* argv[])
{	
	string i_file = argv[1];
	string o_file = argv[2];
	srand(time(NULL));
	init1(i_file);
	fp.open("model.lp");
	objectiveFunc();
	calculate();
	int Ncol = inequalities();
	fp.close();
	lpsolve(Ncol);
	//init2();//clear label
	return 0;
}

