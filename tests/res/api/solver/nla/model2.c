/* The content of this file was generated using a modified C profile of libCellML 0.5.0. */

#include "model2.h"

const char VERSION[] = "0.4.0.post0";
const char LIBCELLML_VERSION[] = "0.5.0";

const size_t VARIABLE_COUNT_2 = 3;

const VariableInfo VARIABLE_INFO[] = {
    {"x", "dimensionless", "my_algebraic_system", ALGEBRAIC},
    {"z", "dimensionless", "my_algebraic_system", ALGEBRAIC},
    {"y", "dimensionless", "my_algebraic_system", ALGEBRAIC}
};

double * createVariablesArray()
{
    double *res = (double *) malloc(VARIABLE_COUNT_2*sizeof(double));

    for (size_t i = 0; i < VARIABLE_COUNT_2; ++i) {
        res[i] = strtod("NAN", nullptr);
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

void objectiveFunction0(double *u, double *f, void *data)
{
    double *variables = ((RootFindingInfo *) data)->variables;

    variables[0] = u[0];
    variables[1] = u[1];
    variables[2] = u[2];

    f[0] = variables[0]+variables[2]+variables[1]-5.0;
    f[1] = 6.0*variables[0]-4.0*variables[2]+5.0*variables[1]-73.0;
    f[2] = 5.0*variables[0]+2.0*variables[2]+2.0*variables[1]-19.0;
}

void findRoot0(double *variables)
{
    RootFindingInfo rfi = { variables };
    double u[3];

    u[0] = variables[0];
    u[1] = variables[1];
    u[2] = variables[2];

    libOpenCOR::nlaSolve(KINSOL_NLA_SOLVER, objectiveFunction0, u, 3, &rfi);

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
