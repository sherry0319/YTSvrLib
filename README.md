# YTSvrLib [![TravisCI Status](https://travis-ci.org/sherry0319/YTSvrLib.svg?branch=master)](https://travis-ci.org/sherry0319/YTSvrLib)  [![AppVeyor Status](https://ci.appveyor.com/api/projects/status/github/sherry0319/ytsvrlib?branch=master&svg=true)](https://ci.appveyor.com/project/sherry0319/ytsvrlib)
a simple&powerful game server library for both linux/windows

What's new in version 2.0
更新2.0测试版本
> Removed libevent dependency and change the network lib to asio.
> 移除了对libevent库的依赖,全面开始转向C++11/14
> Optimized the network lib and websocket lib and make it more easier to use.
> 对网络库做了大量优化,使接口更清晰易用.剔除了很多容易引起使用者误会的接口.
> Update msvc to vs2017 for feature of c++11/14
> 为了利用C++11/14的新特性,升级Windows编译器和所有依赖库至vs2017

# Install Windows
1.Install Visual Studio 2017.

2.Start with YTSvrLib.sln

3.Build solution, get the YTSvrLib.lib

4.Use the .lib in your project.

# Install Linux
1.Install dependent libraries : libiconv libcurl libmysqlpp libmysqlclient libpthread libtcmalloc

2.cd src && ./configure && make install

3.You can get libytsvrlib.so.

4.Use the libytsvrlib.so in your project.

# How to use
See the project "Sample".


# 在Windows下使用
1.安装 Visual Studio 2017 

2.使用YTSvrLib.sln打开项目

3.编译项目,你可以得到YTSvrLib.lib

4.在你的工程中链接lib

# 在Linux下使用
1.先安装以下依赖库 : libiconv libcurl libmysqlpp libmysqlclient libpthread libtcmalloc

2.cd src && ./configure && make install (或者你可以直接用./makeall.sh来获得debug版本或者./makeall_release.sh来获得release版本)

3.你将会得到libytsvrlib.so

4.将它用在你的项目中.

# 如何使用
请参考"Sample"项目了解使用方法.