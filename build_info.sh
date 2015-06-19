
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
mkdir -p $path/build_info
c_file="$path/build_info/$1_build_info.c"
o_file="$path/build_info/$1_build_info.o"
a_file="$path/build_info/$1_build_info.a"

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
