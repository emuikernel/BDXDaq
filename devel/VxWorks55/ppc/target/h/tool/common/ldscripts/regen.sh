#!/bin/sh

# regen.sh
#
# A shell script to regenerate cross-target linker scripts.
#
# All templated scripts are generated into the current directory, then
# compared against previous versions (already present), which need not be
# writable. Messages are printed to indicate which files should be updated.
#
# modification history
# --------------------
# 01d,24apr02,sn   SPR 75835 - added OUT script
# 01c,29jan02,tpw  GNU: add AOUT,RAM and AOUT,ROM scripts.
# 01b,05dec01,tpw  Add .boot and .reset section support, for PPC 4xx series
#                  and Book E.
# 01a,01nov01,tpw  written

usage()
{
[ -n "$*" ] && echo "$*" 1>&2
echo Usage: $0 '[-update] TOOL' 1>&2
exit 1
}

[ -z "$1" ] && usage

dash_update=no
if [ "X-update" = "X$1" ]; then
    dash_update=yes
    shift
fi

TOOL="$1"
shift

[ -n "$*" ] && usage Extra junk after TOOL argument: "$*"

if [ -z "$WIND_BASE" ]; then
    echo $0: 'No value for $WIND_BASE set.' 1>&2
    exit 1
fi

TGT_DIR=$WIND_BASE/target
GEN_DIR=$TGT_DIR/h/tool/common/ldscripts
GEN_SH=$GEN_DIR/genScript.sh

# regen_one_file <old-script-name> <tool>> <property-list>
regen_one_file ()
{
    # TODO keep blank lines we do want but 
    # get rid of blank lines we don't want!
    sh $GEN_SH "$@" #| sed '/^[	 ]*$/d'
}

regen ()
{
variant="$1"

suffix=`echo "$variant" | tr -d ,`
old=link.$suffix
new=new-link.$suffix

gen_args=`echo "$variant" | tr ',' ' '`

rm -f $new
if regen_one_file $old $TOOL $gen_args > $new ; then
    if cmp -s $old $new ; then
	echo "$old has not changed."
	rm -f $new
    elif [ $dash_update = yes ]; then
	echo "$old has changed, updating with new version."
	mv $new $old
    else
	echo "$old has changed, new version placed in $new"
    fi
else
    rm -f $new
fi
}

case "$TOOL" in
    gnu)	tool_script_list="AOUT,RAM AOUT,ROM OUT" ;;
    *)		tool_script_list=""			;;
esac

common_script_list="RAM ROM DOTBOOT,RAM DOTBOOT,ROM"

for v in $common_script_list $tool_script_list
do
    regen $v
done

exit 0
