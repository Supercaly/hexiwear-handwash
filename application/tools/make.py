import os
import subprocess
import click


@click.group()
def cli():
    pass


@click.command()
@click.argument("app")
@click.option("--mode", type=click.Choice(["svm", "knn", "cnn", "lstm"], case_sensitive=False), required=True, help="Select the machine learning model to use.")
@click.option("-w", "--window", type=int, default=10, show_default=True, help="Select the window size to use.")
@click.option("-p", "--profile", type=click.Choice(["debug", "release", "develop"]), default="develop", show_default=True, help="Select a build profile to use.")
@click.option("-c", "--clean", type=bool, default=False, is_flag=True, help="Perform a clean build.")
@click.option("-f", "--flash", type=bool, default=False, is_flag=True, help="Flash the compiled application to a connected device.")
@click.option("-s", "--sterm", type=bool, default=False, is_flag=True, help="Open a serial terminal connection after flashing the application to a device.")
def build(app, mode, window, profile, clean, flash, sterm):
    """
    Build the selected application with following parameters.
    """
    os.environ["APP"] = app
    os.environ["MODE"] = mode
    os.environ["WS"] = str(window)
    cmd = ["mbed-tools", "compile",
           "-t", "GCC_ARM",
           "-m", "HEXIWEAR",
           "--profile", profile]
    if clean:
        cmd.append("--clean")
    if flash:
        cmd.append("--flash")
    if sterm:
        cmd.append("--sterm")
    subprocess.run(cmd)


@click.command()
def clean():
    """
    Clean the build directory.
    """
    print("Deleting all build files")
    subprocess.run(["rm", "-rf", "cmake_build"])


if __name__ == "__main__":
    cli.add_command(build)
    cli.add_command(clean)
    cli()
