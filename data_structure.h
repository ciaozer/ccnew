#ifndef _DATA_STRUCTURE_H_
#define _DATA_STRUCTURE_H_

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

#define pop(stack) stack[--stack ## _count]
#define push(item, stack) stack[stack ## _count++] = item

//处理随机数精度
const int   MY_RAND_MAX_INT = 10000000;
const float BASIC_SCALE     = 0.0000001; 

//尝试次数与时间
int             seed;
int             max_tries = 1000;
unsigned int    max_flips = 4000000000u;
unsigned int    step;
int             cutoff_time;
struct tms start, stop;

//算例的基本性质，初始化后不再改变
int     itemnum;		
int 	elementnum;
int     edgenum;
int*    m_item_count;           //每个集合覆盖的元素数
int**   m_item;                 //m_item[i][j]表示i号集合覆盖j号元素
int*    g_item_count;           //每个集合的冲突个数
int**   g_item;                 //g_item[i][j]表示i号集合与j号集合冲突
int*    n_ele_count;            //每个元素被覆盖的次数
int**   n_ele;                  //n_ele[i][j]表示i号元素被j号集合覆盖
int**   edge;                   //冲突图边数
int**   g_adj;                  //g_adj[i][j]表示i号item的第j条冲突边的编号
int*    g_adj_count;
int**   neighbor;
int*    neighbor_count;
long long*  original_weight;
long long   total_weight;     //所有元素的权重和

//解的记录
int*        current_solution;
int*        best_solution;
long long   best_uncovered_weight;
long long   current_uncovered_weight;
int*        solution_stack;
int         solution_stack_count;
int*        index_in_solution_stack;

//概率与控制
float   rdprob;
int     hd_count_threshold;
float   p_without_hard_conf;
float   p_random_walk;

//冲突栈，未覆盖栈
int*	conflict_edge_stack;		
int		conflict_edge_stack_count;
int*	index_in_conflict_edge_stack;
int*	uncovered_stack;		
int		uncovered_stack_count;
int*	index_in_softunsat_stack;
int*    conflict_stack;
int     conflict_stack_count;
int*    index_in_conflict_stack;
bool*   is_in_conflict_stack;
int*	goodvar_stack;		
int		goodvar_stack_count;
int*	already_in_goodvar_stack;

//需要维护的变量
int*        element_cover_times;   //元素被覆盖的次数
int*        item_conflict_times;
long long*	time_stamp;
long long*  score;
int*	    conf_change;
int*	    hard_cscc;
int*        sat_count;			
int*	    sat_var;		
int*        best_array;
int         best_count;
unsigned int* already_in_ccmpvars;
int*        ccmpvars;
int         ccmpvars_count;

#endif