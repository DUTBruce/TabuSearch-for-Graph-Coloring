#include<iostream>
#include<fstream>
#include<vector>
#include<cstdlib> //rand()函数头文件
#include<ctime> //clock()函数头文件
#include<cmath>
using namespace std;

class TabuSearch
{
public:
    int v_num, e_num; //顶点和边的数目（无向图）
    int K_left, K_right, K, MaxIter, iter; //初始染色区间(1 - v_num，二分法调整K)，最大迭代次数
    int f, best_f; //当前以及最优conflict number
    vector<vector<int>> graph; //邻接矩阵存储图结构
    int **adjacent_color = NULL;//邻接的各颜色数量表 [结点数量][染色数量]
    int *sol = NULL, *bestSol = NULL;//（结点颜色）当前解和历史最优解，bestSol没用到
    int **table = NULL; //tabu tenure table禁忌表
    int t_start, t_end, now_start, best_t; //开始和结束时间，以及最优解那次的迭代时间（毫秒）
    int tabuTenure;//禁忌期
    void TS()
    {
        t_start = clock();
        int u, vi, vj, delt;
        K_left = 1, K_right = v_num, K = (1+v_num)/2;
        srand(t_start);//随机数种子
        while(K != K_left)
        {
            now_start = clock();
            cout<<"染色"<<K<<endl;
            initialization();
            while(iter++ < MaxIter)
            {
                if(FindMove(u,vi,vj,delt))
                {
                    MakeMove(u,vi,vj,delt);
                    if(best_f == 0)
                    {
                        break;
                    }
                }
                else
                {
                    cout<<"no move found! "<<endl;
                    break;
                }
            }
            if(best_f == 0)
            {
                cout<<"染色成功，"<<"迭代次数："<<iter<<endl;
                K_right = K;
                best_t = clock() - now_start;
            }
            else
            {
                K_left = K;
                cout<<"迭代完毕，染色失败，"<<"迭代次数："<<iter-1<<endl;
            }
            free();
            K = (K_left+K_right)/2;
        }
        t_end = clock();
        //染色结果K_right
        cout<<"目标染色数量："<<K_right<<endl;

    }
    /*如果bestmove不止一个，从中随机选取（未成功实现，运行结果错误）
    bool FindMove(int& u, int& vi, int& vj, int& delt)
    {
        int least_tabu_delt_value = 10000, least_nontabu_delt_value = 10000, now_delt;
        int tabu_count=0, nontabu_count=0;
        //如果最优move不止一个，从中随机选一个
        vector<int> least_tabu_delt, least_nontabu_delt;
        vector<int> best_tabumove_u, best_tabumove_vi, best_tabumove_vj;
        vector<int> best_nontabumove_u, best_nontabumove_vi, best_nontabumove_vj;
        for(int i=0; i<v_num; i++)
        {
            if(adjacent_color[i][sol[i]]>0)
            {
                for(int k=0; k<K; k++)
                {
                    if(k != sol[i])
                    {
                        /*结点i颜色从sol[i]变成k时总仇恨值的变化量
                        （通过仇恨表可以直接算出，不必再通过邻接矩阵算，
                        因为仇恨表的计算已经考虑了邻接矩阵的仇恨信息）
                        now_delt = adjacent_color[i][k] - adjacent_color[i][sol[i]];
                        if(iter >= table[i][k]) //非禁忌move
                        {
                            if(now_delt < least_nontabu_delt_value) //出现新的最小值，则集合重置
                            {
                                least_nontabu_delt_value = now_delt;
                                least_nontabu_delt.resize(1, now_delt);
                                best_nontabumove_u.resize(1, i);
                                best_nontabumove_vi.resize(1, sol[i]);
                                best_nontabumove_vj.resize(1, k);
                                nontabu_count = 1;
                            }
                            else if(now_delt = least_nontabu_delt_value)
                            {
                                least_nontabu_delt.push_back(now_delt);
                                best_nontabumove_u.push_back(i);
                                best_nontabumove_vi.push_back(sol[i]);
                                best_nontabumove_vj.push_back(k);
                                nontabu_count++;
                            }

                        }
                        else if(now_delt < least_tabu_delt_value) //禁忌move，×××错误记录，原本写成了now_delt < least_nontabu_delt
                        {
                            least_tabu_delt_value = now_delt;
                            least_tabu_delt.resize(1, now_delt);
                            best_tabumove_u.resize(1, i);
                            best_tabumove_vi.resize(1, sol[i]);
                            best_tabumove_vj.resize(1, k);
                            tabu_count = 1;
                        }
                        else if(now_delt = least_tabu_delt_value)
                        {
                            least_tabu_delt.push_back(now_delt);
                            best_tabumove_u.push_back(i);
                            best_tabumove_vi.push_back(sol[i]);
                            best_tabumove_vj.push_back(k);
                            tabu_count++;
                        }
                    }
                }
            }
        }
        //best tabu move 是否满足 tabu aspiration criterion
        if(least_tabu_delt_value < least_nontabu_delt_value && f+least_tabu_delt_value < best_f)  //比nontabu更优而且比历史最好解更好
        {
            int choose = rand() % least_tabu_delt.size();
            cout<<tabu_count<<"tabu从"<<least_tabu_delt.size()<<"个数中选择"<<choose<<endl;
            delt = least_tabu_delt[choose];
            u = best_tabumove_u[choose];
            vi = best_tabumove_vi[choose];
            vj = best_tabumove_vj[choose];
        }
        else if(least_nontabu_delt_value < 10000) //等于初值10000时说明没找到
        {
            int choose = rand() % least_nontabu_delt.size();
            cout<<nontabu_count<<"nontabu从"<<least_nontabu_delt.size()<<"个数中选择"<<choose<<endl;
            delt = least_nontabu_delt[choose];
            u = best_nontabumove_u[choose];
            vi = best_nontabumove_vi[choose];
            vj = best_nontabumove_vj[choose];
        }
        //没找到move
        else
            return false;

        return true;
    }*/
    bool FindMove(int& u, int& vi, int& vj, int& delt)
    {
        int least_tabu_delt = 10000, least_nontabu_delt = 10000, now_delt;
        int best_tabumove_u, best_tabumove_vi, best_tabumove_vj;
        int best_nontabumove_u, best_nontabumove_vi, best_nontabumove_vj;
        for(int i=0; i<v_num; i++)
        {
            if(adjacent_color[i][sol[i]]>0)
            {
                for(int k=0; k<K; k++)
                {
                    if(k != sol[i])
                    {
                        /*结点i颜色从sol[i]变成k时总仇恨值的变化量
                        （通过仇恨表可以直接算出，不必再通过邻接矩阵算，
                        因为仇恨表的计算已经考虑了邻接矩阵的仇恨信息）*/
                        now_delt = adjacent_color[i][k] - adjacent_color[i][sol[i]];
                        if(iter >= table[i][k]) //非禁忌move
                        {
                            if(now_delt < least_nontabu_delt)
                            {
                                least_nontabu_delt = min(least_nontabu_delt, now_delt);
                                best_nontabumove_u = i;
                                best_nontabumove_vi = sol[i];
                                best_nontabumove_vj = k;
                            }

                        }
                        else if(now_delt < least_tabu_delt) //禁忌move，×××错误记录，原本写成了now_delt < least_nontabu_delt
                        {
                            least_tabu_delt = min(least_tabu_delt, now_delt);
                            best_tabumove_u = i;
                            best_tabumove_vi = sol[i];
                            best_tabumove_vj = k;
                        }
                    }
                }
            }
        }
        //best tabu move 是否满足 tabu aspiration criterion
        if(least_tabu_delt < least_nontabu_delt && f+least_tabu_delt < best_f)  //比nontabu更优而且比历史最好解更好
        {
            delt = least_tabu_delt;
            u = best_tabumove_u;
            vi = best_tabumove_vi;
            vj = best_tabumove_vj;
        }
        else if(least_nontabu_delt < 100)
        {
            delt = least_nontabu_delt;
            u = best_nontabumove_u;
            vi = best_nontabumove_vi;
            vj = best_nontabumove_vj;
        }
        //没找到move
        else
            return false;

        return true;
    }
    void MakeMove(const int& u, const int& vi, const int& vj, const int& delt)
    {
        sol[u] = vj;
        f = f + delt;
        table[u][vi] = iter + f + rand()%tabuTenure + 1; //为什么要加iter？因为表示解禁时间，而不是禁忌时间
        if(f < best_f)
        {
            best_f = f;
        }
        for(auto neighbor: graph[u])
        {
            adjacent_color[neighbor][vi]--;
            adjacent_color[neighbor][vj]++;
        }
    }
    void initialization()
    {
        iter = 0;
        MaxIter = e_num*100;
        tabuTenure = sqrt(e_num)*50/v_num;
        adjacent_color = new int*[v_num];
        sol = new int[v_num];
        table = new int*[v_num];
        for(int i=0; i<v_num; i++)
        {
            adjacent_color[i] = new int[K];
            table[i] = new int[K];
            sol[i] = 0; //初始染色0
            for(int j=0; j<K; j++)
                table[i][j] = 0;
            adjacent_color[i][0] = graph[i].size();
            for(int j=1; j<K; j++)
                adjacent_color[i][j] = 0;
        }
        best_f = f = e_num/2; //初始化为边数（所有边都冲突，但一条边只计算一次）
    }
    void free()
    {
        for(int i=0; i<v_num; i++)
        {
            delete []adjacent_color[i];
            delete []table[i];
        }
        delete []adjacent_color;
        delete []table;
        delete []sol;
    }
    void ReadFromFile(string path)
    {
        ifstream fin(path);
        char line[100];
        char c=fin.peek();
        while(c!='p')
        {
            fin.getline(line,100);
            c=fin.peek();
        }
        fin.ignore(7);
        fin>>v_num>>e_num;
        graph.resize(v_num);
        fin>>c;
        int v1, v2;
        while(c=='e' && !fin.eof())
        {
            fin>>v1>>v2;
            graph[v1-1].push_back(v2-1);
            graph[v2-1].push_back(v1-1);
            fin>>c;
        }
        fin.close();
    }

};

int main()
{
    TabuSearch ts[8];
    string path[8] = {  ".\\data\\DSJC125.1.col",
                        ".\\data\\DSJC125.5.col",
                        ".\\data\\DSJC125.9.col",
                        ".\\data\\DSJC250.1.col",
                        ".\\data\\DSJC250.5.col",
                        ".\\data\\DSJC250.9.col",
                        ".\\data\\DSJC500.1.col",
                        ".\\data\\DSJC500.5.col"};
    for(int i=0; i<8; i++)
    {
        cout<<path[i]<<"染色："<<endl;
        ts[i].ReadFromFile(path[i]);
        ts[i].TS();
        cout<<"运行"<<(ts[i].t_end-ts[i].t_start)/1000.0<<"秒，目标解运行"<<ts[i].best_t/1000.0<<"秒"<<endl<<endl;
    }

}
