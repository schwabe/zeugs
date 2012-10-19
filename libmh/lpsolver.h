/*
 * partalgorithm.h
 *
 *  Created on: 20.04.2010
 *      Author: arne
 */

#ifndef LPSOLVER_H_
#define LPSOLVER_H_

#include <ostream>
#include <memory>
#include <limits>
#include <boost/shared_ptr.hpp>

//using boost::shared_ptr;
//using boost::shared_ptr;

enum ilpStatus {
	solutionFound,
	solutionTimeout,
	solutionError,
	solutionInfeasable
};




// Values are comptabile with lp_solve
enum constraint_type {
	LessOrEqualConstraint =1,
	EqualConstraint=3,
	GreaterOrEqualConstraint=2
};


class ilpExpr;
class ilpVariable;
class ilpConstraint;


ilpExpr operator* (const ilpVariable  v, double rs);
ilpExpr operator* (double ls, const ilpVariable  v);

class ilpVariableImpl {
public:
	virtual ~ilpVariableImpl() {}
	virtual ilpExpr opmul(double rs)=0;
	virtual std::string getName()=0;
	virtual ilpExpr operator+(const boost::shared_ptr<ilpVariableImpl>)=0;
	virtual ilpExpr operator-(const boost::shared_ptr<ilpVariableImpl>)=0;
};



class ilpConstraintImpl {
public:
	virtual ~ilpConstraintImpl() {}
};


class ilpConstraint {
public:
	ilpConstraint(ilpConstraintImpl* implptr):impl(implptr) { }
	//ilpConstraint(const ilpConstraint & impcon):impl(impcon.impl) { }
public:
	boost::shared_ptr<ilpConstraintImpl> impl;
};

class ilpExprImpl {
public:
	virtual void operator+= (const ilpVariable& rs)=0;
	virtual void operator-= (const ilpVariable& rs)=0;
	virtual void operator+= (const ilpExpr& rs)=0;
	virtual void operator-= (const ilpExpr& rs)=0;

	virtual ilpExpr operator-(const boost::shared_ptr<ilpExprImpl> rs)=0;
	virtual ilpExpr operator+(const boost::shared_ptr<ilpExprImpl> rs)=0;

	virtual ilpConstraint operator== (const double rs)=0;
	virtual ilpConstraint operator<= (const double rs)=0;
	virtual ilpConstraint operator>= (const double rs)=0;
	virtual ~ilpExprImpl() {}
};



class ilpExpr {
 public:
	virtual ~ilpExpr() {};
	void operator+= (const ilpVariable& rs) { impl->operator+=(rs); }
	void operator-= (const ilpVariable& rs) { impl->operator-=(rs); }
	void operator+= (const ilpExpr& rs) { impl->operator+=(rs); }
	void operator-= (const ilpExpr& rs) { impl->operator-=(rs); }

	
	ilpConstraint operator<= (const double rs) { return impl->operator<=(rs); }
	ilpConstraint operator== (const double rs) { return impl->operator==(rs); }
	ilpConstraint operator>= (const double rs) { return impl->operator>=(rs); }
	ilpExpr operator-(const ilpExpr rs) { return impl->operator-(rs.impl); }
	ilpExpr operator+(const ilpExpr rs) { return impl->operator+(rs.impl); }

	ilpExpr operator-(const ilpVariable rs); 
	ilpExpr operator+(const ilpVariable rs); 

	ilpExpr(ilpExprImpl* implptr):impl(implptr) { }
public:
	boost::shared_ptr<ilpExprImpl> impl;
};




class ilpVariable {
public:
	ilpVariable(ilpVariableImpl* implptr):impl(implptr) {}
	~ilpVariable() {};
	ilpVariable() {}
	ilpExpr operator+(const ilpVariable & v2) { return impl->operator+(v2.impl); }
	ilpExpr operator-(const ilpVariable & v2) { return impl->operator-(v2.impl); }
	ilpConstraint operator>=(const double rs) { return *this*1 >= rs;}
	ilpConstraint operator<=(const double rs) { return operator*(*this,1) <= rs;}
	ilpConstraint operator==(const double rs) { return operator*(*this,1) == rs;}


public:
	boost::shared_ptr<ilpVariableImpl> impl;
	// Alow the implementation of the expression to access the implementation of the variables
	
};



/** Abstract base class for ILP Models */
class ilpmodel {
public:
	virtual void resize(int rows, int columns)=0;	
	virtual ilpStatus solve(int timeout)=0;

	virtual ilpVariable addBool(const std::string name)=0;
	virtual ilpVariable addInt(const std::string name, int lb=0, int ub=std::numeric_limits<int>::max())=0;
	virtual ilpVariable addNum(const std::string name, double lb=0, double ub=std::numeric_limits<double>::max())=0;
	virtual ilpExpr newExpr()=0;
	virtual ilpConstraint addConstraint(ilpConstraint) =0;
	virtual void setminizetarget(ilpVariable &  var, double coefficent=1.0)=0;
	virtual void writeModel(const std::string filename)=0;
	virtual void removeConstraint(ilpConstraint ilpconstraint)=0;

	virtual double getValue(ilpVariable & ilpv)=0;
	virtual void updateVariables()=0;
	virtual ~ilpmodel() {}

	virtual void setLogCallback(void (*logCBFun)(const char*))=0;
};




std::ostream & operator<<(std::ostream & out, const ilpVariable&);

inline ilpExpr operator* (const ilpVariable  v, double rs)
{
	return v.impl->opmul(rs);
}
inline ilpExpr operator* (double ls, const ilpVariable  v)
{
	return v.impl->opmul(ls);
}

inline std::ostream & operator<<(std::ostream & out, const ilpVariable & v)
{
	return out << v.impl->getName();
}


inline ilpExpr ilpExpr::operator-(const ilpVariable rs) { return operator-(rs*1); }
inline ilpExpr ilpExpr::operator+(const ilpVariable rs) { return operator+(rs*1); }


#endif /* LPSOLVER_H_ */
