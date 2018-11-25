#!/usr/bin/python3
import json

print("此脚本用来将JSON文件转换为字符串，方便C语言处理")
"""
第一行：
第一个数字为版本号，只识别自己支持的版本号，否则丢弃该procedure
<version>
"""

def converter_1(js):
    st = "%d %d %d %d\n" % (int(js["mask"] * 128), js["mask_start"], js["mask_end"], js["repeat"])  # mask 乘128方便右移实现
    for ele in js["procedure"]:
        if ele["type"] == "frame" and ele["subtype"] == "gradual":
            st += "fg:"
            for e in ele["data"]:
                st += "%d %d %d %d;" % (e[0], e[1], e[2], e[3])
            st += "\n"
        elif ele["type"] == "sleep" and ele["subtype"] == "gradual":
            st += "sg:%d\n" % ele["time"]
        elif ele["type"] == "sleep" and ele["subtype"] == "raw":
            st += "sr:%d\n" % ele["time"]
    return st

filename = input("输入JSON文件路径：")
with open(filename, 'r', encoding='utf-8') as f:
    js = json.load(f)
    print(js)
    print("版本为%d，使用converter_%d" % (js["version"], js["version"]))
    if js["version"] == 1:
        st = "1 %s\n" % js["name"] + converter_1(js)
        print(st)
        print("长度为：%d，压缩率为%f%%" % (len(st), len(st) * 100 / len(str(js))))
        with open(filename + ".txt", 'wb') as fw:
            fw.write(bytes(st, encoding="utf-8"))
    else:
        print("版本%d不支持" % print(js["version"]))
