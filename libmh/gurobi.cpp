#include "gurobi.h"
#include <iostream>

gurobi_solver::gurobi_solver() : model(env)
{
	
}

void gurobi_solver::resize(int ,int )
{
}

void gurobi_solver::setminizetarget(ilpVariable&  var, double value)
{
	boost::shared_ptr<gurobiILPVariableImpl> cvar = boost::dynamic_pointer_cast<gurobiILPVariableImpl>(var.impl);
	try {
		cvar->grbvar.set(GRB_DoubleAttr_Obj,value);
	} catch ( GRBException ge) {
		std::cout << "Error code = " << ge.getErrorCode() << std::endl;
		std::cout << ge.getMessage() << std::endl;
	}
}




class LoggingProxy: public GRBCallback {
public:
	LoggingProxy(void (*logFun)(const char*))
	{
		logCBFun = logFun;
	}
private:
	void (*logCBFun)(const char*);
protected:
	void callback() {
		if (where == GRB_CB_MESSAGE) {
			string msg = getStringInfo(GRB_CB_MSG_STRING);

			logCBFun(msg.c_str());
		}
	}
};
	


void gurobi_solver::setLogCallback(void (*logCBFun)(const char*))
{
	LoggingProxy* lp = new LoggingProxy(logCBFun);
	model.setCallback(lp);
}



gurobi_solver::~gurobi_solver()
{

}

ilpVariable gurobi_solver::addBool(const std::string name)
{
	gurobiILPVariableImpl* var = new gurobiILPVariableImpl(model,0,1,GRB_BINARY,name);
	return ilpVariable(var);
}

ilpVariable gurobi_solver::addInt(const std::string name, int lb, int ub)
{
	gurobiILPVariableImpl* var = new gurobiILPVariableImpl(model,lb,ub,GRB_INTEGER,name);
	return ilpVariable(var);

}


ilpVariable gurobi_solver::addNum(const std::string name, double lb, double ub)
{
	gurobiILPVariableImpl* var = new gurobiILPVariableImpl(model,lb,ub,GRB_CONTINUOUS,name);
	return ilpVariable(var);

}

void gurobi_solver::updateVariables()
{
	model.update();
}


ilpExpr gurobi_solver::newExpr()
{
	return ilpExpr( new gurobiILPExprImpl());

}

ilpConstraint gurobi_solver::addConstraint(ilpConstraint  e)

{		
	boost::shared_ptr<gurobiILPTempConstraintImpl> ce = boost::dynamic_pointer_cast<gurobiILPTempConstraintImpl>(e.impl);
			
	try {
		return ilpConstraint(new gurobiILPConstraintImpl(model.addConstr(ce->grbconstraint)));
	} 	catch (GRBException ex) {
		std::cout << "Error code = " << ex.getErrorCode() << std::endl;
		std::cout << ex.getMessage() << std::endl;
		assert(false);
		return ilpConstraint(NULL);
	}
	
}

void gurobi_solver::removeConstraint(ilpConstraint e)
{
	boost::shared_ptr<gurobiILPConstraintImpl> ce = boost::dynamic_pointer_cast<gurobiILPConstraintImpl>(e.impl);
	model.remove(ce->grbconstraint);
}

void gurobi_solver::setname(int , const std::string )
{

}

void gurobi_solver::print()
{

}

void gurobi_solver::printResult()
{

}

ilpStatus gurobi_solver::solve(int timeout)
{
	try {
	  
		model.update();
		if(timeout) {
		  std::cout << "Setting solver timeout to " << timeout <<std::endl;
		}
		model.getEnv().set(GRB_DoubleParam_TimeLimit,(double)timeout);
		model.optimize();

		
	    int optimstatus = model.get(GRB_IntAttr_Status);

		if (optimstatus == GRB_OPTIMAL) {
#ifndef NDEBUG
			std::cout << "Writing solution to model.sol" << std::endl;
			model.write("model.sol");
#endif
		} else if (optimstatus == GRB_INF_OR_UNBD) {
			std::cout << "Model is infeasible or unbounded" <<std::endl;
			std::cout << "Trying to write IIS to iis.ilp" << std::endl;
			model.computeIIS();
			model.write("iis.ilp");
			return solutionError;
		} else if (optimstatus == GRB_INFEASIBLE) {
			std::cout << "Model is infeasible" <<std::endl;
			std::cout << "Trying to write IIS to iis.ilp" << std::endl;
			model.computeIIS();
			model.write("iis.ilp");
			return solutionInfeasable;
		} else if (optimstatus == GRB_UNBOUNDED) {
			std::cout << "Model is unbounded" <<std::endl;
			return solutionError;
		} else if (optimstatus == GRB_TIME_LIMIT) {
			std::cout << "Timeout while solving" << std::endl;
			return solutionTimeout;
		} else {
			std::cout << "Optimization was stopped with status = "
				<< optimstatus <<std::endl;
			return solutionError;
		}


		
	} catch (GRBException ex) {
		std::cout << "Error code = " << ex.getErrorCode() << std::endl;
		std::cout << ex.getMessage() << std::endl;
		return solutionError;
	

	} catch (...) {
		std::cerr << "error unkown" << std::endl;
		std::cerr.flush();
		return solutionError;
	}


   

	return solutionFound;
}



void gurobiILPExprImpl::operator+= ( const ilpVariable &  v2) {
	const boost::shared_ptr<const gurobiILPVariableImpl> ce2 = boost::dynamic_pointer_cast<const gurobiILPVariableImpl>(v2.impl);
	expr+=ce2->grbvar;

}


void gurobiILPExprImpl::operator-=( const ilpVariable &  v2) {
	const boost::shared_ptr<const gurobiILPVariableImpl> ce2 = boost::dynamic_pointer_cast<const gurobiILPVariableImpl>(v2.impl);
	expr-=ce2->grbvar;

}

void gurobiILPExprImpl::operator+= ( const ilpExpr &  v2) {
	const boost::shared_ptr<const gurobiILPExprImpl> ce2 = boost::dynamic_pointer_cast<const gurobiILPExprImpl>(v2.impl);
	expr+=ce2->expr;

}


void gurobiILPExprImpl::operator-=( const ilpExpr &  v2) {
	const boost::shared_ptr<const gurobiILPExprImpl> ce2 = boost::dynamic_pointer_cast<const gurobiILPExprImpl>(v2.impl);
	expr-=ce2->expr;

}



ilpConstraint gurobiILPExprImpl::operator== (const double rs)
{
	return ilpConstraint(new gurobiILPTempConstraintImpl(expr == rs));
}


ilpConstraint gurobiILPExprImpl::operator>= (const double rs)
{
	return ilpConstraint(new gurobiILPTempConstraintImpl(expr >= rs));
}

ilpConstraint gurobiILPExprImpl::operator<= (const double rs)
{
	gurobiILPTempConstraintImpl* impl = new gurobiILPTempConstraintImpl(expr <= rs);
	return ilpConstraint(impl);
}


ilpExpr gurobiILPVariableImpl::opmul(double rs) 
 {

	 return ilpExpr(new gurobiILPExprImpl(grbvar * rs));
 }

void gurobi_solver::writeModel(const std::string filename) 
{
	updateVariables();
	model.write(filename);
}

double gurobiILPVariableImpl::getValue() {
	return grbvar.get(GRB_DoubleAttr_X);
}


ilpExpr gurobiILPVariableImpl::operator+(boost::shared_ptr<ilpVariableImpl> v2)
{
	 const boost::shared_ptr<const gurobiILPVariableImpl> ce2 = boost::dynamic_pointer_cast<const gurobiILPVariableImpl>(v2);
	 return ilpExpr(new gurobiILPExprImpl(grbvar + ce2->grbvar));
}

ilpExpr gurobiILPVariableImpl::operator-(boost::shared_ptr<ilpVariableImpl> v2)
{
	 const boost::shared_ptr<const gurobiILPVariableImpl> ce2 = boost::dynamic_pointer_cast<const gurobiILPVariableImpl>(v2);
	 return ilpExpr(new gurobiILPExprImpl(grbvar - ce2->grbvar));
}



ilpExpr gurobiILPExprImpl::operator-(const boost::shared_ptr<ilpExprImpl> rs)
{
	const boost::shared_ptr<const gurobiILPExprImpl> ce2 = boost::dynamic_pointer_cast<const gurobiILPExprImpl>(rs);
	return ilpExpr(new gurobiILPExprImpl(expr - ce2->expr));
}
ilpExpr gurobiILPExprImpl::operator+(const boost::shared_ptr<ilpExprImpl> rs)
{
	const boost::shared_ptr<const gurobiILPExprImpl> ce2 = boost::dynamic_pointer_cast<const gurobiILPExprImpl>(rs);
	return ilpExpr(new gurobiILPExprImpl(expr + ce2->expr));
}


double gurobi_solver::getValue(ilpVariable & ilpvar)
{
	const boost::shared_ptr< gurobiILPVariableImpl> ce2 = boost::dynamic_pointer_cast< gurobiILPVariableImpl>(ilpvar.impl);
	return ce2->getValue();
}
