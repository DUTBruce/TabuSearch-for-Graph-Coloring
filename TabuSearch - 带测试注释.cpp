#include<iostream>
#include<fstream>
#include<vector>
#include<cstdlib> //rand()����ͷ�ļ�
#include<ctime> //clock()����ͷ�ļ�
#include<cmath>
using namespace std;



class TabuSearch
{
public:
    int v_num, e_num; //����ͱߵ���Ŀ������ͼ��
    int K_left, K_right, K, MaxIter, iter; //��ʼȾɫ����(1 - v_num�����ַ�����K)������������
    int f, best_f; //��ǰ�Լ�����conflict number
    vector<vector<int>> graph; //�ڽӾ���洢ͼ�ṹ
    int **adjacent_color = NULL;//�ڽӵĸ���ɫ������ [�������][Ⱦɫ����]
    int *sol = NULL, *bestSol = NULL;//�������ɫ����ǰ�����ʷ���Ž⣬bestSolû�õ�
    int **table = NULL; //tabu tenure table���ɱ�
    int t_start, t_end, now_start, best_t; //��ʼ�ͽ���ʱ�䣬�Լ����Ž��Ǵεĵ���ʱ�䣨���룩
    int tabuTenure;//������
    void TS()
    {
        t_start = clock();
        int u, vi, vj, delt;
        K_left = 1, K_right = v_num, K = 50;
        srand(clock());//���������
        while(K != K_left)
        {
            now_start = clock();
            cout<<"Ⱦɫ"<<K<<endl;
            initialization();
            while(iter++ < MaxIter)
            {
                if(FindMove(u,vi,vj,delt))
                {
                    MakeMove(u,vi,vj,delt);
                    /*cout<<"Move "<<iter<<" times "<<"u:"<<u<<" vi:"<<vi<<" vj:"<<vj<<" delt:"<<delt<<" best f:"<<best_f<<" f:"<<f<<endl;
                    int num=0;
                    for(int i=0; i<v_num; i++)
                        num+=adjacent_color[i][sol[i]];
                    cout<<"�ܳ��ֵ:"<<num/2<<endl;*/
                    if(best_f == 0)
                    {
                        /*����
                        cout<<"Move "<<iter<<" times "<<"u:"<<u<<" vi:"<<vi<<" vj:"<<vj<<" delt:"<<delt<<endl;
                        cout<<"best f:"<<best_f<<" f:"<<f<<endl;
                        cout<<"��ޱ�:"<<endl;
                        for(int i=0; i<v_num; i++)
                        {
                            for(int j=0; j<K; j++)
                                cout<<adjacent_color[i][j]<<" ";
                            cout<<endl;
                        }
                        cout<<"color number is "<<K<<endl;
                        for(int i=0; i<v_num; i++)
                        {
                            cout<<"node: "<<i<<"color :"<<sol[i]<<endl;
                        }
                        int num=0;
                        for(int i=0; i<v_num; i++)
                            num+=adjacent_color[i][sol[i]];
                        cout<<"�ܳ��ֵ:"<<num/2<<endl;*/

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
                cout<<"Ⱦɫ�ɹ���"<<"����������"<<iter<<endl;
                K_right = K;
                K--;
                best_t = clock() - now_start;
                return;
            }
            else
            {
                K_left = K;
                cout<<"������ϣ�Ⱦɫʧ�ܣ�"<<"����������"<<iter-1<<endl;
            }
            /*
            for(int i=0; i<v_num; i++)
            {
                cout<<"node: "<<i<<"color :"<<sol[i]<<endl;
            }
            */
            free();
            //K = (K_left+K_right)/2;
        }
        t_end = clock();
        //Ⱦɫ���K_right
        cout<<"Ŀ��Ⱦɫ������"<<K_right<<endl;

    }
    /*���bestmove��ֹһ�����������ѡȡ��δ�ɹ�ʵ�֣����н������
    bool FindMove(int& u, int& vi, int& vj, int& delt)
    {
        int least_tabu_delt_value = 10000, least_nontabu_delt_value = 10000, now_delt;
        int tabu_count=0, nontabu_count=0;
        //�������move��ֹһ�����������ѡһ��
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
                        /*���i��ɫ��sol[i]���kʱ�ܳ��ֵ�ı仯��
                        ��ͨ����ޱ����ֱ�������������ͨ���ڽӾ����㣬
                        ��Ϊ��ޱ�ļ����Ѿ��������ڽӾ���ĳ����Ϣ��
                        now_delt = adjacent_color[i][k] - adjacent_color[i][sol[i]];
                        if(iter >= table[i][k]) //�ǽ���move
                        {
                            if(now_delt < least_nontabu_delt_value) //�����µ���Сֵ���򼯺�����
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
                        else if(now_delt < least_tabu_delt_value) //����move�������������¼��ԭ��д����now_delt < least_nontabu_delt
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
        //best tabu move �Ƿ����� tabu aspiration criterion
        if(least_tabu_delt_value < least_nontabu_delt_value && f+least_tabu_delt_value < best_f)  //��nontabu���Ŷ��ұ���ʷ��ý����
        {
            int choose = rand() % least_tabu_delt.size();
            cout<<tabu_count<<"tabu��"<<least_tabu_delt.size()<<"������ѡ��"<<choose<<endl;
            delt = least_tabu_delt[choose];
            u = best_tabumove_u[choose];
            vi = best_tabumove_vi[choose];
            vj = best_tabumove_vj[choose];
        }
        else if(least_nontabu_delt_value < 10000) //���ڳ�ֵ10000ʱ˵��û�ҵ�
        {
            int choose = rand() % least_nontabu_delt.size();
            cout<<nontabu_count<<"nontabu��"<<least_nontabu_delt.size()<<"������ѡ��"<<choose<<endl;
            delt = least_nontabu_delt[choose];
            u = best_nontabumove_u[choose];
            vi = best_nontabumove_vi[choose];
            vj = best_nontabumove_vj[choose];
        }
        //û�ҵ�move
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
                        /*���i��ɫ��sol[i]���kʱ�ܳ��ֵ�ı仯��
                        ��ͨ����ޱ����ֱ�������������ͨ���ڽӾ����㣬
                        ��Ϊ��ޱ�ļ����Ѿ��������ڽӾ���ĳ����Ϣ��*/
                        now_delt = adjacent_color[i][k] - adjacent_color[i][sol[i]];
                        if(iter >= table[i][k]) //�ǽ���move
                        {
                            if(now_delt < least_nontabu_delt)
                            {
                                least_nontabu_delt = min(least_nontabu_delt, now_delt);
                                best_nontabumove_u = i;
                                best_nontabumove_vi = sol[i];
                                best_nontabumove_vj = k;
                            }

                        }
                        else if(now_delt < least_tabu_delt) //����move�������������¼��ԭ��д����now_delt < least_nontabu_delt
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
        //best tabu move �Ƿ����� tabu aspiration criterion
        if(least_tabu_delt < least_nontabu_delt && f+least_tabu_delt < best_f)  //��nontabu���Ŷ��ұ���ʷ��ý����
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
        //û�ҵ�move
        else
            return false;

        return true;
    }
    void MakeMove(const int& u, const int& vi, const int& vj, const int& delt)
    {
        sol[u] = vj;
        f = f + delt;
        table[u][vi] = iter + f + rand()%tabuTenure + 1; //ΪʲôҪ��iter����Ϊ��ʾ���ʱ�䣬�����ǽ���ʱ��
        if(f < best_f)
        {
            best_f = f;
        }
        for(auto neighbor: graph[u])
        {
            adjacent_color[neighbor][vi]--;
            adjacent_color[neighbor][vj]++;
        }
        /*�������
        cout<<"Move "<<iter<<" times "<<"u:"<<u<<" vi:"<<vi<<" vj:"<<vj<<" delt:"<<delt<<endl;
        cout<<"best f:"<<best_f<<" f:"<<f<<endl;
        cout<<"��ޱ�:"<<endl;
        for(int i=0; i<v_num; i++)
        {
            cout<<i<<": ";
            for(int j=0; j<K; j++)
                cout<<adjacent_color[i][j]<<" ";
            cout<<endl;
        }*/

    }
    void initialization()
    {
        iter = 0;
        MaxIter = 5e8;
        tabuTenure = sqrt(e_num)*50/v_num;
        adjacent_color = new int*[v_num];
        sol = new int[v_num];
        table = new int*[v_num];
        for(int i=0; i<v_num; i++)
        {
            adjacent_color[i] = new int[K];
            table[i] = new int[K];
            sol[i] = 0; //��ʼȾɫ0
            for(int j=0; j<K; j++)
                table[i][j] = 0;
            adjacent_color[i][0] = graph[i].size();
            for(int j=1; j<K; j++)
                adjacent_color[i][j] = 0;
        }
        best_f = f = e_num/2; //��ʼ��Ϊ���������б߶���ͻ����һ����ֻ����һ�Σ�
        //cout<<"initialization completed"<<endl;
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
            //cout<<c<<endl;
            c=fin.peek();
        }
        fin.ignore(7);
        fin>>v_num>>e_num;
        //cout<<v_num<<e_num<<endl;
        graph.resize(v_num);
        fin>>c;
        int v1, v2;
        //cout<<c<<endl;
        while(c=='e' && !fin.eof())
        {
            fin>>v1>>v2;
            graph[v1-1].push_back(v2-1);
            graph[v2-1].push_back(v1-1);
            //cout<<v1<<" "<<v2<<endl;
            fin>>c;
        }
        fin.close();
        /*����ͼ�ṹ�����Ƿ���ȷ
        for(int i=0; i<graph.size(); i++)
        {
            for(int j=0; j<graph[i].size(); j++)
                cout<<graph[i][j]<<" ";
            cout<<endl;
        }*/
    }

};

int main()
{
    /*cout<<clock()<<endl;
    srand(clock());
    cout<<rand()<<endl<<rand()<<endl;
    cout<<clock()<<endl;
    srand(clock());
    cout<<rand()<<endl<<rand()<<endl;*/
    TabuSearch ts[8];
    string path[8] = {  ".\\data\\DSJC125.1.col",
                        ".\\data\\DSJC125.5.col",
                        ".\\data\\DSJC125.9.col",
                        ".\\data\\DSJC250.1.col",
                        ".\\data\\DSJC250.5.col",
                        ".\\data\\DSJC250.9.col",
                        ".\\data\\DSJC500.1.col",
                        ".\\data\\DSJC500.5.col"};

    for(int j=0; j<8; j++)
    {
        int i = 7;
        ts[j].ReadFromFile(path[i]);
        ts[j].TS();
        cout<<"����"<<(ts[j].t_end-ts[j].t_start)/1000.0<<"�룬���һ�ν�����"<<ts[j].best_t/1000.0<<"��"<<endl<<endl;
    }

    /*for(int i=0; i<8; i++)
    {
        cout<<path[i]<<"Ⱦɫ��"<<endl;
        ts[i].ReadFromFile(path[i]);
        ts[i].TS();
        cout<<"����"<<(ts[i].t_end-ts[i].t_start)/1000.0<<"�룬Ŀ�������"<<ts[i].best_t/1000.0<<"��"<<endl<<endl;
    }
    */

    //���ļ���ȡ���ݣ���ȡ����ͱߵ���Ŀ�ͱ���Ϣ
    /*
    ts[0].ReadFromFile(".\\data\\DSJC125.1.col");
    ts[0].TS();
    cout<<"����"<<(ts[0].t_end-ts[0].t_start)/1000.0<<"�룬���һ�ν�����"<<ts[0].best_t/1000.0<<"��"<<endl<<endl;
    ts[1].ReadFromFile(".\\data\\DSJC125.5.col");
    ts[1].TS();
    cout<<"����"<<(ts[1].t_end-ts[1].t_start)/1000.0<<"�룬���һ�ν�����"<<ts[1].best_t/1000.0<<"��"<<endl<<endl;
    ts[2].ReadFromFile(".\\data\\DSJC125.9.col");
    cout<<"����"<<(ts[2].t_end-ts[2].t_start)/1000.0<<"�룬���һ�ν�����"<<ts[2].best_t/1000.0<<"��"<<endl<<endl;
    ts[2].TS();
    ts[3].ReadFromFile(".\\data\\DSJC250.1.col");
    ts[3].TS();
    ts[4].ReadFromFile(".\\data\\DSJC250.5.col");
    ts[4].TS();
    ts[5].ReadFromFile(".\\data\\DSJC250.9.col");
    ts[5].TS();
    ts[6].ReadFromFile(".\\data\\DSJC500.1.col");
    ts[6].TS();
    ts[7].ReadFromFile(".\\data\\DSJC500.5.col");
    ts[7].TS();
    */


}
