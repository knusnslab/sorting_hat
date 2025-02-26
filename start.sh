#!/bin/bash

# 파일명을 인자로 받는지 확인
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <filename> <filename>"
    exit 1
fi

# 인자로 받은 파일명 저장
INPUT_FILE=$1
OUTPUT_FILE=$2

# 파일이 존재하는지 확인
if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: File '$INPUT_FILE' does not exist."
    exit 1
fi

# 라인 수 계산
LINE_COUNT=$(wc -l < "$INPUT_FILE")

# trage 프로그램 실행
# rm undoc_result.txt
./classifier "$INPUT_FILE" "$LINE_COUNT" "$OUTPUT_FILE"

