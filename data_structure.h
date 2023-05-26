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

using namespace std;

#define pop(stack) stack[--stack ## _count]
#define push(item, stack) stack[stack ## _count++] = item

const int MY_RAND_MAX_INT = 10000000;
const float BASIC_SCALE = 0.0000001; 
int max_tries = 1000;
unsigned int max_flips = 4000000000u;
unsigned int step;
int cutoff_time;
long long*	time_stamp;

struct tms start, stop;

struct lit {
    int             clause_num;		
    int             var_num;		
    int             sense;			
};

int     itemnum;		
int 	elementnum;
int     num_clauses;		
int     num_hclauses;
int     num_sclauses;
				
lit**	var_lit;				
int*	var_lit_count;			
lit**	clause_lit;			
int*	clause_lit_count; 						

long long*    item_conflict_times;
long long*    score;

int*	conf_change;
int**	var_neighbor;
int*	var_neighbor_count;

int*	neighbor_flag;
int* temp_neighbor;
int temp_neighbor_count;

int*	hard_cscc;

long long     top_clause_weight;
long long*    org_clause_weight;	

int*    sat_count;			
int*	sat_var;		

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
int     edgenum;
int**   edge;
int**   g_adj;
int*    g_adj_count;

int*	goodvar_stack;		
int		goodvar_stack_count;
int*	already_in_goodvar_stack;

int*    current_solution;
int*    best_solution;
long long best_uncovered_weight;
long long current_uncovered_weight;
int*    solution_stack;
int     solution_stack_count;
int*    index_in_solution_stack;

double  opt_time;

int* best_array;
int best_count;
unsigned int* already_in_ccmpvars;
int* ccmpvars;
int ccmpvars_count;

int *m_item_count;          //每个集合覆盖的元素数
int **m_item;               //m_item[i][j]表示i号集合覆盖j号元素
int *g_item_count;          //每个集合的冲突个数
int **g_item;               //g_item[i][j]表示i号集合与j号集合冲突
int *n_ele_count;           //每个元素被覆盖的次数
int **n_ele;                //n_ele[i][j]表示i号元素被j号集合覆盖

int *element_cover_times;   //元素被覆盖的次数

long long *original_weight;

long long total_weight;     //所有元素的权重和

int seed;
float rdprob;
int hd_count_threshold;
float prob;
float pacprob;

#endif