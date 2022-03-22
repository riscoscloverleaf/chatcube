#/bin/bash
mkdir -p html
cd html
xsltproc /usr/share/sgml/docbook/xsl-stylesheets/html/chunk.xsl ../tbx.xml
cd ..

