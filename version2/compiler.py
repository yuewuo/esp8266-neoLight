#! /usr/bin/python3

import json, sys

type_map = {"sleep": 0, "frame": 1}
subtype_map = {"black": 0, "gradual": 1, "raw": 2}

def convert(data):
    if(data["version"] > 2):
        print("Error: Invalid version (%d)" % (data["version"]))
        return None
    result = bytearray()
    result.append(0x02)  #version
    result.append(0x20)    #空格
    result += bytearray(data["name"], encoding="utf-8")
    result.append(0x00)
    result.append(int(data["mask"]*100))
    result.append(data["mask_start"])
    result.append(data["mask_end"])
    if data["repeat"] < 0:
        result.append(0xff)
    else:
        result.append(data["repeat"])
    for procedure in data["procedure"]:
        result.append((type_map[procedure["type"]] << 4) | subtype_map[procedure["subtype"]])
        if(procedure["type"] == 'sleep'):
            time = procedure["time"]
            result.append(time & 0xff)
            result.append((time >> 8) & 0xff)
        if(procedure["type"] == 'frame'):
            result.append(len(procedure["data"]))
            for point in procedure["data"]:
                for i in point:
                    result.append(i)
    return result


def main():
    for filename in sys.argv[1:]:
        with open(filename, "r", encoding="utf-8") as fin:
            input_data = json.load(fin)
            print("name: %s" % (input_data["name"]))
            output = convert(input_data)
            if not output == None:
                print(output.hex())
                print("长度为：%d，压缩率为%f%%" % (len(output), len(output) * 100 / len(str(input_data))))
                with open(filename + ".ledout", "wb") as fout:
                    fout.write(output)


if __name__ == '__main__':
    main()
