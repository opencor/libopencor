#include <iostream>
#include <libopencor>

int main()
{
    // Output the version of libOpenCOR.

    std::cout << "---[ libOpenCOR version information ]---" << std::endl;
    std::cout << std::endl;
    std::cout << "Version:        " << libOpenCOR::version() << std::endl;
    std::cout << "Version string: " << libOpenCOR::versionString() << std::endl;

    // Load a COMBINE archive from GitHub and output some information about it.

    auto file = libOpenCOR::File::create("https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex");
    auto document = libOpenCOR::SedDocument::create(file);
    auto simulation = std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0]);

    simulation->setOutputEndTime(5.0);
    simulation->setNumberOfSteps(500);

    auto instance = document->instantiate();

    instance->run();

    auto instanceTask = instance->tasks()[0];

    std::cout << std::endl;
    std::cout << "---[ File information ]---" << std::endl;
    std::cout << std::endl;
    std::cout << "URL: " << file->url() << std::endl;
    std::cout << "Number of:" << std::endl;
    std::cout << " - States and rates:    " << instanceTask->stateCount() << std::endl;
    std::cout << " - Constants:           " << instanceTask->constantCount() << std::endl;
    std::cout << " - Computed constants:  " << instanceTask->computedConstantCount() << std::endl;
    std::cout << " - Algebraic variables: " << instanceTask->algebraicCount() << std::endl;

    // Output the results of the simulation.

    std::cout << std::endl;
    std::cout << "---[ Simulation results ]---" << std::endl;
    std::cout << std::endl;
    std::cout << instanceTask->voiName() << " (" << instanceTask->voiUnit() << ")";

    for (size_t i = 0; i < instanceTask->stateCount(); ++i) {
        std::cout << "," << instanceTask->stateName(i) << " (" << instanceTask->stateUnit(i) << ")";
    }

    for (size_t i = 0; i < instanceTask->rateCount(); ++i) {
        std::cout << "," << instanceTask->rateName(i) << " (" << instanceTask->rateUnit(i) << ")";
    }

    for (size_t i = 0; i < instanceTask->constantCount(); ++i) {
        std::cout << "," << instanceTask->constantName(i) << " (" << instanceTask->constantUnit(i) << ")";
    }

    for (size_t i = 0; i < instanceTask->computedConstantCount(); ++i) {
        std::cout << "," << instanceTask->computedConstantName(i) << " (" << instanceTask->computedConstantUnit(i) << ")";
    }

    for (size_t i = 0; i < instanceTask->algebraicCount(); ++i) {
        std::cout << "," << instanceTask->algebraicName(i) << " (" << instanceTask->algebraicUnit(i) << ")";
    }

    std::cout << std::endl;

    for (size_t j = 0; j < instanceTask->voi().size(); ++j) {
        std::cout << instanceTask->voi()[j];

        for (size_t i = 0; i < instanceTask->stateCount(); ++i) {
            std::cout << "," << instanceTask->state(i)[j];
        }

        for (size_t i = 0; i < instanceTask->rateCount(); ++i) {
            std::cout << "," << instanceTask->rate(i)[j];
        }

        for (size_t i = 0; i < instanceTask->constantCount(); ++i) {
            std::cout << "," << instanceTask->constant(i)[j];
        }

        for (size_t i = 0; i < instanceTask->computedConstantCount(); ++i) {
            std::cout << "," << instanceTask->computedConstant(i)[j];
        }

        for (size_t i = 0; i < instanceTask->algebraicCount(); ++i) {
            std::cout << "," << instanceTask->algebraic(i)[j];
        }

        std::cout << std::endl;
    }

    return 0;
}
