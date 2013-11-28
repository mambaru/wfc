
if [[ ! -n "$1" ]]; then
   exit -1
fi

version=`git describe --long`
branch="Branch: `git branch | grep '\*' | grep -o '[^ *].\+$'`"
commit=`git log -1 --pretty=format:"commit %H" .`
author=`git log -1 --pretty=format:"Author: %cn <%ce>" .`
date=`git log -1 --pretty=format:"Date: %cd" .`
message=`git log -1 --pretty=%B .`

h_file="build_info.h"
c_file="/tmp/$1_build_info.c"
# ns = "$2"

if [[ ! -f "$h_file" ]]; then
#echo "extern const char* $1_build_info;" > $h_file 
# echo "extern const char* $1_build_info();" > $h_file
echo "extern const char* $1_build_info_string;" > $h_file 
echo "struct $1_build_info{ const char* operator()() const { return $1_build_info_string;} };" >> $h_file 
fi

# echo "const char* $1_build_info(){" > $c_file
# echo "  return " >> $c_file
echo "const char* $1_build_info_string=" > $c_file
echo "    \"$version\n\""  >> $c_file
echo "    \"$branch\n\""   >> $c_file
echo "    \"$commit\n\""   >> $c_file
echo "    \"$author\n\""   >> $c_file
echo "    \"$date\n\""     >> $c_file
echo "    \"$message\n\";" >> $c_file
# echo "}" >> $c_file

# `pwd` > $c_file
g++ -c $c_file -o "/tmp/$1_build_info.o"  >/dev/null 2>&1
ar rvs "/tmp/$1_build_info.a" "/tmp/$1_build_info.o" >/dev/null 2>&1
# ar rs "/tmp/lib$1_build_info.a" "/tmp/$1_build_info.o" 1> /tmp/rvs_log.txt
# ranlib "/tmp/lib$1_build_info.a"
#   rm $c_file

exit

