# 此文件由makeobj.sh自动生成
############################################################
# 项目名 : 
# 模块名 : 
# 备  注 : 
############################################################

###### 源文件配置区
#@ c_FILE
c_FILE		=	\
			LOGC.c \
			version.c \
			file.c \

###### 目标文件、安装目录配置区
include ../makeinstall
LIB		=	libvh_util.so
LIBINST		=	$(_LIBINST)
HDER		= \
			LOGC.h \
			vh_util.h \

HDERINST	=	$(_HDERBASE)/vh_in

###### 编译选项
CFLAGS		=	$(_CFLAGS) \

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
libvh_util.so		:	$(c_FILE_o)
	$(CC) -o $@ $(c_FILE_o) $(SOFLAGS) $(LFLAGS)

