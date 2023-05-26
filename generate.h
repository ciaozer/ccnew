#ifndef _GENERATE_H_
#define _GENERATE_H_

#include"data_structure.h"
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include"build.h"

#define WEIGHT 1
#define MAX_WEIGHT 100
#define RAND 1000

int ITEMNUM = 200;
int ELEMENTNUM = 1000;
double DENSITY = 0.1;
int AVG_COVER_TIMES = 50;

int *weight_for_generate;        //用于生成数据
vector<vector<bool>> conflict_graph;

struct Item
{
    int covernum;                // the size of the elements it can cover
    int conflict_num;            // the number of items it conflicts with
    int candidate_conflict_num;  // conflict times between candidates
    int cover_of_candidate;      // how many elements a candidate can cover, 0 if not in candidate
    unordered_set<int> elements; // which elements it can cover
};

unordered_map<int, Item> data;

void generate_weight()
{
    switch (WEIGHT)
    {
    case 0:
        for (int i = 1; i <= elementnum; i++)
        {
            weight_for_generate[i] = 1;
        }
        break;

    case 1:
        weight_for_generate = new int[ELEMENTNUM+10];
        for (int i = 0; i < ELEMENTNUM; i++)
        {
            int rand_weight = (random() % MAX_WEIGHT) + 1;
            weight_for_generate[i] = rand_weight;
        }
        break;

    case 2:
        break;

    case 3:
        break;

    default:
        break;
    }
}

void generate_conflict_graph()
{
    conflict_graph = vector<vector<bool>>(ITEMNUM+1, vector<bool>(ITEMNUM+1, false));
    // if not initial, the conflict_num will lose 1, but i don't know why
    for (auto it = data.begin(); it != data.end(); it++)
    {
        it->second.conflict_num = 0;
    }
    double tmp;
    for (int i = 1; i <= ITEMNUM; i++)
        for (int j = i + 1; j <= ITEMNUM; j++)
        {
            tmp = random() % RAND / (double)RAND;
            if (tmp < DENSITY)
            {
                conflict_graph[i][j] = true;
                conflict_graph[j][i] = true;
                data[i].conflict_num++;
                data[j].conflict_num++;
            }
        }
}

void generate_data(string filename)
{
    ofstream outfile;
    outfile.open(filename, ios::out);
    if (!outfile.is_open())
    {
        cout << "read file error" << endl;
        cout << "i am generate_data";
        return;
    }

    outfile << ITEMNUM << " " << ELEMENTNUM << endl;

    // output weight
    int cnt = 0;
    for ( int i=0; i<ELEMENTNUM; i++, cnt++ )
    {
        outfile << weight_for_generate[i] << " ";
        if (cnt % 50 == 49)
            outfile << endl;
    }
    outfile << endl;

    for (int i = 1; i <= ITEMNUM; i++)
    {
        Item itemtmp = data[i];

        // show the item number and what elements it covers
        outfile << i << " " << itemtmp.covernum << endl;
        for (auto it = itemtmp.elements.begin(); it != itemtmp.elements.end(); it++)
        {
            outfile << *it << " ";
        }
        outfile << endl;

        // show what items it conflicts with
        outfile << itemtmp.conflict_num << endl;
        for (int j = 1; j <= ITEMNUM; j++)
            if (conflict_graph[i][j])
                outfile << j << " ";
        outfile << endl;
    }
    outfile.close();
}

void generate_random_once(string filename)
{
    itemnum = ITEMNUM;
    elementnum = ELEMENTNUM;
    int avg_cover_num = elementnum / itemnum * AVG_COVER_TIMES;

    generate_weight();

    for (int i = 1; i <= itemnum; i++)
    {
        Item itemtmp;

        int rand_covernum = random() % (2*avg_cover_num);
        // cout << rand_covernum << endl;
        itemtmp.covernum = rand_covernum;

        // generate cover elements
        unordered_set<int> rand_elements;
        while (rand_covernum)
        {
            // element conts from 1
            int element = random() % elementnum;
            // element repetitiom
            if (rand_elements.count(element))
            {
                continue;
            }
            rand_covernum--;
            rand_elements.insert(element);
        }
        itemtmp.elements = rand_elements;
        data[i] = itemtmp;
    }

    // generate cover first or conflict message will lose.3
    generate_conflict_graph();
    generate_data(filename);
    delete[] weight_for_generate;

    cout << "finish generate" << endl;
}

void change_to_maxsat(char *input_filename, char *output_filename)
{
    build(input_filename);

    const int INF = total_weight + 1;

    int edgenum = 0;

    for ( int i=1; i<=itemnum; i++ )
    {
        edgenum += g_item_count[i];
    }
    edgenum = edgenum / 2;        //会重复计算所以除以2

    ofstream outfile;
    outfile.open(output_filename, ios::out);
    if (!outfile.is_open())
    {
        cout << "read file error" << endl;
        cout << "i am change_to_maxsat" << endl;
        return;
    }

    outfile << "p wcnf " << itemnum << " " << edgenum + elementnum << " " << INF << endl;

    for (int i = 1; i <= itemnum; i++)
    {
        for ( int j=0; j<g_item_count[i]; j++ )
        {
            if ( g_item[i][j] > i )         //防止重复输出
            {
                outfile << INF << " "
                << "-" << i << " "
                << "-" << g_item[i][j] << " " << 0 << endl;
            }
        }
    }

    for (int i = 0; i < elementnum; i++)
    {
        outfile << original_weight[i] << " ";
        for ( int j=0; j<n_ele_count[i]; j++ )
        {
            outfile << n_ele[i][j] << " ";
        }
        outfile << 0 << endl;
    }

    outfile.close();
}

#endif