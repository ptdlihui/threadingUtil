#pragma once
#include "parameters.h"
#include "HyMesh.h"
#include "common.h"

class IHyMeshProcessor
{
public:
    virtual ~IHyMeshProcessor() {};

    virtual void SetParameters(ParameterTable& params)      = 0;
    virtual void Process()                                  = 0;
};


typedef ModuleLoader<IHyMeshProcessor, ParameterTable*> AlgorithmLoader;

#define CreateAlgorithmFunc "CreateProcessor"