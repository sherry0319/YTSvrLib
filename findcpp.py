#!/usr/bin/python3
import os
import os.path
import sys
import getopt
rootdir = os.getcwd() # 指明被遍历的文件夹
relative = "."
findpath = ""
suffix = ""
outputfile = os.path.join(rootdir,"cppfile.ini");

opts, args = getopt.getopt(sys.argv[1:], "hp:f:o:s:",["pre=","find=","out=","suffix="])
for opt, arg in opts:
	if opt == '-h':
		print ('findcpp.py -p <prefix> -o <outputfile> -f <searchpath>')
		sys.exit()
	elif opt in ("-p", "--pre"):
		relative = arg
		print("relative : " + relative)
	elif opt in ("-o", "--out"):
		outputfile = arg
		print("outputfile : " + outputfile)
	elif opt in ("-f", "--find"):
		findpath = arg
		print("findpath : " + findpath)
	elif opt in ("-s", "--suffix"):
		suffix = arg
		print("suffix : " + suffix)

infofile = open(outputfile, 'w')

try:
	for parent,dirnames,filenames in os.walk(os.path.join(rootdir,findpath)):# 1.父目录 2.所有文件夹名字 3.所有文件名字
		parentRelative = relative + parent.replace(rootdir,'')
		for filename in filenames:# 输出文件信息
			if (filename.find(".cpp") != -1):
				curpath = os.path.join(parentRelative,filename)
				curpath = curpath.replace("\\","/")
				infofile.write(curpath + suffix + "\n")
				print("Find cpp file : " + curpath)# 输出文件路径信息
finally:
	infofile.close()