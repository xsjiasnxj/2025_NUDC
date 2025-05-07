#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import chardet
import shutil
from pathlib import Path

def detect_encoding(file_path):
    """检测文件编码"""
    with open(file_path, 'rb') as f:
        raw_data = f.read()
        if not raw_data:
            return None
        result = chardet.detect(raw_data)
        return result['encoding']

def is_text_file(file_path):
    """判断是否为文本文件（简单判断）"""
    try:
        with open(file_path, 'rb') as f:
            chunk = f.read(1024)
            if b'\0' in chunk:  # 二进制文件通常包含空字节
                return False
            return True
    except Exception:
        return False

def convert_file(file_path, backup=True):
    """将文件从GB2312转换为UTF-8"""
    try:
        # 检测当前编码
        encoding = detect_encoding(file_path)
        
        # 如果无法检测编码或已经是UTF-8，则跳过
        if not encoding or encoding.lower() in ('utf-8', 'utf8', 'ascii'):
            return False, f"跳过: {file_path} (编码: {encoding})"
        
        # 判断是否为GB2312相关编码
        is_gb_encoding = encoding.lower() in ('gb2312', 'gbk', 'gb18030')
        if not is_gb_encoding:
            return False, f"跳过: {file_path} (编码: {encoding}, 非GB2312相关编码)"
        
        # 备份原文件
        if backup:
            backup_path = str(file_path) + '.bak'
            shutil.copy2(file_path, backup_path)
        
        # 读取文件内容
        with open(file_path, 'r', encoding=encoding, errors='replace') as f:
            content = f.read()
        
        # 以UTF-8编码写回文件
        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(content)
        
        return True, f"转换完成: {file_path} (从 {encoding} 转为 UTF-8)"
    
    except Exception as e:
        return False, f"错误: {file_path} - {str(e)}"

def convert_directory(directory_path, recursive=True, backup=True):
    """转换目录中的所有文件"""
    directory = Path(directory_path)
    if not directory.exists() or not directory.is_dir():
        print(f"错误: '{directory_path}'不是有效目录!")
        return
    
    # 获取所有需要处理的文件
    files_to_process = []
    if recursive:
        for root, _, files in os.walk(directory_path):
            for filename in files:
                filepath = os.path.join(root, filename)
                files_to_process.append(filepath)
    else:
        files_to_process = [os.path.join(directory_path, f) 
                           for f in os.listdir(directory_path) 
                           if os.path.isfile(os.path.join(directory_path, f))]
    
    # 统计
    total_files = len(files_to_process)
    converted_files = 0
    skipped_files = 0
    error_files = 0
    
    print(f"开始处理目录: {directory_path}")
    print(f"共发现 {total_files} 个文件")
    
    # 处理每个文件
    for file_path in files_to_process:
        try:
            if not is_text_file(file_path):
                print(f"跳过二进制文件: {file_path}")
                skipped_files += 1
                continue
                
            success, message = convert_file(file_path, backup)
            print(message)
            
            if success:
                converted_files += 1
            else:
                skipped_files += 1
                
        except Exception as e:
            print(f"处理文件出错 {file_path}: {str(e)}")
            error_files += 1
    
    # 输出汇总信息
    print("\n转换完成!")
    print(f"总文件数: {total_files}")
    print(f"成功转换: {converted_files}")
    print(f"跳过文件: {skipped_files}")
    print(f"处理出错: {error_files}")

def main():
    # 解析命令行参数
    import argparse
    parser = argparse.ArgumentParser(description='将目录中的GB2312编码文件转换为UTF-8')
    parser.add_argument('directory', help='需要处理的目录路径')
    parser.add_argument('-r', '--recursive', action='store_true', 
                        help='是否递归处理子目录 (默认: 是)')
    parser.add_argument('--no-backup', action='store_true',
                        help='不创建备份文件 (默认: 创建)')
    parser.add_argument('--no-recursive', action='store_true',
                        help='不递归处理子目录')
    
    args = parser.parse_args()
    
    # 设置处理参数
    directory = args.directory
    recursive = not args.no_recursive
    backup = not args.no_backup
    
    # 执行转换
    convert_directory(directory, recursive, backup)

if __name__ == "__main__":
    main()