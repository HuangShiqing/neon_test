CROSS_COMPILE =arm-linux-gnueabihf-#指定交叉编译器
DEBUG = 1#指定当前为debug模式
CC = $(CROSS_COMPILE)gcc#指定编译器
CFLAGS = -Wall#指定头文件目录
LDFLAGS = #指定库文件目录
LIBS = #指定库文件名称
TARGET = neon_test#最终生成的可执行文件名

VPATH = #告诉makefile去哪里找依赖文件和目标文件

#选择debug还是release
ifeq ($(DEBUG), 1)
CFLAGS+=-O0 -g
else
CFLAGS+=-Ofast
endif

OBJ = main.o neon.o#中间过程所涉及的.o文件
OBJDIR = ./obj/#存放.o文件的文件夹
OBJS = $(addprefix $(OBJDIR), $(OBJ))#添加路径

#指定需要完成的编译的对象
all: obj $(TARGET)

#将所有的.o文件链接成最终的可执行文件，需要库目录和库名，注意，OBJS要在LIBS之前。另外，如果要指定.o的生成路径，需要保证TARGET的依赖项是含路径的
$(TARGET):$(OBJS)
		$(CC) $(OBJS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(TARGET)
#这个不是静态模式，而是通配符，第一个%类似bash中的*。
$(OBJDIR)%.o: %.c
		$(CC) -mfpu=neon -c $(CFLAGS) $< -o $@
$(OBJDIR)%.o: %.s
		$(CC) -mfpu=neon -c $(CFLAGS) $< -o $@

#用于生成存放.o文件的文件夹
obj:
		mkdir obj
.PHONY : clean
clean :#删除生成的文件夹
		-rm -r obj