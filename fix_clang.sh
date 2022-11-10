#!/bin/bash

if [ "$(uname)" == "Darwin" ]; then
  find -E . -regex '.*\.(cpp|hpp|c|cc|cxx|h)' -exec clang-format -i {} \;
elif [ "$(uname)" == "Linux" ]; then
  find . -regextype posix-extended -regex '.*\.(cpp|hpp|c|cc|cxx|h)' -exec clang-format -i {} \;
fi
