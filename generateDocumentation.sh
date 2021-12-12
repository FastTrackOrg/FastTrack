#!/bin/bash

set -o nounset
set -o errexit

PROG="$0"
USER=0
DEV=0

usage()
{
  cat <<EOF
Usage: ${PROG} [TYPE]

TYPE may be "user", "dev", or "all" (default: "all").
EOF
}

if (( $# > 1 )); then usage; exit 1; fi
case "${1-all}" in
  -h,--help) usage; exit 0 ;;
  user) USER=1 ;;
  dev) DEV=1 ;;
  all) USER=1; DEV=1 ;;
  *) usage; exit 1 ;;
esac

if [[ "${DEV}" != 0 ]]
then
  doxygen docs/dev/Doxyfile
  cp docs/dev/*png Documentation/html/
  cd Documentation/latex/
  make
  cd ../..
  mkdir -p manual/dev/html
  mv Documentation/html manual/dev/html
  mv Documentation/latex/refman.pdf manual/dev/
  rm -r Documentation
fi

if [[ "${USER}" != 0 ]]
then
  cd docs/user
  sed -n "3p" intro.md > User_Manual.md
  tail -n +7 intro.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" installation.md >> User_Manual.md
  tail -n +7 installation.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" example.md >> User_Manual.md
  tail -n +7 example.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" dataPreparation.md >> User_Manual.md
  tail -n +7 dataPreparation.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" timeline.md >> User_Manual.md
  tail -n +7 timeline.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" interactiveTracking.md >> User_Manual.md
  tail -n +7 interactiveTracking.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" batchTracking.md >> User_Manual.md
  tail -n +7 batchTracking.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" trackingInspector.md >> User_Manual.md
  tail -n +7 trackingInspector.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" trackingCli.md >> User_Manual.md
  tail -n +7 trackingCli.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" trackingParameters.md >> User_Manual.md
  tail -n +7 trackingParameters.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" parametersSelection.md >> User_Manual.md
  tail -n +7 parametersSelection.md >> User_Manual.md
  echo -e "\n" >> User_Manual.md
  sed -n "3p" dataOutput.md >> User_Manual.md
  tail -n +7 dataOutput.md >> User_Manual.md
  sed -i "s/title:/#/g" User_Manual.md
  mkdir -p ../../manual/user/html
  pandoc User_Manual.md -f markdown --mathjax --toc -t html5 -c css/style.css -s -o user_manual.html
  cp -r css ../../manual/user/html
  cp -r assets ../../manual/user/html
  mv user_manual.html ../../manual/user/html
  pandoc ../../manual/user/html/user_manual.html -o ../../manual/user/user_manual.pdf
  rm User_Manual.md
  cd ../..
  tar -czvf manual.tar.gz manual/
fi
