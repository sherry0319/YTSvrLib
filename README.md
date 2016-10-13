# YTSvrLib
a simple game server library for linux/windows

# Install Windows
1.Install Visual Studio 2013.

2.Start with YTSvrLib.sln

3.Build solution, get the YTSvrLib.lib

4.Use the .lib in your project.

# Install Linux
1.Install dependent libraries : libiconv libevent2 libcurl libhiredis libmysqlpp libmysqlclient libpthread libtcmalloc

2.cd src && ./configure && make install

3.You can get libytsvrlib.so.

4.Use the libytsvrlib.so in your project.
# How to use
See the project "Sample".


# 在Windows下使用
1.安装 Visual Studio 2013 

2.使用YTSvrLib.sln打开项目

3.编译项目,你可以得到YTSvrLib.lib

4.在你的工程中链接lib

# 在Linux下使用
1.先安装以下依赖库 : libiconv libevent2 libcurl libhiredis libmysqlpp libmysqlclient libpthread libtcmalloc

2.cd src && ./configure && make install

3.你将会得到libytsvrlib.so

4.将它用在你的项目中.

# 如何使用
请参考"Sample"项目了解使用方法.
