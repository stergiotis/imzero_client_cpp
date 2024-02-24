function link() {
   d=$(dirname "$2")
   b=$(basename "$2")
   cd "$here"
   mkdir -p "$d"
   rm -f "$2"
   touch "$2"
   l=$(realpath --strip --relative-to="$d" "$1")

   cd "$d"
   rm -f "$b"
   echo "$l --> $b"
   ln -s "$l" "$b"
}
