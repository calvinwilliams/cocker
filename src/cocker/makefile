# 此文件由makeobj.sh自动生成
############################################################
# 项目名 : 
# 模块名 : 
# 备  注 : 
############################################################

###### 源文件配置区
#@ c_FILE
c_FILE		=	\
			util.c \
			main.c \
			env.c \
			show_images.c \
			show_containers.c \
			show_container_root.c \
			action_create.c \
			action_destroy.c \
			action_boot.c \
			action_shutdown.c \
			action_version.c \
			action_vip.c \
			action_port_mapping.c \
			action_volume.c \
			action_attach.c \
			action_run.c \
			action_rplfile.c \
			action_putfile.c \
			action_getfile.c \
			action_install_test.c \
			action_to_container.c \
			action_to_image.c \
			action_copy_image.c \
			action_del_image.c \
			action_export.c \
			action_import.c \
			show_ssearch.c \
			action_spush.c \
			action_spull.c \

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
			-lcrypto \

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

