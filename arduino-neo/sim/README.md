# LED模拟器

用于模拟和测试led控制程序，生成从上到下代表时间流的灯带模拟结果图片

## Usage

1. 执行 ``make clean && make sim``, 生成led_sim程序
2. 将convert.py处理过的led描述文件放入progs文件夹内
3. 将测试用例放入tests文件夹内
4. 执行``./driver``对tests文件夹内所有用例进行模拟，将生成png输出至output文件夹内（此操作会清空output文件夹之前的内容）

## 测试用例格式

至多64行的文本文件，每行格式为

> 文件相对/sim文件夹的路径 开始帧 结束帧

如 ``progs/pro1.in 30 80``，progs/pro1.in描述的灯效将于第30帧加载，于第80帧卸载

其中路径最长64字符，开始帧与结束帧在0-200之间

*注意：目前至多同时加载5个光效*

## 其它

若需单独执行某些测试样例，将它们相对/sim的路径作为命令行参数启动``led_sim``

如``led_sim tests/filename1 tests/filename2``

***注意：此操作会将生成的png输出至tests文件夹，务必在下次调用``driver.sh``前手动移出（懒得判断了hhh）***