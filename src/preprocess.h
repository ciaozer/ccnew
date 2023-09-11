#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include"data_structure.h"
#include<unordered_set>
#include<unordered_map>
#include<cstring>
#include<vector>
#include<algorithm>
#include<queue>

unordered_set<int> item_pre;
unordered_set<int> element_pre;
unordered_map<int, unordered_set<int>> m_item_pre;
unordered_map<int, unordered_set<int>> n_ele_pre;
unordered_map<int, unordered_set<int>> g_item_pre;
int *old_to_new_item;
int *new_to_old_item;
int *old_to_new_element;
int *new_to_old_element;
vector<pair<int, unordered_set<int>>> vec_tmp;
vector<unordered_set<int>> component;
vector<int> index_of_component;

bool item_rule1(int item);
bool item_rule2(int item);

bool compare_up(const pair<int, unordered_set<int>>& a, const pair<int, unordered_set<int>>& b) 
{
    return a.second.size() < b.second.size();
}

bool compare_down(const pair<int, unordered_set<int>>& a, const pair<int, unordered_set<int>>& b) 
{
    return a.second.size() > b.second.size();
}

bool is_subset(unordered_set<int>& set1, unordered_set<int>& set2)
{//要求set1的size小于等于set2，如果1是2的子集，返回真
    for( auto it=set1.begin(); it!=set1.end(); it++ )
    {
        if( set2.find(*it) == set2.end() )
        {
            return false;
        }
    }
    return true;
}

bool is_gsubset(int item1, int item2)
{//要求set1的size小于等于set2，如果1是2的子集，返回真, 删除对方后是否为子集关系, a为对方
    for( auto it=g_item_pre[item1].begin(); it!=g_item_pre[item1].end(); it++ )
    {
        if( *it == item2 )
        {
            continue;
        }
        if( g_item_pre[item2].find(*it) == g_item_pre[item2].end() )
        {
            return false;
        }
    }
    return true;
}

void mapping_node()
{//建立集合元素的重新映射
    old_to_new_item = new int[itemnum+10];
    new_to_old_item = new int[itemnum+10];
    old_to_new_element = new int[elementnum+10];
    new_to_old_element = new int[elementnum+10];
    vector<int> item_tmp;
    vector<int> element_tmp;

    item_tmp.assign(item_pre.begin(), item_pre.end());
    sort(item_tmp.begin(), item_tmp.end());
    for( int i=0; i<item_tmp.size(); i++ )
    {
        new_to_old_item[i+1] = item_tmp[i];
        old_to_new_item[item_tmp[i]] = i+1;
    }

    element_tmp.assign(element_pre.begin(), element_pre.end());
    sort(element_tmp.begin(), element_tmp.end());
    for( int i=0; i<element_tmp.size(); i++ )
    {
        new_to_old_element[i] = element_tmp[i];
        old_to_new_element[element_tmp[i]] = i;
    }
}

void write_pre_result(char *filename)
{//将预处理的结果文件写入filename
    ofstream outfile;
    outfile.open(pre_filename, ios::out);
    if (!outfile.is_open())
    {
        cout << "read file error" << endl;
        cout << "i am write_pre_result" << endl;
        return;
    }
    outfile << item_pre.size() << " " << element_pre.size() << endl;

    int cnt = 0;
    for ( int i=0; i<element_pre.size(); i++, cnt++ )
    {
        int cur_element = new_to_old_element[i];
        outfile << original_weight[cur_element] << " ";
        if (cnt % 50 == 49)
            outfile << endl;
    }
    outfile << endl;

    for( int i=1; i<=item_pre.size(); i++ )
    {
        int real_item = new_to_old_item[i];
        outfile << i << " " << m_item_pre[real_item].size() << endl;
        for( auto it=m_item_pre[real_item].begin(); it!=m_item_pre[real_item].end(); it++ )
        {
            int cur_element = *it;
            outfile << old_to_new_element[cur_element] << " ";
        }

        outfile << endl << g_item_pre[real_item].size() << endl;
        for( auto it=g_item_pre[real_item].begin(); it!=g_item_pre[real_item].end(); it++ )
        {
            int cur_item = *it;
            outfile << old_to_new_item[cur_item] << " ";
        }
        outfile << endl;
    }
}

void build(char *filename)
{
    ifstream infile;
    infile.open(filename, ios::in);
    if( !infile.is_open() )
    {
        cout << "read file " << filename << "error" << endl;
        exit(-1);
    }
    strcpy(pre_filename, "pre");
    // strcat(pre_filename, filename);

    infile >> itemnum >> elementnum;
    original_weight = new long long[elementnum+10];

    edgenum = 0;
    current_uncovered_weight = 0;
    for( int i=0; i<elementnum; i++ )
    {
        infile >> original_weight[i];
        element_pre.insert(i);
    }

    current_uncovered_weight = total_weight;
    best_uncovered_weight = total_weight;

    for( int i=1; i<=itemnum; i++ )
    {
        int item_index, m_count, g_count;
        infile >> item_index;
        item_pre.insert(i);

        //读入该集合覆盖的元素
        infile >> m_count;

        for( int ele=0; ele<m_count; ele++ )
        {
            int cur_element;
            infile >> cur_element;
            n_ele_pre[cur_element].insert(i);
            m_item_pre[i].insert(cur_element);
        }

        //读入该集合冲突的集合
        infile >> g_count;
        edgenum += g_count;
        //g_adj[i] = new int[g_item_count[i]+1];

        for( int it=0; it<g_count; it++ )
        {
            int cur_item;
            infile >> cur_item;
            g_item_pre[i].insert(cur_item);
        }
    }

    infile.close();

    // edgenum = edgenum / 2;        //会重复计算所以除以2

	// int edge_index = 0;
	// for( int i=1; i<=itemnum; i++ )
    // {
	// 	for( int j=0; j<g_item_count[i]; j++ )
	// 	{
	// 		if( g_item[i][j] > i )
	// 		{
    //             edge[edge_index] = new int[3];
    //             edge[edge_index][0] = i;
    //             edge[edge_index][1] = g_item[i][j];
    //             g_adj[i][g_adj_count[i]] = edge_index;
    //             g_adj_count[i]++;
    //             g_adj[g_item[i][j]][g_adj_count[g_item[i][j]]] = edge_index;
    //             g_adj_count[g_item[i][j]]++;
	// 			edge_index++;
	// 		}
	// 	}
    // }   
}

void cover_element(int element)
{
    element_pre.erase(element);

    //处理所有能覆盖它的集合
    for( auto it=n_ele_pre[element].begin(); it!=n_ele_pre[element].end(); it++ )
    {
        m_item_pre[*it].erase(element);
        //item_rule2(*it);
    }
    n_ele_pre.erase(element);
}

void uncover_element(int element)
{
    element_pre.erase(element);

    //处理所有能覆盖它的集合
    for( auto it=n_ele_pre[element].begin(); it!=n_ele_pre[element].end(); it++ )
    {
        m_item_pre[*it].erase(element);
        //item_rule2(*it);
    }
    n_ele_pre.erase(element);

    current_uncovered_weight += original_weight[element];
}

void delete_item(int deleted_item)
{
    item_pre.erase(deleted_item);

    //处理与它冲突的集合
    for( auto it=g_item_pre[deleted_item].begin(); it!=g_item_pre[deleted_item].end(); it++ )
    {
        g_item_pre[*it].erase(deleted_item);
        //item_rule1(*it);
    }
    g_item_pre.erase(deleted_item);

    //处理它能覆盖的元素
    for( auto it=m_item_pre[deleted_item].begin(); it!=m_item_pre[deleted_item].end(); it++ )
    {
        n_ele_pre[*it].erase(deleted_item);
    }
    m_item_pre.erase(deleted_item);
}

void add_item(int added_item)
{
    item_pre.erase(added_item);
    
    //删除所有的冲突邻居
    while( !g_item_pre[added_item].empty() )
    {
        cout << "delete item " << *g_item_pre[added_item].begin() << " by add" << endl;
        delete_item(*g_item_pre[added_item].begin());
    }
    g_item_pre.erase(added_item);

    //覆盖所有它覆盖的元素
    while( !m_item_pre[added_item].empty() )
    {
        cover_element(*m_item_pre[added_item].begin());
    }
    m_item_pre.erase(added_item);
}

bool item_rule1(int item)
{//选中g_item=0的集合
    if(g_item_pre[item].size() == 0)
    {
        cout << "select item " << item << " by item rule 1" << endl;
        add_item(item);
        return true;
    }
    return false;
}

bool item_rule2(int item)
{//删除m_item为0的集合
    if( m_item_pre[item].size() == 0 )
    {
        cout << "delete item " << item << " by item rule 2" << endl;
        delete_item(item);
        return true;
    }
    return false;
}

bool item_rule3(int item1, int item2)
{//如果m1>=m2,g1<=g2(不包括对方)，删除2,传入的时候保证m1.size>=m2.size
    if( is_gsubset(item1, item2) && is_subset(m_item_pre[item2], m_item_pre[item1]) )
    {
        cout << "delete item " << item2 << " by item rule 3" << endl;
        delete_item(item2);
        return true;
    }
    return false;
}

bool item_rule4(int item)
{//如果该集合能覆盖所有的冲突邻居，选入该集合
    for( auto it=g_item_pre[item].begin(); it!=g_item_pre[item].end(); it++ )
    {
        int cur_item = *it;
        if( !is_subset(m_item_pre[cur_item], m_item_pre[item]) )
        {
            return false;
        }
    }
    cout << "select item " << item << " by item rule 4" << endl;
    add_item(item);
    return true;
}

bool item_rule5(int item)
{//传入的集合只与一个结合冲突，删掉两者都能覆盖的节点
    bool flag = false;
    int neighbor = *g_item_pre[item].begin();
    if( m_item_pre[item].size() > m_item_pre[neighbor].size() )
    {
        for( auto it=m_item_pre[neighbor].begin(); it!=m_item_pre[neighbor].end(); )
        {
            auto itnext = next(it, 1);
            if( m_item_pre[item].find(*it) != m_item_pre[item].end() )
            {
                cout << "cover element " << *it << " by item rule 5" << ", the item is " << item << endl;
                cover_element(*it);
                flag = true;
            }
            it = itnext;
        }
    }
    else 
    {
        for( auto it=m_item_pre[item].begin(); it!=m_item_pre[item].end(); )
        {
            auto itnext = next(it, 1);
            if( m_item_pre[neighbor].find(*it) != m_item_pre[neighbor].end() )
            {
                cout << "cover element " << *it << " by item rule 5" << ", the item is " << item << endl;
                cover_element(*it);
                flag = true;
            }
            it = itnext;
        }
    }
    return flag;
}

bool element_rule1(int element)
{  //删除n_ele为0的元素
    if( n_ele_pre[element].size() == 0 )
    {
        uncover_element(element);
        return true;
    }
    return false;
}

bool element_rule2(int element1, int element2)
{
    if( n_ele_pre[element1].size() != n_ele_pre[element2].size() )
    {
        return false;
    }
    if( is_subset(n_ele_pre[element1], n_ele_pre[element2]) )
    {
        original_weight[element2] += original_weight[element1];
        cover_element(element1);
        cout << "delete element " << element1 << " by element rule 2" << endl;
        return true;
    }
    return false;
}

bool element_rule3(int element1, int element2)
{//保证n_ele 1的大小小于2,如果存在子集关系，wi += wj, n_elej -= n_elei, 需要注意n_ele i不能为空, 剩余两个部分应该全连接，确保不能两个都选
    if( is_subset(n_ele_pre[element1], n_ele_pre[element2]) )
    {
        if( n_ele_pre[element1].size() == n_ele_pre[element2].size() )
        {   //两个集合完全一样，删掉一个
            original_weight[element2] += original_weight[element1];
            cover_element(element1);
        }
        else
        {
            unordered_set<int> tmp_n;
            for( auto it=n_ele_pre[element2].begin(); it!=n_ele_pre[element2].end(); it++ )
            {
                if( n_ele_pre[element1].find(*it) == n_ele_pre[element1].end() )
                {
                    tmp_n.insert(*it);
                }
            }
            for( auto it=tmp_n.begin(); it!=tmp_n.end(); it++ )
            {
                for( auto it1=n_ele_pre[element1].begin(); it1!=n_ele_pre[element1].end(); it1++ )
                {
                    if( g_item_pre[*it1].find(*it) == g_item_pre[*it1].end() )
                    {
                        return false;
                    }
                }
            }
            original_weight[element1] += original_weight[element2];
            for( auto it=n_ele_pre[element1].begin(); it!=n_ele_pre[element1].end(); it++ )
            {
                n_ele_pre[element2].erase(*it);
                m_item_pre[*it].erase(element2);
            }
        }
        cout << "element rule 2 worked " << element1 << " " << element2 << endl;
        return true;
    }
    return false;
}

bool element_rule4(int element, vector<int> index)
{//判断该元素是否一定会被覆盖，找到一个小子图，选中所有不覆盖该元素的集合，如果还能加入，则一定能被覆盖
    //先判断是否存在50以下的子图，将满足条件的子图放入index中
    for( int i=0; i<index.size(); i++ )
    {
        bool flag = 1; //为0表示这个情况已经排除了
        unordered_set<int> cur_component = component[index[i]];
        unordered_set<int> copy;
        for( auto it=cur_component.begin(); it!=cur_component.end(); it++ )
        {
            copy.insert(*it);
        }
        for( auto it=cur_component.begin(); it!=cur_component.end(); it++ )
        {
            if( n_ele_pre[element].find(*it) != n_ele_pre[element].end() )
            {//当前集合覆盖了该元素，不处理
                continue;
            }
            copy.erase(*it);    //选入不覆盖该元素的该集合
            for( auto it1=g_item_pre[*it].begin(); it1!=g_item_pre[*it].end(); it1++ )
            {//遍历该集合的所有邻居
                if( n_ele_pre[element].find(*it1) == n_ele_pre[element].end() )
                {   //如果邻居也不覆盖，不处理这种情况
                    flag = 0;
                    break;
                }
                copy.erase(*it1);
            }
            if( flag == 0 )
            {
                break;
            }
        }
        if( flag == 0 )
        {
            continue;
        }
        if( !copy.empty() )
        {
            cout << "cover element " << element << " by element rule 4" << endl;
            cover_element(element);
            return true;
        }
    }

    return false;
}

unordered_set<int> bfs(int item)
{//找到item对应的连通分量
    queue<int> q;
    q.push(item);
    unordered_set<int> cur_component;
    cur_component.insert(item);
    while( !q.empty() )
    {
        int cur_item = q.front();
        q.pop();
        for( auto it=g_item_pre[cur_item].begin(); it!=g_item_pre[cur_item].end(); it++ )
        {
            if( cur_component.find(*it) == cur_component.end() )
            { //还没有被加入当前连通分量
                q.push(*it);
                cur_component.insert(*it);
            }
        }
    }

    return cur_component;
}

void find_component(unordered_set<int> item_set)
{//寻找当前冲突图的所有连通分量
    component.clear();
    while( !item_set.empty() )
    {
        int cur_item = *item_set.begin();
        unordered_set<int> cur_component;
        cur_component = bfs(cur_item);
        component.push_back(cur_component);
        for( auto it=cur_component.begin(); it!=cur_component.end(); it++ )
        {
            item_set.erase(*it);
        }
    }
}

void preprocess()
{
    bool used = false;
    bool total_used = false;
    while(1) //不常用规则
    {
        total_used = false;    
        while(1) //常用规则
        {  
            used = false;

            //找到所有g_item=0和m_item=0
            for( auto it=item_pre.begin(); it!=item_pre.end(); )
            {
                int cur_item = *it;
                auto itnext = next(it, 1);
                if( item_rule1(cur_item) )
                {
                    used = true;               
                }
                else if( item_rule2(cur_item) )
                {
                    used = true;
                }
                if( item_pre.empty() )
                {
                    break;
                }
                it = itnext;
            }

            //找到一个m_item和g_item子集关系
            vec_tmp.assign(m_item_pre.begin(), m_item_pre.end());
            sort(vec_tmp.begin(), vec_tmp.end(), compare_up);
            for( int i=0; i<vec_tmp.size(); i++ )
            {
                if( used == true )
                {
                    break;
                }
                int cur_item = vec_tmp[i].first;
                if( vec_tmp[i].second.size() == 0 )
                {
                    item_rule2(cur_item);
                }
                else
                {
                    int cur_ele = *vec_tmp[i].second.begin();//覆盖同一个元素才能成为子集关系
                    for( auto it=n_ele_pre[cur_ele].begin(); it!=n_ele_pre[cur_ele].end(); it++ )
                    {
                        if( *it == cur_item )
                        {
                            continue;
                        }
                        if( m_item_pre[cur_item].size() > m_item_pre[*it].size() )
                        {
                            continue;
                        }
                        if( item_rule3(*it, cur_item) )
                        {
                            used = true;
                            break;
                        }
                    }
                }
            }

            //寻找超级集合，同时处理冲突图中度为1的点使用rule 5
            if( used == false )
            {
                for( int i=0; i<vec_tmp.size(); i++ )
                {
                    if( g_item_pre[vec_tmp[i].first].size() == 1 )
                    {
                        if( item_rule5(vec_tmp[i].first) )
                        {
                            used = true;
                        }
                    }
                    if( item_rule4(vec_tmp[i].first) )
                    {
                        used = true;
                        break;
                    }
                }
            }

            //判断是否有过程有效
            if( used == false )
            {
                break;
            }
        }
        find_component(item_pre);
        index_of_component.clear();
        for( int i=0; i<component.size(); i++ )
        {
            if( component[i].size() <= 50 )
            {
                index_of_component.push_back(i);
            }
        }
        if( !index_of_component.empty() )
        {
            for( auto it=element_pre.begin(); it!=element_pre.end(); )
            {
                auto itnext = next(it, 1);
                if( element_rule4(*it, index_of_component) )
                {
                    total_used = true;
                }
                it = itnext;
            }
        }
        if( total_used == false )
        {
            break;
        }
    }

    //删除n_ele=0和完全相同的
    vec_tmp.assign(n_ele_pre.begin(), n_ele_pre.end());
    sort(vec_tmp.begin(), vec_tmp.end(), compare_up);
    for( int i=0; i<vec_tmp.size(); i++ )
    {
        int cur_ele = vec_tmp[i].first;
        if( vec_tmp[i].second.size() == 0 )
        {
            element_rule1(cur_ele);
        }
        else
        {
            for( int j=i+1; j<vec_tmp.size(); j++ )
            {
                if( vec_tmp[j].second.size() > vec_tmp[i].second.size() )
                {
                    break;
                }
                element_rule2(vec_tmp[i].first, vec_tmp[j].first);
                if( get_runtime() > 60 )
                {
                    break;
                }
            }
        }
        if( get_runtime() > 60 )
        {
            break;
        }
    }
    
    mapping_node();
    write_pre_result(pre_filename);
    delete[] old_to_new_item;
    delete[] new_to_old_item;
    delete[] old_to_new_element;
    delete[] new_to_old_element;
    delete[] original_weight;
    
    cout << "finish preprocess" << endl;
    cout << current_uncovered_weight << endl;
}

#endif