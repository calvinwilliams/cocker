# 此文件由makeobj.sh自动生成
############################################################
# 项目名 : 
# 模块名 : 
# 备  注 : 
############################################################

###### 源文件配置区
#@ c_FILE
c_FILE		=	\
			main.c \
			env.c \
			show_images.c \
			show_containers.c \
			action_install_test.c \
			action_create.c \
			action_destroy.c \
			action_start.c \
			action_stop.c \

###### 目标文件、安装目录配置区
include ../../makeinstall
BIN		=	cocker
BININST		=	$(_BININST)
HDER		= \
			cocker_in.h \

###### 编译选项
CFLAGS		=	$(_CFLAGS) \
			-std=gnu99 \
			-I$(_HDERBASE)/cocker_in \

###### 链接选项
LFLAGS		=	$(_LFLAGS) \
			-L$(_LIBBASE) \
			-lcocker_util \

###### 额外宏定义区
CLEAN_ADDITION	=

PREMAKE		=

###### 加载mktpl模板库
#@ make_all
#@ make_clean
#@ make_install
#@ make_install_BININST
#@ make_install_HDERINST
#@ make_install_DFTHDERINST
#@ make_uninstall
#@ make_uninstall_BININST
#@ make_uninstall_HDERINST
#@ make_uninstall_DFTHDERINST
include $(MKTPL2_HOME)/makeobj_$(MKTPL2_OS).inc

###### 目标文件依赖关系
cocker		:	$(c_FILE_o)
	$(CC) -o $@ $(c_FILE_o) $(LFLAGS)

