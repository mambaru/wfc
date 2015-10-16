
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

version="$version"

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
mkdir -p $path/build_info
c1_file="$path/build_info/$1_build_info1.c"
c2_file="$path/build_info/$1_build_info2.c"
o_file="$path/build_info/$1_build_info.o"
a_file="$path/build_info/$1_build_info.a"

if [[ ! -f "$h_file" ]]; then
  echo "struct $1_build_info{" > $h_file
  echo "  bool enabled() const;" >> $h_file
  echo "  const char* name() const;" >> $h_file
  echo "  const char* version() const;" >> $h_file
  echo "  const char* build_type() const;" >> $h_file
  echo "  const char* build_date() const;" >> $h_file
  echo "  const char* branch() const;" >> $h_file
  echo "  const char* commit() const;" >> $h_file
  echo "  const char* commit_message() const;" >> $h_file
  echo "  const char* commit_author() const;" >> $h_file
  echo "  const char* commit_date() const;" >> $h_file
  echo "  const char* project_author() const;" >> $h_file
  echo "  const char* all_authors() const;" >> $h_file
  echo "};" >> $h_file
fi

echo "#include \"`pwd`/$h_file\"" > $c1_file
echo "#include \"`pwd`/$h_file\"" > $c2_file
echo "bool $1_build_info::enabled() const { return true;}" >> $c1_file
echo "const char* $1_build_info::name() const { return \"$basename\";}" >> $c1_file
echo "const char* $1_build_info::version() const { return \"$version\"; }" >> $c1_file
echo "const char* $1_build_info::build_type() const { return \"$build_type\"; }" >> $c1_file
echo "const char* $1_build_info::build_date() const { return \"$builddate\"; }" >> $c2_file
echo "const char* $1_build_info::branch() const { return \"$branch\"; }" >> $c1_file
echo "const char* $1_build_info::commit() const { return \"$commit\"; }" >> $c1_file
echo "const char* $1_build_info::commit_message() const { return \"$message\"; }" >> $c1_file
echo "const char* $1_build_info::commit_author() const { return \"$commit_author\"; }" >> $c1_file
echo "const char* $1_build_info::commit_date() const { return \"$commitdate\"; }" >> $c1_file
echo "const char* $1_build_info::project_author() const { return \"$project_author\"; }" >> $c1_file
echo "const char* $1_build_info::all_authors() const { return \"$authors\"; }" >> $c1_file

if [[ -f "$c1_file~1" ]]; then
  #echo "--1--"
  #diff $c_file "$c_file~1"
  if diff $c1_file "$c1_file~1" >/dev/null ; then
    #echo "--2--"
    exit 0
  fi
fi
cp $c1_file "$c1_file~1"
echo "BuildInfo"
echo -e "\tname:    $basename"
echo -e "\tversion: $version"
echo -e "\tbuild type:   $build_type"
echo -e "\tbranch:  $branch"
echo -e "\tcommit:  $commit"
echo -e "\tcommit date:    $date"
echo -e "\tcommit author:  $author"
echo -e "\tcommit message: $message"
echo -e "\tall authors: $authors"

g++ -c $c1_file $c2_file -fPIC -o $o_file >/dev/null 2>&1
ar rvs $a_file $o_file >/dev/null 2>&1

#echo "Build Info Done"
exit
