#!/bin/bash

preserve_file="000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f.json"
parent_dir="testenv2"

cd "$parent_dir"

if [ $? -ne 0 ]; then
    echo "Failed to enter directory $parent_dir"
    exit 1
fi

for coin_dir in coin_*; do
    # 检查是否存在 blockchain 子目录
    if [ -d "$coin_dir/blockchain" ]; then
        echo "Cleaning up $coin_dir/blockchain"

        # 进入 blockchain 目录
        cd "$coin_dir/blockchain"

        # 检查是否成功进入目录
        if [ $? -ne 0 ]; then
            echo "Failed to enter directory $coin_dir/blockchain"
            continue
        fi

        # 删除除了特定文件以外的其他文件
        find . -type f ! -name "$preserve_file" -exec rm -f {} \;

        # 返回到初始的父级目录
        cd -
    else
        echo "blockchain directory not found in $coin_dir"
    fi
done

echo "Cleanup completed."