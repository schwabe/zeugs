#pragma once
#define IL_STD 1


// Disable annoying warnings in the ilog cplex headers

#ifdef _MSVC
#pragma warning( push )
#pragma warning( disable : 4244 )
#endif 

#include <ilcplex/ilocplex.h>

#ifdef _MSVC
#pragma warning( pop )
#endif

#include "partalgorithm.h"

/**
 * cplex_solver solves an ilp problem using the CPLEX MILP Solver
 */


class cplex_solver: public ilpmodel {
 public:
	cplex_solver();

	virtual ~cplex_solver();
	virtual void printResult();
	virtual ilpStatus solve(int timeout);
	virtual void resize(int rows, int columns);
	virtual void setname(int index, std::string name);
	virtual void print();
	virtual void setminizetarget(ilpVariable & var, double value);

	virtual ilpVariable addBool(const std::string name);
	virtual ilpVariable addInt(const std::string name, int lb, int ub);
	virtual ilpVariable addNum(const std::string name, double lb, double ub);

	virtual ilpExpr newExpr();

	virtual ilpConstraint addConstraint(ilpConstraint  e);
	virtual void writeModel(const std::string) { /*dummy*/}
	virtual double getValue(ilpVariable & var);
	virtual void updateVariables() {}
	virtual void removeConstraint(ilpConstraint);
 private:
	IloEnv env;
	IloModel model;
	IloCplex cplex;
};






class cplexILPVariableImpl: public ilpVariableImpl {
 public:
	//IloNumVar(const IloEnv env, IloNum lb=0, IloNum ub=IloInfinity, Type type=Float, const char* name=0)
	cplexILPVariableImpl(const IloEnv & env, IloNum lb=0, IloNum ub=IloInfinity, IloNumVar::Type type=IloNumVar::Float, std::string varname=0): name(varname) {
		ilovar = IloNumVar(env,lb,ub,type,name.c_str());
	}

	virtual ~cplexILPVariableImpl() {};
	virtual ilpExpr opmul(int rs);
	virtual ilpExpr operator+(const shared_ptr<ilpVariableImpl> v2);
	virtual ilpExpr operator-(const shared_ptr<ilpVariableImpl> v2);
	virtual std::string getName() { return name; }
	virtual ilpExpr opmul(double rs);

private:
	IloNumVar ilovar;
	friend class cplexILPExprImpl;
	friend void cplex_solver::setminizetarget(ilpVariable&  var, double value);
	friend double cplex_solver::getValue(ilpVariable & var);
	
	// Keep a copy of the name to keep a reference to the string if the Ilo stuff does not do it
	std::string name;

};


class cplexILPExprImpl: public ilpExprImpl {
public:
	cplexILPExprImpl(IloEnv& env): expr(env)  { }
	cplexILPExprImpl(IloExpr newexpr): expr(newexpr) {}
	virtual void operator+= (const ilpVariable& rs);
	virtual void operator-= (const ilpVariable& rs);


	virtual ilpExpr operator+(const shared_ptr<ilpVariableImpl> rs);
	virtual ilpExpr operator-(const shared_ptr<ilpVariableImpl> rs);
	virtual ilpExpr operator+(const shared_ptr<ilpExprImpl> rs);
	virtual ilpExpr operator-(const shared_ptr<ilpExprImpl> rs);

	virtual ilpConstraint operator== (const double rs);
	virtual ilpConstraint operator<= (const double rs);
	virtual ilpConstraint operator>= (const double rs);


	virtual void operator-= (const ilpExpr& rs);
	virtual void operator+= (const ilpExpr& rs);
								
private:
	int rhs;
	constraint_type con;
	IloExpr expr;
	friend class cplex_solver;
};

class cplexILPConstraintImpl: public ilpConstraintImpl {
public:
	cplexILPConstraintImpl(IloRange  newilocon) : iloconstraint(newilocon) {}
private:
	IloRange iloconstraint;
	friend class cplex_solver;

};

