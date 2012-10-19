#pragma once

#include "gurobi.h"
#include <vector>

#include "lpsolver.h"


class mpbase {
private:
	mpbase(const mpbase&);
	mpbase& operator=(const mpbase&);

protected:
	ilpmodel* model;


public:
	mpbase() { model = new gurobi_solver(); }

	void setLogCallback(void (*logCBFun)(const char*))
	{
		model->setLogCallback(logCBFun);
	}

	~mpbase() { delete model; };

};
