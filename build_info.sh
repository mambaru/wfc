
if [[ ! -n "$1" ]]; then
   exit -1
fi


#if [[ ! -n "$2" ]]; then
#   exit -2
# fi


version=`git describe --long`
branch="Branch: `git branch | grep '\*' | grep -o '[^ *].\+$'`"
commit=`git log -1 --pretty=format:"commit %H" .`
author=`git log -1 --pretty=format:"Author: %cn <%ce>" .`
date=`git log -1 --pretty=format:"Date: %cd" .`
message=`git log -1 --pretty=%B .`
#author=`git log . | head -n4 | grep 'Author'`
#date=`git log . | head -n4 | grep 'Date'`
#comment=`git log . | sed '5!d'` 
#git_status=`git status | grep 'modified:' | grep -v 'untracked'`
#if [[ -z $git_status ]]; then
#  git_status=`git status | grep 'deleted:'`
#fi

h_file="build_info.h"
c_file="/tmp/$1_build_info.c"
# ns = "$2"

# echo "namespace" > $h_file
# echo "{" >> $h_file
if [[ ! -f "$h_file" ]]; then
  echo "extern const char* $1_build_info;" > $h_file 
fi

# echo "}" >> $h_file
echo "const char* $1_build_info=" > $c_file
echo "    \"$version\n\""  >> $c_file
echo "    \"$branch\n\""   >> $c_file
echo "    \"$commit\n\""   >> $c_file
echo "    \"$author\n\""   >> $c_file
echo "    \"$date\n\""     >> $c_file
echo "    \"$message\n\";" >> $c_file

# `pwd` > $c_file
g++ -c $c_file -o "/tmp/$1_build_info.o"  >/dev/null 2>&1
#   rm $c_file

exit

echo "namespace" >> 
echo "{"
echo "  const char* build_info="
echo "    \"$version\n\""
echo "    \"$branch\n\""
echo "    \"$commit\n\""
echo "    \"$author\n\""
echo "    \"$date\n\""
echo "    \"$message\n\";"
echo "}"


