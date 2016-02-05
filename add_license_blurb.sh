#!/bin/bash

origAuthor=""
moreAuthors=""
#AUTHORS: sets author variables for use in the file header
# origAuthor = is the fullname and email of the person who created the file
# moreAuthors = is a list (fullname and email) of other people who contributed
#               more than once to the file since creation.
# newLine = if moreAuthors is not empty, newLine will contain a newline char.
AUTHORS() {
  origAuthor="`git log --format='%aN <%aE>' --reverse $1 | grep 'kdab\.com' | head -1 | awk '{printf("  Author: "); for(i=1;i<NF; i++)printf("%s ",$i); printf("%s",$NF);}'`"
  origEmail=`echo $origAuthor | cut -d\< -f2 | cut -d\> -f1`
  moreAuthors=`git log --format='%aN <%aE>' $FILE | grep -v $origEmail | grep 'kdab\.com' | sort | uniq -c | sort -nr | \
  awk '{ \
    if($1>1) {\
      printf("  Author: "); \
      for(i=2;i<NF; i++)printf("%s ",$i); \
      printf("%s\n",$NF); \
    } \
  }'`
  newLine="$moreAuthors"
  if(test -n "$newLine")
  then
    newLine=$'\n'
  else
    newLine=""
  fi
}

find "$@" -name '*.h' -o -name '*.cpp' -o -name '*.c' -o -name '*.qml' -o -name '*.js' | grep -v /3rdparty/ | grep -v /build | grep -v /codeeditor | grep -v /qopengl2pexvertexarray | grep -v examples/qsmdebugger/trafficlight | while read FILE; do
    if grep -qiE "Copyright \(C\) [0-9, -]{4,} Klar.*lvdalens Datakonsult AB" "$FILE" ; then continue; fi
    AUTHORS $FILE
    thisYear=`date +%Y`
    cat <<EOF > "$FILE".tmp
/*
  This file is part of the KDAB State Machine Editor Library.

  Copyright (C) $thisYear Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.
  All rights reserved.
$origAuthor
$moreAuthors
$newLine  Licensees holding valid commercial KDAB State Machine Editor Library
  licenses may use this file in accordance with the KDAB State Machine Editor
  Library License Agreement provided with the Software.

  This file may be distributed and/or modified under the terms of the
  GNU Lesser General Public License version 2.1 as published by the
  Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.

  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

  Contact info@kdab.com if any conditions of this licensing are not
  clear to you.
*/

EOF
    cat "$FILE" >> "$FILE".tmp
    mv "$FILE".tmp "$FILE"
done

#remove the following exit if you want to add a header to CMakeLists.txt files
exit

find "$@" -name 'CMakeLists.txt' | while read FILE; do
    if grep -qiE "Copyright \(C\) [0-9, -]{4,} Klar.*lvdalens Datakonsult AB" "$FILE" ; then continue; fi
    AUTHORS $FILE
    cat <<EOF > "$FILE".tmp
#  This file is part of the KDAB State Machine Editor Library.
#
#  Copyright (C) $thisYear Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com.  All rights reserved.
#$origAuthor
#
#  Licensees holding valid commercial KDAB State Machine Editor Library
#  licenses may use this file in accordance with the KDAB State Machine Editor
#  Library License Agreement provided with the Software.
#
#  This file may be distributed and/or modified under the terms of the
#  GNU Lesser General Public License version 2.1 as published by the
#  Free Software Foundation and appearing in the file LICENSE.LGPL.txt included.
#
#  This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
#  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
#  Contact info@kdab.com if any conditions of this licensing are not
#  clear to you.

EOF
    cat "$FILE" >> "$FILE".tmp
    mv "$FILE".tmp "$FILE"
done
