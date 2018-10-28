from shutil import copy2
from os.path import basename


Import("env")


# copy files from a given array
def copy_itmes(items):
    for item in items:
        copy2(
            item.__str__(),
            "/".join(["dist", basename(item.__str__())])
        )


# run this after build
def after_build(target, source, env):
    print target[0].__str__()
    copy_itmes(target)
    copy_itmes(source)


# register callback
env.AddPostAction("$BUILD_DIR/${PROGNAME}.hex", after_build)
