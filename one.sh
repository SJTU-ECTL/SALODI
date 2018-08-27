rm model.lp
test_file="input_file/sinpix.blif"
./insert2 $test_file 8

cat model_.lp >> model.lp
./gurobi_cl ResultFile=model.sol model.lp
