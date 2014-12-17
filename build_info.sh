
if [[ ! -n "$1" ]]; then
   exit -1
fi

if [[ ! -n "$2" ]]; then
   path="/tmp"
else
   path=$2
fi

version=`git describe --long 2>/dev/null`
branch="Branch: `git branch | grep '\*' | grep -o '[^ *].\+$' `"
commit=`git log -1 --pretty=format:"commit %H" . `
author=`git log -1 --pretty=format:"Author: %cn <%ce>" .`
date=`git log -1 --pretty=format:"Date: %cd" .`
message=`git log -1 --pretty=format:"%s" .`

h_file="$1_build_info.h"
c_file="$path/$1_build_info.c"
o_file="$path/$1_build_info.o"
a_file="$path/$1_build_info.a"

if [[ ! -f "$h_file" ]]; then
echo "extern const char* $1_build_info_string;" > $h_file 
echo "struct $1_build_info{ const char* operator()() const { return $1_build_info_string;} };" >> $h_file 
fi

if [[ -f "$c_file" ]]; then
  mv $c_file "$c_file~1"
fi

echo "const char* $1_build_info_string=" > $c_file
echo "    \"$version\n\""  >> $c_file
echo "    \"$branch\n\""   >> $c_file
echo "    \"$commit\n\""   >> $c_file
echo "    \"$author\n\""   >> $c_file
echo "    \"$date\n\""     >> $c_file
echo "    \"$message\n\";" >> $c_file

if [[ -f "$c_file~1" ]]; then
  if diff $c_file "$c_file~1" >/dev/null ; then
    exit 0
  fi
fi

g++ -c $c_file -o $o_file >/dev/null 2>&1
ar rvs $a_file $o_file >/dev/null 2>&1

exit
