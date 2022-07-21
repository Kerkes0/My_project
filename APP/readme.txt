1.文件目录说明
CORE--存放MCU内核相关代码文件
DEBUG--存放编译调试生成文件
	BIN--存放烧写代码，含BIN格式和HEX格式
	JLK--存放调试配置文件
	LST--存放编译信息
	OBJ--存放编译的目标文件
FWLib--存放MCU外设固件库
USER--存放项目功能代码

2.关键文件说明
.\xx.uvprojx--工程文件
.\DEBUG\BIN\xx.hex--烧写代码
.\DEBUG\LST\xx.map--代码链接信息文件
.\CLEAN.bat--垃圾文件清理执行程序

3.工程移植说明
修改工程文件名称同时，需要打开工程，把Options for Target--Output--Name of Executable：改成新的工程名

4.已知存在的问题
	①.工程移植后，使用CLEAN.bat无法清理旧的工程垃圾