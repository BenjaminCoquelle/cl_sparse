FILE(REMOVE_RECURSE
  "CMakeFiles/tplgen"
  "CMakeFiles/tplgen-complete"
  "tplgen-prefix/src/tplgen-stamp/tplgen-install"
  "tplgen-prefix/src/tplgen-stamp/tplgen-mkdir"
  "tplgen-prefix/src/tplgen-stamp/tplgen-download"
  "tplgen-prefix/src/tplgen-stamp/tplgen-update"
  "tplgen-prefix/src/tplgen-stamp/tplgen-patch"
  "tplgen-prefix/src/tplgen-stamp/tplgen-configure"
  "tplgen-prefix/src/tplgen-stamp/tplgen-build"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/tplgen.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
