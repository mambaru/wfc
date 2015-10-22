#
# update arguments
#

if [[ ! -n "$1" ]]; then
   exit -1
fi

if [[ ! -n "$2" ]]; then
   path="/tmp"
else
   path="$2/$3"
fi

if [[ ! -n "$2" ]]; then
   build_type=""
else
   build_type=$3
fi

if [[ ! -n "$4" ]]; then
   compiler=""
else
   compiler=$4
   readlink=$(readlink $compiler 2>/dev/null)
   if [[ -n "$readlink" ]]; then
      compiler=$readlink
   fi
   compiler=$(basename $compiler 2>/dev/null)
fi

if [[ ! -n "$5" ]]; then
   flags=""
else
   flags=$5
fi

buildflags="$compiler $flags"

#
# make version
#

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
lastmodifytime=$(stat -c %z `pwd`)

verex=""
if [[ $(expr $untaged_commits + $uncommited + $unpushed) -ne "0"  ]]; then
  verex="$untaged_commits.$unpushed.$uncommited"
fi

toplevel=`git rev-parse --show-toplevel  2>/dev/null`
basename=`basename $toplevel`

commit=`git log -1 --pretty=format:"%H"`
commit_author=`git log -1 --pretty=format:"%cn <%ce>"`
project_author=$commit_author
message=$(git log -1 --pretty=format:"%s")
commitdate=$(date "+%F %T %:z" -d @`git log -1 --pretty=format:"%ct"`)
builddate=$(date "+%F %T %:z")
#date="$builddate (commit: $commitdate)"
authors=$(git log --format="%aN <%aE>" | sort | uniq -c | sort -nr | awk '{for(i=2;i<=NF;i++) printf "%s ",$i; printf "(%s), ",$1;}')


#
# create file names
#

h_file="$1_build_info.h"
count_file=".$1_build.counter"
mkdir -p $path/build_info
c1_file="$path/build_info/$1_build_info1.c"
c2_file="$path/build_info/$1_build_info2.c"
o1_file="$path/build_info/$1_build_info1.o"
o2_file="$path/build_info/$1_build_info2.o"
a_file="$path/build_info/$1_build_info.a"

#
# create basic cpp
#

echo "// $1 $lastmodifytime" > $c1_file
echo "#include \"`pwd`/$h_file\"" >> $c1_file
echo "bool $1_build_info::enabled() const { return true;}" >> $c1_file
echo "const char* $1_build_info::name() const { return \"$basename\";}" >> $c1_file
echo "const char* $1_build_info::version() const { return \"$version\"; }" >> $c1_file
echo "const char* $1_build_info::verex() const { return \"$verex\"; }" >> $c1_file
echo "const char* $1_build_info::build_type() const { return \"$build_type\"; }" >> $c1_file
echo "const char* $1_build_info::build_flags() const { return \"$buildflags\"; }" >> $c1_file
echo "const char* $1_build_info::branch() const { return \"$branch\"; }" >> $c1_file
echo "const char* $1_build_info::commit() const { return \"$commit\"; }" >> $c1_file
echo "const char* $1_build_info::commit_date() const { return \"$commitdate\"; }" >> $c1_file
echo "const char* $1_build_info::commit_message() const { return \"$message\"; }" >> $c1_file
echo "const char* $1_build_info::commit_author() const { return \"$commit_author\"; }" >> $c1_file
echo "const char* $1_build_info::project_author() const { return \"$project_author\"; }" >> $c1_file
echo "const char* $1_build_info::all_authors() const { return \"$authors\"; }" >> $c1_file

# check rebuild needs

if [[ -f "$c1_file~1" ]]; then
  if diff $c1_file "$c1_file~1" >/dev/null ; then
     exit 0
  fi
fi

echo "Create build info files $1"

# build counter

count=0
if [[ -f "$count_file" ]]; then
  count=`cat $count_file`
  count=$((count+1))
fi
echo $count > $count_file

#
# create extended cpp
#

echo "#include \"`pwd`/$h_file\"" > $c2_file
echo "const char* $1_build_info::build_date() const { return \"$builddate\"; }" >> $c2_file
echo "const char* $1_build_info::build_count() const { return \"$count\"; }" >> $c2_file

#
# create hpp
#

if [[ ! -f "$h_file" ]]; then
  echo "struct $1_build_info{" > $h_file
  echo "  bool enabled() const;" >> $h_file
  echo "  const char* name() const;" >> $h_file
  echo "  const char* version() const;" >> $h_file
  echo "  const char* verex() const;" >> $h_file
  echo "  const char* build_type() const;" >> $h_file
  echo "  const char* build_date() const;" >> $h_file
  echo "  const char* build_flags() const;" >> $h_file
  echo "  const char* build_count() const;" >> $h_file
  echo "  const char* branch() const;" >> $h_file
  echo "  const char* commit() const;" >> $h_file
  echo "  const char* commit_date() const;" >> $h_file
  echo "  const char* commit_author() const;" >> $h_file
  echo "  const char* commit_message() const;" >> $h_file
  echo "  const char* project_author() const;" >> $h_file
  echo "  const char* all_authors() const;" >> $h_file
  echo "};" >> $h_file
fi

# build
g++ -c $c1_file -fPIC -o $o1_file
g++ -c $c2_file -fPIC -o $o2_file
ar rvs $a_file $o1_file $o2_file >/dev/null 
cp $c1_file "$c1_file~1"

# show build_info

echo "BuildInfo"
echo -e "\tName:            $basename"
echo -e "\tVersion:         $version[$verex]-$count"
echo -e "\tBuild Type:      $build_type"
echo -e "\tBuild Date:      $builddate"
echo -e "\tBuild Flags:     $buildflags"
echo -e "\tBranch:          $branch"
echo -e "\tCommit:          $commit"
echo -e "\tCommit Date:     $commitdate"
echo -e "\tCommit Message:  $message"
echo -e "\tCommit Author:   $commit_author"
echo -e "\tProject Authors: $project_author"
echo -e "\tAll Authors:     $authors"
