#!/bin/sh

ASSET_DIR="assets"
HEADER="texture_enum.h"
SOURCE="texture_enum.c"

echo "// auto-generated from $ASSET_DIR the $(date +%Y-%m-%d)" > $HEADER
echo "#pragma once" >> $HEADER
echo "" >> $HEADER

echo "typedef enum FACE_TEXTURE {" >> $HEADER

COUNT=0
for file in "$ASSET_DIR"/*.png; do
    base=$(basename "$file" .png)
    enum_name="TEX_$(echo "$base" | tr '[:lower:]' '[:upper:]' | sed 's/[^A-Z0-9_]/_/g')"
    echo "    $enum_name," >> $HEADER
    COUNT=$((COUNT + 1))
done

echo "} FACE_TEXTURE;" >> $HEADER
echo "" >> $HEADER
echo "#define TEXTURE_COUNT $COUNT" >> $HEADER
echo "" >> $HEADER

echo "extern const char *TexturePaths[TEXTURE_COUNT];" >> $HEADER

echo "// auto-generated source from $ASSET_DIR the $(date +%Y-%m-%d)" > $SOURCE
echo "#include \"$HEADER\"" >> $SOURCE
echo "" >> $SOURCE

echo "const char *TexturePaths[TEXTURE_COUNT] = {" >> $SOURCE

for file in "$ASSET_DIR"/*.png; do
    base=$(basename "$file" .png)
    enum_name="TEX_$(echo "$base" | tr '[:lower:]' '[:upper:]' | sed 's/[^A-Z0-9_]/_/g')"
    echo "    [$enum_name] = \"$file\"," >> $SOURCE
done

echo "};" >> $SOURCE

echo "Generated $HEADER and $SOURCE with $COUNT texture(s)"
