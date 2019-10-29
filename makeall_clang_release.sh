#!/bin/bash
base_dir=`pwd`
web_dir=/home/webnew/update
echo $base_dir
svn update
cd $base_dir"/cmake_release"
make -j5 install
file="libytsvrlib.so"
if test -f $file
then
        if [ ! -f $web_dir"/"$file ]
        then
                cp -rf $file $web_dir
                tar -czkf $web_dir"/"$file".tar.gz" $file
        fi

        newer=`find $file -newer $web_dir"/"$file`
        if [ "$newer" == "$file" ]
        then
                cp -rf $file $web_dir
                tar -czkf $web_dir"/"$file".tar.gz" $file
        fi
fi
cd $web_dir
zipfile=$file".tar.gz"
md5file=$zipfile".md5"
if test -f $zipfile
then
        rm -rf $md5file
        md5sum $zipfile >> $md5file
fi
cd $base_dir

