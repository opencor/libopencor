/* The content of this file was generated using a modified C profile of libCellML 0.5.0. */

#include "model.h"

const char VERSION[] = "0.4.0.post0";
const char LIBCELLML_VERSION[] = "0.5.0";

const size_t VARIABLE_COUNT = 3;

const VariableInfo VARIABLE_INFO[] = {
    {"x", "dimensionless", "my_algebraic_system", ALGEBRAIC},
    {"z", "dimensionless", "my_algebraic_system", ALGEBRAIC},
    {"y", "dimensionless", "my_algebraic_system", ALGEBRAIC}
};

double * createVariablesArray()
{
    double *res = (double *) malloc(VARIABLE_COUNT*sizeof(double));

    for (size_t i = 0; i < VARIABLE_COUNT; ++i) {
        res[i] = NAN;
    }

    return res;
}

void deleteArray(double *array)
{
    free(array);
}

typedef struct {
    double *variables;
} RootFindingInfo;

extern void nlaSolve(void (*objectiveFunction)(double *, double *, void *),
                     double *u, int n, void *data);

void objectiveFunction0(double *u, double *f, void *data)
{
    double *variables = ((RootFindingInfo *) data)->variables;

    variables[0] = u[0];
    variables[1] = u[1];
    variables[2] = u[2];

    f[0] = variables[0]+variables[2]+variables[1]-2.0;
    f[1] = 6.0*variables[0]-4.0*variables[2]+5.0*variables[1]-31.0;
    f[2] = 5.0*variables[0]+2.0*variables[2]+2.0*variables[1]-13.0;
}

void findRoot0(double *variables)
{
    RootFindingInfo rfi = { variables };
    double u[3];

    u[0] = variables[0];
    u[1] = variables[1];
    u[2] = variables[2];

    nlaSolve(objectiveFunction0, u, 3, &rfi);

    variables[0] = u[0];
    variables[1] = u[1];
    variables[2] = u[2];
}

void initialiseVariables(double *variables)
{
    variables[0] = 1.0;
    variables[1] = 1.0;
    variables[2] = 1.0;
}

void computeComputedConstants(double *variables)
{
}

void computeVariables(double *variables)
{
    findRoot0(variables);
}