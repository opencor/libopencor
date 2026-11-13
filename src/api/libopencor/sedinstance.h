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

#pragma once

#include "libopencor/logger.h"

namespace libOpenCOR {

/**
 * @brief The SedInstance class.
 *
 * The SedInstance class is used to describe an instance of a simulation experiment description.
 *
 * Deleting an instance stops any run in progress (be it running or paused) and waits for it to stop, so the results of
 * a task that is kept beyond the deletion of its instance may be incomplete.
 */

class LIBOPENCOR_EXPORT SedInstance: public Logger
{
    friend class SedDocument;

public:
    /**
     * Constructors, destructor, and assignment operators.
     */

    ~SedInstance() override; /**< Destructor, @private. */

    SedInstance(const SedInstance &pOther) = delete; /**< No copy constructor allowed, @private. */
    SedInstance(SedInstance &&pOther) noexcept = delete; /**< No move constructor allowed, @private. */

    SedInstance &operator=(const SedInstance &pRhs) = delete; /**< No copy assignment operator allowed, @private. */
    SedInstance &operator=(SedInstance &&pRhs) noexcept = delete; /**< No move assignment operator allowed, @private. */

    /**
     * @brief The status of an instance.
     *
     * The status of an instance, i.e. whether it is idle, running, or paused.
     */

    enum class Status
    {
        IDLE, /**< The instance is idle. */
        RUNNING, /**< The instance is currently running. */
        PAUSED /**< The instance is currently paused. */
    };

    /**
     * @brief Return the status of this instance.
     *
     * Return the status of this instance.
     *
     * @return The status of this instance.
     */

    Status status() const noexcept;

    /**
     * @brief Return the delay between two output points of a run.
     *
     * Return the delay, in milliseconds, between two consecutive output points of a run (see @ref setDelay).
     *
     * @return The delay in milliseconds.
     */

    double delay() const noexcept;

    /**
     * @brief Set the delay between two output points of a run.
     *
     * Set the delay, in milliseconds, between two consecutive output points of a run, so that it can be slowed down
     * (e.g., to watch its results being plotted progressively). The delay can be fractional (e.g., @c 0.001 for one
     * microsecond), which is useful for runs with many output points, while a negative or NaN delay is treated as no
     * delay. Once computed, an output point is only made available when the delay has elapsed since the previous one
     * was, meaning that output points are made available at a steady rate, whatever the time it takes to compute them
     * (unless it takes longer than the delay). Very short delays may not be honoured exactly for each output point
     * (since the operating system may wait for longer than requested), but they are honoured on average. The delay can
     * be changed while a run is in progress, in which case it takes effect straight away (i.e. a run that is waiting
     * stops waiting if the new delay has already elapsed). However, if a delay is set while a run without one is in
     * progress, then the next output point is made available straight away and the delay only applies from there.
     * Pausing a run suspends its delay (i.e. once resumed, the run waits for whatever was left of its delay when it got
     * paused) while stopping a run interrupts it. If this instance has several tasks, then they are run one after the
     * other and the initial output point of a task is made available straight after the last output point of the
     * previous task (i.e. the delay only applies between the output points of a given task). The time spent waiting is
     * not included in the elapsed time of a run (see @ref run and @ref waitForRun). The delay is @c 0 (i.e. no delay)
     * by default.
     *
     * @param pDelay The delay in milliseconds.
     */

    void setDelay(double pDelay);

    /**
     * @brief Run all the tasks associated with this instance.
     *
     * Run all the tasks associated with this instance. If a run is already in progress (see @ref startRun), then wait
     * for it to complete before running all the tasks associated with this instance.
     *
     * Note: in a Web browser, calling this method on the main thread straight after @ref startRun() blocks forever
     * since the background thread cannot start until the main thread returns to its event loop, so poll @ref status()
     * until the instance is idle first.
     *
     * @return The elapsed time in milliseconds.
     */

    double run();

    /**
     * @brief Start running, in a background thread, all the tasks associated with this instance.
     *
     * Start running, in a background thread, all the tasks associated with this instance. The results of the tasks
     * are (re)allocated before this method returns, so they can be retrieved while the tasks are being run (e.g., to
     * plot them progressively). The simulation settings and model changes used are those in effect when this method is
     * called.
     *
     * Note: in a Web browser, this method can only be used if the page is cross-origin isolated (i.e. served with the
     * "Cross-Origin-Opener-Policy: same-origin" and "Cross-Origin-Embedder-Policy: require-corp" HTTP headers) since
     * threads are otherwise not supported, in which case an error is thrown. Also, if the page has a Content Security
     * Policy, it must allow blob: workers (i.e. worker-src blob:) since the run would otherwise never start, i.e. the
     * status of the instance would remain RUNNING. Finally, deleting the instance on the main thread before its run has
     * actually started (e.g., straight after calling this method) blocks forever since the instance stops its run and
     * waits for it to complete before being deleted, but the background thread cannot start until the main thread
     * returns to its event loop.
     *
     * @return @c true if a new run was started, @c false if a run is already in progress.
     */

    bool startRun();

    /**
     * @brief Wait for any currently-running instance to complete.
     *
     * Wait for any currently-running instance to complete.
     *
     * Note: in a Web browser, calling this method on the main thread straight after @ref startRun() blocks forever
     * since the background thread cannot start until the main thread returns to its event loop, so poll @ref status()
     * until the instance is idle first.
     *
     * @return The elapsed time in milliseconds for the last completed instance run.
     */

    double waitForRun();

    /**
     * @brief Pause a currently-running instance.
     *
     * Pause a currently-running instance.
     */

    void pauseRun();

    /**
     * @brief Resume a currently-paused instance.
     *
     * Resume a currently-paused instance.
     */

    void resumeRun();

    /**
     * @brief Stop any currently-running instance.
     *
     * Stop any currently-running instance. This is also done when an instance is deleted.
     */

    void stopRun();

    /**
     * @brief Return the progress of the current instance run.
     *
     * Return the progress of the current instance run as a value between @c 0.0 (not started) and @c 1.0 (complete).
     *
     * @return The progress as a value in [0.0, 1.0].
     */

    double progress() const noexcept;

    /**
     * @brief Return whether there are some tasks.
     *
     * Return whether there are some tasks.
     *
     * @return @c true if there are some tasks, @c false otherwise.
     */

    bool hasTasks() const noexcept;

    /**
     * @brief Return the number of tasks.
     *
     * Return the number of tasks.
     *
     * @return The number of tasks.
     */

    size_t taskCount() const noexcept;

    /**
     * @brief Return all the tasks.
     *
     * Return all the tasks.
     *
     * @return The tasks, as a @ref SedInstanceTaskPtrs.
     */

    const SedInstanceTaskPtrs &tasks() const noexcept;

    /**
     * @brief Return the task at the given index.
     *
     * Return the task at the given index.
     *
     * @param pIndex The index of the task to return.
     *
     * @return The task as a @ref SedInstanceTaskPtr, if the index is valid, @c nullptr otherwise.
     */

    const SedInstanceTaskPtr &task(size_t pIndex) const noexcept;

private:
    class Impl; /**< Forward declaration of the implementation class, @private. */

    explicit SedInstance(const SedDocumentPtr &pDocument); /**< Constructor @private. */

    Impl *pimpl(); /**< Private implementation pointer, @private. */
    const Impl *pimpl() const; /**< Constant private implementation pointer, @private. */
};

} // namespace libOpenCOR
