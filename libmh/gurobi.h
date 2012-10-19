#pragma once

#include <boost/shared_ptr.hpp>
/** Evil hack */
//namespace std { using boost::shared_ptr; using boost::dynamic_pointer_cast; }

#define SPPREF boost::

#include <gurobi_c++.h>

#ifdef _MSVC
#pragma warning( pop )
#endif

#include "lpsolver.h"

/**
 * gurobi_solver solves an ilp problem using the GUROBI MILP Solver
 */


class gurobi_solver: public ilpmodel {
 public:
	gurobi_solver();

	virtual ~gurobi_solver();
	virtual void printResult();
	virtual ilpStatus solve(int timeout);
	virtual void resize(int rows, int columns);
	virtual void setname(int index, std::string name);
	virtual void print();
	virtual void setminizetarget(ilpVariable & var,double value);

	virtual ilpVariable addBool(const std::string name);
	virtual ilpVariable addInt(const std::string name, int lb, int ub);
	virtual ilpVariable addNum(const std::string name, double lb, double ub);
	virtual void removeConstraint(ilpConstraint ilpconstraint);

	virtual ilpExpr newExpr();

	virtual ilpConstraint addConstraint(ilpConstraint  e);
	virtual void writeModel(const std::string filename);
	virtual double getValue(ilpVariable & ilpvar);

	virtual void updateVariables();
	virtual void setLogCallback(void (*logCBFun)(const char*));
 private:
	GRBEnv env;
	GRBModel model;
};

class gurobiILPVariableImpl: public ilpVariableImpl {
 public:

	gurobiILPVariableImpl(GRBModel & model, double lb, double ub, char type, std::string varname): name(varname) {
		// Set objective coefficent to zero
		// Modify later to fit
		grbvar = model.addVar(lb,ub,0,type,name);
	}

	virtual ~gurobiILPVariableImpl() {};
	virtual double getValue();
	virtual std::string getName() {return name; }
	virtual ilpExpr operator+(const boost::shared_ptr<ilpVariableImpl>);
	virtual ilpExpr operator-(const boost::shared_ptr<ilpVariableImpl>);
	virtual ilpExpr opmul(double rs);

private:
	GRBVar grbvar;
	friend class gurobiILPExprImpl;
	friend void gurobi_solver::setminizetarget(ilpVariable&  var, double value);
	
	// Keep a copy of the name to keep a reference to the string if the GRB stuff does not do it
	std::string name;

};


class gurobiILPExprImpl: public ilpExprImpl {
public:
	gurobiILPExprImpl() { }
	gurobiILPExprImpl(GRBLinExpr startexpr): expr(startexpr) { }
	virtual void operator+= (const ilpVariable& rs);
	virtual void operator-= (const ilpVariable& rs);
	virtual void operator+= (const ilpExpr& rs);
	virtual void operator-= (const ilpExpr& rs);

	virtual ilpConstraint operator== (const double rs);
	virtual ilpConstraint operator<= (const double rs);
	virtual ilpConstraint operator>= (const double rs);

	virtual ilpExpr operator-(const boost::shared_ptr<ilpExprImpl> rs);
	virtual ilpExpr operator+(const boost::shared_ptr<ilpExprImpl> rs);
								
private:
	constraint_type con;
	GRBLinExpr expr;
	friend class gurobi_solver;
};

class gurobiILPTempConstraintImpl: public ilpConstraintImpl {
public:
	gurobiILPTempConstraintImpl(GRBTempConstr  newgrbcon) : grbconstraint(newgrbcon) {}
private:
	GRBTempConstr grbconstraint;
	friend class gurobi_solver;

};

class gurobiILPConstraintImpl: public ilpConstraintImpl {
public:
	gurobiILPConstraintImpl(GRBConstr newgrbcon) : grbconstraint(newgrbcon) {}
private:
	GRBConstr grbconstraint;
	friend class gurobi_solver;

};
