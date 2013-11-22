version=`git describe --long`
branch=`git branch | grep '\*' | grep -o '[^ *].\+$'`
show='git show HEAD'
#commit=`git log . | head -n1`
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
echo "    \"$version\""
echo "    \"$commit\""
echo "    \"$author\""
echo "    \"$date\""
echo "    \"Branch: $branch\""
echo "    \"$git_status\""
echo "}"


