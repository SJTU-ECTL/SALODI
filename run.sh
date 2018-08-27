#!/bin/bash 


:<<eof
test_file="test.blif"
./insert2 $test_file 7
#model.lp: first part - functional equivalence
#model_.lp: last part - int W0, W1 ...
cp model.lp model_const.lp
cat model_.lp >> model.lp
./gurobi_cl ResultFile=model.sol model.lp
bound_dff=15
cur_dff=$(awk 'NR==2{print $5}' model.sol)
while true;do
	./insert3 $test_file model.sol delay_constraints
	#tmp_c_path: critical path for the input "model.sol"
	#out: critical path related constraints
	echo "===========critical "
	cat tmp_c_path
	cp tmp_c_path last_c_path
	cp model.sol last_model.sol
	
	cp model_const.lp model.lp
	cat delay_constraints >> model.lp
	cat model_.lp >> model.lp
	./gurobi_cl ResultFile=model.sol model.lp
	cur_dff=$(awk 'NR==2{print $5}' model.sol)


	if [ $cur_dff -gt $bound_dff ];then
		echo "critical path is: "
		cat last_c_path
		break
	fi

done
eof

oneturn(){
	sol_file=$1 
	test_file=input_file/${sol_file}".blif"
	./insert2 $test_file $2
	cp model.lp model_const.lp
	cat model_.lp >> model.lp
	./gurobi_cl ResultFile=model.sol model.lp
	#bound_dff=7
	min_dff=$(awk 'NR==2{print $5}' model.sol)
	M=$(cat dff)
	echo $M
	lower_bound_dff=0
	granularity=90
	echo "0 0 0" > last_c_path
	is_upper=0
	i=0
	cp model.sol $1"_fst.sol"
	while true;do
		i=`expr $i + 1`
		if [  $i -ge 20 ];then
			break;
		fi
		if [ $is_upper -eq 0 ];then
			cp model.sol last_model.sol
		fi
		./insert3 $test_file $is_upper
		#./graph_mindelay $is_upper < tmp #c_path_range: upper, cur, lower cpath; delays.lp for the equations
		c_path_upper=$(awk 'NR==1{print}' c_path_range)
		c_path_lower=$(awk 'NR==3{print}' c_path_range)
		if [ $c_path_upper -eq -1 ];then
			echo "no more space to improve",$i
			break;
		fi
		echo "!!!!!!!!!!!!!!!!!!!!bounds renew!!!!!!!!!!!!!!!!!!!!!!!!!!"		
		cat c_path_range
		let diff=$c_path_upper-$c_path_lower
		if [ $diff -lt $granularity ];then
			echo "critical path is: "
			echo $c_path_upper,$i
			break
		fi
		mv c_path_range last_c_path

		cp model_const.lp model.lp
		cat delays.lp >> model.lp
		cat model_.lp >> model.lp
		#awk '!a[$3]++' model.lp > new.lp;cat new.lp > model.lp
		./gurobi_cl ResultFile=model.sol model.lp
		cur_dff=$(awk 'NR==2{print $5}' model.sol)
		echo $cur_dff, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
		if [ $cur_dff -gt $M ];then #cur critical path failed, go to [   | * ]
			echo "faillllllllllllllllllllllllllllllll"
			is_upper=1
		else #cur critical path succeed, go to [ * |   ]
			echo "successsssssssssssssssssssssssssss"
			is_upper=0
		fi
	done
	cd solution
	mv ../${sol_file}"_fst.sol" .
	mv ../model.sol ${sol_file}"_last.sol"
	mv ../first_cpath ${sol_file}".sol"
	echo $c_path_upper,$i
	echo $c_path_upper >> ${sol_file}".sol"
	echo $min_dff >> ${sol_file}".sol"
	cd ..
}

rm -rf solution
mkdir solution


for file in input_file/*
do
    if test -f $file
    then
	file=$(echo $file | sed 's/input_file\/\(.*\).blif/\1/g')
	echo "======================="
	echo $file
        oneturn $file 8
    fi
done

#./graph_mindff < tmp #model.lp: before bounds part;     bounds.lp: after bounds part
#	cp model.lp model_const.lp 
#	cat bounds.lp >> model.lp
#	./gurobi_cl ResultFile=model.sol model.lp #not knowing critical path
