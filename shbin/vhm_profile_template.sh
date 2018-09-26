#################################################
# OpenVH profile template
#################################################

set -o vi
# ulimit -c unlimited

export PATH=.:/bin:/usr/bin:/usr/local/bin

alias l='ls -l'
alias ll='ls -lF'
alias lf='ls -F'
alias lrt='ls -lrt'

alias rm='rm -i'
alias mv='mv -i'
alias cp='cp -i'

alias view='vi -R'

OSNAME=`uname -a|awk '{print $1}'`
HOSTNAME=`hostname`
USERNAME=$LOGNAME
export PS1='[$USERNAME@$HOSTNAME $PWD] '

echo "--- Welcome to contrainer for OpenVH ---"

