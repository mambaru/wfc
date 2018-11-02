#!/usr/bin/env bash

if [ -f ".git" ]; then
  echo "Ошибка! Не нужно запускать это в субмодуле. Склонируйте этот проект в отдельную директорию."
  exit 1
fi

if [ ! -d ".git" ]; then
  echo "Ошибка! Этот скрипт нужно запускать только в корневой папке проекта."
  exit 1
fi

if [ -d "wfcroot" ]; then
  echo "Ошибка! Этот скрипт работает только с библиотеками и не подходит для демонов."
  exit 1
fi

if ! git diff-index --quiet HEAD -- ; then
  echo "Ошибка! У вас есть не зафиксированные изменения! Сделайте git commit и повторно запустите этот скрипт"
  exit 1
fi

interactive_merge () {
  target=$1
  sdiff -sw 130 -o "${target}.tmp" "${target}" "$PWD/external/cmake-ci/ci/${target}"
  if [ $? -eq 2 ]; then
    echo "Отмена! слияния ${target}"
    rm -f "${target}.tmp"
  else
    mv "${target}.tmp" "${target}"
    cp "$PWD/external/cmake-ci/ci/${target}" ".ci/${target}.orig"
    echo "Слито! ${target}"
  fi
}

see_changes () {
  target=$1
  if ! diff "${target}" "$PWD/external/cmake-ci/ci/${target}"; then
    echo "Выше показаны изменения в обновленном ${target} относительно твоего."
    echo "Как обычно у тебя три опции:"
    
    case1="Не обновлять. Оставить старый ${target} (с твоими изменениями)"
    case2="Принять новый ${target} (с потерей твоих изменений)"
    case3="Интерактивно слить с помощью sdiff (далее 1 - это твои, 2 - новые)"
    
    COLUMNS=12
    select yn in "${case1}" "${case2}" "${case3}"
    do
      case $yn in
        "${case1}" )
            echo "Файл ${target} без обновлений"
            break;;
        "${case2}" )
            cp "$PWD/external/cmake-ci/ci/${target}" "${target}"
            cp "$PWD/external/cmake-ci/ci/${target}" ".ci/${target}.orig"
            echo "Файл ${target} заменен с потерей изменений"
            break;;
        "${case3}" )
            interactive_merge "${target}"
            break;;
        * ) echo "Не тупи! 1,2 или 3"
      esac
    done
  fi
}

see_update () {
  target=$1

  echo "У мне две новости, дорогой друг. Первая заключается в том что прилетело "
  echo "обновление для ${target}. Вторая - ты внес в ${target} некоторые изменения."
  echo "Выше показаны изменения обновления (без учета твоих изменений)."
   
  case1="Не обновлять. Оставить старый ${target} (с твоими изменениями)."
  case2="Принять новый ${target} (с потерей твоих изменений)."
  case3="Посмотреть мои изменения."
    
  COLUMNS=12
  select yn in "${case1}" "${case2}" "${case3}"
  do
    case $yn in
      "${case1}" ) 
          echo "Файл ${target} без обновлений"
          break;;
      "${case2}" )
          echo "Файл ${target} заменен с потерей изменений"
          cp "$PWD/external/cmake-ci/ci/${target}" "${target}"
          cp "$PWD/external/cmake-ci/ci/${target}" ".ci/${target}.orig"
          break;;
      "${case3}" )
          see_changes "${target}"
          break;;
      * ) echo "Не тупи! 1,2 или 3"
    esac
  done
}

update_if () {
  target=$1
  if cmp -s "${target}" ".ci/${target}.orig" ; then
    echo "Файл ${target} обновлен"
    cp "$PWD/external/cmake-ci/ci/${target}" "${target}"
    cp "$PWD/external/cmake-ci/ci/${target}" ".ci/${target}.orig"
  else
    see_update "${target}"
  fi
}

update_file () {
  # проверяем пришло ли обновление 
  target=$1
  if [ ! -e ${target} ]; then
    # сохраняем файл и оригинал для сравнения при следующем update
    echo "Новый файл ${target}"
    cp "$PWD/external/cmake-ci/ci/${target}" "${target}"
    cp "$PWD/external/cmake-ci/ci/${target}" .ci/${target}.orig
  elif diff "$PWD/external/cmake-ci/ci/${target}" ".ci/${target}.orig"; then
    echo "Файл ${target} без обновлений"
  else
    # обновляем файл, если нет локальных изменений, либо опрос
    update_if "${target}"
  fi
}

echo "你好亲爱的朋友！"
echo "Этот скрипт подготовит ваш проект (библиотеку) для непрерывной сборки в gitlab-ci."
echo "Убедитесь, что вы запускаете его из корневой директории проекта. Если проект уже "
echo "подготовлен, то он будет обновлен до последней версии из master (бранч можно указать "
echo "первым аргументом). По завершению работы скрипта будет добавлен git submodule external/cmake-ci "
echo "и обновлены некоторые файлы. Продолжаем?"

select yn in "Yes" "No"
do
  case $yn in
      Yes ) break;;
      No ) exit;;
      * ) echo "Вот ты лопух! Зачем всякую дичь печатаешь? Надо число '1' или '2', что не понятно? Уверен что в состоянии дальше продолжать?"
  esac
done


branch="master"
[ ! -z $1 ] && branch=$1
echo "branch=$branch"
if [ ! -d "external/cmake-ci" ]; then
  git submodule add git@github.lan:cpp/cmake-ci.git external/cmake-ci || exit
else
  git submodule update --init -- external/cmake-ci || exit
  pushd external/cmake-ci
  git checkout $branch || exit
  popd
fi

mkdir -p .ci
echo http://github.lan/cpp/cmake-ci/blob/master/Readme.md
[ ! -f ".ci/suppressions.txt" ] && touch ".ci/suppressions.txt"
ln -fs "$PWD/external/cmake-ci/ci/automr.sh" ".ci/automr.sh"
ln -fs "$PWD/external/cmake-ci/ci/cppcheck.sh" ".ci/cppcheck.sh"
ln -fs "$PWD/external/cmake-ci/ci/build-and-test.sh" ".ci/build-and-test.sh"
update_file Doxyfile
update_file Makefile
update_file customdoxygen.css
update_file .gitlab-ci.yml
rm upgrate.sh
git add .
if git commit -am "cmake-ci upgrade autocommit"; then
  echo "Все изменения будут автоматически зафиксированы. Для отмены, наберите: "
  echo "  git reset --hard HEAD~1"
fi
echo "Дальнейшие инструкции найдете здесь:"
echo "  http://github.lan/cpp/cmake-ci/blob/master/Readme.md"
echo Done
exit 0
