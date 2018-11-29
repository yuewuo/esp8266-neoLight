# Version2

## compiler

将.json文件编译成二进制格式

### 格式

**version:** 02

**name:** 20(空格) XX XX XX ... XX 00(结束符)

**header:**

1. M (1byte unsigned, 表示mask值的100倍)
2. mask_start (1 byte unsigned)
3. mask_end (1 byte unsigned)
4. repeat (1 byte unsigned, FF表示无限循环)

**procedures:**

每部分编码成如下格式：

1. type:subtype (1 byte)
2. data (data/time/..., some bytes)

其中：

|val|type|
|:-:|:-:|
|0|sleep|
|1|frame|

|val|subtype|type|
|:-:|:-:|:-:|
|0|black|sleep|
|1|gradual|sleep & frame|
|2|raw|sleep|

#### data

若type为sleep: data部分为2 bytes unsigned, 表示睡眠时间毫秒数(小端法)

若type为frame: data部分开头为1 byte unsigned 表示接下来的块数，之后为若干4字节块，块内每字节分别对应灯号(0x00-0xFF)，R，G，B