#ifdef USE_CPLEX

#include "cplex.h"
#include <iostream>
// lpsolve55.lib;wsock32.lib;concert.lib;cplex121.lib;ilocplex.lib


cplex_solver::cplex_solver() : model(env)
{

}
void cplex_solver::resize(int rows,int columns)
{
	// bla
	IloNumVar e1, e2;
}

void cplex_solver::setminizetarget(ilpVariable&  var, double value)
{
  std::cout << "Setting minimize target" << std::endl;
	std::tr1::shared_ptr<cplexILPVariableImpl> cvar = std::tr1::dynamic_pointer_cast<cplexILPVariableImpl>(var.impl);
	IloObjective obj(env,1* cvar->ilovar,IloObjective::Minimize);
	model.add(obj);
//	obj.end();
}



cplex_solver::~cplex_solver()
{
	env.end();
}

ilpVariable cplex_solver::addBool(const std::string name)
{
	cplexILPVariableImpl* var = new cplexILPVariableImpl(env,0,1,ILOBOOL,name);
	return ilpVariable(var);
}

ilpVariable cplex_solver::addInt(const std::string name, int lb, int ub)
{
	return  ilpVariable(new cplexILPVariableImpl(env,lb,ub,ILOINT,name));
}

ilpVariable cplex_solver::addNum(const std::string name, double lb, double ub)
{
	return  ilpVariable(new cplexILPVariableImpl(env,lb,ub,ILOFLOAT,name));
}

ilpExpr cplex_solver::newExpr()
{
	return ilpExpr( new cplexILPExprImpl(env));

}

ilpConstraint cplex_solver::addConstraint(ilpConstraint  e)
{		
	std::tr1::shared_ptr<cplexILPConstraintImpl> ce = std::tr1::dynamic_pointer_cast<cplexILPConstraintImpl>(e.impl);
	model.add(ce->iloconstraint);
	return e;
}

void cplex_solver::setname(int index, const std::string name)
{

}

void cplex_solver::print()
{

}

void cplex_solver::printResult()
{

}

bool cplex_solver::solve(int timeout)
{
	std::cout << "Solving with CPLEX" << std::endl;
	try {

		cplex=IloCplex(model);

		int ret= cplex.solve();
		if(!ret) {
			std::cerr << "error unkown, return code:  "<< ret << std::endl;
			std::cerr.flush();
			return false;
		}
	} catch (IloException& ex) {
		std::cerr << "Error: " << ex << std::endl;
		std::cerr.flush();
		return false;
	} catch (...) {
		std::cerr << "error unkown" << std::endl;
		std::cerr.flush();
		return false;
	}
	return true;
}



void cplexILPExprImpl::operator+= ( const ilpVariable &  v2) {
	const std::tr1::shared_ptr<const cplexILPVariableImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPVariableImpl>(v2.impl);
	expr+=ce2->ilovar;

}


void cplexILPExprImpl::operator-=( const ilpVariable &  v2) {
	const std::tr1::shared_ptr<const cplexILPVariableImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPVariableImpl>(v2.impl);
	expr-=ce2->ilovar;
}



void cplexILPExprImpl::operator+= ( const ilpExpr &  e2) {
	const std::tr1::shared_ptr<const cplexILPExprImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPExprImpl>(e2.impl);
	expr+=ce2->expr;

}


void cplexILPExprImpl::operator-= ( const ilpExpr &  e2) {
	const std::tr1::shared_ptr<const cplexILPExprImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPExprImpl>(e2.impl);
	expr-=ce2->expr;

}




ilpConstraint cplexILPExprImpl::operator== (const double rs)
{
	return ilpConstraint(new cplexILPConstraintImpl(expr == rs));
}
ilpConstraint cplexILPExprImpl::operator<= (const double rs)
{
	cplexILPConstraintImpl* impl = new cplexILPConstraintImpl(expr <= rs);
	return ilpConstraint(impl);
}


ilpConstraint cplexILPExprImpl::operator>= (const double rs)
{
	cplexILPConstraintImpl* impl = new cplexILPConstraintImpl(expr >= rs);
	return ilpConstraint(impl);
}


double cplex_solver::getValue(ilpVariable & var) 
{

	const std::tr1::shared_ptr<const cplexILPVariableImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPVariableImpl>(var.impl);
	return cplex.getValue(ce2->ilovar);
	
}

ilpExpr cplexILPVariableImpl::opmul(const int ls)
{
	return ilpExpr(new cplexILPExprImpl(ilovar* ls));
}


ilpExpr cplexILPExprImpl::operator+(const shared_ptr<ilpExprImpl> rs)
{
	const std::tr1::shared_ptr<const cplexILPExprImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPExprImpl>(rs);
	return ilpExpr(new cplexILPExprImpl(expr + ce2->expr));
}

ilpExpr cplexILPExprImpl::operator-(const shared_ptr<ilpExprImpl> rs)
{
	const std::tr1::shared_ptr<const cplexILPExprImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPExprImpl>(rs);
	return ilpExpr(new cplexILPExprImpl(expr - ce2->expr));
}



ilpExpr cplexILPExprImpl::operator+(const shared_ptr<ilpVariableImpl> rs)
{
	const std::tr1::shared_ptr<const cplexILPVariableImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPVariableImpl>(rs);
	return ilpExpr(new cplexILPExprImpl(expr + ce2->ilovar));
}

ilpExpr cplexILPExprImpl::operator-(const shared_ptr<ilpVariableImpl> rs)
{
	const std::tr1::shared_ptr<const cplexILPVariableImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPVariableImpl>(rs);
	return ilpExpr(new cplexILPExprImpl(expr - ce2->ilovar));
}





ilpExpr cplexILPVariableImpl::operator+(const shared_ptr<ilpVariableImpl> v2)
{
	 const std::tr1::shared_ptr<const cplexILPVariableImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPVariableImpl>(v2);
	 return ilpExpr(new cplexILPExprImpl(ilovar + ce2->ilovar));
}

ilpExpr cplexILPVariableImpl::operator-(const shared_ptr<ilpVariableImpl> v2)
{
	 const std::tr1::shared_ptr<const cplexILPVariableImpl> ce2 = std::tr1::dynamic_pointer_cast<const cplexILPVariableImpl>(v2);
	 return ilpExpr(new cplexILPExprImpl(ilovar - ce2->ilovar));
}

ilpExpr cplexILPVariableImpl::opmul(double rs) 
 {

	 return ilpExpr(new cplexILPExprImpl(ilovar * rs));
 }

void cplex_solver::removeConstraint(ilpConstraint e)
{
	std::tr1::shared_ptr<cplexILPConstraintImpl> ce = std::tr1::dynamic_pointer_cast<cplexILPConstraintImpl>(e.impl);
	model.remove(ce->iloconstraint);
}

#endif //USE_CPLEX

