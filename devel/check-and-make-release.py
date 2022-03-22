#!../pythonenv/bin/python
import os
import sys
import imp
import shutil
import glob
import fnmatch
import time

def make_release(release_dir):
    if os.access(release_dir, os.R_OK):
        raise Exception("Release directory [{}] already exists!".format(release_dir))

    shutil.copytree("../devel", release_dir, symlinks=True, ignore=shutil.ignore_patterns('*.pyc', '*.log', '*.log.*'))
    for root, dirnames, filenames in os.walk(release_dir):
        for filename in fnmatch.filter(filenames, '*.pyc'):
            f = os.path.join(root, filename)
            os.unlink(f)

    for root, dirnames, filenames in os.walk(release_dir+"/log"):
        for filename in fnmatch.filter(filenames, '*'):
            f = os.path.join(root, filename)
            os.unlink(f)

    os.chdir(release_dir)
    os.system("./m-prod.py collectstatic --noinput")
    os.chdir("..")
    os.system("ln -snf {} prod".format(os.path.basename(release_dir)))
    os.system("touch conf/uwsgi-prod.ini")

def cleanup_old(keep_these):
    #keep 3-days old releases, 2 weekly, 2 monthly
    curdir = os.path.abspath(os.path.dirname(__file__)+"/../")
    now = time.time()
    tokeep_monthly = 0
    tokeep_weekly = 0
    dirs = glob.glob(curdir + "/release-*")
    for d in dirs:
        is_keep = False
        for keep in keep_these:
            if keep in d:
                is_keep = True
                break
        if is_keep:
            continue

        delta = now - os.path.getmtime(d)
        if delta < 86400 * 3:
            continue
        elif delta < 86400 * 7:
            tokeep_weekly += 1
            if tokeep_weekly > 2:
                shutil.rmtree(d)
        elif delta < 86400 * 30:
            tokeep_monthly += 1
            if tokeep_monthly > 2:
                shutil.rmtree(d)
        else:
            shutil.rmtree(d)

if __name__ == "__main__":
    dev_dir = os.path.abspath(os.path.dirname(__file__))
    os.chdir(dev_dir)

    mod = imp.load_source("dev.globalconf", '../devel/settings/globalconf.py')
    dev_app_ver = mod.APP_VERSION

    mod = imp.load_source("prod.globalconf", '../prod/settings/globalconf.py')
    prod_app_ver = mod.APP_VERSION

    if prod_app_ver != dev_app_ver:
        make_release(os.path.abspath("../release-{}".format(dev_app_ver)))
        #time.sleep(2)
        os.chdir(dev_dir)
        cleanup_old([prod_app_ver, dev_app_ver])
    else:
        print("Versions in prod and devel are the same. Release not made.")
