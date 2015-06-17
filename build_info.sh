
if [[ ! -n "$1" ]]; then
   exit -1
fi

if [[ ! -n "$2" ]]; then
   path="/tmp"
else
   path=$2
fi

if [[ ! -n "$2" ]]; then
   build_type=""
else
   build_type=$3
fi



tag=$(git describe --abbrev=0 --tags 2>/dev/null)
commits_head=$(git rev-list HEAD --count 2>/dev/null)
if [[ ! -z "$tag" ]]; then
  commits_tag=$(git rev-list $tag --count)
else
  commits_tag="0"
fi

version=$tag
branch=`git rev-parse --abbrev-ref HEAD`
if [[ -z $version ]]; then
  version="($branch)"
fi

untaged_commits=`expr $commits_head - $commits_tag`
uncommited=`expr $(git status --porcelain 2>/dev/null | egrep "^(M| M)" | wc -l)`
unpushed=$(git log --branches --not --remotes --simplify-by-decoration --decorate --oneline | wc -l)

if [[ $(expr $untaged_commits + $uncommited) -ne "0"  ]]; then
  version="$version-$untaged_commits.$uncommited"
fi

if [[ "$unpushed" -ne "0"  ]]; then
  version="$version[$unpushed]"
fi

version="$version($build_type)"

toplevel=`git rev-parse --show-toplevel  2>/dev/null`
basename=`basename $toplevel`

commit=`git log -1 --pretty=format:"%H"`
commit_author=`git log -1 --pretty=format:"%cn <%ce>"`
project_author=$commit_author
message=$(git log -1 --pretty=format:"%s")
commitdate=$(date "+%F %T %:z" -d @`git log -1 --pretty=format:"%ct"`)
builddate=$(date "+%F %T %:z")
date="$builddate (commit: $commitdate)"
authors=$(git log --format="%aN <%aE>" | sort | uniq -c | sort -nr | awk '{for(i=2;i<=NF;i++) printf "%s ",$i; printf "(%s), ",$1;}')

h_file="$1_build_info.h"
c_file="$path/$1_build_info.c"
o_file="$path/$1_build_info.o"
a_file="$path/$1_build_info.a"

if [[ ! -f "$h_file" ]]; then
  echo "struct $1_build_info{" > $h_file
  echo "  bool enabled() const;" >> $h_file
  echo "  const char* name() const;" >> $h_file
  echo "  const char* version() const;" >> $h_file
  echo "  const char* branch() const;" >> $h_file
  echo "  const char* commit() const;" >> $h_file
  echo "  const char* date() const;" >> $h_file
  echo "  const char* author() const;" >> $h_file
  echo "  const char* project() const;" >> $h_file
  echo "  const char* message() const;" >> $h_file
  echo "  const char* authors() const;" >> $h_file
  echo "};" >> $h_file
fi

echo "#include \"`pwd`/$h_file\"" > $c_file
echo "bool $1_build_info::enabled() const { return true;}" >> $c_file
echo "const char* $1_build_info::name() const { return \"$basename\";}" >> $c_file
echo "const char* $1_build_info::version() const { return \"$version\"; }" >> $c_file
echo "const char* $1_build_info::branch() const { return \"$branch\"; }" >> $c_file
echo "const char* $1_build_info::commit() const { return \"$commit\"; }" >> $c_file
echo "const char* $1_build_info::author() const { return \"$commit_author\"; }" >> $c_file
echo "const char* $1_build_info::project() const { return \"$project_author\"; }" >> $c_file
echo "const char* $1_build_info::message() const { return \"$message\"; }" >> $c_file
echo "const char* $1_build_info::authors() const { return \"$authors\"; }" >> $c_file

if [[ -f "$c_file~1" ]]; then
  if diff $c_file "$c_file~1" >/dev/null ; then
    exit 0
  fi
fi
cp $c_file "$c_file~1"
echo "const char* $1_build_info::date() const { return \"$date\"; }" >> $c_file

echo "BuildInfo"
echo -e "\tname:    $basename"
echo -e "\tversion: $version"
echo -e "\tbranch:  $branch"
echo -e "\tcommit:  $commit"
echo -e "\tdate:    $date"
echo -e "\tauthor:  $author"
echo -e "\tmessage: $message"
echo -e "\tauthors: $authors"

g++ -c $c_file -fPIC -o $o_file >/dev/null 2>&1
ar rvs $a_file $o_file >/dev/null 2>&1

exit
















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
message2=$(git log -1 --pretty=format:"%s")

commitdate2=$(date "+%F %T %:z" -d @`git log -1 --pretty=format:"%ct"`)
builddate2=$(date "+%F %T %:z")
date2="$builddate2 (commit: $commitdate2)"
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

h_file2="$1_2build_info.h"
c_file2="$path/$1_2build_info.c"
o_file2="$path/$1_2build_info.o"
a_file2="$path/$1_2build_info.a"

if [[ ! -f "$h_file2" ]]; then
  echo "struct $1_build_info{" > $h_file2
  echo "  const char* name() const;" >> $h_file2
  echo "  const char* version() const;" >> $h_file2
  echo "  const char* branch() const;" >> $h_file2
  echo "  const char* commit() const;" >> $h_file2
  echo "  const char* date() const;" >> $h_file2
  echo "  const char* author() const;" >> $h_file2
  echo "  const char* message() const;" >> $h_file2
  echo "  const char* authors() const;" >> $h_file2
  echo "  const char* build() const;" >> $h_file2
  echo "};" >> $h_file2
fi

echo "#include \"`pwd`/$h_file2\"" > $c_file2
echo "const char* $1_build_info::name() const { return \"$basename2\";}" >> $c_file2
echo "const char* $1_build_info::version() const { return \"$version2\"; }" >> $c_file2
echo "const char* $1_build_info::branch() const { return \"$branch2\"; }" >> $c_file2
echo "const char* $1_build_info::commit() const { return \"$commit2\"; }" >> $c_file2
echo "const char* $1_build_info::date() const { return \"$date2\"; }" >> $c_file2
echo "const char* $1_build_info::author() const { return \"$author2\"; }" >> $c_file2
echo "const char* $1_build_info::message() const { return \"$message2\"; }" >> $c_file2
echo "const char* $1_build_info::authors() const { return \"$authors2\"; }" >> $c_file2
echo "const char* $1_build_info::build() const {}" >> $c_file2




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

if [[ -f "$c_file2~1" ]]; then
  if diff $c_file2 "$c_file2~1" >/dev/null ; then
    exit 0
  fi
fi

g++ -c $c_file -o $o_file >/dev/null 2>&1
ar rvs $a_file $o_file >/dev/null 2>&1

echo "---------------------------"
echo 'g++ -c $c_file2 -o $o_file2'
echo "+++++++++++++++++++++++++++"
g++ -c $c_file2 -o $o_file2 
ar rvs $a_file2 $o_file2 >/dev/null 2>&1

exit
