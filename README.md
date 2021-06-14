# re-os 概述
该Project用于学习OS, 尝试着提出问题，再试着解决问题。  

### TODO:
00.在模拟的x86_64平台上运行简单OS的虚拟环境以及配置 [Done]  
当前使用BOCHS,并提供一个BOCHS的配置 or QEMU的配置  

##### 01.编写在存储设备上0扇区的512字节boot.asm代码[Done]
a).在存储设备0扇区上构建FAT12文件系统分区  

##### 02.编写loader程序，loader要做到如下：[Done]
a).检测硬件信息  
b).处理器切换模式  
c).向内核传递数据  

##### 03.内核执行头程序(head.S):[Done]
a).初始化GDT/IDT  
b).初始化PML4  
c).跳转并执行Start kernel  

##### 04.编写Makefile[Done]

##### 05.实现printk，在屏幕上打印log
a).在屏幕上显示  
b).实现printk  

### Kernel是怎么启动的(kernel的启动流程)
##### 01.编写Boot和Loader来加载并运行kernel。
按下电源，CPU上电，寄存器填充了默认的数据，CPU指针RIP指向了一个地址0xFFFFFFF0(Base+EIP)  
[Accroding to SDM Vol.3A IA-32 and Intel 64 Processor States Following Power-up, Reset, or INIT]。  
这是第一条指令，长跳转到BIOS ROM(大小为1M)，执行BIOS功能，比如上电自检，硬件自检...等。  
在BIOS执行完成之前，会从存储设备中的0号扇区加载512字节到内存0x7C00(512字节内容就是boot程序，boot最终存储到0x7C00位置),  
并是PC指向boot程序地址，执行boot运行。  
CPU处于实模式(x86模式：实模式，保护模式，IA-32e模式，vm8086模式)的boot阶段，主要进行模式切换，从是模式->保护模式->IA32-e模式。  
为下一步加载并执行loader做准备。  
loader阶段：主要加载并运行kernel  


### Setup the image
1.Create boot.image for bochs  
$ bxtimage  

2.Generate boot.bin & loader.bin  
$ nasm boot.asm -o boot.bin  
$ nasm loader.asm -o loader.bin

3.Burn boot.bin into the boot.image  
$ dd if=boot.bin of=boot.image bs=512 count=1 conv=notrunc  

4.Copy loader.bin into the boot.image(already FAT12 filesystem)  
$ mount boot.image /media/ -t vfat -o loop  
$ cp loader.bin /media/  
$ sync; umount /media  

5.Run bochs to verify function for boot.bin and loader.bin  
$ bochs  

### Bochs debug usage
