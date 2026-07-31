/*
Copyright libOpenCOR contributors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <libopencor>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <numeric>
#include <string>
#include <vector>

#ifndef _WIN32
#    include <sys/resource.h>
#endif

namespace {

size_t peakMemoryUsage()
{
#ifdef _WIN32
    // Not currently supported on Windows.

    return 0;
#else
    rusage usage {};

    getrusage(RUSAGE_SELF, &usage);

    // Note: ru_maxrss is in bytes on macOS and in kilobytes on Linux.

#    ifdef __APPLE__
    return static_cast<size_t>(usage.ru_maxrss);
#    else
    return 1024 * static_cast<size_t>(usage.ru_maxrss);
#    endif
#endif
}

bool runBenchmark(const std::string &pName, const std::string &pResourcePath, const libOpenCOR::SolverOdePtr &pOdeSolver, int pNumberOfSteps)
{
    // Create our document, using the given model, and customise its simulation, if needed.

    auto file {libOpenCOR::File::create(std::string(BENCHMARK_RESOURCE_LOCATION) + "/" + pResourcePath)};
    auto document {libOpenCOR::SedDocument::create(file)};
    const auto &simulation {std::dynamic_pointer_cast<libOpenCOR::SedUniformTimeCourse>(document->simulations()[0])};

    if (pOdeSolver != nullptr) {
        simulation->setOdeSolver(pOdeSolver);
    }

    if (pNumberOfSteps > 0) {
        simulation->setOutputEndTime(simulation->outputEndTime() * static_cast<double>(pNumberOfSteps) / static_cast<double>(simulation->numberOfSteps()));
        simulation->setNumberOfSteps(pNumberOfSteps);
    }

    // Instantiate our document and time how long it takes (this includes JIT compiling the model).

    const auto instantiateStart {std::chrono::steady_clock::now()};
    auto instance {document->instantiate()};
    const auto instantiateMs {std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - instantiateStart).count()};

    if (instance->hasIssues()) {
        std::fprintf(stderr, "ERROR: the %s benchmark could not be instantiated:\n", pName.c_str());

        for (size_t i {0}; i < instance->issueCount(); ++i) {
            std::fprintf(stderr, " - %s\n", instance->issue(i)->description().c_str());
        }

        return false;
    }

    // Warm up (i.e. a first, untimed, run) and then time several runs.

    instance->run();

    if (instance->hasIssues()) {
        std::fprintf(stderr, "ERROR: the %s benchmark could not be run.\n", pName.c_str());

        return false;
    }

    static constexpr size_t REPETITION_COUNT {5};

    std::vector<double> times;

    times.reserve(REPETITION_COUNT);

    for (size_t i {0}; i < REPETITION_COUNT; ++i) {
        const auto start {std::chrono::steady_clock::now()};

        instance->run();

        times.push_back(std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count());
    }

    std::sort(times.begin(), times.end());

    const auto mean {std::accumulate(times.begin(), times.end(), 0.0) / static_cast<double>(REPETITION_COUNT)};
    const auto numberOfSteps {simulation->numberOfSteps()};
    const auto peakMemoryMb {static_cast<double>(peakMemoryUsage()) / (1024.0 * 1024.0)};

    std::printf("%-16s | %7d | %9.3f | %8.3f | %11.3f | %9.3f | %14.1f | %9.1f\n",
                pName.c_str(), numberOfSteps, instantiateMs,
                times.front(), times[times.size() / 2], mean,
                mean * 1.0e6 / static_cast<double>(numberOfSteps), peakMemoryMb);

    return true;
}

} // namespace

int main()
{
    std::printf("Benchmark        |   Steps | Inst (ms) | Min (ms) | Median (ms) | Mean (ms) | Mean (ns/step) | Peak (MB)\n");
    std::printf("-----------------+---------+-----------+----------+-------------+-----------+----------------+----------\n");

    auto ok {true};

    ok = runBenchmark("cvode-50k", "cellml_2.sedml", nullptr, -1) && ok;
    ok = runBenchmark("cvode-1m", "cellml_2.sedml", nullptr, 1000000) && ok;
    ok = runBenchmark("euler-1m", "cellml_2.sedml", libOpenCOR::SolverForwardEuler::create(), 1000000) && ok;
    ok = runBenchmark("heun-1m", "cellml_2.sedml", libOpenCOR::SolverHeun::create(), 1000000) && ok;
    ok = runBenchmark("rk2-1m", "cellml_2.sedml", libOpenCOR::SolverSecondOrderRungeKutta::create(), 1000000) && ok;
    ok = runBenchmark("rk4-1m", "cellml_2.sedml", libOpenCOR::SolverFourthOrderRungeKutta::create(), 1000000) && ok;
    ok = runBenchmark("dae-cvode", "api/sed/dae/model.sedml", nullptr, -1) && ok;
    ok = runBenchmark("tt04-3k", "benchmark/tt04.sedml", nullptr, -1) && ok;
    ok = runBenchmark("tt04-100k", "benchmark/tt04.sedml", nullptr, 100000) && ok;
    ok = runBenchmark("hypercapnea", "benchmark/hypercapnea.sedml", nullptr, -1) && ok;
    ok = runBenchmark("hypercapnea-30k", "benchmark/hypercapnea.sedml", nullptr, 30000) && ok;

    return ok ? 0 : 1;
}
