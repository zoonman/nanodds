from shutil import copy2
from os.path import basename


Import("env")


# copy files from a given array
def copy_items(items):
    for item in items:
        copy2(
            item.__str__(),
            "/".join(["dist", basename(item.__str__())])
        )


# run this after build
def after_build(target, source, env):
    copy_items(target)
    copy_items(source)


# register callback
env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", after_build)
