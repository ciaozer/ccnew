# -*- coding: utf-8 -*-
import cplex

# 读取输入数据
with open('cp.txt') as f:
    itemnum, elementnum, edgenum = map(int, f.readline().split())
    content = f.read() # 一次性读取整个文件内容
    edges = [tuple(map(int, line.split())) for line in content.splitlines()[:edgenum]]
    elements = [list(map(int, line.split())) for line in content.splitlines()[edgenum:edgenum+elementnum]]
    weights = list(map(int, content.splitlines()[-1].split()))

# 创建模型对象
myLP = cplex.Cplex()

# 添加决策变量
x_vars = myLP.variables.add(names=['x{}'.format(i) for i in range(1, itemnum+1)],
                             types=[myLP.variables.type.binary]*itemnum)
y_vars = myLP.variables.add(names=['y{}'.format(i) for i in range(1, elementnum+1)],
                             types=[myLP.variables.type.binary]*elementnum)

def add_constraint(problem, var1, var2, rhs):
    constraint = [[var1, var2], [1.0, 1.0]]
    problem.linear_constraints.add(lin_expr=[constraint], senses=["L"], rhs=[rhs])
# 添加约束条件
for i, j in edges:
    add_constraint(myLP, "x"+str(i), "x"+str(j), 1.0)

def add_constraint1(problem, var_names, rhs):
    constraint = [var_names, [1.0] * len(var_names)]
    problem.linear_constraints.add(lin_expr=[constraint], senses=["G"], rhs=[rhs])

def add_constraint2(problem, var_names, rhs, i):
    constraint = [var_names, [1.0] * len(var_names)] + ["y"+str(i+1), float(n)]
    problem.linear_constraints.add(lin_expr=[constraint], senses=["L"], rhs=[rhs])

for i in range(elementnum):
    cur_ele = elements[i]
    n = len(cur_ele)
    variables = []
    for item in cur_ele:
        variables += ["x"+str(item)]
    add_constraint1(myLP, variables+["y"+str(i+1)], 1.0)
    add_constraint2(myLP, variables, float(n), i)

# 添加目标函数
myLP.objective.set_sense(myLP.objective.sense.minimize)
myLP.objective.set_linear(zip(y_vars, weights))

# 设置求解器选项
myLP.parameters.lpmethod.set(myLP.parameters.lpmethod.values.dual)

# 求解模型
myLP.solve()

# 输出结果
print("Solution status = ", myLP.solution.get_status())
print("Objective value = ", myLP.solution.get_objective_value())
for i, x_var in enumerate(x_vars):
    if myLP.solution.get_values(x_var):
        print i+1, 