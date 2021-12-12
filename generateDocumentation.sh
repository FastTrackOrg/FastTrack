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
  {
    for part in \
        intro \
        installation \
        example \
        dataPreparation \
        timeline \
        interactiveTracking \
        batchTracking \
        trackingInspector \
        trackingCli \
        trackingParameters \
        parametersSelection  \
        dataOutput
    do
      sed -n '3p' "${part}.md"
      tail -n '+7' "${part}.md"
      echo -e '\n'
    done
  } | sed 's/title:/#/g' > User_Manual.md

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
