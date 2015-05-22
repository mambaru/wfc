
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

#################################################

# Последний доступный тег
tag2=$(git describe --abbrev=0 --tags 2>/dev/null)
# всего коммитов
commits_head2=$(git rev-list HEAD --count 2>/dev/null)
# всего коммитов до последнего тега
if [[ ! -z "$tag2" ]]; then
  commits_tag2=$(git rev-list $tag2 --count)
else
  commits_tag2="0"
fi
# число коммтов от тега
untaged_commits2=`expr $commits_head2 - $commits_tag2`
# число файлов не закомиченных
uncommited2=`expr $(git status --porcelain 2>/dev/null | egrep "^(M| M)" | wc -l)`
# число комитов не запушеных
unpushed2=$(git log --branches --not --remotes --simplify-by-decoration --decorate --oneline | wc -l)
# Проверочная строка для проверки )) 
checkstr2="$tag2-$untaged_commits2-$uncommited2-$unpushed2"
chk_file2="$path/$1_build_info.txt"

if [ "$checkstr2" == "$(cat $chk_file2 2>/dev/null )" ]; then
  echo "DONE"
  #exit 0
fi
echo $checkstr2 > $chk_file2

toplevel2=`git rev-parse --show-toplevel  2>/dev/null`
basename2=`basename $toplevel2`
branch2=`git rev-parse --abbrev-ref HEAD`
commit2=`git log -1 --pretty=format:"%H"`
author2=`git log -1 --pretty=format:"%cn <%ce>"`
date2=`git log -1 --pretty=format:"%cd"`
allauthors2=$(git log --format="%aN <%aE>" | sort | uniq -c | sort -nr | awk '{for(i=2;i<=NF;i++) printf "%s ",$i; printf "(%s), ",$1;}')

version2=$tag2
if [[ -z $version2 ]]; then
  version2="($branch2)"
fi

exver=$(expr $untaged_commits2 + $uncommited2)

if [[ "$exver" -ne "0"  ]]; then
  version2="$version2-$untaged_commits2.$uncommited2"
fi

if [[ "$unpushed2" -ne "0"  ]]; then
  version2="$version2-$unpushed2"
fi

# всего коммитов
#head_commit_count2=`git rev-list HEAD --count`
# всего коммитов до последнего тега
#last_tag_commit_count2=`git rev-list $tag2 --count`
#коммтов от последнего тега
#untaged_commits2=`expr $head_commit_count2 - $last_tag_commit_count2`
# количество назапушеных коммитов
#unpushed_commits2=`git log --branches --not --remotes --simplify-by-decoration --decorate --oneline | wc -l`
# Get number of total uncommited files
#uncommited2=`expr $(git status --porcelain 2>/dev/null| egrep "^(M| M)" | wc -l)`
#TODO: inital author
#TODO: real author, default as initial author
#TODO: author list почислу коммитов (count)
#allauthors2=`git shortlog -e -s -n | awk '{printf "%s %s %s %s(%s), ",$2,$3,$4,$5,$1}'`
#allauthors2="expr $(git shortlog -e -s -n)"

#echo `git shortlog -e -s -n` >> /tmp/aaa1
#aaa=`git --status`
#allauthors2=`expr $(git --no-pager shortlog -e -s -n | awk '{printf "%s %s %s %s(%s), ",$2,$3,$4,$5,$1}')`
#allauthors2=zzz`git --no-pager shortlog -e -s -n 2>/dev/null`

#echo "aaa: $allauthors2" >> /tmp/aaa
#git shortlog -e -s -n | awk '{printf "%s %s %s %s(%s), ",$2,$3,$4,$5,$1}' >> /tmp/aaa
#git shortlog -e -s -n | cat
#git status


#if [[ ! -z $tag2 ]]; then
#  revlist2=`git rev-list $tag2 | wc -l`
#fi

if [[ -z "$version2" ]]; then
  version2="Use git --tag as version"
fi
echo "BuildInfo"
echo -e "\tversion2: $version2"
echo -e "\tbasename2: $basename2"
echo -e "\ttag2: $tag2"
echo -e "\tbranch2: $branch2"
echo -e "\tcommit2: $commit2"
echo -e "\tauthor2: $author2"
echo -e "\tdate2: $date2"
echo -e "\tuncommited2: $uncommited2"
echo -e "\trevlist2: $revlist2"
echo -e "\tuntaged_commits2: $untaged_commits2"
echo -e "\thead_commit_count2: $head_commit_count2"
echo -e "\tunpushed_commits2: $unpushed_commits2"
echo -e "\tallauthors2: ${allauthors2::-2}"


#echo "DEBUG build_info.sh $version"
#echo "DEBUG build_info.sh $branch"

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
