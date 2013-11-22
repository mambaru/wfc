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


echo "namespace"
echo "{"
echo "  const char* build_info="
echo "    \"$version\n\""
echo "    \"$branch\n\""
echo "    \"$commit\n\""
echo "    \"$author\n\""
echo "    \"$date\n\""
echo "    \"$message\n\";"
echo "}"


