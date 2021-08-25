Source code for the following paper:

Zhijing Li, Zhao Chen, Yili Zhang, Zixin Huang, and Weikang Qian, "[Simultaneous area and latency optimization for stochastic circuits by D flip-flop insertion](https://umji.sjtu.edu.cn/~wkqian/papers/Li_Chen_Zhang_Huang_Qian_Simultaneous_Area_and_Latency_Optimization_for_Stochastic_Circuits_by_D_Flip-flop_Insertion.pdf)," 
in *IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems*, vol. 38, no. 7, pp. 1251-1264, 2019.

dependency:需要安装gorubi

运行one.sh可以得到最少的DFF:
1. insert2 标明了functional equivalence constraints
	加上objective function 一起存入model.lp
2. 调用 gurobi_cl 把ILP解出来，完整的解存在model.sol

运行run.sh可以得到给定DFF的时候，最小的critcal path。
1. 第一遍还是先调用insert2和gurobi_cl解出最初的model.lp和DFF
2. 然后通过反复调用insert3更新constraint on critical path得到最小的critical path。
3. 上面两步都在run.sh的oneturn函数里执行了，主函数就是读取input_file文件夹下面所有的文件，用oneturn跑一遍，然后结果存入solution文件夹里。

solution文件夹后缀ax+b的含义: a代表读入的函数是x的a次方，b代表DFF+b作为给定DFF，求此条件下的critical path

注意，这里给定的DFF，作为上限，是根据读入的DFF + b 得到的，b 是在DFFinsertion2中得到的，代码长这样：

    ofstream df;
    df.open("dff");
    df<<x_num+2<<endl;//"2" here is a parameter that one can be tuned
    df.close();

所以，不是很智能……需要每次make以后才会生效。
勤快的人可以改一改。
