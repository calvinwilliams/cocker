# 此文件由makeobj.sh自动生成
############################################################
# 项目名 : 
# 模块名 : 
# 备  注 : 
############################################################

###### 源文件配置区
#@ c_FILE
c_FILE		=	\
			list.c \
			LOGC.c \
			filerpl.c \
			version.c \
			file.c \
			string.c \
			socket.c \
			pts.c \

###### 目标文件、安装目录配置区
include ../../makeinstall
LIB		=	libcocker_util.so
LIBINST		=	$(_LIBINST)
HDER		= \
			list.h \
			LOGC.h \
			filerpl.h \
			cocker_util.h \

HDERINST	=	$(_HDERBASE)/cocker_in

###### 编译选项
CFLAGS		=	$(_CFLAGS) \
			-std=gnu99 \

###### 链接选项
LFLAGS		=	$(_LFLAGS) \

###### 额外宏定义区
CLEAN_ADDITION	=

PREMAKE		=

###### 加载mktpl模板库
#@ make_all
#@ make_clean
#@ make_install
#@ make_install_LIBINST
#@ make_install_HDERINST
#@ make_install_DFTHDERINST
#@ make_uninstall
#@ make_uninstall_LIBINST
#@ make_uninstall_HDERINST
#@ make_uninstall_DFTHDERINST
include $(MKTPL2_HOME)/makeobj_$(MKTPL2_OS).inc

###### 目标文件依赖关系
libcocker_util.so	:	$(c_FILE_o)
	$(CC) -o $@ $(c_FILE_o) $(SOFLAGS) $(LFLAGS)

