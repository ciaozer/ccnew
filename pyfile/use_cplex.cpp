#include <ilcplex/ilocplex.h>

int main() {
  IloEnv env;
  try {
    // 创建模型
    IloModel model(env);

    // 添加变量
    IloBoolVar x1(env, "x1");
    IloBoolVar x2(env, "x2");
    IloBoolVar x3(env, "x3");

    // 将变量添加到模型中
    model.add(x1);
    model.add(x2);
    model.add(x3);

    // 添加目标函数
    IloObjective obj = IloMaximize(env, x1 + 2 * x2 + 3 * x3);
    model.add(obj);

    // 添加约束条件
    model.add(x1 + 2 * x2 >= 2);
    model.add(x3 + x2 <= 1);

    // 创建Cplex实例
    IloCplex cplex(model);

    // 求解问题
    cplex.solve();

    // 输出结果
    std::cout << "Solution status = " << cplex.getStatus() << std::endl;
    std::cout << "Objective value = " << cplex.getObjValue() << std::endl;
    std::cout << "x1 = " << cplex.getValue(x1) << std::endl;
    std::cout << "x2 = " << cplex.getValue(x2) << std::endl;
    std::cout << "x3 = " << cplex.getValue(x3) << std::endl;

    // 释放内存
    cplex.end();
    model.end();
    env.end();
  } catch (IloException& e) {
    std::cerr << "Exception caught: " << e << std::endl;
  }
  return 0;
}