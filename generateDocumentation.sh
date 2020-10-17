#!/bin/bash

doxygen docs/dev/Doxyfile

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
mkdir ../../user_manual/
pandoc User_Manual.md -f markdown --mathjax --toc -t html5 -c css/style.css -s -o user_manual.html
cp -r css ../../user_manual
cp -r assets ../../user_manual
mv user_manual.html ../../user_manual
pandoc ../../user_manual/user_manual.html -o ../../user_manual/user_manual.pdf
rm User_Manual.md
