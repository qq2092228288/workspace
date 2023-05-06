#!/bin/bash
#################    Qt打包脚本源码    ########################
#================================================================================
# Copyright (c) 2012 - 2013 by William Hallatt.
# 
# This script is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This script is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this script (GNUGPL.txt).  If not, see
#
#                    <http://www.zeyaotebco.com/>
# 
# Should you wish to contact me for whatever reason, please do so via:
#
#                 <http://www.zeyaotebco.com/>
# 
#================================================================================
 
# Copy your executable as well as this script to the directory where you 
# want to create the final tar ball.  To execute, simply pass the name of the
# executable as command line parameter to this script.
#
# Worth noting is that I built Qt from source myself, you may or may not need 
# additional plugins, etc and different or additional directory structures and
# will have to edit this script to suit your needs!
 
if [ $# -ne 1 ]
then
        echo "Usage: $0 <executable name>"
        exit 1
fi
 
deployscript=${0##*/}	# delete path and leave file name remained
executable=$1
 
# Obtain the Linux flavour and version.
# distro=`lsb_release -d | awk '{print $2$3$4}' | sed 's/\./_/g'`
 
# Create the directory that will be tarred up for distribution.
# tardir=`echo $executable"_"$distro | awk '{print tolower($0)}'`
tardir=`echo $executable"_deploy" | awk '{print tolower($0)}'`
mkdir $tardir
echo "Created tar ball directory: "$tardir
 
# Copy executable across.
chmod u+x $executable
cp $executable $tardir
echo "Copied executable "$executable" to "$tardir
 
# Copy other files and dirs.
nfiles=0
ndirs=0
for files_dirs in `ls ./`
do
  if [ "$files_dirs" != "$deployscript" ] && [ "$files_dirs" != "$executable" ] && [ "$files_dirs" != "$tardir" ]; then
	if [ -d "$files_dirs" ]; then
		ndirs=$(($ndirs+1))
	else
		nfiles=$(($nfiles+1))
	fi
 
  	cp -r $files_dirs $tardir
  fi
done
echo "Copied other $nfiles file(s) and $ndirs dir(s) to "$tardir
 
# Create the libs directory.
libsdir=$PWD/$tardir/libs
mkdir $libsdir 
echo "Created libs directory: "$libsdir
 
# Copy all dependencies across to the tar directory.
echo "Copying dependencies..."
 
for dep in `ldd ./$executable | awk '{print $3}' | grep -v "("`
do
  cp $dep $libsdir
  echo "Copied dependency "$dep" to "$libsdir
done

# Qt plugins directory
qtpluginsdir=/home/ubuntu/Qt5.12.12/5.12.12/gcc_64/plugins
 
# You will need to change this to point to wherever libqxcb.so lives on your PC.
qtplatformplugin=$qtpluginsdir/platforms/libqxcb.so #替换到本机下“libqxcb.so”的路径
qtplatformplugindir=$tardir/platforms
mkdir $qtplatformplugindir
echo "Created platforms directory: "$qtplatformplugindir
cp $qtplatformplugin $qtplatformplugindir
echo "Copied platform "$qtplatformplugin" to "$qtplatformplugindir
 
echo "Copying qtplatformplugin dependencies..."
for dep in `ldd $qtplatformplugin | awk '{print $3}' | grep -v "("`
do
  cp -u $dep $libsdir
  echo "Copied qtplatformplugin dependency "$dep" to "$libsdir
done
echo "Copied qtplatformplugin dependencies"

# Create the plugins directory.
pluginsdir=$PWD/$tardir/plugins
mkdir $pluginsdir
echo "Created plugins directory: "$pluginsdir
cp -r $qtpluginsdir/sqldrivers $pluginsdir
echo "Copied sqldrivers directory"
 
# Create the run script.
execscript=$tardir/"run$executable.sh"
echo "Created run script: "$execscript
 
echo "#!/bin/sh" > $execscript
echo "export LD_LIBRARY_PATH=\`pwd\`/libs" >> $execscript
echo "export QT_PLUGIN_PATH=\`pwd\`/plugins" >> $execscript
# echo "export QT_QPA_FONTDIR=\`pwd\`/fonts" >> $execscript
echo "nohup ./$executable &" >> $execscript
 
# Make executable.
chmod u+x $execscript
 
# Create a README
echo "Creating README..."
 
readme=$tardir/README
echo "================================================================================" >> $readme
echo "Please launch $executable via" >> $readme
echo "" >> $readme
echo "                 $execscript" >> $readme
echo "" >> $readme
echo "If you run into any trouble regarding dependencies, all you need to do is to" >> $readme
echo "run " >> $readme
echo "                 $fixscript " >> $readme
echo "" >> $readme
echo "in order to automatically resolve dependencies on your behalf " >> $readme
echo "(note that you will need administrator privileges for this as this script will" >> $readme
echo "download the necessary libraries for your platform). " >> $readme
echo "" >> $readme
echo "Should you wish to contact me for whatever reason, please do so via:" >> $readme
echo "" >> $readme
echo "                 <http://www.zeyaotebco.com/>" >> $readme
echo "" >> $readme
echo "================================================================================" >> $readme
 
echo "Creating tarball..."
tar -zcvf $tardir".tar" $tardir
 
echo "Cleaning up..."
rm -rf $tardir
echo "Done!"