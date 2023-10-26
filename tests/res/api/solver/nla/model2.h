/* The content of this file was generated using a modified C profile of libCellML 0.5.0. */

#pragma once

extern const char VERSION[];
extern const char LIBCELLML_VERSION[];

extern const size_t VARIABLE_COUNT_2;

typedef enum {
    CONSTANT,
    COMPUTED_CONSTANT,
    ALGEBRAIC
} VariableType;

typedef struct {
    char name[2];
    char units[14];
    char component[20];
    VariableType type;
} VariableInfo;

extern const VariableInfo VARIABLE_INFO[];

double * createVariablesArray();
void deleteArray(double *array);

void initialiseVariables(double *variables);
void computeComputedConstants(double *variables);
void computeVariables(double *variables);
