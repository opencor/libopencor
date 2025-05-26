import libopencor as loc

if __name__ == "__main__":
    # Output the version of libOpenCOR.

    print("---[ libOpenCOR version information ]---")
    print("")
    print(f"Version:        {loc.version()}")
    print(f"Version string: {loc.version_string()}")

    # Load a COMBINE archive from GitHub and output some information about it.

    file = loc.File(
        "https://raw.githubusercontent.com/opencor/libopencor/master/tests/res/cellml_2.omex"
    )
    document = loc.SedDocument(file)
    simulation = document.simulations[0]

    simulation.output_end_time = 5.0
    simulation.number_of_steps = 500

    instance = document.instantiate()

    instance.run()

    instance_task = instance.tasks[0]

    print("")
    print("---[ File information ]---")
    print("")
    print(f"URL: {file.url}")
    print("Number of:")
    print(f" - States and rates:    {instance_task.state_count}")
    print(f" - Constants:           {instance_task.constant_count}")
    print(f" - Computed constants:  {instance_task.computed_constant_count}")
    print(f" - Algebraic variables: {instance_task.algebraic_count}")

    # Output the results of the simulation.

    print("")
    print("---[ Simulation results ]---")
    print("")
    print(f"{instance_task.voi_name} ({instance_task.voi_unit})", end="")

    for i in range(instance_task.state_count):
        print(
            f", {instance_task.state_name(i)} ({instance_task.state_unit(i)})", end=""
        )

    for i in range(instance_task.rate_count):
        print(f", {instance_task.rate_name(i)} ({instance_task.rate_unit(i)})", end="")

    for i in range(instance_task.constant_count):
        print(
            f", {instance_task.constant_name(i)} ({instance_task.constant_unit(i)})",
            end="",
        )

    for i in range(instance_task.computed_constant_count):
        print(
            f", {instance_task.computed_constant_name(i)} ({instance_task.computed_constant_unit(i)})",
            end="",
        )

    for i in range(instance_task.algebraic_count):
        print(
            f", {instance_task.algebraic_name(i)} ({instance_task.algebraic_unit(i)})",
            end="",
        )

    print("")

    for j in range(len(instance_task.voi)):
        print(instance_task.voi[j], end="")

        for i in range(instance_task.state_count):
            print(f", {instance_task.state(i)[j]}", end="")

        for i in range(instance_task.rate_count):
            print(f", {instance_task.rate(i)[j]}", end="")

        for i in range(instance_task.constant_count):
            print(f", {instance_task.constant(i)[j]}", end="")

        for i in range(instance_task.computed_constant_count):
            print(f", {instance_task.computed_constant(i)[j]}", end="")

        for i in range(instance_task.algebraic_count):
            print(f", {instance_task.algebraic(i)[j]}", end="")

        print("")
