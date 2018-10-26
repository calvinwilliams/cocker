/*
 * cocker - Container Machine Engine
 * author	: calvin
 * email	: calvinwilliams@163.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

export PATH=.:/bin:/usr/bin:/usr/local/bin
export PATH=.:/sbin:/usr/sbin:/usr/local/sbin:$PATH

alias l='ls -l'
alias ll='ls -lF'
alias lf='ls -F'
alias lrt='ls -lrt'

alias rm='rm -i'
alias mv='mv -i'
alias cp='cp -i'

alias view='vi -R'

set -o vi

OSNAME=`uname -a|awk '{print $1}'`
HOSTNAME=`hostname`
USERNAME=$LOGNAME
export PS1='[$USERNAME@$HOSTNAME $PWD] '


