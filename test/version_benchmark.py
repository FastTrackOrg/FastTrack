#!/usr/bin/env python

import os
import pathlib

script_path = str(pathlib.Path(__file__).parent.resolve())

def get_latest_version():
  latest_release = ""
  with open(script_path + "/../FastTrack.pri") as f:
    latest_release = f.readline()[10:15].strip()
  return "v" + latest_release

def compile(versions, cmd):
  os.system("git stash")
  for i in versions:
    os.system("make distclean -s")
    os.system("git checkout -f {}".format(i))
    os.system("qmake6 CONFIG+=release {}/../src/FastTrack-Cli.pro".format(script_path))
    os.system("make")
    os.system("make clean")
    os.system("mv build_cli {}/tmp/{}".format(script_path, i))
    cmd += "\'{base}/tmp/{version}/fasttrack-cli --path {base}/dataSet/images/frame_000001.pgm --cfg {base}/dataSet/images/Groundtruth/Tracking_Result/cfg.toml\' ".format(version=i, base=script_path)
  return cmd

def prepare():
  os.system("mkdir {}/tmp".format(script_path))

def clean_up():
  os.system("rm -r {}/tmp".format(script_path))
  os.system("git checkout -f master")
  os.system("git clean -f {}/dataSet/".format(script_path))
  os.system("git stash apply")

versions = ["master", get_latest_version()]
prepare()
cmd = compile(versions, "hyperfine -w 20 -m 50 ")
os.system(cmd)
clean_up()
