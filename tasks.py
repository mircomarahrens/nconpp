# standard
import os
import json
import subprocess
from pathlib import Path

# 3rd party
from invoke import task
from dotenv import load_dotenv
from git import Repo

# load variables from .env-file
load_dotenv()

# common paths
root_p = Path(os.getenv("PWD"))
vcpkg_p = Path(str(root_p) + "/utils/" + os.getenv("VCPKG_DIR"))


@task
def init(ctx):
    print("Initializing the project...")

    # # create vcpkg.json
    # if not os.path.exists(os.getenv("PWD") + "/vcpkg.json"):
    #     with open("templates/vcpkg.json", "r") as f:
    #         data = f.read()

    #     data = data.replace("${PROJECT_NAME}", os.getenv("PROJECT_NAME"))

    #     with open("vcpkg.json", "w") as f:
    #         f.write(data)

    # # create CMakeLists.txt
    # if not os.path.exists(os.getenv("PWD") + "/CMakeLists.txt"):
    #     with open("templates/CMakeLists.txt", "r") as f:
    #         data = f.read()

    #     data = data.replace("${PROJECT_NAME}", os.getenv("PROJECT_NAME"))
    #     data = data.replace("${PROJECT_VERSION}", os.getenv("PROJECT_VERSION"))

    #     with open("CMakeLists.txt", "w") as f:
    #         f.write(data)

    # # create vcpkg directory
    # vcpkg_p.mkdir(parents=True, exist_ok=True)

    # # clone vcpkg repo to the utils directory
    # if os.listdir(vcpkg_p) == 0:
    #     Repo.clone_from(os.getenv("VCPKG_REPO"), vcpkg_p)

    # # add VCPKG_ROOT to .env-file
    # if os.getenv("VCPKG_ROOT") is None:
    #     with open(".env", "a") as f:
    #         f.write("VCPKG_ROOT=" + "\"" + str(vcpkg_p) + "\"" + "\n")

    # # bootstrap vcpkg
    # if not os.path.exists(os.getenv("VCPKG_ROOT") + "/vcpkg"):
    #     subprocess.run([os.getenv("VCPKG_ROOT") + "/bootstrap-vcpkg.sh",
    #                     "--disableMetrics"])

    # # add initial baseline to vcpkg.json
    # with open(os.getenv("PWD") + "/vcpkg.json", "r") as f:
    #     data = json.load(f)
    #     if "builtin-baseline" not in data:
    #         subprocess.run([os.getenv("VCPKG_ROOT") + "/vcpkg",
    #                        "x-update-baseline", "--add-initial-baseline"])

    print("Project initialized!")


@task
def config(ctx):
    print("Configuring the project...")


@task
def build(ctx):
    print("Building the project...")


@task
def test(ctx):
    print("Testing the project...")


@task
def clean(ctx):
    print("Cleaning the project...")


@task
def deploy(ctx):
    print("Deploying the project...")
